/*********************************************************************************************

  This comment block must appear in the load page (e.g., main() or setup()) in any source code
  that uses code presented as whole or part of the T41-EP source code.

  (c) Frank Dziock, DD4WH, 2020_05_8
  "TEENSY CONVOLUTION SDR" substantially modified by Jack Purdum, W8TEE, and Al Peter, AC8GY

  This software is made available under the Freeware license agreement, which prevents the use of
  the software for commercial use. If any commercial use of this software is planned, the interested
  parties must first get written approval from Jack Purdum, W8TEE, and Al Peter, AC8GY. User for non-
  commerical use is acceptable.

  Any and all other commercial uses, written or implied, are forbidden without written permission from
  Dr. Jack Purdum, W8TEE, and Al Peter, AC8GY.

  V010 June, 03, 2022  Jack Purdum (W8TEE): CW sending section still incomplete
  FLASH: code:193424, data:82860, headers:8384   free for files:7841796
   RAM1: variables:162528, code:190152, padding:6456   free for local variables:165152
   RAM2: variables:429920  free for malloc/new:94368
*********************************************************************************************/

                          // setup() and loop() at the bottom of this file

#ifndef BEENHERE
#include "SDT.h"
#endif

#define TEMPMON_ROOMTEMP 25.0f
//const uint32_t FFT_LENGTH                 = 512;
uint32_t FFT_length                       = FFT_LENGTH;

extern "C"
uint32_t set_arm_clock(uint32_t frequency);
// lowering this from 600MHz to 200MHz makes power consumption less
uint32_t T4_CPU_FREQUENCY  =  500000000UL;                             //AFP 2-10-21
//uint32_t T4_CPU_FREQUENCY  =  300000000UL;

//======================================== Global object definitions ==================================================

AudioInputI2SQuad     i2s_quadIn;
AudioOutputI2SQuad    i2s_quadOut;

AudioMixer4           recMix_3;         // JJP

AudioMixer4           modeSelectOutL;
AudioMixer4           modeSelectOutR;
AudioMixer4           modeSelectOutExL;
AudioMixer4           modeSelectOutExR;

AudioMixer4           modeSelectInR;
AudioMixer4           modeSelectInL;

AudioPlayQueue        Q_out_L;
AudioPlayQueue        Q_out_R;
AudioRecordQueue      Q_in_L;
AudioRecordQueue      Q_in_R;

AudioConnection       patchCord25(i2s_quadIn, 0, modeSelectInL, 0); //Mixer Input 0 = Ex
AudioConnection       patchCord26(i2s_quadIn, 1, modeSelectInR, 0);

AudioConnection       patchCord21(i2s_quadIn, 2, modeSelectInL, 1); //Mixer Input 1 = Rec
AudioConnection       patchCord22(i2s_quadIn, 3, modeSelectInR, 1);

AudioConnection       patchCord23(modeSelectInR, 0, Q_in_R,   0);
AudioConnection       patchCord24(modeSelectInL, 0, Q_in_L,   0);

AudioConnection       patchCord3(Q_out_L,    0, modeSelectOutL,  0);
AudioConnection       patchCord4(Q_out_R,    0, modeSelectOutR, 0);

AudioConnection       patchCord9(modeSelectOutL,    0, i2s_quadOut, 3);    //AFP12-06-21
AudioConnection       patchCord10(modeSelectOutR,  0, i2s_quadOut, 2);    //AFP12-06-21

AudioConnection       patchCord13(Q_out_L,    0, modeSelectOutExL,  0);
AudioConnection       patchCord14(Q_out_R,    0, modeSelectOutExR, 0);

AudioConnection       patchCord15(modeSelectOutExL,    0, i2s_quadOut, 0);    //AFP12-06-21
AudioConnection       patchCord16(modeSelectOutExR,  0, i2s_quadOut, 1);    //AFP12-06-21

AudioControlSGTL5000  sgtl5000_1;
AudioControlSGTL5000  sgtl5000_2;

Bounce decreaseBand       = Bounce(BAND_MENUS,       50);
Bounce increaseBand       = Bounce(BAND_PLUS,        50);
Bounce modeSwitch         = Bounce(CHANGE_MODE,      50);
Bounce decreaseMenu       = Bounce(MENU_MINUS,       50);
Bounce frequencyIncrement = Bounce(CHANGE_INCREMENT, 50);
Bounce filterSwitch       = Bounce(CHANGE_FILTER,    50);
Bounce increaseMenu       = Bounce(MENU_PLUS,        50);
Bounce selectExitMenues   = Bounce(CHANGE_MENU2,     50);
Bounce changeNR           = Bounce(CHANGE_NOISE,     50);
Bounce demodSwitch        = Bounce(CHANGE_DEMOD,     50);
Bounce zoomSwitch         = Bounce(CHANGE_ZOOM,      50);
Bounce cursorSwitch       = Bounce(SET_FREQ_CURSOR,  50);

Rotary tuneEncoder = Rotary(TUNE_ENCODER_A,  TUNE_ENCODER_B);           //(16, 17)
Encoder fastTuneEncoder(ENCODER3_ENCODER_A,  ENCODER3_ENCODER_B);       //( 4,  5)
Encoder filterEncoder(FILTER_ENCODER_A,      FILTER_ENCODER_B);         //(14, 15)
Encoder volumeEncoder(VOLUME_ENCODER_A,      VOLUME_ENCODER_B);         //( 2,  3)

Metro ms_500          = Metro(500);                             // Set up a Metro
Metro encoder_check   = Metro(100);                             // Set up a Metro

Si5351 si5351;
#ifndef RA8875_DISPLAY
ILI9488_t3 tft = ILI9488_t3(&SPI, TFT_CS, TFT_DC, TFT_RST);
#else
#define RA8875_CS       TFT_CS
#define RA8875_RESET    TFT_DC                         // any pin or nothing!
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
#endif
SPISettings settingsA(70000000UL, MSBFIRST, SPI_MODE1);

//================== Excite Variables=================
/**********************************************************************************
             The Excite variables are declared and in some cases initialized
             Both the EQ filter parameters and Hilbert Tranformer parameters are created here
 **********************************************************************************/

//uint8_t SampleRate = 192000;
const uint32_t N_B_EX             = 16;
#define IIR_ORDER 8
#define IIR_NUMSTAGES (IIR_ORDER / 2)
int NumExBlocks = 8;

//Setup for EQ filters
float32_t EQ_Band1_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};//declare and zero biquad state variables
float32_t EQ_Band2_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band3_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band4_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band5_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band6_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band7_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band8_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};//declare and zero biquad state variables
float32_t EQ_Band9_state[IIR_NUMSTAGES * 2]  = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band10_state[IIR_NUMSTAGES * 2] = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band11_state[IIR_NUMSTAGES * 2] = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band12_state[IIR_NUMSTAGES * 2] = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band13_state[IIR_NUMSTAGES * 2] = {0, 0, 0, 0, 0, 0, 0, 0};
float32_t EQ_Band14_state[IIR_NUMSTAGES * 2] = {0, 0, 0, 0, 0, 0, 0, 0};

int updateFilter[5];

//EQ band Gain variables
float EQBand1GaindB  = 0;
float EQBand2GaindB  = 0;
float EQBand3GaindB  = 0;
float EQBand4GaindB  = 0;
float EQBand5GaindB  = 0;
float EQBand6GaindB  = 0;
float EQBand7GaindB  = 0;
float EQBand8GaindB  = 0;
float EQBand9GaindB  = 0;
float EQBand10GaindB = 0;
float EQBand11GaindB = 0;
float EQBand12GaindB = 0;
float EQBand13GaindB = 0;
float EQBand14GaindB = 0;

float EQBand1Scale  = 1;
float EQBand2Scale  = 1;
float EQBand3Scale  = 1;
float EQBand4Scale  = 1;
float EQBand5Scale  = 1;
float EQBand6Scale  = 1;
float EQBand7Scale  = 1;
float EQBand8Scale  = 1;
float EQBand9Scale  = 1;
float EQBand10Scale = 1;
float EQBand11Scale = 1;
float EQBand12Scale = 1;
float EQBand13Scale = 1;
float EQBand14Scale = 1;

//EQ filter instances
arm_biquad_cascade_df2T_instance_f32 S1_EXcite = {IIR_NUMSTAGES, EQ_Band1_state, EQ_Band1Coeffs};
arm_biquad_cascade_df2T_instance_f32 S2_EXcite  = {IIR_NUMSTAGES, EQ_Band2_state, EQ_Band2Coeffs};
arm_biquad_cascade_df2T_instance_f32 S3_EXcite  = {IIR_NUMSTAGES, EQ_Band3_state, EQ_Band3Coeffs};
arm_biquad_cascade_df2T_instance_f32 S4_EXcite  = {IIR_NUMSTAGES, EQ_Band4_state, EQ_Band4Coeffs};
arm_biquad_cascade_df2T_instance_f32 S5_EXcite  = {IIR_NUMSTAGES, EQ_Band5_state, EQ_Band5Coeffs};
arm_biquad_cascade_df2T_instance_f32 S6_EXcite  = {IIR_NUMSTAGES, EQ_Band6_state, EQ_Band6Coeffs};
arm_biquad_cascade_df2T_instance_f32 S7_EXcite  = {IIR_NUMSTAGES, EQ_Band7_state, EQ_Band7Coeffs};
arm_biquad_cascade_df2T_instance_f32 S8_EXcite  = {IIR_NUMSTAGES, EQ_Band8_state, EQ_Band8Coeffs};
arm_biquad_cascade_df2T_instance_f32 S9_EXcite  = {IIR_NUMSTAGES, EQ_Band9_state, EQ_Band9Coeffs};
arm_biquad_cascade_df2T_instance_f32 S10_EXcite = {IIR_NUMSTAGES, EQ_Band10_state, EQ_Band10Coeffs};
arm_biquad_cascade_df2T_instance_f32 S11_EXcite = {IIR_NUMSTAGES, EQ_Band11_state, EQ_Band11Coeffs};
arm_biquad_cascade_df2T_instance_f32 S12_EXcite = {IIR_NUMSTAGES, EQ_Band12_state, EQ_Band12Coeffs};
arm_biquad_cascade_df2T_instance_f32 S13_EXcite = {IIR_NUMSTAGES, EQ_Band13_state, EQ_Band13Coeffs};
arm_biquad_cascade_df2T_instance_f32 S14_EXcite = {IIR_NUMSTAGES, EQ_Band14_state, EQ_Band14Coeffs};



//EQBuffers
float32_t float_buffer_L1_EX [256];
float32_t float_buffer_L2_EX [256];
float32_t float_buffer_L3_EX [256];
float32_t float_buffer_L4_EX [256];
float32_t float_buffer_L5_EX [256];
float32_t float_buffer_L6_EX [256];
float32_t float_buffer_L7_EX [256];
float32_t float_buffer_L8_EX [256];
float32_t float_buffer_L9_EX [256];
float32_t float_buffer_L10_EX[256];
float32_t float_buffer_L11_EX[256];
float32_t float_buffer_L12_EX[256];
float32_t float_buffer_L13_EX[256];
float32_t float_buffer_L14_EX[256];

//Hilbert FIR Filters
float32_t FIR_Hilbert_state_L [100 + 256 - 1];
float32_t FIR_Hilbert_state_R [100 + 256 - 1];

arm_fir_instance_f32 FIR_Hilbert_L;
arm_fir_instance_f32 FIR_Hilbert_R;

//Decimation and Interpolation Filters
arm_fir_decimate_instance_f32 FIR_dec1_EX_I;
arm_fir_decimate_instance_f32 FIR_dec1_EX_Q;
arm_fir_decimate_instance_f32 FIR_dec2_EX_I;
arm_fir_decimate_instance_f32 FIR_dec2_EX_Q;

arm_fir_interpolate_instance_f32 FIR_int1_EX_I;
arm_fir_interpolate_instance_f32 FIR_int1_EX_Q;
arm_fir_interpolate_instance_f32 FIR_int2_EX_I;
arm_fir_interpolate_instance_f32 FIR_int2_EX_Q;


float32_t DMAMEM FIR_dec1_EX_I_state[2095];
float32_t DMAMEM FIR_dec1_EX_Q_state[2095];


float32_t DMAMEM FIR_dec2_EX_I_state[535];
float32_t DMAMEM FIR_dec2_EX_Q_state[535];

float32_t DMAMEM FIR_int2_EX_I_state[519];
float32_t DMAMEM FIR_int2_EX_Q_state[519];
float32_t DMAMEM FIR_int1_EX_coeffs[48];
float32_t DMAMEM FIR_int2_EX_coeffs[48];

float32_t DMAMEM FIR_int1_EX_I_state[279];
float32_t DMAMEM FIR_int1_EX_Q_state[279];

float32_t DMAMEM float_buffer_L_EX[2048];
float32_t DMAMEM float_buffer_R_EX[2048];
float32_t DMAMEM float_buffer_LTemp[2048];
float32_t DMAMEM float_buffer_RTemp[2048];

//==================== End Excite Variables================================

//======================================== Global structure declarations ===============================================

//=== CW Filter ===
float32_t CW_Filter_state[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//---------  Code Filter instance -----
arm_biquad_cascade_df2T_instance_f32 S1_CW_Filter = {IIR_CW_NUMSTAGES, CW_Filter_state, CW_Filter_Coeffs};

//=== end CW Filter ===


struct config_t EEPROMData;

const struct SR_Descriptor SR[18] =
{ // x_factor, x_offset and f1 to f4 are NOT USED ANYMORE !!!
  //   SR_n ,            rate,  text,   f1,   f2,   f3,   f4, x_factor = pixels per f1 kHz in spectrum display, x_offset
  {  SAMPLE_RATE_8K,     8000,  "  8k", " 1", " 2", " 3", " 4",  64.00, 11}, // not OK
  {  SAMPLE_RATE_11K,   11025,  " 11k", " 1", " 2", " 3", " 4",  43.10, 17}, // not OK
  {  SAMPLE_RATE_16K,   16000,  " 16k", " 4", " 4", " 8", "12",  64.00,  1}, // OK
  {  SAMPLE_RATE_22K,   22050,  " 22k", " 5", " 5", "10", "15",  58.05,  6}, // OK
  {  SAMPLE_RATE_32K,   32000,  " 32k", " 5", " 5", "10", "15",  40.00, 24}, // OK, one more indicator?
  {  SAMPLE_RATE_44K,   44100,  " 44k", "10", "10", "20", "30",  58.05,  6}, // OK
  {  SAMPLE_RATE_48K,   48000,  " 48k", "10", "10", "20", "30",  53.33, 11}, // OK
  {  SAMPLE_RATE_50K,   50223,  " 50k", "10", "10", "20", "30",  53.33, 11}, // NOT OK
  {  SAMPLE_RATE_88K,   88200,  " 88k", "20", "20", "40", "60",  58.05, 6},  // OK
  {  SAMPLE_RATE_96K,   96000,  " 96k", "20", "20", "40", "60",  53.33, 12}, // OK
  {  SAMPLE_RATE_100K, 100000,  "100k", "20", "20", "40", "60",  53.33, 12}, // NOT OK
  {  SAMPLE_RATE_101K, 100466,  "101k", "20", "20", "40", "60",  53.33, 12}, // NOT OK
  {  SAMPLE_RATE_176K, 176400,  "176k", "40", "40", "80", "120", 58.05,  6}, // OK
  {  SAMPLE_RATE_192K, 192000,  "192k", "40", "40", "80", "120", 53.33, 12}, // OK
  {  SAMPLE_RATE_234K, 234375,  "234k", "40", "40", "80", "120", 53.33, 12}, // NOT OK
  {  SAMPLE_RATE_256K, 256000,  "256k", "40", "40", "80", "120", 53.33, 12}, // NOT OK
  {  SAMPLE_RATE_281K, 281000,  "281k", "40", "40", "80", "120", 53.33, 12}, // NOT OK
  {  SAMPLE_RATE_353K, 352800,  "353k", "40", "40", "80", "120", 53.33, 12} // NOT OK
};

const arm_cfft_instance_f32 *S;
const arm_cfft_instance_f32 *iS;
const arm_cfft_instance_f32 *maskS;
const arm_cfft_instance_f32 *NR_FFT;
const arm_cfft_instance_f32 *NR_iFFT;
const arm_cfft_instance_f32 *spec_FFT;

arm_biquad_casd_df1_inst_f32 biquad_lowpass1;
arm_biquad_casd_df1_inst_f32 IIR_biquad_Zoom_FFT_I;
arm_biquad_casd_df1_inst_f32 IIR_biquad_Zoom_FFT_Q;

arm_fir_decimate_instance_f32 FIR_dec1_I;
arm_fir_decimate_instance_f32 FIR_dec1_Q;
arm_fir_decimate_instance_f32 FIR_dec2_I;
arm_fir_decimate_instance_f32 FIR_dec2_Q;
arm_fir_decimate_instance_f32 Fir_Zoom_FFT_Decimate_I;
arm_fir_decimate_instance_f32 Fir_Zoom_FFT_Decimate_Q;
arm_fir_interpolate_instance_f32 FIR_int1_I;
arm_fir_interpolate_instance_f32 FIR_int1_Q;
arm_fir_interpolate_instance_f32 FIR_int2_I;
arm_fir_interpolate_instance_f32 FIR_int2_Q;
arm_lms_norm_instance_f32 LMS_Norm_instance;
arm_lms_instance_f32      LMS_instance;


struct band bands[NUM_BANDS] = {  //AFP Changed 1-30-21
  //   freq       band low   band hi    mode    LSB/USB    Low    Hi  Gain  type    gain  AGC   pixel
  //                                                     filter filter             correct     offset
  3700000,   3500000,  3800000, "80M", DEMOD_LSB, -100, -4000, 15, HAM_BAND, 6.0, 30,   20,
  7150000,   7000000,  7300000, "40M", DEMOD_LSB, -100, -4000,  0, HAM_BAND, 4.0, 30,   20,
  14200000, 14000000, 14350000, "20M", DEMOD_USB, 4000,   100,  0, HAM_BAND, 7.0, 30,   20,
  18100000, 18068000, 18168000, "17M", DEMOD_USB, 4000,   100,  5, HAM_BAND, 6.0, 30,   20,
  21200000, 21000000, 21450000, "15M", DEMOD_USB, 4000,   100,  5, HAM_BAND, 6.0, 30,   20,
  24920000, 24890000, 24990000, "12M", DEMOD_USB, 4000,   100,  6, HAM_BAND, 6.0, 30,   20,
  28350000, 28000000, 29700000, "10M", DEMOD_USB, 4000,   100,  6, HAM_BAND, 0.0, 30,   20
};
//DEMOD_Desc;
const DEMOD_Descriptor DEMOD[4] =
{
  //   DEMOD_n, name
  {  DEMOD_USB, "(USB) "},
  {  DEMOD_LSB, "(LSB) "},
  {  DEMOD_AM2, "(AM) "},
  {  DEMOD_SAM, "(SAM) "},
};

dispSc displayScale[] =  //r *dbText,dBScale, pixelsPerDB, baseOffset, offsetIncrement
{
  "20 dB/", 10.0,   2,  24, 1,
  "10 dB/", 20.0,   4,  10, 0.5,  //AFP  Changed 03-12-21
  "5 dB/",  40.0,   8, 58, 0.25,
  "2 dB/",  100.0, 20, 120, 0.1,
  "1 dB/",  200.0, 40, 200, 0.05
};

//======================================== Global variables declarations for Quad Oscillator 2 ===============================================
//  AFP 12-11-21
float32_t NCO_FREQ; // AFP 04-16-22
//float32_t NCO_FREQ = AUDIO_SAMPLE_RATE_EXACT /8;  //LSB

double stepFT = 50.0;
double stepFT2 = 50.0;
int stepFTOld = 0;
float32_t NCO_INC; // AFP 04-16-22
double OSC_COS;
double  OSC_SIN ;
double  Osc_Vect_Q = 1.0;
double  Osc_Vect_I = 0.0;
double  Osc_Gain = 0.0;
double  Osc_Q = 0.0;
double  Osc_I = 0.0;
float32_t i_temp = 0.0;
float32_t q_temp = 0.0;

//========================================


//======================================== Global variables declarations ===============================================
//=============================== Any variable initialized to zero is done for documentation ===========================
//=============================== purposes since the compiler does that for globals by default =========================


//================== Global CW Correlation and FFT Variables =================
float32_t corrResult;      //AFP 02-04-22
uint32_t corrResultIndex;  //AFP 02-04-22
float32_t sinBuffer[256];  //AFP 02-04-22
float32_t sinBuffer2[256];
float32_t aveCorrResult;   //AFP 02-04-22
float32_t aveCorrResultR;   //AFP 02-06-22
float32_t aveCorrResultL;   //AFP 02-06-2
float32_t magFFTResults[256];  //AFP 02-04-22
float32_t float_Corr_Buffer[511];   //AFP 02-02-22
float32_t corrResultR;  //AFP 02-02-22
uint32_t corrResultIndexR;  //AFP 02-02-22
float32_t corrResultL;  //AFP 02-02-22
uint32_t corrResultIndexL;  //AFP 02-02-22
float32_t combinedCoeff;
float32_t combinedCoeff2;
float32_t combinedCoeff2Old;
int CWCoeffLevelOld = 0.0;
float CWLevelTimer = 0.0;
float CWLevelTimerOld = 0.0;
float ticMarkTimer = 0.0;
float ticMarkTimerOld = 0.0;

float32_t float_Corr_BufferR[511];   //AFP 02-06-22
float32_t float_Corr_BufferL[511];   //AFP 02-06-22
long tempSigTime = 0;

int audioTemp  = 0;
int audioTempPrevious  = 0;
float sigStart = 0.0;
float sigDuration = 0.0;
float gapStartData = 0.0;
float gapDurationData = 0.0;
int audioValuePrevious = 0;
int CWOnState;
float goertzelMagnitude;

bool gEEPROM_current                      = false;            //mdrhere does the data in EEPROM match the current structure contents
bool NR_gain_smooth_enable                = false;
bool NR_long_tone_reset                   = true;
bool NR_long_tone_enable                  = false;
bool omitOutputFlag                       = false;
bool timeflag                             = 0;

char letterTable[] = {                 // Morse coding: dit = 0, dah = 1
  0b101,              // A                first 1 is the sentinel marker
  0b11000,            // B
  0b11010,            // C
  0b1100,             // D
  0b10,               // E
  0b10010,            // F
  0b1110,             // G
  0b10000,            // H
  0b100,              // I
  0b10111,            // J
  0b1101,             // K
  0b10100,            // L
  0b111,              // M
  0b110,              // N
  0b1111,             // O
  0b10110,            // P
  0b11101,            // Q
  0b1010,             // R
  0b1000,             // S
  0b11,               // T
  0b1001,             // U
  0b10001,            // V
  0b1011,             // W
  0b11001,            // X
  0b11011,            // Y
  0b11100             // Z
};

char numberTable[] = {
  0b111111,           // 0
  0b101111,           // 1
  0b100111,           // 2
  0b100011,           // 3
  0b100001,           // 4
  0b100000,           // 5
  0b110000,           // 6
  0b111000,           // 7
  0b111100,           // 8
  0b111110            // 9
};

char punctuationTable[] = {
  0b01101011,         // exclamation mark 33
  0b01010010,         // double quote 34
  0b10001001,         // dollar sign 36
  0b00101000,         // ampersand 38
  0b01011110,         // apostrophe 39
  0b01011110,         // parentheses (L) 40, 41
  0b01110011,         // comma 44
  0b00100001,         // hyphen 45
  0b01010101,         // period  46
  0b00110010,         // slash 47
  0b01111000,         // colon 58
  0b01101010,         // semi-colon 59
  0b01001100,         // question mark 63
  0b01001101,         // underline 95
  0b01101000,         // paragraph
  0b00010001          // break
};
int ASCIIForPunctuation[] = {33, 34, 36, 39, 41, 44, 45, 46, 47, 58, 59, 63, 95};  // Indexes into code

int kTemp;

long startTime = 0;


char theversion[10];
char decodeBuffer[30];                   // The buffer for holding the decoded characters

char *bigMorseCodeTree  = (char *) "-EISH5--4--V---3--UF--------?-2--ARL---------.--.WP------J---1--TNDB6--.--X/-----KC------Y------MGZ7----,Q------O-8------9--0----";
//012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678
//         10        20        30        40        50        60        70        80        90       100       110       120

const char *tune_text       = "Fast Tune";
const char *topMenus[]      = {"CW Options",  "Display Choices", "Spectrum Set",    "AGC",        "NR Set",
                               "IQ Manual",   "EQ Rec Set",      "EQ Xmt Set",      "Mic Comp",   "Freq Cal",
                               "Noise Floor", "RF Set",          "Audio Post Proc", "VFO Select", "EEPROM",
                               "Unused 1",    "Unused 2"
                              };
const char *zoomOptions[]   = {"1x ", "2x ", "4x ", "8x ", "16x"};


byte currentDashJump      = DECODER_BUFFER_SIZE;
byte currentDecoderIndex  = 0;

int8_t AGCMode                            = 1;
int8_t auto_IQ_correction;
int filterWidthX;                                           // The current filter X.
int filterWidthY;                                           // The current filter Y.
float32_t pixel_per_khz = ((1 << spectrum_zoom) * SPECTRUM_RES * 1000.0 / SR[SampleRate].rate) ; //AFP 03-27-22 Layers
int pos_left            = centerLine - (int)(bands[currentBand].FLoCut / 1000.0 * pixel_per_khz);//AFP 03-27-22 Layers
int centerLine  = (MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2;//AFP 03-27-22 Layers
int filterWidth             = (int)((bands[currentBand].FHiCut - bands[currentBand].FLoCut) / 1000.0 * pixel_per_khz);//AFP 03-27-22 Layers
int h               = SPECTRUM_HEIGHT + 3;
int8_t first_block                        = 1;

int8_t Menu2                              = MENU_F_LO_CUT;
int8_t mesz                               = -1;
int8_t mesz_old                           = 0;
int8_t NB_taps                            = 10;
int8_t NB_impulse_samples                 = 7;
int8_t NR_first_block                     = 1;
int8_t pos_x_date                         = 14;
int8_t pos_y_date                         = 68;
int8_t RF_attenuation                     = 0;
int8_t xmtMode                            = SSB_MODE;      // 0 = SSB, 1 = CW

uint8_t agc_action                        = 0;
uint8_t agc_switch_mode                   = 0;
uint8_t ANR_on                            = 0;
uint8_t ANR_notch                         = 0;
uint8_t ANR_notchOn                       = 0;
uint8_t atan2_approx                      = 1;
uint8_t auto_codec_gain                   = 1;
uint8_t audio_flag                        = 1;
uint8_t autotune_flag                     = 0;
uint8_t autotune_wait                     = 10;
uint8_t bitnumber                         = 16;      // test, how restriction to twelve bit alters sound quality
uint8_t codec_restarts                    = 0;
uint8_t dbm_state                         = 0;
uint8_t dcfParityBit;
uint8_t decay_type                        = 0;
uint8_t digimode                          = 0;
uint8_t digits_old[2][10]                 = { {9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  {9, 9, 9, 9, 9, 9, 9, 9, 9, 9}
};
uint8_t display_dbm                       = DISPLAY_S_METER_DBM;
uint8_t display_S_meter_or_spectrum_state = 0;
uint8_t eeprom_saved                      = 0;
uint8_t eeprom_loaded                     = 0;
uint8_t erase_flag                        = 0;
uint8_t fade_leveler                      = 1;
uint8_t FIR_filter_window                 = 1;
uint8_t flagg                             = 0;
uint8_t freq_flag[2]                      = {0, 0};
uint8_t half_clip                         = 0;
uint8_t hang_enable;
uint8_t hour10_old;
uint8_t hour1_old;
uint8_t IQCalFlag = 0;
uint8_t iFFT_flip                         = 0;
uint8_t IQ_state                          = 1;
uint8_t IQ_RecCalFlag = 0; //AFP 04-17-22
uint8_t LastSampleRate                    = SAMPLE_RATE_192K;
uint8_t minute10_old;
uint8_t minute1_old;
uint8_t NB_on = 0;
uint8_t NB_test = 0;
uint8_t notchButtonState = 0;
uint8_t notchIndex = 0;
uint8_t notches_on[2]                     = {0, 0};
uint8_t NR_first_time                     = 1;
uint8_t NR_Kim;
uint8_t NR_LMS                            = 0;
uint8_t NR_Spect;
uint8_t NR_use_X                          = 0;
uint8_t NR_VAD_enable                     = 1;
uint8_t precision_flag                    = 0;
uint8_t quarter_clip                      = 0;
uint8_t SampleRate                        = SAMPLE_RATE_192K;
uint8_t save_energy;
uint8_t sch                               = 0;
uint8_t second10_old;
uint8_t second1_old;
uint8_t show_spectrum_flag                = 1;
uint8_t spectrum_mov_average              = 0;
uint8_t state                             = 0;
uint8_t twinpeaks_tested                  = 2;                      // initial value --> 2 !!
uint8_t wait_flag;
uint8_t WDSP_SAM                          = 1;
uint8_t which_menu                        = 1;
uint8_t write_analog_gain                 = 0;
uint8_t zoom_display                      = 1;

const uint8_t NR_L_frames                 = 3;
const uint8_t NR_N_frames                 = 15;

uint16_t base_y = spectrum_y + spectrum_WF_height + 4;

int16_t activeVFO;
int16_t currentMode;
int16_t displayMode                       = BOTH_DISPLAYS;  // Show Spectrum and waterfall
int16_t DMAMEM pixelnew[SPECTRUM_RES];
int16_t DMAMEM pixelold[SPECTRUM_RES];
int16_t DMAMEM pixelnew2[MAX_WATERFALL_WIDTH + 1];//AFP
int16_t DMAMEM pixelold2[MAX_WATERFALL_WIDTH];
int16_t EqualizerRecValues[5];
int16_t EqualizerXmtValues[5];
int16_t notch_L[2]                        = {156, 180};
int16_t  fineEncoderRead;
int16_t notch_R[2]                        = {166, 190};
int16_t notch_pixel_L[2]                  = {1, 2};
int16_t notch_pixel_R[2]                  = {2, 3};
int16_t offsetPixels;
int16_t pos_x_dbm                         = pos_x_smeter + 170;
int16_t pos_y_dbm                         = pos_y_smeter - 10;       // Was 7
int16_t pos_y_db;
int16_t pos_y_frequency                   = 48;
int16_t pos_x_time                        = 390;         // 14;
int16_t pos_y_time                        = 5; //114;
int16_t s_w                               = 10;
int16_t *sp_L;
int16_t *sp_R;
int16_t spectrum_brightness               = 255;
int16_t spectrum_height                   = 96;
int16_t spectrum_pos_centre_f             = 64 * xExpand;               //AFP
//int16_t h                                 = spectrum_height + 3;        //AFP
int16_t spectrum_WF_height                = 96;
int16_t spectrum_x                        = SPECTRUM_LEFT_X;
int16_t spectrum_y                        = SPECTRUM_TOP_Y;

uint16_t adcMaxLevel, dacMaxLevel;
uint16_t autotune_counter;
uint16_t barGraphUpdate                   = 0;
uint16_t currentScale                     = 1;        // 20 dB/division

//===== New histogram stuff  AFO 02-20-22 ===
int endDitFlag = 0;
int topDitIndex;
int topDitIndexOld;
int endGapFlag = 0;
int topGapIndex;
int topGapIndexOld;

int32_t gapHistogram[HISTOGRAM_ELEMENTS];
int32_t signalHistogram[HISTOGRAM_ELEMENTS];

uint32_t histMaxIndexDitOld = 80;                   // Defaults for 15wpm
uint32_t histMaxIndexDahOld = 200;

uint32_t histMaxDit;
uint32_t histMaxDah;
uint32_t histMaxIndexDit;
uint32_t histMaxIndexDah;

int atomGapLength;
int atomGapLength2;
int charGapLength;
int charGapLength2;
long valRef1;
long valRef2;
long gapRef1;
int valFlag = 0;
long signalStartOld = 0;
int valCounter;
long aveDitLength = 80;
long aveDahLength = 200;
float thresholdGeometricMean = 140.0;     // This changes as decoder runs
float thresholdArithmeticMean;
float aveAtomGapLength = 40;
float thresholdGapGeometricMean;
float thresholdGapArithmeticMean;
long CWFreqShift;
// ============ end new stuff =======


uint16_t notches_BW[2]                    = {4, 4}; // no. of bins --> notch BW = no. of bins * bin_BW
uint16_t SAM_display_count;
uint16_t temp_check_frequency;      /*!< The temperature measure frequency.*/
uint16_t uAfter;
uint16_t uB4;
uint16_t xx;

int16_t y_old, y_new, y1_new, y1_old, y_old2;//A
int16_t y1_old_minus = 0;
int16_t y1_new_minus = 0;

const float32_t DF1                       = 4.0;           // decimation factor
const float32_t DF2                       = 2.0;           // decimation factor
const float32_t DF                        = DF1 * DF2;     // decimation factor
const float32_t n_samplerate              = 176.0;         // samplerate before decimation

const uint32_t N_B                        = FFT_LENGTH / 2 / BUFFER_SIZE * (uint32_t)DF;
const uint32_t N_DEC_B                    = N_B / (uint32_t)DF;
const uint32_t NR_add_counter             = 128;

const float32_t n_att                     = 90.0;         // need here for later def's
const float32_t n_desired_BW              = 9.0;  // desired max BW of the filters
const float32_t n_fpass1                  = n_desired_BW / n_samplerate;
const float32_t n_fpass2                  = n_desired_BW / (n_samplerate / DF1);
const float32_t n_fstop1                  = ( (n_samplerate / DF1) - n_desired_BW) / n_samplerate;
const float32_t n_fstop2                  = ((n_samplerate / (DF1 * DF2)) - n_desired_BW) / (n_samplerate / DF1);

const uint32_t IIR_biquad_Zoom_FFT_N_stages = 4;
const uint32_t N_stages_biquad_lowpass1   = 1;
const uint16_t n_dec1_taps                = (1 + (uint16_t) (n_att / (22.0 * (n_fstop1 - n_fpass1))));
const uint16_t n_dec2_taps                = (1 + (uint16_t) (n_att / (22.0 * (n_fstop2 - n_fpass2))));

int resultOldFactor;
float incrFactor;
int audio_volumeOld2 = 50;
int mute = 0;
int (*functionPtr[])()                    = {&CWOptions, &ButtonDisplayOptions, &SpectrumOptions, &AGCOptions,
                                             &NROptions, &IQOptions, &EqualizerRecOptions, &EqualizerXmtOptions,
                                             &MicOptions, &FrequencyOptions, &ButtonSetNoiseFloor, &RFOptions,
                                             &PostProcessorAudio, &VFOSelect, &EEPROMOptions
                                            };
int exciteOn = 0;
int agc_decay                             = 100;
int agc_slope                             = 100;
int agc_thresh                            = 30;
int ANR_buff_size                         = FFT_length / 2.0;
int ANR_delay                             = 16;
int ANR_dline_size                        = ANR_DLINE_SIZE;
int ANR_in_idx                            = 0;
int ANR_mask                              = ANR_dline_size - 1;
int ANR_position                          = 0;
int ANR_taps                              = 64;
int attack_buffsize;
int audio_volume                          = 50;
int audioYPixel[1024];
int audioPostProcessorCells[AUDIO_POST_PROCESSOR_BANDS];

int bandswitchPins[]                      = {30,   // 80M
                                             31,   // 40M
                                             28,   // 20M
                                             0,   // 17M
                                             29,   // 15M
                                             0,   // 12M
                                             0    // 10M
                                            };
int button9State;
int buttonRead = 0;
int cwSidetone                            = 700;
int currentBand                           = BAND_40M;
int dahLength;
int dcfCount;
int dcfLevel;
int dcfSilenceTimer;
int dcfTheSecond;
int dcfPulseTime;
int decoderFlag;
int demodIndex                            = 0;//AFP 2-10-21
int ditLength;
int EEPROMChoice;
int encoderStepOld;
int equalizerRecChoice;
int equalizerXmtChoice;
int fastTuneActive;
int filterLoPositionMarkerOld;// AFP 03-27-22 Layers
int filterHiPositionMarkerOld;// AFP 03-27-22 Layers
int FLoCutOld;
int FHiCutOld;
int freqCalibration = -1000;
int freqIncrement                         = DEFAULTFREQINCREMENT;
int gapAtom;                                  // Space between atoms
int gapChar;                                  // Space between characters
int hang_counter                          = 0;
int helpmin;
int helphour;
int helpday;
int helpmonth;
int helpyear;
int helpsec;
int idx, idpk;
int lidx, hidx;
int kDisplay = 0; //AFP
int IQChoice;
int LMS_nr_strength;
int LP_F_help                             = 3500;
int micChoice;
int minPinRead = 1024;
int oldCursorPosition                     = 256;
int operatingMode;
int NR_Index = 0;
int n_L;
int n_R;
int n_tau;
int NBChoice;
int nrOptionSelect;
int newCursorPosition =                     256;
int NR_Choice                             = 0;
int NR_Filter_Value                       = -1;
int NR_KIM_K_int                          = 1000;
int NR_VAD_delay                          =  0;
int NR_VAD_duration                       = 0 ;
int old_demod_mode                        = -99;
int oldnotchF                             = 10000;
int out_index                             = -1;
int paddleDah                             = KEYER_DAH_INPUT_RING;
int paddleDit                             = KEYER_DIT_INPUT_TIP;
int pmode                                 = 1;
int pos_centre_f                          = 64;
int pos_x_frequency                       = 12;
int pos_y_smeter                          = (spectrum_y - 12);
int SAM_AM_Choice;
int spectrumNoiseFloor                    = SPECTRUM_NOISE_FLOOR;
int secondaryMenuChoiceMade;
int smeterLength;

int SSB_AUTOTUNE_counter;
int switchFilterSideband                  = 0;

int switchThreshholds[]                   = {888, 827, 772, 730,
                                             673, 603, 552, 505,
                                             448, 387, 318, 268,
                                             12,  202, 150, 73
                                            };
int termCursorXpos                        = 0;
int x2 = 0;                                                             //AFP
int zeta_help                             = 65;
int zoom_sample_ptr                       = 0;
int zoomIndex                             = 0; //AFP 2-10-21
int tuneIndex                             = DEFAULTFREQINDEX;           //AFP 2-10-21
int wtf;
int updateDisplayFlag = 1;

const int BW_indicator_y                  = SPECTRUM_TOP_Y + SPECTRUM_HEIGHT + 2;
const int DEC2STATESIZE                   = n_dec2_taps + (BUFFER_SIZE * N_B / (uint32_t)DF1) - 1;
const int INT1_STATE_SIZE                 = 24 + BUFFER_SIZE * N_B / (uint32_t)DF - 1;
const int INT2_STATE_SIZE                 = 8 + BUFFER_SIZE * N_B / (uint32_t)DF1 - 1;
const int myInput                         = AUDIO_INPUT_LINEIN;
//const int myInput                         = AUDIO_INPUT_MIC;
const int pos_x_smeter                    = 11;
const int waterfallBottom                = spectrum_y + spectrum_height + 4;
const int waterfallTop                   = spectrum_y + spectrum_height + 4;

unsigned framesize;
unsigned nbits;
unsigned ring_buffsize                    = RB_SIZE;
unsigned tcr5;
unsigned tcr2div;

int32_t FFT_shift                         = 2048;
int32_t IFFreq                            = SR[SampleRate].rate / 4;      // IF (intermediate) frequency
int32_t IF_FREQ1 = 0;
int32_t mainMenuIndex                     = START_MENU;                   // Done so we show menu[0] at startup
int32_t secondaryMenuIndex                = -1;                           // -1 means haven't determined secondary menu
int32_t subMenuMaxOptions;                                                // holds the number of submenu options
int32_t subMenuIndex                      = currentBand;
int32_t O_iiSum19;
int32_t O_integrateCount19;
int32_t spectrum_zoom                     = SPECTRUM_ZOOM_2;

uint32_t N_BLOCKS                         = N_B;
uint32_t BUF_N_DF                         = BUFFER_SIZE * N_BLOCKS / (uint32_t)DF;
uint32_t highAlarmTemp;  /*!< The high alarm temperature.*/
uint32_t in_index;
uint32_t lowAlarmTemp;   /*!< The low alarm */
uint32_t MDR;
uint32_t n_para                           = 512;
uint32_t NR_X_pointer                     = 0;
uint32_t NR_E_pointer                     = 0;
uint32_t IQ_counter                       = 0;
uint32_t m_NumTaps                        = (FFT_LENGTH / 2) + 1;
uint32_t panicAlarmTemp; /*!< The panic alarm temperature.*/
uint32_t roomCount;   /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at the hot temperature.*/
uint32_t s_roomC_hotC; /*!< The value of s_roomCount minus s_hotCount.*/
uint32_t s_hotTemp;    /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at room temperature .*/
uint32_t s_hotCount;   /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at the hot temperature.*/
uint32_t twinpeaks_counter                = 0;

long jackStart, jackEnd;
long averageDit;
long averageDah;
long calibration_constant                 = 10000L;

long currentFreq;
long centerFreq                           = 7150000L;
long CWRecFreq;     //  = TxRxFreq +/- 700Hz
long currentFreqA                         = 7150000;  //Initial VFOA center freq
long currentFreqB                         = 7030000;  //Initial VFOB center freq
long currentWPM                           = 15L;
long frequencyCorrection;
long lastWPM;
long int n_clear;
long notchPosOld;
long notchFreq = 1000;
long notchCenterBin;
long recClockFreq;                  //  = TxRxFreq+IFFreq  IFFreq from FreqShift1()=48KHz
long signalElapsedTime;
long spaceSpan;
long signalStart;
long signalEnd;                     // Start-end of dit or dah
long spaceStart;
long spaceEnd;
long spaceElapsedTime;
long TxRxFreq;                      // = centerFreq+NCOFreq  NCOFreq from FreqShift2()

long gapEnd, gapLength, gapStart;                            // Time for noise measures
long ditTime = 80L, dahTime = 240L;                          // Assume 15wpm to start

ulong samp_ptr;

uint64_t output12khz;

//unsigned long long calibration_factor     = 1000000000ULL;
unsigned long long calibration_factor     = 999845000ULL;   // AFP 12-30-21
unsigned long long hilfsf                 = 1000000000ULL;
unsigned long long hilfsfEx               = 1000000000ULL;
float dcfRefLevel;
float CPU_temperature                     = 0.0;

float DD4WH_RF_gain                       = 6.0;
float help;
float s_hotT_ROOM;     /*!< The value of s_hotTemp minus room temperature(25¡æ).*/
float lastII                               = 0;
float lastQQ                               = 0;
float RXbit                                = 0;
float bitSampleTimer                       = 0;
float Tsample                              = 1.0 / 12000.0;

float32_t a[3 * SAM_PLL_HILBERT_STAGES + 3];
float32_t abs_ring[RB_SIZE];
float32_t abs_out_sample;
float32_t ai, bi, aq, bq;
float32_t ai_ps, bi_ps, aq_ps, bq_ps;
float32_t ANR_d[ANR_DLINE_SIZE];
float32_t ANR_den_mult                    = 6.25e-10;
float32_t ANR_gamma                       = 0.1;
float32_t ANR_lidx                        = 120.0;
float32_t ANR_lidx_min                    = 120.0;
float32_t ANR_lidx_max                    = 200.0;
float32_t ANR_lincr                       = 1.0;
float32_t ANR_ldecr                       = 3.0;
float32_t ANR_ngamma                      = 0.001;
float32_t ANR_two_mu                      = 0.0001;
float32_t ANR_w[ANR_DLINE_SIZE];
float32_t attack_mult;
float32_t audio;
float32_t audiotmp                        = 0.0f;
float32_t audiou;
float32_t audioSpectBuffer[1024];
float32_t b[3 * SAM_PLL_HILBERT_STAGES + 3];
float32_t bass                            = 0.0;
float32_t farnsworthValue;
float32_t micCompression                  = 0.7;
float32_t midbass                         = 0.0;
float32_t mid                             = 0.0;
float32_t midtreble                       = 0.0;
float32_t treble                          = 0.0;
float32_t bin_BW                          = 1.0 / (DF * FFT_length) * SR[SampleRate].rate;
float32_t bin                             = 2000.0 / bin_BW;
float32_t biquad_lowpass1_state[N_stages_biquad_lowpass1 * 4];
float32_t biquad_lowpass1_coeffs[5 * N_stages_biquad_lowpass1] = {0, 0, 0, 0, 0};
float32_t DMAMEM buffer_spec_FFT[1024] __attribute__ ((aligned (4)));
float32_t c[3 * SAM_PLL_HILBERT_STAGES + 3];     // Filter c variables
float32_t c0[SAM_PLL_HILBERT_STAGES];
float32_t c1[SAM_PLL_HILBERT_STAGES];
float32_t coefficient_set[5]              = {0, 0, 0, 0, 0};
float32_t corr[2];
float32_t Cos                             = 0.0;
float32_t cursorIncrementFraction;
//float32_t CPU_temperature                 = 0.0;
float32_t d[3 * SAM_PLL_HILBERT_STAGES + 3];
float32_t dc                              = 0.0;
float32_t dc_insert                       = 0.0;
float32_t dcu                             = 0.0;
float32_t dc_insertu;
float32_t dbm                             = -145.0;;
float32_t dbm_calibration                 = 22.0;
float32_t dbm_old                         = -145.0;;
float32_t dbmhz                           = -145.0;
float32_t decay_mult;
float32_t display_offset;
float32_t DMAMEM FFT_buffer[FFT_LENGTH * 2] __attribute__ ((aligned (4)));
float32_t DMAMEM FFT_spec[1024];
float32_t DMAMEM FFT_spec_old[1024];
float32_t dsI;
float32_t dsQ;
float32_t fast_backaverage;
float32_t fast_backmult;
float32_t fast_decay_mult;
float32_t det = 0.0;
float32_t del_out = 0.0;
float32_t fil_out = 0.0;
float32_t DMAMEM FIR_Coef_I[(FFT_LENGTH / 2) + 1];
float32_t DMAMEM FIR_Coef_Q[(FFT_LENGTH / 2) + 1];
float32_t DMAMEM FIR_dec1_I_state[n_dec1_taps + (uint16_t) BUFFER_SIZE * (uint32_t) N_B - 1];
float32_t DMAMEM FIR_dec2_I_state[DEC2STATESIZE];
float32_t DMAMEM FIR_dec2_coeffs[n_dec2_taps];
float32_t DMAMEM FIR_dec2_Q_state[DEC2STATESIZE];
float32_t DMAMEM FIR_int2_I_state[INT2_STATE_SIZE];
float32_t DMAMEM FIR_int2_Q_state[INT2_STATE_SIZE];
float32_t DMAMEM FIR_int1_coeffs[48];
float32_t DMAMEM FIR_int2_coeffs[32];
float32_t DMAMEM FIR_dec1_Q_state[n_dec1_taps + (uint16_t)BUFFER_SIZE * (uint16_t)N_B - 1];
float32_t DMAMEM FIR_dec1_coeffs[n_dec1_taps];
float32_t DMAMEM FIR_filter_mask[FFT_LENGTH * 2] __attribute__ ((aligned (4)));
float32_t DMAMEM FIR_int1_I_state[INT1_STATE_SIZE];
float32_t DMAMEM FIR_int1_Q_state[INT1_STATE_SIZE];
float32_t DMAMEM Fir_Zoom_FFT_Decimate_I_state[4 + BUFFER_SIZE * N_B - 1];
float32_t DMAMEM Fir_Zoom_FFT_Decimate_Q_state[4 + BUFFER_SIZE * N_B - 1];
float32_t DMAMEM Fir_Zoom_FFT_Decimate_coeffs[4];
float32_t fixed_gain                            = 1.0;
float32_t DMAMEM float_buffer_L[BUFFER_SIZE * N_B];
float32_t DMAMEM float_buffer_R[BUFFER_SIZE * N_B];
float32_t DMAMEM float_buffer_L2[BUFFER_SIZE * N_B];
float32_t DMAMEM float_buffer_R2[BUFFER_SIZE * N_B];
float32_t float_buffer_L_3[BUFFER_SIZE * N_B];
float32_t float_buffer_R_3[BUFFER_SIZE * N_B];
float32_t g1                                    = 1.0 - exp(-2.0 * omegaN * zeta * DF / SR[SampleRate].rate);
float32_t g2                                    = - g1 + 2.0 * (1 - exp(- omegaN * zeta * DF / SR[SampleRate].rate) * cosf(omegaN * DF / SR[SampleRate].rate * sqrtf(1.0 - zeta * zeta)));
float32_t hang_backaverage                      = 0.0;
float32_t hang_backmult;
float32_t hang_decay_mult;
float32_t hang_thresh;
float32_t hang_level;
float32_t hangtime;
float32_t hh1                                   = 0.0;
float32_t hh2                                   = 0.0;
//float32_t DMAMEM iFFT_buffer[FFT_LENGTH * 2] __attribute__ ((aligned (4)));
float32_t DMAMEM iFFT_buffer[FFT_LENGTH * 2 + 1];
float32_t I_old                                 = 0.2;
float32_t I_sum;
float32_t IIR_biquad_Zoom_FFT_I_state[IIR_biquad_Zoom_FFT_N_stages * 4];
float32_t IIR_biquad_Zoom_FFT_Q_state[IIR_biquad_Zoom_FFT_N_stages * 4];
float32_t inv_max_input;
float32_t inv_out_target;
float32_t IQ_amplitude_correction_factor        = 1.0;
float32_t IQ_phase_correction_factor            = 0.0;
float32_t IQ_Xamplitude_correction_factor        = 1.0;
float32_t IQ_Xphase_correction_factor            = 0.0;
float32_t IQ_sum                                = 0.0;
float32_t K_dirty                               = 0.868;
float32_t K_est                                 = 1.0;
float32_t K_est_old                             = 0.0;
float32_t K_est_mult                            = 1.0 / K_est;
float32_t last_dc_level                         = 0.0f;
float32_t DMAMEM last_sample_buffer_L[BUFFER_SIZE * N_DEC_B];
float32_t DMAMEM last_sample_buffer_R[BUFFER_SIZE * N_DEC_B];
float32_t DMAMEM L_BufferOffset[BUFFER_SIZE * N_B];
float32_t LMS_errsig1[256 + 10];
float32_t LMS_NormCoeff_f32[MAX_LMS_TAPS + MAX_LMS_DELAY];
float32_t LMS_nr_delay[512 + MAX_LMS_DELAY];
float32_t LMS_StateF32[MAX_LMS_TAPS + MAX_LMS_DELAY];
float32_t LP_Astop                              = 90.0;
float32_t LP_Fpass                              = 3500.0;
float32_t LP_Fstop                              = 3600.0;
float32_t LPF_spectrum                          = 0.82;
float32_t M_c1                                  = 0.0;
float32_t M_c2                                  = 0.0;
float32_t m_AttackAlpha                         = 0.03;
float32_t m_AttackAvedbm                        = -73.0;
float32_t m_DecayAvedbm                         = -73.0;;
float32_t m_DecayAlpha                          = 0.01;
float32_t m_AverageMagdbm                       = -73.0;;
float32_t m_AttackAvedbmhz                      = -103.0;
float32_t m_DecayAvedbmhz                       = -103.0;
float32_t m_AverageMagdbmhz                     = -103.0;
float32_t max_gain;
float32_t max_input                             = -0.1;
float32_t min_volts;
float32_t mtauI                                 = exp(- DF / (SR[SampleRate].rate * tauI));
float32_t mtauR                                 = exp(- DF / (SR[SampleRate].rate * tauR));
float32_t noiseThreshhold;
float32_t notches[10]                           = {500.0, 1000.0, 1500.0, 2000.0, 2500.0, 3000.0, 3500.0, 4000.0, 4500.0, 5000.0};
float32_t DMAMEM NR_FFT_buffer[512] __attribute__ ((aligned (4)));
float32_t NR_sum                                = 0 ;
float32_t NR_PSI                                = 3.0;
float32_t NR_KIM_K                              = 1.0;
float32_t NR_alpha                              = 0.95;
float32_t NR_onemalpha                          = (1.0 - NR_alpha);
float32_t NR_beta                               = 0.85;
float32_t NR_onemtwobeta                        = (1.0 - (2.0 * NR_beta));
float32_t NR_onembeta                           = 1.0 - NR_beta;
float32_t NR_G_bin_m_1;
float32_t NR_G_bin_p_1;
float32_t NR_T;
float32_t DMAMEM NR_output_audio_buffer[NR_FFT_L];
float32_t DMAMEM NR_last_iFFT_result[NR_FFT_L / 2];
float32_t DMAMEM NR_last_sample_buffer_L[NR_FFT_L / 2];
float32_t DMAMEM NR_last_sample_buffer_R[NR_FFT_L / 2];
float32_t DMAMEM NR_X[NR_FFT_L / 2][3];
float32_t DMAMEM NR_E[NR_FFT_L / 2][15];
float32_t DMAMEM NR_M[NR_FFT_L / 2];
float32_t DMAMEM NR_Nest[NR_FFT_L / 2][2]; //
float32_t NR_vk;
float32_t DMAMEM NR_lambda[NR_FFT_L / 2];
float32_t DMAMEM NR_Gts[NR_FFT_L / 2][2];
float32_t DMAMEM NR_G[NR_FFT_L / 2];
float32_t DMAMEM NR_SNR_prio[NR_FFT_L / 2];
float32_t DMAMEM NR_SNR_post[NR_FFT_L / 2];
float32_t NR_SNR_post_pos;
float32_t DMAMEM NR_Hk_old[NR_FFT_L / 2];
float32_t NR_VAD                          = 0.0;
float32_t NR_VAD_thresh                   = 6.0;
float32_t DMAMEM NR_long_tone[NR_FFT_L / 2][2];
float32_t DMAMEM NR_long_tone_gain[NR_FFT_L / 2];
float32_t NR_long_tone_alpha              = 0.9999;
float32_t NR_long_tone_thresh             = 12000;
float32_t NR_gain_smooth_alpha            = 0.25;
float32_t NR_temp_sum = 0.0;
float32_t NB_thresh                       = 2.5;
float32_t offsetDisplayDB                 = 10.0;
float32_t omega2 = 0.0;
float32_t omegaN                          = 200.0;
float32_t omega_max                       = TPI * pll_fmax * DF / SR[SampleRate].rate;
float32_t omega_min                       = TPI * - pll_fmax * DF / SR[SampleRate].rate;
float32_t onem_mtauI                      = 1.0 - mtauI;
float32_t onem_mtauR                      = 1.0 - mtauR;
float32_t onemfast_backmult;
float32_t onemhang_backmult;
float32_t out_sample[2];
float32_t out_targ;
float32_t out_target;
float32_t P_dirty                         = 1.0;
float32_t P_est;
float32_t P_est_old;
float32_t P_est_mult                      = 1.0 / (sqrtf(1.0 - P_est * P_est));
float32_t pll_fmax                        = 4000.0;
float32_t phaseLO                         = 0.0;
float32_t phzerror                        = 0.0;
float32_t pop_ratio;
float32_t Q_old                           = 0.2;
float32_t Q_sum;
float32_t DMAMEM R_BufferOffset[BUFFER_SIZE * N_B];
float32_t ring[RB_SIZE * 2];
float32_t ring_max                        = 0.0;
float32_t SAM_carrier                     = 0.0;
float32_t SAM_lowpass                     = 2700.0;
float32_t SAM_carrier_freq_offset         = 0.0;
float32_t Sin                             = 0.0;
float32_t sample_meanL                    = 0.0;
float32_t sample_meanR                    = 0.0;
float32_t sample_meanLNew                 = 0.0;
float32_t sample_meanRNew                 = 0.0;
float32_t save_volts                      = 0.0;
float32_t slope_constant;

float32_t spectrum_display_scale          = 20.0;          // 30.0
float32_t stereo_factor                   = 100.0;
float32_t tau_attack;
float32_t tau_decay;
float32_t tau_fast_backaverage            = 0.0;
float32_t tau_fast_decay;
float32_t tau_hang_backmult;
float32_t tau_hang_decay;
float32_t tauI                            = 1.4;
float32_t tauR                            = 0.02;
float32_t teta1                           = 0.0;
float32_t teta2                           = 0.0;
float32_t teta3                           = 0.0;
float32_t teta1_old                       = 0.0;
float32_t teta2_old                       = 0.0;
float32_t teta3_old                       = 0.0;
float32_t tmp;
float32_t var_gain;
float32_t volts = 0.0;
float32_t w;
float32_t wold = 0.0f;
float32_t zeta                            = (float32_t)zeta_help / 100.0;

float angl;
float bitSamplePeriod                     = 1.0 / 500.0;
float bsq, usq;
float cf1, cf2;
float dcfMean;
float dcfSum;
float lolim, hilim;
float partr, parti;
float pi                                  = 3.14159265358979;
float tau;
float temp;
float xExpand = 1.5; //
float x;

const float32_t sqrtHann[256] =
{
  0, 0.01231966, 0.024637449, 0.036951499, 0.049259941, 0.061560906,
  0.073852527, 0.086132939, 0.098400278, 0.110652682, 0.122888291, 0.135105247, 0.147301698,
  0.159475791, 0.171625679, 0.183749518, 0.195845467, 0.207911691, 0.219946358, 0.231947641, 0.24391372,
  0.255842778, 0.267733003, 0.279582593, 0.291389747, 0.303152674, 0.314869589, 0.326538713, 0.338158275,
  0.349726511, 0.361241666, 0.372701992, 0.384105749, 0.395451207, 0.406736643, 0.417960345, 0.429120609,
  0.440215741, 0.451244057, 0.462203884, 0.473093557, 0.483911424, 0.494655843, 0.505325184, 0.515917826,
  0.526432163, 0.536866598, 0.547219547, 0.557489439, 0.567674716, 0.577773831, 0.587785252, 0.597707459,
  0.607538946, 0.617278221, 0.626923806, 0.636474236, 0.645928062, 0.65528385,  0.664540179, 0.673695644,
  0.682748855, 0.691698439, 0.700543038, 0.709281308, 0.717911923, 0.726433574, 0.734844967, 0.743144825,
  0.75133189,  0.759404917, 0.767362681, 0.775203976, 0.78292761,  0.790532412, 0.798017227, 0.805380919,
  0.812622371, 0.819740483, 0.826734175, 0.833602385, 0.840344072, 0.846958211, 0.853443799, 0.859799851,
  0.866025404, 0.872119511, 0.878081248, 0.88390971,  0.889604013, 0.895163291, 0.900586702, 0.905873422,
  0.911022649, 0.916033601, 0.920905518, 0.92563766,  0.930229309, 0.934679767, 0.938988361, 0.943154434,
  0.947177357, 0.951056516, 0.954791325, 0.958381215, 0.961825643, 0.965124085, 0.968276041, 0.971281032,
  0.974138602, 0.976848318, 0.979409768, 0.981822563, 0.984086337, 0.986200747, 0.988165472, 0.989980213,
  0.991644696, 0.993158666, 0.994521895, 0.995734176, 0.996795325, 0.99770518,  0.998463604, 0.999070481,
  0.99952572,  0.99982925,  0.999981027, 0.999981027, 0.99982925,  0.99952572,  0.999070481, 0.998463604,
  0.99770518,  0.996795325, 0.995734176, 0.994521895, 0.993158666, 0.991644696, 0.989980213, 0.988165472,
  0.986200747, 0.984086337, 0.981822563, 0.979409768, 0.976848318, 0.974138602, 0.971281032, 0.968276041,
  0.965124085, 0.961825643, 0.958381215, 0.954791325, 0.951056516, 0.947177357, 0.943154434, 0.938988361,
  0.934679767, 0.930229309, 0.92563766,  0.920905518, 0.916033601, 0.911022649, 0.905873422, 0.900586702,
  0.895163291, 0.889604013, 0.88390971,  0.878081248, 0.872119511, 0.866025404, 0.859799851, 0.853443799,
  0.846958211, 0.840344072, 0.833602385, 0.826734175, 0.819740483, 0.812622371, 0.805380919, 0.798017227,
  0.790532412, 0.78292761,  0.775203976, 0.767362681, 0.759404917, 0.75133189, 0.743144825, 0.734844967,
  0.726433574, 0.717911923, 0.709281308, 0.700543038, 0.691698439, 0.682748855, 0.673695644, 0.664540179,
  0.65528385,  0.645928062, 0.636474236, 0.626923806, 0.617278221, 0.607538946, 0.597707459, 0.587785252,
  0.577773831, 0.567674716, 0.557489439, 0.547219547, 0.536866598, 0.526432163, 0.515917826, 0.505325184,
  0.494655843, 0.483911424, 0.473093557, 0.462203884, 0.451244057, 0.440215741, 0.429120609, 0.417960345,
  0.406736643, 0.395451207, 0.384105749, 0.372701992, 0.361241666, 0.349726511, 0.338158275, 0.326538713,
  0.314869589, 0.303152674, 0.291389747, 0.279582593, 0.267733003, 0.255842778, 0.24391372,  0.231947641,
  0.219946358, 0.207911691, 0.195845467, 0.183749518, 0.171625679, 0.159475791, 0.147301698, 0.135105247,
  0.122888291, 0.110652682, 0.098400278, 0.086132939, 0.073852527, 0.061560906, 0.049259941, 0.036951499,
  0.024637449, 0.01231966, 0
};

double elapsed_micros_idx_t               = 0;
double elapsed_micros_mean;
double elapsed_micros_sum;

/*****
  Purpose: To read the local time

  Parameter list:
    void

  Return value:
    time_t                a time data point
*****/
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

#pragma GCC diagnostic ignored "-Wunused-variable"

PROGMEM



//==================================================================================

/*****
  Purpose: To set the codec gain

  Parameter list:
    void

  Return value:
    void

*****/
void Codec_gain()
{

  static uint32_t timer = 0;
  timer ++;
  if (timer > 10000) timer = 10000;
  if (half_clip == 1)      // did clipping almost occur?
  {
    if (timer >= 20)   // 100  // has enough time passed since the last gain decrease?
    {
      if (bands[currentBand].RFgain != 0)       // yes - is this NOT zero?
      {
        bands[currentBand].RFgain -= 1;    // decrease gain one step, 1.5dB
        if (bands[currentBand].RFgain < 0)
        {
          bands[currentBand].RFgain = 0;
        }
        timer = 0;  // reset the adjustment timer
        AudioNoInterrupts();
        AudioInterrupts();
        if (Menu2 == MENU_RF_GAIN) {
          //         ShowMenu(1);
        }
      }
    }
  }
  else if (quarter_clip == 0)     // no clipping occurred
  {
    if (timer >= 50)    // 500   // has it been long enough since the last increase?
    {
      bands[currentBand].RFgain += 1;    // increase gain by one step, 1.5dB
      timer = 0;  // reset the timer to prevent this from executing too often
      if (bands[currentBand].RFgain > 15)
      {
        bands[currentBand].RFgain = 15;
      }
      AudioNoInterrupts();
      AudioInterrupts();
      if (Menu2 == MENU_RF_GAIN) {
        //        ShowMenu(1);
      }
    }
  }
  half_clip = 0;      // clear "half clip" indicator that tells us that we should decrease gain
  quarter_clip = 0;   // clear indicator that, if not triggered, indicates that we can increase gain
}



// is added in Teensyduino 1.52 beta-4, so this can be deleted !?
/*****
  Purpose: To set the real time clock

  Parameter list:
    void

  Return value:
    void
*****/
void T4_rtc_set(unsigned long t)
{
  //#if defined (T4)
#if 0
  // stop the RTC
  SNVS_HPCR &= ~(SNVS_HPCR_RTC_EN | SNVS_HPCR_HP_TS);
  while (SNVS_HPCR & SNVS_HPCR_RTC_EN); // wait
  // stop the SRTC
  SNVS_LPCR &= ~SNVS_LPCR_SRTC_ENV;
  while (SNVS_LPCR & SNVS_LPCR_SRTC_ENV); // wait
  // set the SRTC
  SNVS_LPSRTCLR = t << 15;
  SNVS_LPSRTCMR = t >> 17;
  // start the SRTC
  SNVS_LPCR |= SNVS_LPCR_SRTC_ENV;
  while (!(SNVS_LPCR & SNVS_LPCR_SRTC_ENV)); // wait
  // start the RTC and sync it to the SRTC
  SNVS_HPCR |= SNVS_HPCR_RTC_EN | SNVS_HPCR_HP_TS;
#endif
}

#define TABLE_SIZE_64 64


/*****
  Purpose: void initTempMon
  Parameter list:
    void
  Return value;
    void
*****/
void initTempMon(uint16_t freq, uint32_t lowAlarmTemp, uint32_t highAlarmTemp, uint32_t panicAlarmTemp)
{

  uint32_t calibrationData;
  uint32_t roomCount;
  uint32_t mode1;
  //first power on the temperature sensor - no register change
  TEMPMON_TEMPSENSE0 &= ~TMS0_POWER_DOWN_MASK;
  TEMPMON_TEMPSENSE1 = TMS1_MEASURE_FREQ(freq);

  calibrationData = HW_OCOTP_ANA1;
  s_hotTemp = (uint32_t)(calibrationData & 0xFFU) >> 0x00U;
  s_hotCount = (uint32_t)(calibrationData & 0xFFF00U) >> 0X08U;
  roomCount = (uint32_t)(calibrationData & 0xFFF00000U) >> 0x14U;
  s_hotT_ROOM = s_hotTemp - TEMPMON_ROOMTEMP;
  s_roomC_hotC = roomCount - s_hotCount;
}

/*****
  Purpose: Read the Teensy's temperature. Get worried over 50C

  Parameter list:
    void

  Return value:
    float           temperature Centigrade
*****/
float TGetTemp()
{
  uint32_t nmeas;
  float tmeas;
  while (!(TEMPMON_TEMPSENSE0 & 0x4U))
  {
    ;
  }
  /* ready to read temperature code value */
  nmeas = (TEMPMON_TEMPSENSE0 & 0xFFF00U) >> 8U;
  tmeas = s_hotTemp - (float)((nmeas - s_hotCount) * s_hotT_ROOM / s_roomC_hotC);   // Calculate temperature
  return tmeas;
}

/*****
  Purpose: scale volume from 0 to 100

  Parameter list:
    int volume        the current reading

  Return value;
    void
*****/
float VolumeToAmplification(int volume)
{
  float x = volume / 100.0f; //"volume" Range 0..100

#if 0
  float a = 3.1623e-4;
  float b = 8.059f;
  float ampl = a * expf( b * x );
  if (x < 0.1f) ampl *= x * 10.0f;
#else
  //Approximation:
  float ampl = x * x * x * x * x; //70dB
#endif

  return ampl;
}

// Teensy 4.0, 4.1
/*****
  Purpose: To set the I2S frequency

  Parameter list:
    int freq        the frequency to set

  Return value:
    int             the frequency or 0 if too large

*****/
int SetI2SFreq(int freq) {
  int n1;
  int n2 ;
  int c0;
  int c2;
  int c1;
  double C;

  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  // Fudge to handle 8kHz - El Supremo
  if (freq > 8000) {
    n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  } else {
    n1 = 8;
  }
  n2 = 1 + (24000000 * 27) / (freq * 256 * n1);
  if (n2 > 63) {
    // n2 must fit into a 6-bit field
    Serial.printf("ERROR: n2 exceeds 63 - %d\n", n2);
    return 0;
  }
  C = ((double)freq * 256 * n1 * n2) / 24000000;
  c0 = C;
  c2 = 10000;
  c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
               | CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1) // &0x07
               | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1); // &0x3f

  CCM_CS2CDR = (CCM_CS2CDR & ~(CCM_CS2CDR_SAI2_CLK_PRED_MASK | CCM_CS2CDR_SAI2_CLK_PODF_MASK))
               | CCM_CS2CDR_SAI2_CLK_PRED(n1 - 1) // &0x07
               | CCM_CS2CDR_SAI2_CLK_PODF(n2 - 1); // &0x3f)
  return freq;
}


/*****
  Purpose: to cause a delay in program execution

  Paramter list:
  unsigned long millisWait    // the number of millseconds to wait

  Return value:
  void
*****/
void MyDelay(unsigned long millisWait)
{
  unsigned long now = millis();

  while (millis() - now < millisWait)
    ;                                   // Twiddle thumbs until delay ends...
}
/*****
  Purpose: to collect array inits in one place

  Paramter list:
    void

  Return value:
    void
*****/
void InitializeDataArrays()
{

  memset(FFT_spec_old,            0, SPECTRUM_RES * sizeof(FFT_spec_old[0]));
  memset(pixelnew,                0, SPECTRUM_RES * sizeof(pixelold[0]));
  memset(pixelold,                0, SPECTRUM_RES * sizeof(pixelold[0]));
  memset(pixelold,                0, SPECTRUM_RES * sizeof(buffer_spec_FFT[0]));
  memset(FFT_spec,                0, SPECTRUM_RES * 2 * sizeof(FFT_spec[0]));
  memset(NR_FFT_buffer,           0, NR_FFT_L * sizeof(NR_FFT_buffer[0]));
  memset(NR_output_audio_buffer,  0, NR_FFT_L * sizeof(NR_output_audio_buffer[0]));
  memset(NR_last_iFFT_result,     0, NR_FFT_L * sizeof(NR_last_iFFT_result[0]));
  memset(NR_last_sample_buffer_L, 0, NR_FFT_L * sizeof(NR_last_sample_buffer_L[0]));
  memset(NR_last_sample_buffer_R, 0, NR_FFT_L * sizeof(NR_last_sample_buffer_R[0]));
  memset(NR_M,                    0, NR_FFT_L * sizeof(NR_M[0]));
  memset(NR_lambda,               0, NR_FFT_L * sizeof(NR_lambda[0]));
  memset(NR_G,                    0, NR_FFT_L * sizeof(NR_G[0]));
  memset(NR_SNR_prio,             0, NR_FFT_L * sizeof(NR_SNR_prio[0]));
  memset(NR_SNR_post,             0, NR_FFT_L * sizeof(NR_SNR_post[0]));
  memset(NR_Hk_old,               0, NR_FFT_L * sizeof(NR_Hk_old[0]));
  memset(NR_X,                    0, NR_FFT_L / 2 * 3 * sizeof(NR_X[0]));
  memset(NR_Nest,                 0, NR_FFT_L / 2 * 2 * sizeof(NR_Nest[0]));
  memset(NR_Gts,                  0, NR_FFT_L / 2 * 2 * sizeof(NR_Gts[0]));
  memset(NR_E,                    0, NR_FFT_L / 2 * NR_N_frames * sizeof(NR_E[0]));
  memset(ANR_d,                   0, ANR_DLINE_SIZE * sizeof(ANR_d[0]));
  memset(ANR_w,                   0, ANR_DLINE_SIZE * sizeof(ANR_w[0]));
  memset(LMS_StateF32,            0, (MAX_LMS_TAPS + MAX_LMS_DELAY) * sizeof(LMS_StateF32[0]));
  memset(LMS_NormCoeff_f32,       0, (MAX_LMS_TAPS + MAX_LMS_DELAY) * sizeof(LMS_NormCoeff_f32[0]));
  memset(LMS_nr_delay,            0, (512 + MAX_LMS_DELAY) * sizeof(LMS_nr_delay[0]));

  CalcCplxFIRCoeffs(FIR_Coef_I, FIR_Coef_Q, m_NumTaps, (float32_t)bands[currentBand].FLoCut, (float32_t)bands[currentBand].FHiCut, (float)SR[SampleRate].rate / DF);

  /****************************************************************************************
     init complex FFTs
  ****************************************************************************************/
  switch (FFT_length)
  {
    case 2048:
      S     = &arm_cfft_sR_f32_len2048;
      iS    = &arm_cfft_sR_f32_len2048;
      maskS = &arm_cfft_sR_f32_len2048;
      break;
    case 1024:
      S     = &arm_cfft_sR_f32_len1024;
      iS    = &arm_cfft_sR_f32_len1024;
      maskS = &arm_cfft_sR_f32_len1024;
      break;
    case 512:
      S     = &arm_cfft_sR_f32_len512;
      iS    = &arm_cfft_sR_f32_len512;
      maskS = &arm_cfft_sR_f32_len512;
      break;
  }

  spec_FFT  = &arm_cfft_sR_f32_len512;  //Changed specification to 512 instance
  NR_FFT    = &arm_cfft_sR_f32_len256;
  NR_iFFT   = &arm_cfft_sR_f32_len256;

  /****************************************************************************************
     Calculate the FFT of the FIR filter coefficients once to produce the FIR filter mask
  ****************************************************************************************/
  InitFilterMask();

  /****************************************************************************************
     Set sample rate
  ****************************************************************************************/
  SetI2SFreq(SR[SampleRate].rate);
  // essential ?
  IFFreq = SR[SampleRate].rate / 4;

  biquad_lowpass1.numStages = N_stages_biquad_lowpass1; // set number of stages
  biquad_lowpass1.pCoeffs   = biquad_lowpass1_coeffs; // set pointer to coefficients file

  for (unsigned i = 0; i < 4 * N_stages_biquad_lowpass1; i++)
  {
    biquad_lowpass1_state[i] = 0.0; // set state variables to zero
  }
  biquad_lowpass1.pState = biquad_lowpass1_state; // set pointer to the state variables

  /****************************************************************************************
     set filter bandwidth of IIR filter
  ****************************************************************************************/
  // also adjust IIR AM filter
  // calculate IIR coeffs
  LP_F_help = bands[currentBand].FHiCut;
  if (LP_F_help < - bands[currentBand].FLoCut)
    LP_F_help = - bands[currentBand].FLoCut;
  SetIIRCoeffs((float32_t)LP_F_help, 1.3, (float32_t)SR[SampleRate].rate / DF, 0); // 1st stage
  for (int i = 0; i < 5; i++)
  { // fill coefficients into the right file
    biquad_lowpass1_coeffs[i] = coefficient_set[i];
  }

  ShowBandwidth();

  /****************************************************************************************
     Initiate decimation and interpolation FIR filters
  ****************************************************************************************/
  // Decimation filter 1, M1 = DF1
  //    CalcFIRCoeffs(FIR_dec1_coeffs, 25, (float32_t)5100.0, 80, 0, 0.0, (float32_t)SR[SampleRate].rate);
  CalcFIRCoeffs(FIR_dec1_coeffs, n_dec1_taps, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);

  if (arm_fir_decimate_init_f32(&FIR_dec1_I, n_dec1_taps, (uint32_t)DF1 , FIR_dec1_coeffs, FIR_dec1_I_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1);
  }

  if (arm_fir_decimate_init_f32(&FIR_dec1_Q, n_dec1_taps, (uint32_t)DF1, FIR_dec1_coeffs, FIR_dec1_Q_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1);
  }

  // Decimation filter 2, M2 = DF2
  CalcFIRCoeffs(FIR_dec2_coeffs, n_dec2_taps, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)(SR[SampleRate].rate / DF1));
  if (arm_fir_decimate_init_f32(&FIR_dec2_I, n_dec2_taps, (uint32_t)DF2, FIR_dec2_coeffs, FIR_dec2_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1);
  }

  if (arm_fir_decimate_init_f32(&FIR_dec2_Q, n_dec2_taps, (uint32_t)DF2, FIR_dec2_coeffs, FIR_dec2_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1);
  }

  // Interpolation filter 1, L1 = 2
  // not sure whether I should design with the final sample rate ??
  // yes, because the interpolation filter is AFTER the upsampling, so it has to be in the target sample rate!
  //    CalcFIRCoeffs(FIR_int1_coeffs, 8, (float32_t)5000.0, 80, 0, 0.0, 12000);
  //    CalcFIRCoeffs(FIR_int1_coeffs, 16, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, SR[SampleRate].rate / 4.0);
  CalcFIRCoeffs(FIR_int1_coeffs, 48, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, SR[SampleRate].rate / 4.0);
  //    if(arm_fir_interpolate_init_f32(&FIR_int1_I, (uint32_t)DF2, 16, FIR_int1_coeffs, FIR_int1_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
  if (arm_fir_interpolate_init_f32(&FIR_int1_I, (uint8_t)DF2, 48, FIR_int1_coeffs, FIR_int1_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
    while (1);
  }
  //    if(arm_fir_interpolate_init_f32(&FIR_int1_Q, (uint32_t)DF2, 16, FIR_int1_coeffs, FIR_int1_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
  if (arm_fir_interpolate_init_f32(&FIR_int1_Q, (uint8_t)DF2, 48, FIR_int1_coeffs, FIR_int1_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
    while (1);
  }
  // Interpolation filter 2, L2 = 4
  // not sure whether I should design with the final sample rate ??
  // yes, because the interpolation filter is AFTER the upsampling, so it has to be in the target sample rate!
  //    CalcFIRCoeffs(FIR_int2_coeffs, 4, (float32_t)5000.0, 80, 0, 0.0, 24000);
  //    CalcFIRCoeffs(FIR_int2_coeffs, 16, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);
  CalcFIRCoeffs(FIR_int2_coeffs, 32, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);

  if (arm_fir_interpolate_init_f32(&FIR_int2_I, (uint8_t)DF1, 32, FIR_int2_coeffs, FIR_int2_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1);
  }
  //    if(arm_fir_interpolate_init_f32(&FIR_int2_Q, (uint32_t)DF1, 16, FIR_int2_coeffs, FIR_int2_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
  if (arm_fir_interpolate_init_f32(&FIR_int2_Q, (uint8_t)DF1, 32, FIR_int2_coeffs, FIR_int2_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1);
  }

  SetDecIntFilters(); // here, the correct bandwidths are calculated and set accordingly

  /****************************************************************************************
     Zoom FFT: Initiate decimation and interpolation FIR filters AND IIR filters
  ****************************************************************************************/
  float32_t Fstop_Zoom = 0.5 * (float32_t) SR[SampleRate].rate / (1 << spectrum_zoom);

  CalcFIRCoeffs(Fir_Zoom_FFT_Decimate_coeffs, 4, Fstop_Zoom, 60, 0, 0.0, (float32_t)SR[SampleRate].rate);

  // Attention: max decimation rate is 128 !
  //  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_I, 4, 1 << spectrum_zoom, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_I_state, BUFFER_SIZE * N_BLOCKS)) {
  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_I, 4, 128, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_I_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1);
  }
  // same coefficients, but specific state variables
  //  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_Q, 4, 1 << spectrum_zoom, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_Q_state, BUFFER_SIZE * N_BLOCKS)) {
  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_Q, 4, 128, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_Q_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1);
  }

  IIR_biquad_Zoom_FFT_I.numStages = IIR_biquad_Zoom_FFT_N_stages;   // set number of stages
  IIR_biquad_Zoom_FFT_Q.numStages = IIR_biquad_Zoom_FFT_N_stages;   // set number of stages
  for (unsigned i = 0; i < 4 * IIR_biquad_Zoom_FFT_N_stages; i++)
  {
    IIR_biquad_Zoom_FFT_I_state[i] = 0.0; // set state variables to zero
    IIR_biquad_Zoom_FFT_Q_state[i] = 0.0; // set state variables to zero
  }
  IIR_biquad_Zoom_FFT_I.pState = IIR_biquad_Zoom_FFT_I_state; // set pointer to the state variables
  IIR_biquad_Zoom_FFT_Q.pState = IIR_biquad_Zoom_FFT_Q_state; // set pointer to the state variables

  // this sets the coefficients for the ZoomFFT decimation filter
  // according to the desired magnification mode
  // for 0 the mag_coeffs will a NULL  ptr, since the filter is not going to be used in this  mode!
  IIR_biquad_Zoom_FFT_I.pCoeffs = mag_coeffs[spectrum_zoom];
  IIR_biquad_Zoom_FFT_Q.pCoeffs = mag_coeffs[spectrum_zoom];

  ZoomFFTPrep();

  /****************************************************************************************
     IQ imbalance correction
  ****************************************************************************************/

  /****************************************************************************************
     start local oscillator Si5351
  ****************************************************************************************/
  si5351.init(SI5351_CRYSTAL_LOAD_10PF, Si_5351_crystal, 230000);

  SetFreq();
  MyDelay(100L);

  SpectralNoiseReductionInit();
  InitLMSNoiseReduction();

  temp_check_frequency  = 0x03U;      //updates the temp value at a RTC/3 clock rate
  //0xFFFF determines a 2 second sample rate period
  highAlarmTemp         = 85U;        //42 degrees C
  lowAlarmTemp          = 25U;
  panicAlarmTemp        = 90U;

  initTempMon(temp_check_frequency, lowAlarmTemp, highAlarmTemp, panicAlarmTemp);
  // this starts the measurements
  TEMPMON_TEMPSENSE0 |= 0x2U;
}
/*****
  Purpose: The initial screen display on startup. Expect this to be customized

  Parameter list:
    void

  Return value:
    void
*****/
void Splash()
{
  tft.fillWindow(RA8875_BLACK);
  tft.setFontScale(3);
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(XPIXELS / 3 - 120, YPIXELS / 5);
  tft.print("T41-EP SDR Radio");
  tft.setTextColor(RA8875_MAGENTA);
  tft.setCursor(XPIXELS / 2 - 60, YPIXELS / 5 + 55);
  tft.setFontScale(2);
  tft.print(VERSION);
  tft.setFontScale(1);
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(XPIXELS / 2 - 10, YPIXELS / 2 - 20);
  tft.print("By");
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(240, YPIXELS / 2 + 50);
  tft.print("Al Peter, AC8GY");
  tft.setCursor(200, YPIXELS / 2 + 90);
  tft.print("Jack Purdum, W8TEE");

  MyDelay(SPLASH_DELAY);
  tft.fillWindow(RA8875_BLACK);
}

/*****
  Purpose: Call to refresh main screen

  Parameter list:
    int displayMode     SPECTRUM_ONLY         0                 // Display state
                        WATERFALL_ONLY        1
                        BOTH_DISPLAYS         2
                        NO_DISPLAY            3
                        NOISE_DISPLAY         4
                        DO_NOTHING           -1

  Return value;
    void
*****/
void RefreshMainDisplay(int displayMode)
{
  switch (displayMode) {
    case DO_NOTHING:
      return;
      break;
    case SPECTRUM_ONLY:
      break;
    case WATERFALL_ONLY:
      tft.fillRect(SPECTRUM_LEFT_X - 1, SPECTRUM_TOP_Y, MAX_WATERFALL_WIDTH + 2, SPECTRUM_HEIGHT + 30,  RA8875_BLACK);  // Spectrum box

      break;
    case BOTH_DISPLAYS:
      //      tft.fillWindow();
      DrawSpectrumDisplayContainer();
      DrawFrequencyBarValue();
      tft.drawRect(BAND_INDICATOR_X - 10, BAND_INDICATOR_Y - 2, 260, 200, RA8875_LIGHT_GREY);
      tft.fillRect(TEMP_X_OFFSET, TEMP_Y_OFFSET + 80, 80, tft.getFontHeight() + 10, RA8875_BLACK);  // Clear volume field

      SetupMode(bands[currentBand].mode);
      SetBand();
      ControlFilterF();
      FilterBandwidth();
      DrawSMeterContainer();
      DrawAudioSpectContainer();
      SetAttenuator(RF_attenuation);
      SetFreq();
      AGCPrep();
      EncoderVolume();
      break;
    case NO_DISPLAY:
      break;
    case NOISE_DISPLAY:
      break;

  }

}
//===============================================================================================================================
//===============================================================================================================================
//==========================  Setup ================================
PROGMEM
/*****
  Purpose: program entry point that sets the environment for program

  Paramter list:
    void

  Return value:
    void
*****/
void setup()
{

  Serial.begin(115200);
  //while (!Serial)
  //  ;
  AudioMemory(400);
  MyDelay(100L);

  setSyncProvider(getTeensy3Time);            // get TIME from real time clock with 3V backup battery
  setTime(now());
  Teensy3Clock.set(now()); // set the RTC
  T4_rtc_set(Teensy3Clock.get());

  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  AudioMemory(400); //
  sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);

  sgtl5000_1.micGain(60);
  sgtl5000_1.lineInLevel(0);
  sgtl5000_1.lineOutLevel(20);

  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_2.volume(0.5);

  pinMode(FILTERPIN15M,     OUTPUT);
  pinMode(FILTERPIN20M,     OUTPUT);
  pinMode(FILTERPIN40M,     OUTPUT);
  pinMode(FILTERPIN80M,     OUTPUT);
  pinMode(RXTX,             OUTPUT);
  pinMode(MUTE,             OUTPUT);
  digitalWrite(MUTE,        HIGH);
  pinMode(PTT,              INPUT_PULLUP);
  pinMode(BUSY_ANALOG_PIN,  INPUT);

  pinMode(OPTO_OUTPUT,      OUTPUT);
  pinMode(KEYER_DIT_INPUT_TIP,  INPUT_PULLUP);
  pinMode(KEYER_DAH_INPUT_RING, INPUT_PULLUP);
  //  attachInterrupt(digitalPinToInterrupt(KEYER_DIT_INPUT_TIP),  Dit, CHANGE);
  //  attachInterrupt(digitalPinToInterrupt(KEYER_DAH_INPUT_RING), Dah, CHANGE);

  pinMode(TFT_MOSI,         OUTPUT);
  digitalWrite(TFT_MOSI,    HIGH);
  pinMode(TFT_SCLK,         OUTPUT);
  digitalWrite(TFT_SCLK,    HIGH);
  pinMode(TFT_CS,           OUTPUT);
  digitalWrite(TFT_CS,      HIGH);
  delay(1);

  arm_fir_init_f32(&FIR_Hilbert_L, 100, FIR_Hilbert_coeffs_45, FIR_Hilbert_state_L, 256);  //AFP01-16-22
  arm_fir_init_f32(&FIR_Hilbert_R, 100, FIR_Hilbert_coeffs_neg45, FIR_Hilbert_state_R, 256);

  arm_fir_decimate_init_f32(&FIR_dec1_EX_I, 48, 4 , coeffs192K_10K_LPF_FIR, FIR_dec1_EX_I_state, 2048);
  arm_fir_decimate_init_f32(&FIR_dec1_EX_Q, 48, 4, coeffs192K_10K_LPF_FIR, FIR_dec1_EX_Q_state, 2048) ;

  arm_fir_decimate_init_f32(&FIR_dec2_EX_I, 24, 2, coeffs48K_8K_LPF_FIR, FIR_dec2_EX_I_state, 512);
  arm_fir_decimate_init_f32(&FIR_dec2_EX_Q, 24, 2, coeffs48K_8K_LPF_FIR, FIR_dec2_EX_Q_state, 512);

  arm_fir_interpolate_init_f32(&FIR_int1_EX_I, 2, 48, coeffs48K_8K_LPF_FIR, FIR_int1_EX_I_state, 256);
  arm_fir_interpolate_init_f32(&FIR_int1_EX_Q, 2, 48, coeffs48K_8K_LPF_FIR, FIR_int1_EX_Q_state, 256);

  arm_fir_interpolate_init_f32(&FIR_int2_EX_I, 4, 32, coeffs192K_10K_LPF_FIR, FIR_int2_EX_I_state, 512);
  arm_fir_interpolate_init_f32(&FIR_int2_EX_Q, 4, 32, coeffs192K_10K_LPF_FIR, FIR_int2_EX_Q_state, 512);

  //***********************  EQ Gain Settings ************

  EQBand1GaindB = 0;  //200
  EQBand2GaindB = 0;  //250
  EQBand3GaindB = 0;  //315
  EQBand4GaindB = 0;  //400
  EQBand5GaindB = 0;  //500
  EQBand6GaindB = 0;  //630
  EQBand7GaindB = 0;  //800
  EQBand8GaindB = 0;  //1000
  EQBand9GaindB = 0;  //1250
  EQBand10GaindB = 0; //1600
  EQBand11GaindB = 0; //2000
  EQBand12GaindB = 0; //2500
  EQBand13GaindB = 0; //3150
  EQBand14GaindB = 0; //4000

  //tft.begin(50000000);
  uint32_t iospeed_display                  = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(1);
  *(digital_pin_to_info_PGM + 13)->pad      = iospeed_display;                            //clk
  *(digital_pin_to_info_PGM + 11)->pad      = iospeed_display;                            //MOSI
  *(digital_pin_to_info_PGM + TFT_CS)->pad  = iospeed_display;

  tuneEncoder.begin(true);

  tft.begin(RA8875_800x480);              // Can do graphics now
  tft.setRotation(0);

  // Setup for scrolling attributes. Part of  initSpectrum_RA8875() call written by Mike Lewis
  tft.useLayers(1);                 //mainly used to turn on layers! //AFP 03-27-22 Layers
  tft.layerEffect(OR);
  tft.writeTo(L2);
  tft.clearMemory();
  tft.writeTo(L1);
  Splash();

  spectrum_x        = 10;
  spectrum_y        = 150;
  xExpand           = 1.4;
  h                 = 135;
  nrOptionSelect    = 0;
  freq_flag[1]      = 0;

  Q_in_L.begin();
  Q_in_R.begin();

  MyDelay(100L);
  InitializeDataArrays();

  displayMode = BOTH_DISPLAYS;

  // ==============================================  Intial set up of EEPROM data ===============
  //  EEPROMSaveDefaults();                           // uncomment this line first time running new Teensy to set up the EEPROM


  if (EEPROMData.version_of_settings[0] != 'V') {       // EEPROM has not been set
    SaveAnalogSwitchValues();                           // Call this function to reset switch matrix values
  }

  EEPROMRead();                                     // Read stored settings

  activeVFO = EEPROMData.currentVFO = VFO_A;

  if (activeVFO == VFO_A)
    centerFreq = TxRxFreq = currentFreqA;
  else
    centerFreq = TxRxFreq = currentFreqB;

#ifdef DEBUG1
  EEPROMShow();                                     // To see EEPROM values on Serial monitor
#endif

  currentFreqA    = TxRxFreq = centerFreq = 7047700L;   // Test freq for W1AW

#ifdef DEBUG
  currentFreqA    = TxRxFreq = centerFreq = 7047700L;
  decoderFlag     = CW_MODE;        // These could be SSB_MODE too
  xmtMode         = CW_MODE;
  //  xmtMode         = SSB_MODE;
  filterWidth     = FILTER_WIDTH;
#endif

  kTemp = 0;
  SetupMode(bands[currentBand].mode);

  ditLength     = STARTING_DITLENGTH;                // 80 = 1200 / 15 wpm
  averageDit    = ditLength;
  averageDah    = ditLength * 3L;

  filterEncoder.write(currentWPM);
  SetDitLength(currentWPM);

  mainMenuIndex = 0;

  RedrawDisplayScreen();
  jackStart = millis();
  digitalWrite(PTT, HIGH);

  float32_t theta = 0.0;                      //AFP 02-02-22

  for (int kf = 0; kf < 255; kf++) {          //Calc 750 hz sine wave, use 750 because it is 8 whole cycles in 256 buffer.
    //theta = kf * 2*PI * 1600/ 24000;
    theta = kf * 2 * PI * 11 / 256;
    sinBuffer[kf] = sin(theta);
  }
  ShowName();                                 // Show name and version
  sineTone(BUFFER_SINE_COUNT);                // Set t0 7 from above

  pinMode(A0, OUTPUT);  //PWM output
  ditLength = 80;

}
//============================================================== END setup() =================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================

FASTRUN                   // Causes function to be allocated in RAM1 at startup for fastest performance.

elapsedMicros usec = 0;   // Automatically increases as time passes; no ++ necessary.

/*****
  Purpose: Code here executes forever, or until: 1) power is removed, 2) user does a reset, 3) a component
           fails, or 4) the cows come home.

  Paramter list:
    void

  Return value:
    void
*****/
void loop()
{
  static int passFlag = 0;
  int pushButtonSwitchIndex;
  int valPin;
  long centerFreq;

  valPin = ReadSelectedPushButton();                        // Poll UI push buttons

  if (valPin != BOGUS_PIN_READ) {                           // If a button was pushed...
    pushButtonSwitchIndex = ProcessButtonPress(valPin);     // Winner, winner...chicken dinner!
    ExecuteButtonPress(pushButtonSwitchIndex);
  }

// This is polling code. I am not sure how I want the CW coding to work. This code does not work...YET!

  if (digitalRead(KEYER_DIT_INPUT_TIP) == LOW) {
    Dit();
  } else {
    if (digitalRead(KEYER_DAH_INPUT_RING) == LOW) {
      Dah();
    }
  }

  /*
    if (xmtMode == CW_MODE) {
      if (digitalRead(KEYER_DIT_INPUT_TIP) == LOW || digitalRead(KEYER_DAH_INPUT_RING) == LOW) {        //=========================== CW Transmit ================

      CWOnState = 1;

      while (CWOnState == 1) {
        digitalWrite(MUTE, LOW);
        digitalWrite(RXTX, HIGH);
        CWFreqShift = 750;
        SetFreq();

        si5351.output_enable(SI5351_CLK2, 0);
        modeSelectInR.gain(0, 1.0);               //Selects Ex
        modeSelectInR.gain(1, 0.0);               //Selects Ex
        modeSelectInL.gain(0, 1.0);               //Selects Ex
        modeSelectInL.gain(1, 0.0);               //Selects Ex

        modeSelectOutL.gain(0, 0);
        modeSelectOutR.gain(0, 0);
        modeSelectOutExL.gain(0, 1.0);
        modeSelectOutExR.gain(0, 1.0);
        ExciterIQData();

          digitalWrite(RXTX, LOW);
          CWOnState   = 0;
          CWFreqShift = 0;
          break;
      }
    } else {
  */
  if (xmtMode == SSB_MODE && digitalRead(PTT) == LOW) {         //   SSB Transmit
    digitalWrite(MUTE, LOW);
    digitalWrite(RXTX, HIGH);
    si5351.output_enable(SI5351_CLK1, 1);
    si5351.output_enable(SI5351_CLK2, 0);
    modeSelectInR.gain(0, 1.0);               //Selects Ex
    modeSelectInR.gain(1, 0.0);               //Selects Ex
    modeSelectInL.gain(0, 1.0);               //Selects Ex
    modeSelectInL.gain(1, 0.0);               //Selects Ex

    modeSelectOutL.gain(0, 0);
    modeSelectOutR.gain(0, 0);
    modeSelectOutExL.gain(0, 1.0);
    modeSelectOutExR.gain(0, 1.0);
    CWFreqShift = 0;
    SetFreq();
    ExciterIQData();
    CWOnState = 0;
    CWOnState = 0;
  }
  //===========================  Receive ================
  if (digitalRead(PTT) == HIGH && (digitalRead(KEYER_DIT_INPUT_TIP) == HIGH || digitalRead(KEYER_DAH_INPUT_RING) == HIGH)) {
    //Serial.println("In receive");
    si5351.output_enable(SI5351_CLK2, 1);

    modeSelectInR.gain(0, 0.0);                 //Selects Rec
    modeSelectInR.gain(1, 1.0);                 //Selects Rec
    modeSelectInL.gain(0, 0.0);                 //Selects Rec
    modeSelectInL.gain(1, 1.0);                 //Selects Rec
    digitalWrite(RXTX, LOW);                    // New board with wisker fix
    digitalWrite(MUTE, HIGH);

    modeSelectOutExL.gain(0, 0);
    modeSelectOutExR.gain(0, 0);
    modeSelectOutL.gain(0, 1.0);
    modeSelectOutR.gain(0, 1.0);

    phaseLO             = 0.0;
    barGraphUpdate      = 0;
    omitOutputFlag      = false;

    SetFastTuneFrequency();                                   // draw FastTune frequency line
    ShowSpectrum();                                           //Now calls ProcessIQData and Encoders calls

    valPin = ReadSelectedPushButton();                        // Poll UI push buttons

    if (valPin != BOGUS_PIN_READ) {                           // If a button was pushed...
      pushButtonSwitchIndex = ProcessButtonPress(valPin);     // Winner, winner...chicken dinner!
      ExecuteButtonPress(pushButtonSwitchIndex);
    }
#ifdef DEBUG1
    if (elapsed_micros_idx_t > (SR[SampleRate].rate / 960) )
    {
      ShowTempAndLoad();                                      // Used to monitor CPU temp and load factors
    }
#endif
    if (encoder_check.check() == 1)                           // Checks every 100ms for signal. Will likely change
    {
      EncoderVolume();
      ShowAnalogGain();
    }
    if (ms_500.check() == 1)                                  // For clock updates
    {
      wait_flag = 0;
      DisplayClock();
    }
  }
} // end loop
