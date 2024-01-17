#ifndef BEENHERE
#define BEENHERERA8875_DISPLAYRA8875_DISPLAY

//======================================== Library include files ========================================================
#include <Adafruit_GFX.h>
#include "Fonts/FreeMonoBold24pt7b.h"
#include "Fonts/FreeMonoBold18pt7b.h"
#include "Fonts/FreeMono24pt7b.h"
#include <Audio.h>
#include <TimeLib.h>                                // Part of Teensy Time library
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Metro.h>
#include <Bounce.h>
#include <arm_math.h>
#include <arm_const_structs.h>
#include <si5351.h>
#include <Encoder.h>
#include <Rotary.h>                                 // https://github.com/brianlow/Rotary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <hardware/teensy/avr/libraries/Adafruit_GFX/Fonts/FreeMono18pt7b.h>                         // In Adafrui_GFX
#include <util/crc16.h>                             //mdrhere
#include <utility/imxrt_hw.h>                       // for setting I2S freq, Thanks, FrankB!
#include <EEPROM.h>

//======================================== Symbolic Constants for Debugging and Version Control ========================
#define VERSION                     "V010  "
#define RIGNAME                     "T41-EP SDT"
#define SPLASH_DELAY                1000L         // Probably should be 4000 to actually read it
#define RIGNAME_X_OFFSET            580
#define T4                                        // Says we are using a Teensy 4 or 4.1
#define RA8875_DISPLAY              1             // Comment out if not using RA8875 display

#define DEBUG                       1             // This must be uncommented for ANY debugging statements to work
//#define STORE_SWITCH_VALUES                       // Uncomment to save the analog switch values for your push button matrix

#define OFF                         0
#define ON                          1


//#define DEBUG_JACK
//#define DEBUG1
//#define DEBUG2
//#define DEBUG3
//#define DEBUG4
//#define DEBUG6
//#define DEBUG7
//#define DEBUG8
//#define DEBUG9
#define NUMBER_OF_ELEMENTS(x) (sizeof(x)/sizeof(x[0]))
#define NEW_SI5351_FREQ_MULT    1UL


//======================================== Symbolic constants ==========================================================

// These constants are used by the voltage divider network so only 1 analog pin is used for the 16 option switches. These may need
// to be changed for the exact value for your system. They are initialized in the INO file.

#define BUSY_ANALOG_PIN              39     // This is the analog pin that controls the 16 switches
#define NOTHING_TO_SEE_HERE         950     // If the analog pin is greater than this value, nothing's going on
#define BOGUS_PIN_READ               -1     // If no push button read
#define WIGGLE_ROOM                  15     // This is the number of pixels within which a switch analog value must fall
#define SWITCH_DEBOUNCE_DELAY       50L     // Milliseconds for the switch to settle down
#define NUMBER_OF_SWITCHES           16     // Number of push button switches
#define CHAR_HEIGHT                  32
#define PIXELS_PER_EQUALIZER_DELTA   10     // Number of pixeks per detent of encoder for equalizer changes
#define AUDIO_PLOT_CEILING          119     // (SPECTRUM_BOTTOM - AUDIO_SPECTRUM_TOP)

#define TOP_MENU_COUNT               16     // Menus to process
#define MAX_FAVORITES                10     // Max number of favorite frequencies stored in EEPROM
#define PRIMARY_MENU                  0
#define SECONDARY_MENU                1
#define SELECTED_INDEX                3     // This is the index for MENU_OPTION_SELECT

/*
  const char *topMenus[]                    = {"CW",              "Display Choices", "Spectrum Set",   "AGC",        "NR Set",
                                             "IQ Manual",       "EQ Rec Set",   "EQ Xmt Set",     "Mic Comp",   "Freq Cal",
                                             "Noise Reduction", "RF Set",       "Audio Post Proc","VFO Select", "EEPROM",
                                             "Noise Floor",    "Unused 1"
                                            };
*/
#define MENU_OPTION_SELECT           0     // These are the expected values from the switch ladder
#define MAIN_MENU_UP                 1
#define BAND_UP                      2
#define ZOOM                         3
#define MAIN_MENU_DN                 4
#define BAND_DN                      5
#define FILTER                       6
#define DEMODULATION                 7
#define SET_MODE                     8
#define NOISE_REDUCTION              9
#define NOTCH_FILTER                10
#define DISPLAY_OPTIONS             11
#define INCREMENT                   15
#define NOISE_FLOOR                 13
#define UNUSED_1                    14
#define UNUSED_2                    12
//=======================================================
//#define XPIXELS             320           // This is for the standard 2.8" display
//#define YPIXELS             240
//#define XPIXELS             480           // This is for the standard 3.5" display
//#define YPIXELS             320
#define XPIXELS               800           // This is for the 5.0" display
#define YPIXELS               480
#define PIXELHEIGHT           20            // Used in fillRec() to erase a line

#define SPECTRUM_LEFT_X       3            // Used to plot left edge of spectrum display  AFP 12-14-21
#define WATERFALL_LEFT_X      SPECTRUM_LEFT_X

#ifdef RA8875_DISPLAY // -------------------------------------             Standard display

#define CLIP_AUDIO_PEAK       115           // The pixel value where audio peak overwrites S-meter

#define SPECTRUM_RES          512
#define SPECTRUM_TOP_Y        100           // Start of spectrum plot space
#define SPECTRUM_HEIGHT       150           // This is the pixel height of spectrum plot area without disturbing the axes
#define SPECTRUM_BOTTOM       (SPECTRUM_TOP_Y + SPECTRUM_HEIGHT - 3)
#define AUDIO_SPECTRUM_TOP    129
#define AUDIO_SPECTRUM_BOTTOM SPECTRUM_BOTTOM
#define MAX_WATERFALL_WIDTH   512           // Pixel width of waterfall
#define MAX_WATERFALL_ROWS    170           // Waterfall rows

#define WATERFALL_RIGHT_X     (WATERFALL_LEFT_X + MAX_WATERFALL_WIDTH)
#define WATERFALL_TOP_Y       (SPECTRUM_TOP_Y + SPECTRUM_HEIGHT + 5)        // 130 + 120 + 5 = 255
#define FIRST_WATERFALL_LINE  (WATERFALL_TOP_Y + 20)                        // 255 + 35 = 290
#define WATERFALL_BOTTOM      (FIRST_WATERFALL_LINE + MAX_WATERFALL_ROWS)   // 290 + 170 = 460
//#define TEMP_X_OFFSET         20
#define TEMP_X_OFFSET         15
#define TEMP_Y_OFFSET         YPIXELS * 0.97
#define AGC_Y_OFFSET          260
#define AGC_X_OFFSET          720
#define VOLUME_Y_OFFSET       180
#define INCREMENT_X           WATERFALL_RIGHT_X + 25
#define INCREMENT_Y           WATERFALL_TOP_Y   + 70
#define SPECTRUMCORNER_X      INCREMENT_X
#define SPECTRUMCORNER_Y      INCREMENT_Y
#define BAND_INDICATOR_X      WATERFALL_RIGHT_X + 25
#define BAND_INDICATOR_Y      WATERFALL_TOP_Y + 37
#define OPERATION_STATS_X     130
#define OPERATION_STATS_Y     75
#define BAND_SUMMARY_X        BAND_INDICATOR_X
#define BAND_SUMMARY_Y        150
#define START_BAND_DATA_X     TEMP_X_OFFSET
#define START_BAND_DATA_Y     YPIXELS * 0.25
#define SMETER_X              WATERFALL_RIGHT_X + 16
#define SMETER_Y              YPIXELS * 0.22
#define SMETER_BAR_HEIGHT     18
#define SMETER_BAR_LENGTH     180
#define SPECTRUM_NOISE_FLOOR  (SPECTRUM_TOP_Y + SPECTRUM_HEIGHT - 3)
#define TIME_X                (XPIXELS * 0.73)                            // Upper-left corner for time
#define TIME_Y                (YPIXELS * 0.07)
#define WHICH_SIDEBAND_X      (XPIXELS * 0.70)
#define WHICH_SIDEBAND_Y      (YPIXELS * 0.20)
#define FILTER_PARAMETERS_X   (XPIXELS * 0.22)
#define FILTER_PARAMETERS_Y   (YPIXELS * 0.213)
#define DEFAULT_EQUALIZER_BAR 100                                         // Default equalizer bar height
#define FREQUENCY_X           5
#define FREQUENCY_Y           45
#define FREQUENCY_X_SPLIT     285
#define VFO_A                 0
#define VFO_B                 1
#define VFOA_PIXEL_LENGTH     275
#define VFOB_PIXEL_LENGTH     280
#define FREQUENCY_PIXEL_HI    45
// Offsets for status info
#define FIELD_OFFSET_X        WATERFALL_RIGHT_X + 118                     // X coordinate for field
#define NOTCH_X               WATERFALL_RIGHT_X + 58
#define NOTCH_Y               WATERFALL_TOP_Y   + 90
#define NOISE_REDUCE_X        WATERFALL_RIGHT_X + 58
#define NOISE_REDUCE_Y        WATERFALL_TOP_Y   + 110
#define ZOOM_X                WATERFALL_RIGHT_X + 65
#define ZOOM_Y                WATERFALL_TOP_Y   + 130
#define COMPRESSION_X         WATERFALL_RIGHT_X + 33
#define COMPRESSION_Y         WATERFALL_TOP_Y   + 150
#define DECODER_X             WATERFALL_RIGHT_X + 43
#define DECODER_Y             WATERFALL_TOP_Y   + 190
#define WPM_X                 WATERFALL_RIGHT_X + 58
#define WPM_Y                 WATERFALL_TOP_Y   + 170
#define NR_X_OFF              WATERFALL_RIGHT_X + 80
#define NR_Y_OFF              WATERFALL_TOP_Y   + 190

#define MAX_DECODE_CHARS        30                    // Max chars that can appear on decoder line
#define DECODER_BUFFER_SIZE     128                   // Max chars in binary search string with , . ?
#define DECODER_CAP_VALUE       6.0
#define DITLENGTH_DELTA         5                     // Number of milliseconds to change ditLEngth with encoder
#define HISTOGRAM_ELEMENTS      750
#define LOWEST_ATOM_TIME         20                   // 60WPM has an atom of 20ms
#define HIGHEST_ATOM_TIME       240                   // 5WPM has an atom of 240ms                              
#define DIT_WEIGHT              0.3                   // Previous values account for 90% of average
#define AVERAGE_DIT_WEIGHT      0.7                   // The number above and this one must equal 1.0
#define DITLENGTH_OBSERVATIONS  10                    // Number of ditlength observations to compute average
#define ADAPTIVE_SCALE_FACTOR   0.8                   // The amount of old histogram values are presesrved
#define SCALE_CONSTANT          (1.0 / (1.0 - ADAPTIVE_SCALE_FACTOR)) // Insure array has enough observations to scale
#define FILTER_WIDTH            25                    // The default filter highlight in spectrum displah
#define ZOOM_2X_BIN_COUNT       187.5                 // The 2x bin count for display

#define AUDIO_POST_PROCESSOR_BANDS  8                     // Number of audio segments
#define EEPROM_FAVORITES_X          100
#define EEPROM_FAVORITES_Y          50
#define BANDWIDTH_INDICATOR_Y       SPECTRUM_BOTTOM
#define FAST_TUNE_CENTERLINE        ((MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X+6) / 2)

#else // --------------------------------------             Non-standard display

//#define BAND_INDICATOR_X      410
//#define BAND_INDICATOR_Y      260
#define SPECTRUM_TOP_Y        115                   // Start of spectrum plot space
#define SPECTRUM_HEIGHT       60                    // This is the pixel height of spectrum plot area without disturbing the axes
#define MAX_WATERFALL_WIDTH   383
#define MAX_WATERFALL_ROWS    60                    // Waterfall rows
#define BAND_SUMMARY_X        BAND_INDICATOR_X
#define BAND_SUMMARY_Y        150
#define SMETER_X              BAND_INDICATOR_X
#define SMETER_Y              90

#endif

#define DO_NOTHING           -1
#define SPECTRUM_ONLY         0                 // Display state
#define WATERFALL_ONLY        1
#define BOTH_DISPLAYS         2
#define NO_DISPLAY            3
#define NOISE_DISPLAY         4

#define FLOAT_PRECISION       6                 // Assumed precision for a float
#define BUFFER_SINE_COUNT     7                 // Leads to a 750Hz signal

#define EQUALIZER_CELL_COUNT  14
#define USE_LOG10FAST
#define USE_W7PUA
#define MP3
#define T4
#define TEMPMON_ROOMTEMP 25.0f
#define ENCODER_DELAY             100L        // Menu options scroll too fast!

//--------------------- decoding stuff
#define FFT_LENGTH                512
#define NOISE_SAMPLE_SIZE         500
#define SD_MULTIPLIER             3
#define NOISE_MULTIPLIER          0.5         // Signal must be this many time greater than the noise floor
#define STARTING_DITLENGTH        80          // dit length for 15wpm

#ifndef RA8875_DISPLAY
#include <ILI9488_t3.h>                     // https://github.com/mjs513/ILI9488_t3
#include <ILI9488_t3_font_Arial.h>
#define  BLACK       0x0000                     /*   0,   0,   0 */
#define  RA8875_BLUE 0x000F                     /*   0,   0, 128 */
#define  DARK_GREEN  0x03E0                     /*   0, 128,   0 */
#define  DARKCYAN    0x03EF                     /*   0, 128, 128 */
#define  MAROON      0x7800                     /* 128,   0,   0 */
#define  PURPLE      0x780F                     /* 128,   0, 128 */
#define  OLIVE       0x7BE0                     /* 128, 128,   0 */
#define  RA8875_LIGHT_GREY   0xC618             /* 192, 192, 192 */
#define  DARKGREY    0x7BEF                     /* 128, 128, 128 */
#define  BLUE        0x001F                     /*   0,   0, 255 */
#define  RA8875_GREEN       0x07E0              /*   0, 255,   0 */
#define  CYAN        0x07FF                     /*   0, 255, 255 */
#define  RED         0xF800                     /* 255,   0,   0 */
#define  MAGENTA     0xF81F                     /* 255,   0, 255 */
#define  YELLOW      0xFFE0                     /* 255, 255,   0 */
#define  WHITE       0xFFFF                     /* 255, 255, 255 */
#define  ORANGE      0xFD20                     /* 255, 165,   0 */
#define  RA8875_GREENYELLOW 0xAFE5              /* 173, 255,  47 */
#define  PINK        0xF81F
#else
#include <RA8875.h>                             // https://github.com/sumotoy/RA8875
#define  ORANGE      0xFD20                     /* 255, 165,   0 */
#define  FILTER_WIN  0x10                       // Color of SSB filter width
#endif
//  #define  BLACK              RA8875_BLACK      /*   0,   0,   0 */
//  #define  RA8875_BLUE        0x00007f          /*   0,   0, 128 */
//  #define  RA8875_GREEN       0x007F00          /* 0, 128,   0 */
//  #define  DARKCYAN           0x007f7f          /*   0, 128, 128 */
//  #define  MAROON             0x7f0000          /* 128,   0,   0 */
//  #define  PURPLE             0x7f007f          /* 128,   0, 128 */
//  #define  OLIVE              0x7f7f00          /* 128, 128,   0 */
//  #define  RA8875_LIGHT_GREY  0xC0C0C0          /* 192, 192, 192 */
//  #define  DARKGREY           0x7f7f7f          /* 128, 128, 128 */
//  #define  BLUE               RA8875_BLUE       /*   0,   0, 255 */
//  #define  RA8875_GREEN       RA8875_GREEN      /*   0, 255,   0 */
//  #define  DARK_GREEN         0x007f00          /*   0, 128,   0 */
//  #define  CYAN               RA8875_CYAN       /*   0, 255, 255 */
//  #define  RED                RA8875_RED        /* 255,   0,   0 */
//  #define  MAGENTA            RA8875_MAGENTA    /* 255,   0, 255 */
//  #define  YELLOW             RA8875_YELLOW     /* 255, 255,   0 */
//  #define  WHITE              RA8875_WHITE      /* 255, 255, 255 */
//  #define  ORANGE             0xffa500          /* 255, 165,   0 */
//  #define  RA8875_GREENYELLOW 0xADFF2F          /* 173, 255,  47 */
//  #define  PINK               RA8875_PINK
//#endif

#ifndef FLASHMEM
#define FLASHMEM
#endif

#if defined(T4)
#include <utility/imxrt_hw.h> // for setting I2S freq, Thanks, FrankB!
#define WFM_SAMPLE_RATE   256000.0f
//#define WFM_SAMPLE_RATE   234375.0f
#else
#include <EEPROM.h>
#define F_I2S ((((I2S0_MCR >> 24) & 0x03) == 3) ? F_PLL : F_CPU)
#define WFM_SAMPLE_RATE   234375.0f
#endif

#define MOSELEY                     0
#define TIMEZONE                    "EST: "     // Set for eastern time
//#define CHANG                     0
#define MOSELEY                     0

#define DEFAULTFREQINCREMENT        50          //Values 10, 50, 250, 1000
#define FAST_TUNE_INCREMENT         50L
#define DEFAULTFREQINDEX            3           //  Index 10Hz=> 0, 50Hz=> 1, 250Hz=> 2, 1000Hz=> 3
#define TEMPMON_ROOMTEMP            25.0f
#define MAX_WPM                     60
#define MAX_TONE                    1000

#define ENCODER_FACTOR              0.25F        // use 0.25f with cheap encoders that have 4 detents per step, 
// for other encoders or libs we use 1.0f
#define MAX_ZOOM_ENTRIES            5
#define FREQ_SEP_CHARACTER          '.'

//========================================================= Pin Assignments =====================================
//========================================= Pins 0 and 1 are usually reserved for the USB COM port communications
//========================================= On the Teensy 4.1 board, pins GND, 0-12, and pins 13-23, 3.3V, GND, and
//========================================= Vin are "covered up" by the Audio board. However, not all of those pins are
//========================================= actually used by the board. See: https://www.pjrc.com/store/teensy3_audio.html

//========================================= Display pins
#define BACKLIGHT_PIN               6     // unfortunately connected to 3V3 in DO7JBHs PCB 
#define TFT_DC                      9
#define TFT_CS                      10
#define TFT_MOSI                    11
#define TFT_MISO                    12
#define TFT_SCLK                    13
#define TFT_RST                     255
//========================================= Encoder pins
#define VOLUME_ENCODER_A            2     // Volume encoder  
#define VOLUME_ENCODER_B            3
#define ENCODER3_ENCODER_A          4     // Menu encoder 
#define ENCODER3_ENCODER_B          5
#define FILTER_ENCODER_A            14    // Filter encoder
#define FILTER_ENCODER_B            15
#define TUNE_ENCODER_A              16    // Tune encoder
#define TUNE_ENCODER_B              17
//========================================= Filter Board pins
#define FILTERPIN80M                30    // 80M filter relay
#define FILTERPIN40M                31    // 40M filter relay
#define FILTERPIN20M                28    // 20M filter relay
#define FILTERPIN15M                27    // 15M filter relay
#define RXTX                        22    // Transmit/Receive
#define PTT                         37    // Transmit/Receive
#define MUTE                        38    // Mute Audio,  HIGH = "On" Audio available from Audio PA, LOW = Mute audio
//========================================= Switch pins
#define BAND_MENUS                  100    // encoder2 button = button3SW
#define BAND_PLUS                   101    // BAND+ = button2SW
#define CHANGE_INCREMENT            102   // this is the pushbutton pin of the tune encoder
#define CHANGE_FILTER               103    // this is the pushbutton pin of the filter encoder
#define CHANGE_MODE                104    // Change mode
#define CHANGE_MENU2                105    // this is the pushbutton pin of encoder 3
#define MENU_MINUS                  106    // Menu decrement
#define MENU_PLUS                   107    // this is the menu button pin
#define CHANGE_NOISE                108    // this is the pushbutton pin of NR
#define CHANGE_DEMOD                109    // this is the push button for demodulation
#define CHANGE_ZOOM                 110    // Push button for display zoom feature
#define SET_FREQ_CURSOR             111    // Push button for frequency Cursor feature  was 39 for Al

//========================================= Keyer pins
//#define TONEPIN                     xx    // For sidetone
#define KEYER_DIT_INPUT_TIP          35    // Tip connection for keyer
#define KEYER_DAH_INPUT_RING         36    // Ring connection for keyer  -- default for righthanded user

#define OPTO_OUTPUT                  24    // To optoisolator and keyed circuit
#define STRAIGHT_KEY                  0
#define KEYER                         1
#define KEYONTIME                   500 // AFP17-22 key on time
//========================================================= End Pin Assignments =================================
//===============================================================================================================

#define TMS0_POWER_DOWN_MASK        (0x1U)
#define TMS0_POWER_DOWN_SHIFT       (0U)
#define TMS1_MEASURE_FREQ(x)        (((uint32_t)(((uint32_t)(x)) << 0U)) & 0xFFFFU)
#define TMS0_ALARM_VALUE(x)         (((uint32_t)(((uint32_t)(x)) << 20U)) & 0xFFF00000U)
#define TMS02_LOW_ALARM_VALUE(x)    (((uint32_t)(((uint32_t)(x)) << 0U)) & 0xFFFU)
#define TMS02_PANIC_ALARM_VALUE(x)  (((uint32_t)(((uint32_t)(x)) << 16U)) & 0xFFF0000U)
#define MAX_NUMCOEF (257)           // was (FFT_LENGTH / 2) + 1

#undef  round
#undef  PI
#undef  HALF_PI
#undef  TWO_PI
#define PI                          3.1415926535897932384626433832795f
#define HALF_PI                     1.5707963267948966192313216916398f
#define TWO_PI                      6.283185307179586476925286766559f
#define TPI                         TWO_PI
#define PIH                         HALF_PI
#define FOURPI                      (2.0f * TPI)
#define SIXPI                       (3.0f * TPI)
#define Si_5351_clock               SI5351_CLK2
#define Si_5351_crystal             25000000L
#define MASTER_CLK_MULT             4ULL                                         // QSD frontend requires 4x clock
#define WITHTERM                    1
#define SIGNAL_TAU                  0.1
#define ONEM_SIGNAL_TAU             (1.0 - SIGNAL_TAU)

#define CW_TIMEOUT                  3                                         // Time, in seconds, to trigger display of last Character received
#define ONE_SECOND                  (12000 / cw_decoder_config.blocksize)     // sample rate / decimation rate / block size

#define SSB_MODE                    0
#define CW_MODE                     1
#define RECEIVE_MODE                2

#define DIGIMODE_OFF                0
#define CW                          1
#define EFR                         3
#define DCF77                       5

#define SPECTRUM_ZOOM_MIN           0
#define SPECTRUM_ZOOM_1             0
#define SPECTRUM_ZOOM_2             1
#define SPECTRUM_ZOOM_4             2
#define SPECTRUM_ZOOM_8             3
#define SPECTRUM_ZOOM_16            4
#define SPECTRUM_ZOOM_32            5
#define SPECTRUM_ZOOM_64            6
#define SPECTRUM_ZOOM_128           7
#define SPECTRUM_ZOOM_256           8
#define SPECTRUM_ZOOM_512           9
#define SPECTRUM_ZOOM_1024          10
#define SPECTRUM_ZOOM_2048          11
#define SPECTRUM_ZOOM_4096          12
#define SPECTRUM_ZOOM_MAX           5

#define SAMPLE_RATE_MIN             6
#define SAMPLE_RATE_8K              0
#define SAMPLE_RATE_11K             1
#define SAMPLE_RATE_16K             2
#define SAMPLE_RATE_22K             3
#define SAMPLE_RATE_32K             4
#define SAMPLE_RATE_44K             5
#define SAMPLE_RATE_48K             6
#define SAMPLE_RATE_50K             7
#define SAMPLE_RATE_88K             8
#define SAMPLE_RATE_96K             9
#define SAMPLE_RATE_100K            10
#define SAMPLE_RATE_101K            11
#define SAMPLE_RATE_176K            12
#define SAMPLE_RATE_192K            13
#define SAMPLE_RATE_234K            14
#define SAMPLE_RATE_256K            15
#define SAMPLE_RATE_281K            16 // ??
#define SAMPLE_RATE_353K            17 // does not work !
#define SAMPLE_RATE_MAX             15

#define F_MAX                       3700000000
#define F_MIN                       1200000

#define TEMPMON_ROOMTEMP 25.0f
// out of the nine implemented AM detectors, only
// two proved to be of acceptable quality:
// AM2 and AM_ME2
// however, SAM outperforms all demodulators;-)
#define DEMOD_MIN                   0
#define DEMOD_USB                   0
#define DEMOD_LSB                   1
#define DEMOD_AM2                   2
#define DEMOD_SAM                   3         // synchronous AM demodulation
#define DEMOD_MAX                   3

#define DEMOD_IQ                    4
#define DEMOD_DCF77                 29        // set the clock with the time signal station DCF77
#define DEMOD_AUTOTUNE              5         // AM demodulation with autotune function

#define DEMOD_AM_ME2                26
#define DEMOD_SAM_USB               35
#define DEMOD_SAM_LSB               35
#define DEMOD_WFM                   50
#define DEMOD_SAM_STEREO            51
#define BROADCAST_BAND              0
#define HAM_BAND                    1
#define MISC_BAND                   2
#define BUFFER_SIZE                 128

//#define pos                         50        // position of spectrum display, has to be < 124
//#define pos_version                 119       // position of version number printing
//#define pos_x_tunestep              100
//#define pos_y_tunestep              119       // = pos_y_menu 91

#define NOTCHPOS                    spectrum_y + 6
#define NOTCHL                      15
#define NOTCHCOLOUR                  RA8875_YELLOW

#define TUNE_STEP_MIN               0
#define TUNE_STEP1                  0
#define TUNE_STEP2                  1
#define TUNE_STEP3                  2
#define TUNE_STEP4                  3
#define TUNE_STEP5                  4
#define TUNE_STEP_MAX               4
#define FIRST_TUNEHELP              1
#define LAST_TUNEHELP               3

// Menus !
#define MENU_F_HI_CUT               0
#define MENU_SPECTRUM_ZOOM          1
#define MENU_SAMPLE_RATE            2
#define MENU_SAVE_EEPROM            3
#define MENU_LOAD_EEPROM            4
#define MENU_LPF_SPECTRUM           5
#define MENU_SPECTRUM_OFFSET        6
#define MENU_SPECTRUM_DISPLAY_SCALE 7

#define MENU_IQ_AMPLITUDE           8
#define MENU_IQ_PHASE               9
#define MENU_CALIBRATION_FACTOR     10
#define MENU_CALIBRATION_CONSTANT   11
#define MENU_TIME_SET               12
#define MENU_RESET_CODEC            13
#define MENU_SHOW_SPECTRUM          14

#define FIRST_MENU                  0
#define LAST_MENU                   14
#define START_MENU                  0

#define MENU_RF_GAIN                15
#define MENU_RF_ATTENUATION         16
#define MENU_BASS                   17
#define MENU_MIDBASS                18
#define MENU_MID                    19
#define MENU_MIDTREBLE              20
#define MENU_TREBLE                 21
#define MENU_SAM_ZETA               22
#define MENU_SAM_OMEGA              23
#define MENU_SAM_CATCH_BW           24
#define MENU_NOTCH_1                25
#define MENU_NOTCH_1_BW             26
#define MENU_AGC_MODE               27
#define MENU_AGC_THRESH             28
#define MENU_AGC_DECAY              29
#define MENU_AGC_SLOPE              30
#define MENU_ANR_NOTCH              31
#define MENU_ANR_TAPS               32
#define MENU_ANR_DELAY              33
#define MENU_ANR_MU                 34
#define MENU_ANR_GAMMA              35
#define MENU_NB_THRESH              36
#define MENU_NB_TAPS                37
#define MENU_NB_IMPULSE_SAMPLES     38
#define MENU_BIT_NUMBER             39
#define MENU_F_LO_CUT               40
#define MENU_NR_PSI                 41
#define MENU_NR_ALPHA               42
#define MENU_NR_BETA                43
#define MENU_NR_USE_X               44
#define MENU_NR_USE_KIM             45

#define MENU_LMS_NR_STRENGTH        46
#define MENU_CPU_SPEED              47
#define MENU_USE_ATAN2              48
#define MENU_NR_KIM                 49

#define MENU_NR_SP                  50
#define MENU_NR_LMS1                51
#define MENU_NR_LMS2                52
#define MENU_NR_OFF                 53

#define FIRST_MENU2                 15
#define LAST_MENU2                  53

// AGC
#define AGC_OPTIONS                 6                 // Six options, 0 - 5
#define MAX_SAMPLE_RATE             (24000.0)
#define MAX_N_TAU                   (8)
#define MAX_TAU_ATTACK              (0.01)
#define RB_SIZE                     (int) (MAX_SAMPLE_RATE * MAX_N_TAU * MAX_TAU_ATTACK + 1)

#define RTTYuartFullTime            10
#define RTTYuartHalfTime            6

#define LFCODE                      10
#define CRCODE                      13
#define UU                          'y'

#define CONFIG_VERSION "mr1"  //mdrhere ID of the E settings block, change if structure changes
#define CONFIG_START 0                      // Address start the EEPROM data. (emulated with size of 4K. Actual address managed in library)

#define TERMCHRXWIDTH               9       // print stuff for text terminal
#define TERMCHRYWIDTH               10
#define TERMNROWS                   4  // 15 
#define TERMNCOLS                   28 // 34 
#define CW_X_START                  spectrum_x + 2 // 1
#define CW_Y_START                  spectrum_y - 1 // 55

#ifdef USE_W7PUA

#define BAND_80M                  0
#define BAND_40M                  1
#define BAND_20M                  2
#define BAND_17M                  3
#define BAND_15M                  4
#define BAND_12M                  5
#define BAND_10M                  6

#define FIRST_BAND                BAND_80M
#define LAST_BAND                 BAND_10M    //AFP 1-28-21
#define NUM_BANDS                 7           //AFP 1-28-21
#define STARTUP_BAND              BAND_40M    //AFP 1-28-21

#endif



//=== CW Filter ===

//------------------------- Global CW Filter declarations ----------

extern arm_biquad_cascade_df2T_instance_f32 S1_CW_Filter;
extern float32_t CW_Filter_state[];
#define IIR_CW_ORDER 8
#define IIR_CW_NUMSTAGES   4
extern float32_t CW_Filter_Coeffs[];

//=== end CW Filter ===

#define DISPLAY_S_METER_DBM       0
#define DISPLAY_S_METER_DBMHZ     1
#define N2                        100

#define YTOP_LEVEL_DISP 73

#define ADC_BAR                     10      // ADC Bar on left, DAC bar on right
#define DAC_BAR                     100
#define ANR_DLINE_SIZE              512     //funktioniert nicht, 128 & 256 OK 
#define MAX_LMS_TAPS                96
#define MAX_LMS_DELAY               256
#define NR_FFT_L                    256
#define DISPLAY_S_METER_DBM         0
#define DISPLAY_S_METER_DBMHZ       1
#define NB_FFT_SIZE                 FFT_LENGTH/2
#define SAM_PLL_HILBERT_STAGES      7
#define BIG_SAM_PLL_HILBERT_STAGES  (3 * SAM_PLL_HILBERT_STAGES + 3)
#define OUT_IDX                     (3 * SAM_PLL_HILBERT_STAGES)

#define TABLE_SIZE_64               64
#define EEPROM_BASE_ADDRESS         0U


//================== Global CW Correlation and FFT Variables =================
extern float32_t corrResult;  //AFP 02-02-22
extern uint32_t corrResultIndex;  //AFP 02-02-22
extern float32_t sinBuffer[];    //AFP 02-02-22
extern float32_t sinBuffer2[];
extern float32_t float_Corr_Buffer[];   //AFP 02-02-22
extern float32_t aveCorrResult;   //AFP 02-02-22
extern float32_t magFFTResults[];
extern long tempSigTime;
extern int audioTemp;
extern int audioTempPrevious;
extern int filterWidth;
extern int filterWidthX;                                           // The current filter X.
extern int filterWidthY;                                           // The current filter Y.
extern float sigStart;
extern float sigDuration;
extern float gapStartData;
extern float gapDurationData;
extern int audioValuePrevious;
extern int audioPostProcessorCells[];
extern float goertzelMagnitude;
extern float32_t corrResultR;  //AFP 02-02-22
extern uint32_t corrResultIndexR;  //AFP 02-02-22
extern float32_t corrResultL;  //AFP 02-02-22
extern uint32_t corrResultIndexL;  //AFP 02-02-22
extern float32_t aveCorrResult;   //AFP 02-02-22
extern float32_t aveCorrResultR;   //AFP 02-06-22
extern float32_t aveCorrResultL;   //AFP 02-06-22
extern float32_t float_Corr_BufferR[];   //AFP 02-06-22
extern float32_t float_Corr_BufferL[];   //AFP 02-06-22
extern float32_t combinedCoeff;//AFP 02-06-22
extern int CWCoeffLevelOld;
extern float CWLevelTimer;
extern float CWLevelTimerOld;
extern float32_t combinedCoeff2;
extern float32_t combinedCoeff2Old;
extern float ticMarkTimer;
extern float ticMarkTimerOld;
extern int CWOnState;  //AFP 05-17-22
extern long CWFreqShift; //AFP 05-17-22

//===== New histogram stuff

extern int topDitIndex;  //AFP 02-20-22
extern int topDitIndexOld;
extern int endDitFlag;
extern int topGapIndex;
extern int topGapIndexOld;
extern int endGapFlag;

extern int32_t signalHistogram[];
extern int32_t gapHistogram[];
extern uint32_t histMaxIndexDitOld;
extern uint32_t histMaxIndexDahOld;
extern uint32_t histMaxDit;
extern uint32_t histMaxIndexDit;
extern uint32_t histMaxDah;
extern uint32_t histMaxIndexDah;
extern   float32_t pixel_per_khz  ; //AFP
extern   int pos_left ;
extern   int centerLine;
extern   int filterWidth;
extern int h;
extern int atomGapLength;
extern int atomGapLength2;
extern int charGapLength;
extern int charGapLength2;

extern long valRef1;
extern long valRef2;
extern long gapRef1;
extern int valFlag;
extern long signalStartOld;
extern int valCounter;
extern long aveDitLength;
extern long aveDahLength;
extern float thresholdGeometricMean;
extern float thresholdArithmeticMean;
extern float aveAtomGapLength;
extern float thresholdGapGeometricMean;
extern float thresholdGapArithmeticMean;

extern long notchFreq;
extern long notchPosOld;
// ============ end new stuff =======


//================== Global Excite Variables =================


#define IIR_ORDER 8
#define IIR_NUMSTAGES (IIR_ORDER / 2)
extern float32_t coeffs192K_10K_LPF_FIR[];
extern float32_t coeffs48K_8K_LPF_FIR[];
extern const uint32_t N_B_EX;

extern float32_t EQ_Band1Coeffs[];
extern float32_t EQ_Band2Coeffs[];
extern float32_t EQ_Band3Coeffs[];
extern float32_t EQ_Band4Coeffs[];
extern float32_t EQ_Band5Coeffs[];
extern float32_t EQ_Band6Coeffs[];
extern float32_t EQ_Band7Coeffs[];
extern float32_t EQ_Band8Coeffs[];
extern float32_t EQ_Band9Coeffs[];
extern float32_t EQ_Band10Coeffs[];
extern float32_t EQ_Band11Coeffs[];
extern float32_t EQ_Band12Coeffs[];
extern float32_t EQ_Band13Coeffs[];
extern float32_t EQ_Band14Coeffs[];

extern float32_t FIR_Hilbert_coeffs90[];
extern float32_t FIR_Hilbert_coeffs0[];

extern int NumExBlocks ;
extern float32_t EQ_Band1_state[];
extern float32_t EQ_Band2_state[] ;
extern float32_t EQ_Band3_state[];
extern float32_t EQ_Band4_state[];
extern float32_t EQ_Band5_state[];
extern float32_t EQ_Band6_state[];
extern float32_t EQ_Band7_state[];
extern float32_t EQ_Band8_state[];
extern float32_t EQ_Band9_state[] ;
extern float32_t EQ_Band10_state[];
extern float32_t EQ_Band11_state[];
extern float32_t EQ_Band12_state[];
extern float32_t EQ_Band13_state[];
extern float32_t EQ_Band14_state[];



extern float EQBand1GaindB ;
extern float EQBand2GaindB ;
extern float EQBand3GaindB ;
extern float EQBand4GaindB;
extern float EQBand5GaindB ;
extern float EQBand6GaindB;
extern float EQBand7GaindB ;
extern float EQBand8GaindB ;
extern float EQBand9GaindB ;
extern float EQBand10GaindB ;
extern float EQBand11GaindB;
extern float EQBand12GaindB ;
extern float EQBand13GaindB;
extern float EQBand14GaindB;

extern float EQBand1Scale;
extern float EQBand2Scale;
extern float EQBand3Scale;
extern float EQBand4Scale ;
extern float EQBand5Scale ;
extern float EQBand6Scale;
extern float EQBand7Scale;
extern float EQBand8Scale;
extern float EQBand9Scale;
extern float EQBand10Scale;
extern float EQBand11Scale ;
extern float EQBand12Scale ;
extern float EQBand13Scale;
extern float EQBand14Scale;


extern arm_biquad_cascade_df2T_instance_f32 S1_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S2_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S3_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S4_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S5_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S6_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S7_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S8_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S9_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S10_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S11_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S12_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S13_EXcite ;
extern arm_biquad_cascade_df2T_instance_f32 S14_EXcite ;


//static uint32_t filtBlockSize = BUFFER_SIZE;

extern float32_t float_buffer_L1_EX [];
extern float32_t float_buffer_L2_EX [];
extern float32_t float_buffer_L3_EX [];
extern float32_t float_buffer_L4_EX [];
extern float32_t float_buffer_L5_EX [];
extern float32_t float_buffer_L6_EX [];
extern float32_t float_buffer_L7_EX [];
extern float32_t float_buffer_L8_EX [];
extern float32_t float_buffer_L9_EX [];
extern float32_t float_buffer_L10_EX [];
extern float32_t float_buffer_L11_EX [];
extern float32_t float_buffer_L12_EX [];
extern float32_t float_buffer_L13_EX [];
extern float32_t float_buffer_L14_EX [];



//Hilbert FIR Filter

extern float32_t FIR_Hilbert_state_L [];
extern float32_t FIR_Hilbert_state_R [];

extern float32_t FIR_Hilbert_coeffs_45[];      //AFP 01-16-22
extern float32_t FIR_Hilbert_coeffs_neg45[];  //AFP 01-16-22
extern arm_fir_instance_f32 FIR_Hilbert_L;
extern arm_fir_instance_f32 FIR_Hilbert_R;


extern arm_fir_decimate_instance_f32 FIR_dec1_EX_I;
extern arm_fir_decimate_instance_f32 FIR_dec1_EX_Q;
extern arm_fir_decimate_instance_f32 FIR_dec2_EX_I;
extern arm_fir_decimate_instance_f32 FIR_dec2_EX_Q;

extern arm_fir_interpolate_instance_f32 FIR_int1_EX_I;
extern arm_fir_interpolate_instance_f32 FIR_int1_EX_Q;
extern arm_fir_interpolate_instance_f32 FIR_int2_EX_I;
extern arm_fir_interpolate_instance_f32 FIR_int2_EX_Q;


extern float32_t FIR_dec1_EX_I_state[];    //48 + (uint16_t) BUFFER_SIZE * (uint32_t) N_B - 1
extern float32_t FIR_dec1_EX_Q_state[];
//extern float32_t  FIR_dec2_EX_coeffs[];    //n_dec1_taps

extern float32_t  FIR_dec2_EX_I_state[];     //DEC2STATESIZE
//extern float32_t  FIR_dec2_EX_coeffs[];
extern float32_t  FIR_dec2_EX_Q_state[];

extern float32_t  FIR_int2_EX_I_state[];
extern float32_t  FIR_int2_EX_Q_state[];
extern float32_t  FIR_int1_EX_coeffs[];
extern float32_t  FIR_int2_EX_coeffs[];

extern float32_t  FIR_int1_EX_I_state[];
extern float32_t  FIR_int1_EX_Q_state[];

extern float32_t  float_buffer_L_EX[];
extern float32_t  float_buffer_R_EX[];
extern float32_t  float_buffer_LTemp[];
extern float32_t  float_buffer_RTemp[];

void ExciterIQData();

//==================== End Excite Variables ================================

//======================================== Global object declarations ==================================================
extern float32_t NCO_FREQ ; // AFP 04-16-22
//extern float32_t NCO_FREQ  //LSB
extern double stepFT;
extern double stepFT2;
extern float32_t NCO_INC ;  // AFP 04-16-22
extern double OSC_COS;
extern double  OSC_SIN;
extern double  Osc_Vect_Q;
extern double  Osc_Vect_I ;
extern double  Osc_Gain ;
extern double  Osc_Q ;
extern double  Osc_I;
extern float32_t i_temp;
extern float32_t q_temp;
//extern float32_t Osc2_Q_buffer [BUFFER_SIZE* N_BLOCKS];
//extern float32_t Osc2_I_buffer [BUFFER_SIZE* N_BLOCKS];
//======================================

//======================================== Global object declarations ==================================================
extern AudioMixer4            recMix_3;         // JJP
//extern AudioSynthWaveformSine sine1;            // JJP Sidetone generator for CW monitoring

extern AudioInputI2SQuad        i2s_quadIn;
extern AudioOutputI2SQuad       i2s_quadOut;

extern SPISettings          settingsA;
extern AudioControlSGTL5000 sgtl5000_1;
extern AudioControlSGTL5000 sgtl5000_2;
//extern AudioInputI2S        i2s_in;

extern AudioMixer4          modeSelectInR;
extern AudioMixer4          modeSelectInL;
extern AudioMixer4          modeSelectOutR;
extern AudioMixer4          modeSelectOutL;


extern AudioMixer4          modeSelectOutL;
extern AudioMixer4          modeSelectOutR;

extern AudioMixer4          modeSelectOutExL;
extern AudioMixer4          modeSelectOutExR;
//extern AudioOutputI2S       i2s_out;
extern AudioPlayQueue       Q_out_L;
extern AudioPlayQueue       Q_out_R;
extern AudioRecordQueue     Q_in_L;
extern AudioRecordQueue     Q_in_R;

//extern AudioPlayQueue       Q_out_L;
//extern AudioPlayQueue       Q_out_R;
//extern AudioRecordQueue     Q_in_L;
//extern AudioRecordQueue     Q_in_R;

//extern AudioRecordQueue     Q_in_L;
//extern AudioRecordQueue     Q_in_R;
//extern AudioMixer4          inputleft;
//extern AudioMixer4          inputright;
// Added hardware: A pair of 100K resistors connected to the L&R input pins of the Codec.
// These two resisors are connected together on the other end and go through a 2.2nF cap
// to the A21 DAC pin on the Teensy 3.6.
#ifdef USE_W7PUA
//extern AudioPlayQueue       queueTP;            // Output packets for Twin Peak test
extern AudioOutputAnalog    dac1;               // Generate 24 KHz signal
//extern AudioConnection      patchCord21;
#endif

extern Bounce decreaseBand;
extern Bounce increaseBand;
extern Bounce modeSwitch;
extern Bounce decreaseMenu;
extern Bounce frequencyIncrement;
extern Bounce filterSwitch;
extern Bounce increaseMenu;
extern Bounce selectExitMenues;
extern Bounce changeNR;
extern Bounce demodSwitch;
extern Bounce zoomSwitch;
extern Bounce cursorSwitch;

extern Rotary tuneEncoder;
//extern Rotary fastTuneEncoder;          // 4, 5
extern Encoder fastTuneEncoder;        //(4, 5);
extern Encoder filterEncoder;
extern Encoder volumeEncoder;      //(2, 3);

extern Metro ms_500;            // Set up a Metro
extern Metro encoder_check;     // Set up a Metro

extern Si5351 si5351;
#ifndef RA8875_DISPLAY
extern ILI9488_t3 tft;
#else
extern  RA8875 tft;
#endif

//======================================== Global structure declarations ===============================================

struct secondaryMenuConfiguration {
  byte whichType;                         // 0 = no options, 1 = list, 2 = encoder value
  int numberOfOptions;                     // Number of submenu topions
};

extern struct config_t {
  char version_of_settings[7] = {'V', '2', '0', '0', '\0'}; // "V200"

  int8_t AGCMode                        = 1;  // 1 byte
  int8_t auto_IQ_correction             = 1;  // 1 byte
  int8_t RF_attenuation                 = 2;  // 1 byte

  uint8_t dcfParityBit                  = 3;  // 1 byte
  uint8_t rate                          = 4;  // 1 byte
  uint8_t NR_use_X                      = 5;  // 1 byte
  uint8_t show_spectrum_flag            = 1;  // 1 byte

  int16_t pixel_offset[NUM_BANDS];            // 14 bytes

  int16_t currentVFO                    = 1;  // 2 bytes

  int agc_thresh                        = 0;  // 4 bytes
  int agc_decay                         = 0;  // 4 bytes
  int agc_slope                         = 0;  // 4 bytes
  int AGC_thresh[NUM_BANDS];                  // 4 bytes
  int audioPostProc[AUDIO_POST_PROCESSOR_BANDS];// 32 bytes
  int audio_volume                      = 0;  // 4 bytes

  int bwu[NUM_BANDS];                         // 4 bytes
  int bwl[NUM_BANDS];                         // 4 bytes

  int currentBand                       = 1;  // 4 bytes
  int dcfCount                          = 0;  // 4 bytes
  int dcfLevel                          = 0;  // 4 bytes
  int dcfSilenceTimer                   = 0;  // 4 bytes
  int dcfTheSecond                      = 0;  // 4 bytes
  int dcfPulseTime                      = 0;  // 4 bytes
  int equalizerRec[EQUALIZER_CELL_COUNT];
  int equalizerXmt[EQUALIZER_CELL_COUNT];
  int freqIncrement                     = 0;  // 4 bytes
  int keyType                           = 0;  // straight key = 0, keyer = 1
  int keyerSidetone                     = 700;// Hz
  int mode[NUM_BANDS];                        // 4 bytes
  int NR_Filter_Value;
  int paddleDah                         = 0;  // 4 bytes
  int paddleDit                         = 1;  // 4 bytes
  int powerLevel                        = 8;  // 4 bytes
  int rfg[NUM_BANDS];                         // 4 bytes

  int spectrumNoiseFloor                = SPECTRUM_NOISE_FLOOR;  // 4 bytes

  int switchValues[NUMBER_OF_SWITCHES];       // 64 bytes

  int wordsPerMinute                    = 15; // 4 bytes
  int zetaHelp                          = 65; // 4 bytes

  uint16_t crc                          = 100;// 2 bytes, added when saving
  uint16_t currentScale                 = 1;  // 2 bytes,

  int32_t spectrum_zoom                 = spectrum_zoom; // 4 bytes

  long calibration_constant             = 0;  // 4 bytes
  long centerFreq                       = 7150000L;// 4 bytes
  long currentFreqA                     = 7150000L;// 4 bytes
  long currentFreqB                     = 7030000L;// 4 bytes
  long favoriteFreqs[MAX_FAVORITES];          // 40 bytes
  long frequencyOffset                  = 0L; // 4 bytes

  unsigned long calibration_factor      = 1;  // 8 bytes

  float bitSamplePeriod                 = 1.0 / 500.0;      // 4 bytes
  float bitSampleTimer                  = 0;  // 4 bytes
  float dcfMean                         = 0.0;// 4 bytes
  float dcfSum                          = 0.0;// 4 bytes
  float dcfRefLevel                     = 0.0;// 4 bytes
  float Tsample                         = 1.0 / 12000.0;    // 4 bytes

  float32_t bass                        = 0.0;// 4 bytes
  float32_t LPFcoeff                    = 0.0;// 4 bytes
  float32_t micCompression              = 0.0;// 4 bytes
  float32_t midbass                     = 0.0;// 4 bytes
  float32_t mid                         = 0.0;// 4 bytes
  float32_t midtreble                   = 0.0;// 4 bytes
  float32_t NR_PSI                      = 0.0;// 4 bytes
  float32_t NR_alpha                    = 0.0;// 4 bytes
  float32_t NR_beta                     = 0.0;// 4 bytes
  float32_t offsetDisplayDB             = 0.0;// 4 bytes
  float32_t omegaN                      = 0.0;// 4 bytes
  float32_t pll_fmax                    = 4000.0;  // 4 bytes
  float32_t spectrum_display_scale      = 0.0;// 4 bytes
  float32_t stereo_factor               = 0.0;// 4 bytes
  float32_t treble                      = 0.0;// 4 bytes
  float32_t IQ_AmpCorFactor             = 1.0;
  float32_t IQ_PhaseCorFactor           = 0.0;
  float32_t IQ_XAmpCorFactor            = 1.0;
  float32_t IQ_XPhanseCorFactor         = 0.0;

} EEPROMData;                                 //  Total:       430 bytes

//extern struct config_t EEPROMData;

typedef struct SR_Descriptor
{
  const uint8_t SR_n;
  const uint32_t rate;
  const char* const text;
  const char* const f1;
  const char* const f2;
  const char* const f3;
  const char* const f4;
  const float32_t x_factor;
  const uint8_t x_offset;
} SR_Desc;
extern const struct SR_Descriptor SR[];

extern const arm_cfft_instance_f32 *S;
extern const arm_cfft_instance_f32 *iS;
extern const arm_cfft_instance_f32 *maskS;
extern const arm_cfft_instance_f32 *NR_FFT;
extern const arm_cfft_instance_f32 *NR_iFFT;
extern const arm_cfft_instance_f32 *spec_FFT;

extern arm_biquad_casd_df1_inst_f32 biquad_lowpass1;
extern arm_biquad_casd_df1_inst_f32 IIR_biquad_Zoom_FFT_I;
extern arm_biquad_casd_df1_inst_f32 IIR_biquad_Zoom_FFT_Q;

extern arm_fir_decimate_instance_f32 FIR_dec1_I;
extern arm_fir_decimate_instance_f32 FIR_dec1_Q;
extern arm_fir_decimate_instance_f32 FIR_dec2_I;
extern arm_fir_decimate_instance_f32 FIR_dec2_Q;
extern arm_fir_decimate_instance_f32 Fir_Zoom_FFT_Decimate_I;
extern arm_fir_decimate_instance_f32 Fir_Zoom_FFT_Decimate_Q;
extern arm_fir_interpolate_instance_f32 FIR_int1_I;
extern arm_fir_interpolate_instance_f32 FIR_int1_Q;
extern arm_fir_interpolate_instance_f32 FIR_int2_I;
extern arm_fir_interpolate_instance_f32 FIR_int2_Q;
extern arm_lms_norm_instance_f32 LMS_Norm_instance;
extern arm_lms_instance_f32      LMS_instance;
extern elapsedMicros usec;

struct band {
  unsigned long freq;      // Current frequency in Hz * 100
  unsigned long fBandLow;  // Lower band edge
  unsigned long fBandHigh; // Upper band edge
  const char* name; // name of band
  int mode;
  int FHiCut;
  int FLoCut;
  int RFgain;
  uint8_t band_type;
  float32_t gainCorrection; // is hardware dependent and has to be calibrated ONCE and hardcoded in the table below
  int AGC_thresh;
  int16_t pixel_offset;
};
extern struct band bands[];

typedef struct DEMOD_Descriptor
{ const uint8_t DEMOD_n;
  const char* const text;
} DEMOD_Desc;
extern const DEMOD_Descriptor DEMOD[];

struct dispSc
{
  const char *dbText;
  float32_t   dBScale;
  uint16_t    pixelsPerDB;
  uint16_t    baseOffset;
  float32_t   offsetIncrement;
};

extern struct dispSc displayScale[];

typedef struct Menu_Descriptor
{
  const uint8_t no;           // Menu ID
  const char* const text1;    // upper text
  const char* text2;          // lower text
  const uint8_t menu2;        // 0 = belongs to Menu, 1 = belongs to Menu2
} Menu_D;
extern Menu_D Menus[];

//======================================== Global variables declarations ===============================================
//========================== Some are not in alpha order because of forward references =================================


extern  int kTemp;
extern int timerFlag;

extern bool gEEPROM_current;            //mdrhere does the data in EEPROM match the current structure contents
extern bool NR_gain_smooth_enable;
extern bool NR_long_tone_reset;
extern bool NR_long_tone_enable;
extern bool omitOutputFlag;
extern bool timeflag;

extern char *bigMorseCodeTree;
extern char decodeBuffer[];
extern char *morseCodeTree;
extern char letterTable[];
extern char numberTable[];
extern char punctuationTable[];
extern char theversion[];

extern const char *topMenus[];
extern const char *zoomOptions[];

extern byte currentDashJump;
extern byte currentDecoderIndex;

extern int8_t AGCMode;
extern int8_t auto_IQ_correction;
extern uint8_t IQ_RecCalFlag; //AFP 04-17-22
extern int8_t first_block;
extern int8_t Menu2;
extern int8_t mesz;
extern int8_t mesz_old;
extern int8_t NB_taps;
extern int8_t NB_impulse_samples;
extern int8_t NR_first_block;
extern int8_t RF_attenuation;
extern int8_t xmtMode;

extern uint8_t agc_action;
extern uint8_t agc_switch_mode;
extern uint8_t ANR_on;
extern uint8_t ANR_notch;
extern uint8_t ANR_notchOn;
extern uint8_t atan2_approx;
extern uint8_t auto_codec_gain;
extern uint8_t audio_flag;
extern uint8_t autotune_flag;
extern uint8_t autotune_wait;
extern uint8_t bitnumber;                             // test, how restriction to twelve bit alters sound quality
extern uint8_t codec_restarts;
extern uint8_t dbm_state;
extern uint8_t dcfParityBit;
extern uint8_t decay_type;
extern uint8_t digits_old [][10];
extern uint8_t display_dbm;
extern uint8_t display_S_meter_or_spectrum_state;
extern uint8_t eeprom_saved;
extern uint8_t eeprom_loaded;
extern uint8_t erase_flag;
extern uint8_t fade_leveler;
extern uint8_t FIR_filter_window;
extern uint8_t flagg;
extern uint8_t freq_flag[];
extern uint8_t half_clip;
extern uint8_t hang_enable;
extern uint8_t hour10_old;
extern uint8_t hour1_old;
extern uint8_t IQCalFlag;
extern uint8_t iFFT_flip;
extern uint8_t IQ_state;
extern uint8_t LastSampleRate;
extern uint8_t minute10_old;
extern uint8_t minute1_old;
extern uint8_t NB_on;
extern uint8_t NB_test;
extern uint8_t notchIndex;
extern uint8_t notchButtonState;
extern uint8_t notches_on[];
extern uint8_t NR_first_time;
extern uint8_t NR_Kim;
extern uint8_t NR_LMS;
extern uint8_t NR_Spect;
extern uint8_t NR_use_X;
extern uint8_t NR_VAD_enable;
extern uint8_t precision_flag;
extern uint8_t quarter_clip;
extern uint8_t SampleRate;
extern uint8_t save_energy;
extern uint8_t sch;
extern uint8_t second10_old;
extern uint8_t second1_old;
extern uint8_t show_spectrum_flag;
extern uint8_t spectrum_mov_average;
extern uint8_t state;
extern uint8_t tune_stepper;

extern uint8_t twinpeaks_tested;                      // initial value --> 2 !!
extern uint8_t wait_flag;
extern uint8_t WDSP_SAM;
extern uint8_t which_menu;
extern uint8_t write_analog_gain;
extern uint8_t zoom_display;

extern const uint8_t NR_L_frames;
extern const uint8_t NR_N_frames;

extern int16_t activeVFO;
extern int16_t currentMode;
extern int16_t displayMode;
extern int16_t EqualizerRecValues[];
extern int16_t EqualizerXmtValues[];
extern int16_t  fineEncoderRead;
/*
  extern int16_t DMAMEM pixelnew[];
  extern int16_t DMAMEM pixelold[];
  extern int16_t DMAMEM pixelnew2[];
  extern int16_t DMAMEM pixelold2[];
*/
extern int16_t  pixelnew[];
extern int16_t  pixelold[];
extern int16_t  pixelnew2[];    //AFP
extern int16_t  pixelold2[];

extern int16_t notch_L[];
extern int16_t notch_R[];
extern int16_t notch_pixel_L[];
extern int16_t notch_pixel_R[];
extern int16_t offsetPixels;
//extern int16_t pixelnew[];
extern int16_t pixelold[];
extern int16_t pos_x_dbm;
extern int16_t pos_y_dbm;
extern int16_t pos_y_db;
extern int16_t pos_y_frequency;
extern int16_t pos_x_time;         // 14;
extern int16_t pos_y_time;
extern int16_t s_w;
extern int16_t *sp_L;
extern int16_t *sp_R;
extern int16_t spectrum_brightness;
extern int16_t spectrum_height;
extern int16_t spectrum_pos_centre_f;
extern int16_t spectrum_WF_height;
extern int16_t spectrum_x;
extern int16_t spectrum_y;
extern int16_t y_old, y_new, y1_new, y1_old, y_old2;
extern int16_t y1_old_minus ;
extern int16_t y1_new_minus ;

extern uint16_t adcMaxLevel, dacMaxLevel;
extern uint16_t autotune_counter;
extern uint16_t base_y;
extern uint16_t barGraphUpdate;
extern uint16_t currentScale;
extern uint16_t notches_BW[];
extern uint16_t SAM_display_count;
extern uint16_t SpectrumDeleteColor;
extern uint16_t SpectrumDrawColor;
extern uint16_t uAfter;
extern uint16_t uB4;
extern uint16_t xx;

extern const uint16_t gradient[];

extern const uint32_t IIR_biquad_Zoom_FFT_N_stages;
extern const uint32_t N_stages_biquad_lowpass1;
extern const uint16_t n_dec1_taps;
extern const uint16_t n_dec2_taps;

extern int encoderStepOld;
extern int resultOldFactor;
extern float incrFactor;
extern int audio_volumeOld;
extern int mute;
extern int exciteOn;
extern int agc_decay;
extern int agc_slope;
extern int agc_thresh;
extern int ANR_buff_size;
extern int ANR_delay;
extern int ANR_dline_size;
extern int ANR_in_idx;
extern int ANR_mask;
extern int ANR_position;
extern int ANR_taps;
extern int attack_buffsize;
extern int audio_volume;
extern int audioYPixel[];
extern int bandswitchPins[];
extern int button9State;
extern int buttonRead;
extern int cwSidetone;
extern int currentBand;
extern int dahLength;
extern int dcfCount;
extern int dcfLevel;
extern int dcfSilenceTimer;
extern int dcfTheSecond;
extern int dcfPulseTime;
extern int decoderFlag;
extern int demodIndex;
extern int ditLength;
extern int EEPROMChoice;
extern int equalizerRecChoice;
extern int equalizerXmtChoice;
extern int fastTuneActive;
extern int filterLoPositionMarkerOld; // AFP 03-27-22 Layers
extern int filterHiPositionMarkerOld;// AFP 03-27-22 Layers
extern int freqCalibration;
extern int freqIncrement;
extern int FLoCutOld;
extern int FHiCutOld;
extern int (*functionPtr[])();
extern int gapAtom;                                  //Space between atoms
extern int gapChar;                                  // Space between characters

extern int hang_counter;
extern int helpmin;
extern int helphour;
extern int helpday;
extern int helpmonth;
extern int helpyear;
extern int helpsec;
extern int idx, idpk;
extern int IQChoice;
extern int lidx, hidx;
extern int LMS_nr_strength;
extern int LP_F_help;
extern int micChoice;
extern int minPinRead;
extern int NR_Filter_Value;
extern int operatingMode;
extern int n_L;
extern int n_R;
extern int n_tau;
extern int NBChoice;
extern int newCursorPosition;
extern int nrOptionSelect;
extern int NR_Choice;
extern int NR_KIM_K_int;
extern int NR_VAD_delay;
extern int NR_VAD_duration;
extern int NR_Index;
extern int old_demod_mode;
extern int oldnotchF;
extern int oldCursorPosition;
extern int out_index;
extern int paddleDah;
extern int paddleDit;
extern int pmode;
extern int pos_centre_f;
extern int pos_x_frequency;
extern int pos_y_smeter;
extern int SAM_AM_Choice;
extern int secondaryMenuChoiceMade;
extern int smeterLength;
extern int spectrumNoiseFloor;
extern int SSB_AUTOTUNE_counter;
extern int stepFTOld;
extern int switchFilterSideband;    //AFP 1-28-21
extern int switchThreshholds[];
extern int termCursorXpos;
extern int tuneIndex;
//extern int tunestep;
extern int x2;                      //AFP
extern int zeta_help;
extern int zoom_sample_ptr;
extern int zoomIndex;

extern int updateDisplayFlag;

extern const int DEC2STATESIZE;
extern const int INT1_STATE_SIZE;
extern const int INT2_STATE_SIZE;
extern const int myInput;
extern const int pos_x_smeter;
extern const int waterfallBottom;
extern const int waterfallTop;
extern int wtf;

extern unsigned framesize;
extern unsigned nbits;
extern unsigned ring_buffsize;
extern unsigned tcr5;
extern unsigned tcr2div;


extern int32_t FFT_shift;
extern int32_t IFFreq;     // IF (intermediate) frequency
extern int32_t IF_FREQ1;
extern int32_t O_iiSum19;
extern int32_t O_integrateCount19;
extern int32_t mainMenuIndex;
extern int32_t subMenuMaxOptions;
extern int32_t secondaryMenuIndex;                           // -1 means haven't determined secondary menu
extern int32_t spectrum_zoom;

extern const uint32_t N_B;
extern const uint32_t N_DEC_B;
extern const uint32_t NR_add_counter;

extern uint32_t BUF_N_DF;
extern uint32_t FFT_length;
//extern const uint32_t FFT_L ;
extern int32_t gapHistogram[];
extern uint32_t in_index;
extern uint32_t IQ_counter;
extern uint32_t MDR;
extern uint32_t N_BLOCKS;
extern uint32_t n_para;
extern uint32_t NR_X_pointer;
extern uint32_t NR_E_pointer;
extern uint32_t m_NumTaps;
extern uint32_t roomCount;   /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at the hot temperature.*/
extern uint32_t s_hotTemp;    /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at room temperature .*/
extern uint32_t s_hotCount;   /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at the hot temperature.*/
extern uint32_t s_roomC_hotC; /*!< The value of s_roomCount minus s_hotCount.*/
extern uint32_t T4_CPU_FREQUENCY;
extern uint32_t twinpeaks_counter;

extern long averageDit;
extern long averageDah;
extern long calibration_constant;
extern long currentFreq;
extern long centerFreq;
extern long TxRxFreq;       // = centerFreq+NCOFreq  NCOFreq from FreqShift2()
extern long recClockFreq;  //  = TxRxFreq+IFFreq  IFFreq from FreqShift1()=48KHz
extern long CWRecFreq;     //  = TxRxFreq +/- 700Hz
extern long currentFreqA;;
extern long currentFreqB;
extern long currentWPM;
extern long frequencyCorrection;
extern long lastWPM;
extern long notchCenterBin;
extern long int n_clear;
extern long startTime;
extern long signalElapsedTime;
extern long spaceSpan;
extern long signalStart;
extern long signalEnd;                     // Start-end of dit or dah
extern long spaceStart;
extern long spaceEnd;
extern long spaceElapsedTime;

extern long gapEnd, gapLength, gapStart;                            // Time for noise measures
extern long ditTime, dahTime;                          // Assume 15wpm to start

extern ulong samp_ptr;
extern unsigned long currentFreqs[];

extern uint64_t output12khz;

extern unsigned long long calibration_factor;
extern unsigned long long hilfsf;
extern unsigned long long hilfsfEx;

float32_t arm_atan2_f32(float32_t y, float32_t x);
float ApproxAtan(float z);
float ApproxAtan2(float y, float x);

extern float dcfRefLevel;
extern float DD4WH_RF_gain;
extern float s_hotT_ROOM;     /*!< The value of s_hotTemp minus room temperature(25¡æ).*/

extern float32_t a[];
extern float32_t abs_ring[];
extern float32_t abs_out_sample;
extern float32_t ai, bi, aq, bq;
extern float32_t ai_ps, bi_ps, aq_ps, bq_ps;
extern float32_t ANR_d [];
extern float32_t ANR_den_mult;
extern float32_t ANR_gamma;
extern float32_t ANR_lidx;
extern float32_t ANR_lidx_min;
extern float32_t ANR_lidx_max;
extern float32_t ANR_lincr;
extern float32_t ANR_ldecr;
extern float32_t ANR_ngamma;
extern float32_t ANR_two_mu;
extern float32_t ANR_w [];
extern float32_t attack_mult;
extern float32_t audio;
extern float32_t audiotmp;
extern float32_t audiou;
extern float32_t audioSpectBuffer[];
extern float32_t b[];
extern float32_t bass;
extern float32_t bin_BW;
extern float32_t bin;
extern float32_t biquad_lowpass1_state[];
extern float32_t biquad_lowpass1_coeffs[];
extern float32_t /*DMAMEM*/ buffer_spec_FFT[];
extern float32_t c[];
extern float32_t c0[];
extern float32_t c1[];
extern float32_t coefficient_set[];
extern float32_t corr[];
extern float32_t Cos;
extern float32_t CPU_temperature ;
extern float32_t cursorIncrementFraction;
extern float32_t d[];
extern float32_t dc;
extern float32_t dc_insert;
extern float32_t dcu;
extern float32_t dc_insertu;
extern float32_t dbm;
extern float32_t dbm_calibration;
extern float32_t dbm_old;
extern float32_t dbmhz;
extern float32_t decay_mult;
extern float32_t display_offset;
extern float32_t /*DMAMEM*/ FFT_buffer[];
extern float32_t /*DMAMEM*/ FFT_spec[];
extern float32_t /*DMAMEM*/ FFT_spec_old[];
extern float32_t dsI;
extern float32_t dsQ;
extern float32_t fast_backaverage;
extern float32_t fast_backmult;
extern float32_t fast_decay_mult;
extern float32_t det;
extern float32_t del_out;
extern float32_t farnsworthValue;
extern float32_t FFT_spec[];
extern float32_t FFT_spec_old[];
extern float32_t fil_out;
extern float32_t /*DMAMEM*/ FIR_Coef_I[];
extern float32_t /*DMAMEM*/ FIR_Coef_Q[];
extern float32_t /*DMAMEM*/ FIR_dec1_I_state[];
extern float32_t /*DMAMEM*/ FIR_dec2_I_state[];
extern float32_t /*DMAMEM*/ FIR_dec2_coeffs[];
extern float32_t /*DMAMEM*/ FIR_dec2_I_state[];
extern float32_t /*DMAMEM*/ FIR_dec2_Q_state[];
extern float32_t /*DMAMEM*/ FIR_int2_I_state[];
extern float32_t /*DMAMEM*/ FIR_int2_Q_state[];
extern float32_t /*DMAMEM*/ FIR_int1_coeffs[];
extern float32_t /*DMAMEM*/ FIR_int2_coeffs[];
extern float32_t /*DMAMEM*/ FIR_dec1_Q_state[];
extern float32_t /*DMAMEM*/ FIR_dec1_coeffs[];
extern float32_t /*DMAMEM*/ FIR_dec2_coeffs[];
extern float32_t /*DMAMEM*/ FIR_filter_mask[];
extern float32_t /*DMAMEM*/ FIR_int1_I_state[];
extern float32_t /*DMAMEM*/ FIR_int1_Q_state[];
extern float32_t /*DMAMEM*/ FIR_int2_I_state[];
extern float32_t /*DMAMEM*/ FIR_int2_Q_state[];
extern float32_t /*DMAMEM*/ Fir_Zoom_FFT_Decimate_I_state[];
extern float32_t /*DMAMEM*/ Fir_Zoom_FFT_Decimate_Q_state[];
extern float32_t /*DMAMEM*/ Fir_Zoom_FFT_Decimate_coeffs[];
extern float32_t fixed_gain;
extern float32_t float_buffer_L[];
extern float32_t float_buffer_R[];
extern float32_t float_buffer_L2[];
extern float32_t float_buffer_R2[];
extern float32_t g1;
extern float32_t g2;

//extern float32_t signalHistogram[];

extern float32_t hang_backaverage;
extern float32_t hang_backmult;
extern float32_t hang_decay_mult;
extern float32_t hang_thresh;
extern float32_t hang_level;
extern float32_t hangtime;
extern float32_t hh1;
extern float32_t hh2;
extern float32_t /*DMAMEM*/ iFFT_buffer[];
extern float32_t I_old;
extern float32_t I_sum;
extern float32_t IIR_biquad_Zoom_FFT_I_state[];
extern float32_t IIR_biquad_Zoom_FFT_Q_state[];
extern float32_t inv_max_input;
extern float32_t inv_out_target;
extern float32_t IQ_amplitude_correction_factor;
extern float32_t IQ_phase_correction_factor;
extern float32_t IQ_Xamplitude_correction_factor;
extern float32_t IQ_Xphase_correction_factor;
extern float32_t IQ_sum;
extern float32_t K_dirty;
extern float32_t K_est;
extern float32_t K_est_old;
extern float32_t K_est_mult;
extern float32_t last_dc_level;
extern float32_t /*DMAMEM*/ last_sample_buffer_L[];
extern float32_t /*DMAMEM*/ last_sample_buffer_R[];
extern float32_t L_BufferOffset[];
extern float32_t LMS_errsig1[];
extern float32_t LMS_NormCoeff_f32[];
extern float32_t LMS_nr_delay[];
extern float32_t LMS_StateF32[];
extern float32_t LP_Astop;
extern float32_t LP_Fpass;
extern float32_t LP_Fstop;
extern float32_t LPF_spectrum;
extern float32_t M_c1;
extern float32_t M_c2;
extern float32_t m_AttackAlpha;
extern float32_t m_AttackAvedbm;
extern float32_t m_DecayAvedbm;
extern float32_t m_DecayAlpha;;
extern float32_t m_AverageMagdbm;
extern float32_t m_AttackAvedbmhz;
extern float32_t m_DecayAvedbmhz;
extern float32_t m_AverageMagdbmhz;
extern float32_t *mag_coeffs[];
extern float32_t max_gain;
extern float32_t max_input;
extern float32_t micCompression;
extern float32_t midbass;
extern float32_t mid;
extern float32_t midtreble;
extern float32_t min_volts;
extern float32_t mtauI;
extern float32_t mtauR;
extern float32_t noiseThreshhold;
extern float32_t notches[];
extern float32_t /*DMAMEM*/ NR_FFT_buffer[];
extern float32_t NR_sum;
extern float32_t NR_PSI;
extern float32_t NR_KIM_K;
extern float32_t NR_alpha;
extern float32_t NR_onemalpha;
extern float32_t NR_beta;
extern float32_t NR_onemtwobeta;
extern float32_t NR_onembeta;
extern float32_t NR_G_bin_m_1;
extern float32_t NR_G_bin_p_1;
extern float32_t NR_T;
extern float32_t NR_output_audio_buffer [];
extern float32_t NR_last_iFFT_result [];
extern float32_t NR_last_sample_buffer_L [];
extern float32_t NR_last_sample_buffer_R [];
extern float32_t NR_X[][3];
extern float32_t NR_E[][15];
extern float32_t NR_M[];
extern float32_t NR_Nest[][2]; //
extern float32_t NR_vk;
extern float32_t NR_lambda[];
extern float32_t NR_Gts[][2];
extern float32_t NR_G[];
extern float32_t NR_SNR_prio[];
extern float32_t NR_SNR_post[];
extern float32_t NR_SNR_post_pos;
extern float32_t NR_Hk_old[];
extern float32_t NR_VAD;
extern float32_t NR_VAD_thresh;
extern float32_t NR_long_tone[][2];
extern float32_t NR_long_tone_gain[];
extern float32_t NR_long_tone_alpha;
extern float32_t NR_long_tone_thresh;
extern float32_t NR_gain_smooth_alpha;
extern float32_t NR_temp_sum;
extern float32_t NB_thresh;
extern float32_t offsetDisplayDB;
extern float32_t omega2;
extern float32_t omegaN;
extern float32_t omega_max;
extern float32_t omega_min;
extern float32_t onem_mtauI;
extern float32_t onem_mtauR;
extern float32_t onemfast_backmult;
extern float32_t onemhang_backmult;
extern float32_t out_sample[];
extern float32_t out_targ;
extern float32_t out_target;
extern float32_t P_dirty;
extern float32_t P_est;
extern float32_t P_est_old;
extern float32_t P_est_mult;
extern float32_t pll_fmax;
extern float32_t phaseLO;
extern float32_t phzerror;
extern float32_t pop_ratio;
extern float32_t Q_old;
extern float32_t Q_sum;
extern float32_t R_BufferOffset[];
extern float32_t ring[];
extern float32_t ring_max;
extern float32_t SAM_carrier;
extern float32_t SAM_lowpass;
extern float32_t SAM_carrier_freq_offset;
extern float32_t Sin;
extern float32_t sample_meanL;
extern float32_t sample_meanR;
extern float32_t sample_meanLNew;
extern float32_t sample_meanRNew;
extern float32_t save_volts;
extern float32_t slope_constant;
extern float32_t spectrum_display_scale;          // 30.0
extern float32_t stereo_factor;
extern float32_t tau_attack;
extern float32_t tau_decay;
extern float32_t tau_fast_backaverage;
extern float32_t tau_fast_decay;
extern float32_t tau_hang_backmult;
extern float32_t tau_hang_decay;
extern float32_t tauI;
extern float32_t tauR;
extern float32_t teta1;
extern float32_t teta2;
extern float32_t teta3;
extern float32_t teta1_old;
extern float32_t teta2_old;
extern float32_t teta3_old;
extern float32_t tmp;
extern float32_t treble;
extern float32_t var_gain;
extern float32_t volts;
extern float32_t w;
extern float32_t wold;
extern float32_t zeta;

extern float angl;
extern float bitSamplePeriod;
extern float bitSampleTimer;
extern float bsq, usq;
extern float cf1, cf2;
//extern float CP_buffer[];
//extern float CP_buffer_old;
extern float dcfMean;
extern float dcfSum;
extern float lolim, hilim;
extern float partr, parti;
extern float pi;
extern float tau;
extern float temp;
extern float Tsample;
extern float xExpand;//AFP
extern float x;

extern const float displayscale;
extern const float32_t nuttallWindow256[];
extern const float32_t sqrtHann[];

extern float32_t FFT_buffer [] __attribute__ ((aligned (4)));

extern float32_t float_buffer_L_3[];
extern float32_t float_buffer_R_3[];

extern const float32_t atanTable[];
extern const float32_t DF1;           // decimation factor
extern const float32_t DF2;           // decimation factor
extern const float32_t DF;            // decimation factor
extern const float32_t n_att;         // desired stopband attenuation
extern const float32_t n_desired_BW;  // desired max BW of the filters
extern const float32_t n_fpass1;
extern const float32_t n_fpass2;
extern const float32_t n_fstop1;
extern const float32_t n_fstop2;
extern const float32_t n_samplerate;  // samplerate before decimation

extern double elapsed_micros_idx_t;
extern double elapsed_micros_mean;
extern double elapsed_micros_sum;

/*                                        None in the code

  extern const double O_timeStep ;
  extern const double O_frequency19  ;
  extern const double O_dPhase19  ;
  extern const double O_cicR ;
  extern const double O_gainP ;
  extern const double O_gainI  ;
  extern const double O_limit  ;
  extern double O_phase ;
  extern double O_frequency ;
  extern double O_lastPhase ;
  extern double O_MPXsignal;
  extern double O_phase19;
  extern double vco19 ;
  extern double O_integrate19;
  extern int32_t O_integrateCount19;
  extern double O_Pcontrol;
  extern double O_Icontrol ;
  extern double O_vco19 ;
  extern int32_t O_iiSum19  ;


*/
//======================================== Function prototypes =========================================================

void AGC();
int  AGCOptions();
void AGCPrep();
float32_t AlphaBetaMag(float32_t  inphase, float32_t  quadrature);
void AltNoiseBlanking(float* insamp, int Nsam, float* E);
void AMDecodeSAM();
void AMDemodAM2();
void Autotune();
void ButtonBandDecrease();
void ButtonBandIncrease();
int  BandOptions();
int  ButtonDemod();
void ButtonDemodMode();
int  ButtonDisplayOptions();
void ButtonFilter();
void ButtonFreqIncrement();
void ButtonMenuIncrease();
void ButtonMenuDecrease();
void ButtonMode();
void ButtonNotchFilter();
void ButtonNR();
int  ButtonSetNoiseFloor();
void ButtonZoom();
                                            
void CalcZoom1Magn();
void CalcFIRCoeffs(float * coeffs_I, int numCoeffs, float32_t fc, float32_t Astop, int type, float dfc, float Fsamprate);
void CalcCplxFIRCoeffs(float * coeffs_I, float * coeffs_Q, int numCoeffs, float32_t FLoCut, float32_t FHiCut, float SampleRate);
void CalcNotchBins();
void Calculatedbm();
void CenterFastTune();
void Codec_gain();
void ControlFilterF();
int  CWOptions();
void CW_DecodeLevelDisplay();
void Dah();
void DecodeIQ();
void DisplayClock();
void DisplaydbM();
void DisplayDitLength();
void Dit();
void DoCWDecoding(int audioValue);
void DoGapHistogram(long valGap);
void DrawSignalPlotFrame();
void DoSignalHistogram(long val);
void DoSignalPlot(float val);
void DoSplitVFO();
void DoPaddleFlip();
void DrawBandWidthIndicatorBar(); // AFP 03-27-22 Layers
void DrawFrequencyBarValue();
void DrawInfoWindowFrame();
void DrawSMeterContainer();
void DrawSpectrumBandwidthInfo();
void DrawSpectrumDisplayContainer();
void DrawAudioSpectContainer();

int  EEPROMOptions();
void EEPROMRead();
void EEPROMSaveDefaults();
void EEPROMShow();
void EEPROMStuffFavorites(unsigned long current[]);
void EEPROMWrite();
void EncoderFilterAndWPM();
void EncoderVolume();
void EncoderTune();
int  EqualizerRecOptions();
int  EqualizerXmtOptions();
void EraseSpectrumDisplayContainer();
void ExecuteButtonPress(int val);
void FilterBandwidth();
void FormatFrequency(long f, char *b);
int  FrequencyOptions();
void FreqShift1();
void FreqShift2();
float goertzel_mag(int numSamples, int TARGET_FREQUENCY, int SAMPLING_RATE, float* data);
int  GetEncoderValue(int minValue, int maxValue, int startValue, int increment);
void InitializeDataArrays();
void InitFilterMask();
void InitLMSNoiseReduction();
void initTempMon(uint16_t freq, uint32_t lowAlarmTemp, uint32_t highAlarmTemp, uint32_t panicAlarmTemp);
int IQOptions();
void IQPhaseCorrection(float32_t *I_buffer, float32_t *Q_buffer, float32_t factor, uint32_t blocksize);
float32_t Izero(float32_t x);

void JackClusteredArrayMax(int32_t *array, int32_t elements, int32_t *maxCount, int32_t *maxIndex, int32_t *firstDit, int32_t spread);

void Kim1_NR();

void LetterSpace();
void LMSNoiseReduction(int16_t blockSize, float32_t *nrbuffer);
float32_t log10f_fast(float32_t X);

int  MicOptions();
int  ModeOptions();
void MorseCharacterDisplay(char currentLetter);
void MyDelay(unsigned long millisWait);
void MyDrawFloat(float val, int decimals, int x, int y, char *buff);
float MSinc(int m, float fc);

void NoiseBlanker(float32_t* inputsamples, float32_t* outputsamples );
int  NROptions();

int  PostProcessorAudio();
int  ProcessButtonPress(int valPin);
void ProcessEqualizerChoices(int16_t MyArray[], char *title);
void ProcessIQData();

int  ReadSelectedPushButton();
void RedrawDisplayScreen();
void RefreshMainDisplay(int displayMode);
void ResetHistograms();
int  RFOptions();

int  SampleOptions();
void SetCompressionLevel();
void SetFastTuneFrequency();
void SaveAnalogSwitchValues();
void sineTone(int numCycles);
int  SpectrumOptions();
void Send(char myChar);
void SendCode(char code);
#if (defined(T4))
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#endif
void SetAttenuator(int value);
void SetBand();
void SetBandRelay(int state);
void SetDecIntFilters();
void SetDitLength(int wpm);
void SetFavoriteFrequencies();
void SetFreq();
int  SetI2SFreq(int freq);
void SetIIRCoeffs(float32_t f0, float32_t Q, float32_t sample_rate, uint8_t filter_type);
void SetKeyerSidetone();
void SetKeyType();
void SetupMode(int sideBand);
int  SetWPM();
void ShowAnalogGain();
void ShowBandwidth();
void ShowDefaultSettings();
extern "C"
{
  void sincosf(float err, float *s, float *c);
  void sincos(double err, double *s, double *c);
}
void ShowFrequency();
void ShowMenu(const char *menu[], int where);
void ShowName();
void ShowNotch();
void ShowSpectrum();
void ShowSpectrumdBScale();
void ShowTempAndLoad();
void BandInformation();
float32_t sign(float32_t x);
void sineTone(long freqSideTone);
void SpectralNoiseReduction(void);
void SpectralNoiseReductionInit();
void Splash();
void SSB_AUTOTUNE_ccor(int n, float32_t* dat, float32_t* FFT_buffer);
void SSB_AUTOTUNE_est(int n, float xr[], float xi[], float smpfrq, float *ppeak, float *ppitch, float *pshift);
void SSB_AUTOTUNE_inchist(int nbins, float bins[], float hpitch, float hshift, float incr);
int  SubmenuSelect(const char *options[], int numberOfChoices, int defaultStart);

void T4_rtc_set(unsigned long t);
float TGetTemp();

void UpdateAGCField();
void UpdateCompressionField();
void UpdateDecoderField();
void UpdateIncrementField();
void UpdateNoiseField();
void UpdateNotchField();
void UpdateNRField();
void UpdateVolumeField();
void UpdateWPMField();
void UpdateZoomField();

float VolumeToAmplification(int volume);
int  VFOSelect();

void WordSpace();

void Xanr();

void ZoomFFTPrep();
void ZoomFFTExe(uint32_t blockSize);

#endif
