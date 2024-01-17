#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: Generate Array with variable sinewave frequency tone AFP 05-17-22
  Parameter list:
    void
  Return value;
    void
*****/
void sineTone(int numCycles)
{
  float theta;
  float freqSideTone2;
  float freqSideTone3 = 3000;         // Refactored 32 * 24000 / 256; //AFP 2-7-23
  float freqSideTone4 = 375;   
  freqSideTone2 = numCycles * 24000 / 256;
  for (int kf = 0; kf < 256; kf++) { //Calc: numCycles=8, 750 hz sine wave.
    theta = kf * 2 * PI * freqSideTone2 / 24000;
    sinBuffer2[kf] = sin(theta);
    cosBuffer2[kf] = cos(theta);
    theta = kf * 2.0 * PI * freqSideTone3 / 24000;
    sinBuffer3[kf] = sin(theta);
    cosBuffer3[kf] = cos(theta);
    theta = kf * 2.0 * PI * freqSideTone4 / 24000;
    sinBuffer4[kf] = sin(theta);
    cosBuffer4[kf] = cos(theta);
  }
}


const float32_t atanTable[68] = {
  -0.015623728620477f,
  0.000000000000000f,  // = 0 for in = 0.0
  0.015623728620477f,
  0.031239833430268f,
  0.046840712915970f,
  0.062418809995957f,
  0.077966633831542f,
  0.093476781158590f,
  0.108941956989866f,
  0.124354994546761f,
  0.139708874289164f,
  0.154996741923941f,
  0.170211925285474f,
  0.185347949995695f,
  0.200398553825879f,
  0.215357699697738f,
  0.230219587276844f,
  0.244978663126864f,
  0.259629629408258f,
  0.274167451119659f,
  0.288587361894077f,
  0.302884868374971f,
  0.317055753209147f,
  0.331096076704132f,
  0.345002177207105f,
  0.358770670270572f,
  0.372398446676754f,
  0.385882669398074f,
  0.399220769575253f,
  0.412410441597387f,
  0.425449637370042f,
  0.438336559857958f,
  0.451069655988523f,
  0.463647609000806f,
  0.476069330322761f,
  0.488333951056406f,
  0.500440813147294f,
  0.512389460310738f,
  0.524179628782913f,
  0.535811237960464f,
  0.547284380987437f,
  0.558599315343562f,
  0.569756453482978f,
  0.580756353567670f,
  0.591599710335111f,
  0.602287346134964f,
  0.612820202165241f,
  0.623199329934066f,
  0.633425882969145f,
  0.643501108793284f,
  0.653426341180762f,
  0.663202992706093f,
  0.672832547593763f,
  0.682316554874748f,
  0.691656621853200f,
  0.700854407884450f,
  0.709911618463525f,
  0.718829999621625f,
  0.727611332626511f,
  0.736257428981428f,
  0.744770125716075f,
  0.753151280962194f,
  0.761402769805578f,
  0.769526480405658f,
  0.777524310373348f,
  0.785398163397448f,  // = pi/4 for in = 1.0
  0.793149946109655f,
  0.800781565178043f
};

/*****
  Purpose: Generate Array with variable sinewave frequency tone
  Parameter list:
    void
  Return value;
    void
*****/
/*void SinTone(long freqSideTone) { // AFP 10-25-22
  float theta;
  for (int kf = 0; kf < 255; kf++) { //Calc 750 hz sine wave.  use 750 because it is 8 whole cycles in 256 buffer.
    theta = kf * 2 * PI * freqSideTone / 24000;
    sinBuffer2[kf] = sin(theta);
  }
  }*/

/*****
  Purpose: Correct Phase angle between I andQ channels
  Parameter list:
    void
  Return value;
    void
*****/
void IQPhaseCorrection(float32_t *I_buffer, float32_t *Q_buffer, float32_t factor, uint32_t blocksize)
{
  float32_t temp_buffer[blocksize];
  if (factor < 0.0) {                                                             // mix a bit of I into Q
    arm_scale_f32 (I_buffer, factor, temp_buffer, blocksize);
    arm_add_f32 (Q_buffer, temp_buffer, Q_buffer, blocksize);
  } else {                                                      // mix a bit of Q into I
    arm_scale_f32 (Q_buffer, factor, temp_buffer, blocksize);
    arm_add_f32 (I_buffer, temp_buffer, I_buffer, blocksize);
  }
} // end IQphase_correction

/*****
  Purpose: Calculate sinc function

  Parameter list:
    void
  Return value;
    void
*****/
float MSinc(int m, float fc)
{
  float x = m * PIH;
  if (m == 0)
    return 1.0f;
  else
    return sinf(x * fc) / (fc * x);
}

/*****
  Purpose: Izero

  Parameter list:
    void
  Return value;
    void
*****/
float32_t Izero(float32_t x)
{
  float32_t x2          = x / 2.0;
  float32_t summe       = 1.0;
  float32_t ds          = 1.0;
  float32_t di          = 1.0;
  float32_t errorlimit  = 1e-9;
  float32_t tmp;

  do
  {
    tmp = x2 / di;
    tmp *= tmp;
    ds *= tmp;
    summe += ds;
    di += 1.0;
  } while (ds >= errorlimit * summe);
  return (summe);
}  // END Izero


/*****
  Purpose:    Fast algorithm for log10
              This is a fast approximation to log2()
              Y = C[0]*F*F*F + C[1]*F*F + C[2]*F + C[3] + E;
              log10f is exactly log2(x)/log2(10.0f)
              Math_log10f_fast(x) =(log2f_approx(x)*0.3010299956639812f)

  Parameter list:
    float32_t X       number for conversion

  Return value;
    void
*****/
float32_t log10f_fast(float32_t X) {
  float Y, F;
  int E;
  F = frexpf(fabsf(X), &E);
  Y = 1.23149591368684f;
  Y *= F;
  Y += -4.11852516267426f;
  Y *= F;
  Y += 6.02197014179219f;
  Y *= F;
  Y += -3.13396450166353f;
  Y += E;
  return (Y * 0.3010299956639812f);
}

/*****
  Purpose: void Calculatedbm()

  Parameter list:
    void

  Return value;
    void
*****/
void Calculatedbm()
{

  // calculation of the signal level inside the filter bandwidth
  // taken from the spectrum display FFT
  // taking into account the analog gain before the ADC
  // analog gain is adjusted in steps of 1.5dB
  // bands[currentBand].RFgain = 0 --> 0dB gain
  // bands[currentBand].RFgain = 15 --> 22.5dB gain

  // spectrum display is generated from 256 samples based on 1024 samples of the FIR FFT . . .
  // could this cause errors in the calculation of the signal strength ?
  int posbin              = 0;

  float32_t Lbin, Ubin;
  float32_t slope         = 10.0;
  float32_t cons          = -92;
  float32_t bw_LSB        = 0.0;
  float32_t bw_USB        = 0.0;
  float32_t sum_db        = 0.0; // FIXME: mabye this slows down the FPU, because the FPU does only process 32bit floats ???
  float32_t bin_bandwidth = (float32_t) (SR[SampleRate].rate / (256.0));

  // width of a 256 tap FFT bin @ 96ksps = 375Hz
  // we have to take into account the magnify mode
  // --> recalculation of bin_BW
  bin_bandwidth = bin_bandwidth / (1 << spectrum_zoom); // correct bin bandwidth is determined by the Zoom FFT display setting

  // in all magnify cases (2x up to 16x) the posbin is in the centre of the spectrum display
  if (spectrum_zoom != 0) {
    posbin = 128; // right in the middle!
  } else {
    posbin = 64;
  }

  //  determine Lbin and Ubin from ts.dmod_mode and FilterInfo.width
  //  = determine bandwith separately for lower and upper sideband


  bw_LSB = bands[currentBand].FLoCut;
  bw_USB = bands[currentBand].FHiCut;
  // calculate upper and lower limit for determination of signal strength
  // = filter passband is between the lower bin Lbin and the upper bin Ubin
  Lbin  = (float32_t)posbin + roundf(bw_LSB / bin_bandwidth); // bin on the lower/left side
  Ubin  = (float32_t)posbin + roundf(bw_USB / bin_bandwidth); // bin on the upper/right side

  // take care of filter bandwidths that are larger than the displayed FFT bins
  if (Lbin < 0) {
    Lbin = 0;
  }
  if (Ubin > 255) {
    Ubin = 255;
  }
  if ((int)Lbin == (int)Ubin) {
    Ubin = 1.0 + Lbin;
  }
  // determine the sum of all the bin values in the passband
  for (int c = (int)Lbin; c <= (int)Ubin; c++) {  // sum up all the values of all the bins in the passband
    sum_db = sum_db + FFT_spec_old[c];
  }

  //#ifdef USE_W7PUA
  if (sum_db > 0.0) {
    //#ifdef USE_LOG10FAST
    switch (display_dbm) {
      case DISPLAY_S_METER_DBM:
        dbm = dbm_calibration + bands[currentBand].gainCorrection + (float32_t)attenuator +
              slope * log10f_fast(sum_db) + cons - (float32_t)bands[currentBand].RFgain * 1.5;
        dbmhz = 0;
        break;
      case DISPLAY_S_METER_DBMHZ:
        dbmhz = dbm - 10.0 * log10f_fast((float32_t)(((int)Ubin - (int)Lbin) * bin_BW));
        dbm = 0;
        break;
    }
  }


  // lowpass IIR filter
  // Wheatley 2011: two averagers with two time constants
  // IIR filter with one element analog to 1st order RC filter
  // but uses two different time constants (ALPHA = 1 - e^(-T/Tau)) depending on
  // whether the signal is increasing (attack) or decreasing (decay)
  // m_AttackAlpha = 0.8647; //  ALPHA = 1 - e^(-T/Tau), T = 0.02s (because dbm routine is called every 20ms!)
  // Tau = 10ms = 0.01s attack time
  // m_DecayAlpha = 0.0392; // 500ms decay time
  //

}


/*****
  Purpose: Fast approximation to the trigonometric atan2 function for floating-point data.

  Parameter list:
    x input value           Inputs
    y input value

  Return value;
    atan2(y, x) = atan(y/x) as radians.
*****/
float32_t arm_atan2_f32(float32_t y, float32_t x)
{
  float32_t atan2Val, fract, in;                 /* Temporary variables for input, output */
  uint32_t index;                                /* Index variable */
  uint32_t tableSize = (uint32_t) TABLE_SIZE_64; /* Initialise tablesize */
  float32_t wa, wb, wc, wd;                      /* Cubic interpolation coefficients */
  float32_t a, b, c, d;                          /* Four nearest output values */
  float32_t *tablePtr;                           /* Pointer to table */
  uint8_t flags = 0;                             /* flags providing information about input values:
                                                    Bit0 = 1 if |x| < |y|
                                                    Bit1 = 1 if x < 0
                                                    Bit2 = 1 if y < 0 */

  /* calculate magnitude of input values */
  if (x < 0.0f) {
    x = -x;
    flags |= 0x02;
  }

  if (y < 0.0f) {
    y = -y;
    flags |= 0x04;
  }

  /* calculate in value for LUT [0 1] */
  if (x < y) {
    in = x / y;
    flags |= 0x01;
  } else {                /* x >= y */
    if (x > 0.0f)
      in = y / x;
    else                  /* both are 0.0 */
      in = 0.0;           /* prevent division by 0 */
  }

  /* Calculation of index of the table */
  index = (uint32_t) (tableSize * in);

  /* fractional value calculation */
  fract = ((float32_t) tableSize * in) - (float32_t) index;

  /* Initialise table pointer */
  tablePtr = (float32_t *) & atanTable[index];

  /* Read four nearest values of output value from the sin table */
  a = *tablePtr++;
  b = *tablePtr++;
  c = *tablePtr++;
  d = *tablePtr++;

  /* Cubic interpolation process */
  wa = -(((0.166666667f) * (fract * (fract * fract))) +
         ((0.3333333333333f) * fract)) + ((0.5f) * (fract * fract));
  wb = (((0.5f) * (fract * (fract * fract))) -
        ((fract * fract) + ((0.5f) * fract))) + 1.0f;
  wc = (-((0.5f) * (fract * (fract * fract))) +
        ((0.5f) * (fract * fract))) + fract;
  wd = ((0.166666667f) * (fract * (fract * fract))) -
       ((0.166666667f) * fract);

  atan2Val = ((a * wa) + (b * wb)) + ((c * wc) + (d * wd));     /* Calculate atan2 value */

  if (flags & 0x01)                                             /* exchanged input values? */

    atan2Val = 1.5707963267949f - atan2Val;                     /* output = pi/2 - output */

  if (flags & 0x02)
    atan2Val = 3.14159265358979f - atan2Val;                    /* negative x input? Quadrant 2 or 3 */

  if (flags & 0x04)
    atan2Val = - atan2Val;                                      /* negative y input? Quadrant 3 or 4 */

  return (atan2Val);                                            /* Return the output value */
}

/*****
  Purpose:
  Parameter list:
    float32_t inphase
    float32_t quadrature

  Return value;
    float32_t
*****/
float32_t AlphaBetaMag(float32_t  inphase, float32_t  quadrature)   // (c) András Retzler
{ // taken from libcsdr: https://github.com/simonyiszk/csdr
  // Min RMS Err      0.947543636291 0.392485425092
  // Min Peak Err     0.960433870103 0.397824734759
  // Min RMS w/ Avg=0 0.948059448969 0.392699081699
  const float32_t alpha = 0.960433870103; // 1.0; //0.947543636291;
  const float32_t beta =  0.397824734759;

  float32_t abs_inphase = fabs(inphase);
  float32_t abs_quadrature = fabs(quadrature);
  if (abs_inphase > abs_quadrature) {
    return alpha * abs_inphase + beta * abs_quadrature;
  } else {
    return alpha * abs_quadrature + beta * abs_inphase;
  }
}


/*****
  Purpose: copied from https://www.dsprelated.com/showarticle/1052.php
           Polynomial approximating arctangenet on the range -1,1.
           Max error < 0.005 (or 0.29 degrees)

  Parameter list:
    float z         value to approximate

  Return value;
    float           atan vakye
*****/
float ApproxAtan(float z)
{
  const float n1 = 0.97239411f;
  const float n2 = -0.19194795f;
  return (n1 + n2 * z * z) * z;
}



/*****
  Purpose: function reads the analog value for each matrix switch and stores that value in EEPROM.
           Only called if STORE_SWITCH_VALUES is uncommented.

  Parameter list:
    void

  Return value;
    void
*****/
void SaveAnalogSwitchValues()
{
  /*                                                                        This list is new with V017
    const char *labels[]        = {"Select",       "Menu Up",  "Band Up",
                                 "Zoom",         "Menu Dn",  "Band Dn",
                                 "Filter",       "DeMod",    "Mode",
                                 "NR",           "Notch",    "Noise Floor",
                                 "Fine Tune",    "Decoder",  "Tune incrment",
                                 "User 1",       "User 2",   "User 3"
                                };
  */
  int index;
  int minVal;
  int value;

  tft.fillWindow(RA8875_BLACK);
  tft.setFontScale(1);
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(10, 10);
  tft.print("Press button you");
  tft.setCursor(10, 30);
  tft.print("have assigned to");
  tft.setCursor(10, 50);
  tft.print("the switch shown.");

  for (index = 0; index < NUMBER_OF_SWITCHES; ) {
    tft.setCursor(20, 100);
    tft.print(index + 1);
    tft.print(". ");
    tft.print(labels[index]);
    value = -1;
    minVal = NOTHING_TO_SEE_HERE;
    while (true) {
      value = ReadSelectedPushButton();
      if (value < NOTHING_TO_SEE_HERE && value > 0) {
        MyDelay(100L);
        if (value < minVal) {
          minVal = value;
        } else {
          value = -1;
          break;
        }
      }
    }
    if (value == -1) {
      tft.fillRect(20, 100, 300, 40, RA8875_BLACK);
      tft.setCursor(350, 20 + index * 25);
      tft.print(index + 1);
      tft.print(". ");
      tft.print(labels[index]);
      tft.setCursor(660, 20 + index * 25);
      tft.print(minVal);
      EEPROMData.switchValues[index] = minVal;
      index++;
      MyDelay(100L);
    }
  }
  EEPROM.put(0, EEPROMData);                        // Save values to EEPROM
}

// ================== Clock stuff
/*****
  Purpose: DisplayClock()
  Parameter list:
    void
  Return value;
    void
*****/
void DisplayClock()
{
  char timeBuffer[15];
  char temp[5];

  temp[0]       = '\0';
  timeBuffer[0] = '\0';
  strcpy(timeBuffer, MY_TIMEZONE);         // e.g., EST
#ifdef TIME_24H
  //DB2OO, 29-AUG-23: use 24h format
  itoa(hour(), temp, DEC);
#else
  itoa(hourFormat12(), temp, DEC);
#endif
  if (strlen(temp) < 2) {
    strcat(timeBuffer, "0");
  }
  strcat(timeBuffer, temp);
  strcat(timeBuffer, ":");

  itoa(minute(), temp, DEC);
  if (strlen(temp) < 2) {
    strcat(timeBuffer, "0");
  }
  strcat(timeBuffer, temp);
  strcat(timeBuffer, ":");

  itoa(second(), temp, DEC);
  if (strlen(temp) < 2) {
    strcat(timeBuffer, "0");
  }
  strcat(timeBuffer, temp);

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(TIME_X - 20, TIME_Y, XPIXELS - TIME_X - 1, CHAR_HEIGHT, RA8875_BLACK);
  tft.setCursor(TIME_X - 20, TIME_Y);
  tft.setTextColor(RA8875_WHITE);
  tft.print(timeBuffer);
}                                                   // end function displayTime


// ============== Mode stuff


/*****
  Purpose: SetupMode sets default mode for the selected band

  Parameter list:
    int sideBand            the sideband

  Return value;
    void
*****/
void SetupMode(int sideBand)
{
  int temp;
                                    // AFP 10-27-22
  if (old_demod_mode != -99)                                    // first time radio is switched on and when changing bands
  {
    switch (sideBand) {
      case DEMOD_LSB :
        temp = bands[currentBand].FHiCut;
        bands[currentBand].FHiCut = - bands[currentBand].FLoCut;
        bands[currentBand].FLoCut = - temp;
        break;

      case DEMOD_USB :
        temp = bands[currentBand].FHiCut;
        bands[currentBand].FHiCut = - bands[currentBand].FLoCut;
        bands[currentBand].FLoCut = - temp;
        break;
      case DEMOD_AM :
        bands[currentBand].FHiCut =  -bands[currentBand].FLoCut;
        break;
    }
  }
  ShowBandwidth();
  // tft.fillRect(pos_x_frequency + 10, pos_y_frequency + 24, 210, 16, RA8875_BLACK);
  //tft.fillRect(OPERATION_STATS_X + 170, FREQUENCY_Y + 30, tft.getFontWidth() * 5, tft.getFontHeight(), RA8875_BLACK);        // Clear top-left menu area
  old_demod_mode = bands[currentBand].mode; // set old_mode flag for next time, at the moment only used for first time radio is switched on . . .
} // end void setup_mode


int Xmit_IQ_Cal() //AFP 09-21-22
{
  return -1;
}


/*****
  Purpose: set Band
  Parameter list:
    void
  Return value;
    void
*****/
void SetBand()
{
  old_demod_mode = -99; // used in setup_mode and when changing bands, so that LoCut and HiCut are not changed!
  SetupMode(bands[currentBand].mode);
  SetFreq();
  ShowFrequency();
  FilterBandwidth();
}


/*****
  Purpose: Tries to open the EEPROM SD file to see if an SD card is present in the system

  Parameter list:
    void

  Return value;
    int               0 = SD not initialized, 1 = has data
*****/
int SDPresentCheck()
{
  if (!SD.begin(chipSelect))
  {
    Serial.print("No SD card or cannot be initialized.");
    tft.setFontScale((enum RA8875tsize) 1);
    tft.setForegroundColor(RA8875_RED);
    tft.setCursor(20, 300);
    tft.print("No SD card or not initialized.");
    tft.setForegroundColor(RA8875_WHITE);
    return 0;
  }
  // open the file.
  File dataFile = SD.open("SDEEPROMData.txt");
  
  if (dataFile) {
    return 1;
  } else {
    return 0;
  }

}

