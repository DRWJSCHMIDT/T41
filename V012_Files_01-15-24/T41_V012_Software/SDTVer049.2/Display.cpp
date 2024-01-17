#ifndef BEENHERE
#include "SDT.h"
#endif

const uint16_t gradient[] = {  // Color array for waterfall background
  0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
  0x10, 0x1F, 0x11F, 0x19F, 0x23F, 0x2BF, 0x33F, 0x3BF, 0x43F, 0x4BF,
  0x53F, 0x5BF, 0x63F, 0x6BF, 0x73F, 0x7FE, 0x7FA, 0x7F5, 0x7F0, 0x7EB,
  0x7E6, 0x7E2, 0x17E0, 0x3FE0, 0x67E0, 0x8FE0, 0xB7E0, 0xD7E0, 0xFFE0, 0xFFC0,
  0xFF80, 0xFF20, 0xFEE0, 0xFE80, 0xFE40, 0xFDE0, 0xFDA0, 0xFD40, 0xFD00, 0xFCA0,
  0xFC60, 0xFC00, 0xFBC0, 0xFB60, 0xFB20, 0xFAC0, 0xFA80, 0xFA20, 0xF9E0, 0xF980,
  0xF940, 0xF8E0, 0xF8A0, 0xF840, 0xF800, 0xF802, 0xF804, 0xF806, 0xF808, 0xF80A,
  0xF80C, 0xF80E, 0xF810, 0xF812, 0xF814, 0xF816, 0xF818, 0xF81A, 0xF81C, 0xF81E,
  0xF81E, 0xF81E, 0xF81E, 0xF83E, 0xF83E, 0xF83E, 0xF83E, 0xF85E, 0xF85E, 0xF85E,
  0xF85E, 0xF87E, 0xF87E, 0xF83E, 0xF83E, 0xF83E, 0xF83E, 0xF85E, 0xF85E, 0xF85E,
  0xF85E, 0xF87E, 0xF87E, 0xF87E, 0xF87E, 0xF87E, 0xF87E, 0xF87E, 0xF87E, 0xF87E,
  0xF87E, 0xF87E, 0xF87E, 0xF87E, 0xF88F, 0xF88F, 0xF88F
};

uint16_t waterfall[MAX_WATERFALL_WIDTH];
int maxYPlot;

/*****
  Purpose: Draw audio spectrum box  AFP added 3-14-21

  Parameter list:

  Return value;
    void
*****/
void DrawAudioSpectContainer() {
  tft.drawRect(BAND_INDICATOR_X - 9, SPECTRUM_BOTTOM - 118, 255, 118, RA8875_GREEN);
  for (int k = 0; k < 6; k++) {
    tft.drawFastVLine(BAND_INDICATOR_X - 10 + k * 43.8, SPECTRUM_BOTTOM, 15, RA8875_GREEN);
    tft.setCursor(BAND_INDICATOR_X - 14 + k * 43.8, SPECTRUM_BOTTOM + 16);
    //tft.drawFastVLine(BAND_INDICATOR_X - 10 + k * 42.5, SPECTRUM_BOTTOM, 15, RA8875_GREEN);
    //tft.setCursor(BAND_INDICATOR_X - 14 + k * 42.5, SPECTRUM_BOTTOM + 16);
    tft.print(k);
    tft.print("k");
  }
}
/*****
  Purpose: Show the program name and version number

  Parameter list:
    void

  Return value;
    void
*****/
void ShowName() {
  tft.fillRect(RIGNAME_X_OFFSET, 0, XPIXELS - RIGNAME_X_OFFSET, tft.getFontHeight(), RA8875_BLACK);

  tft.setFontScale((enum RA8875tsize)1);

  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(RIGNAME_X_OFFSET - 20, 1);
  tft.print(RIGNAME);
  tft.setFontScale(0);
  tft.print(" ");                  // Added to correct for deleted leading space 4/16/2022 JACK
#ifdef FOURSQRP                    // Give visual indication that were using the 4SQRP code.  W8TEE October 11, 2023.
  tft.setTextColor(RA8875_GREEN);  // Make it green
#else
  tft.setTextColor(RA8875_RED);  // Make it red
#endif
  tft.print(VERSION);
}

FASTRUN  // Place in tightly-coupled memory
         /*****
  Purpose: Show Spectrum display
            Note that this routine calls the Audio process Function during each display cycle,
            for each of the 512 display frequency bins.  This means that the audio is refreshed at the maximum rate
            and does not have to wait for the display to complete drawinf the full spectrum.
            However, the display data are only updated ONCE during each full display cycle,
            ensuring consistent data for the erase/draw cycle at each frequency point.

  Parameter list:
    void

  Return value;
    void
*****/
  void
  ShowSpectrum()  //AFP Extensively Modified 3-15-21 Adjusted 12-13-21 to align all elements
{
  int centerLine = (MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2;
  int middleSlice = centerLine / 2;  // Approximate center element
  //int j2;  KF5N
  int x1 = 0;  //AFP
  int h = SPECTRUM_HEIGHT + 3;
  int filterLoPositionMarker;
  int filterHiPositionMarker;
  int y_new_plot, y1_new_plot, y_old_plot, y_old2_plot;

  tft.drawFastVLine(centerLine, SPECTRUM_TOP_Y, h, RA8875_GREEN);  // Draws centerline on spectrum display

  pixelnew[0] = 0;
  pixelnew[1] = 0;
  pixelCurrent[0] = 0;
  pixelCurrent[1] = 0;

  for (x1 = 1; x1 < MAX_WATERFALL_WIDTH - 1; x1++)  //AFP, JJP changed init from 0 to 1 for x1: out of bounds addressing in line 112
  //Draws the main Spectrum, Waterfall and Audio displays
  {
    // Update the frequency here only.  This is the beginning of the 512 wide spectrum display.  Modified by KF5N for new tuning scheme.  July 22, 2023
    if (x1 == 1) {
      updateDisplayFlag = 1;  //Set flag so the display data are saved only once during each display refresh cycle at the start of the cycle, not 512 times
                              // if (centerTuneFlag == 1) {  //AFP 10-04-22
                              //   DrawBandWidthIndicatorBar();
                              // ShowFrequency();
      //  SetFreq();                 //AFP 10-04-22
      //}                            //AFP 10-04-22
      //centerTuneFlag = 0;          //AFP 10-04-22
    } else updateDisplayFlag = 0;  //  Do not save the the display data for the remainder of the

    FilterSetSSB();                                           // Insert Filter encoder update here  AFP 06-22-22
    if (T41State == SSB_RECEIVE || T41State == CW_RECEIVE) {  // AFP 08-24-22
      ProcessIQData();                                        // Call the Audio process from within the display routine to eliminate conflicts with drawing the spectrum and waterfall displays
    }
    EncoderCenterTune();  //Moved the tuning encoder to reduce lag times and interference during tuning.
    y_new = pixelnew[x1];
    y1_new = pixelnew[x1 - 1];
    y_old = pixelold[x1];  // pixelold spectrum is saved by the FFT function prior to a new FFT which generates the pixelnew spectrum.  KF5N
    y_old2 = pixelold[x1 - 1];

    y_new_plot = spectrumNoiseFloor - y_new - currentNoiseFloor[currentBand];
    y1_new_plot = spectrumNoiseFloor - y1_new - currentNoiseFloor[currentBand];
    y_old_plot = spectrumNoiseFloor - y_old - currentNoiseFloor[currentBand];
    y_old2_plot = spectrumNoiseFloor - y_old2 - currentNoiseFloor[currentBand];

    // Prevent spectrum from going below the bottom of the spectrum area.  KF5N
    if (y_new_plot > 247) y_new_plot = 247;
    if (y1_new_plot > 247) y1_new_plot = 247;
    if (y_old_plot > 247) y_old_plot = 247;
    if (y_old2_plot > 247) y_old2_plot = 247;

    // Prevent spectrum from going above the top of the spectrum area.  KF5N
    if (y_new_plot < 101) y_new_plot = 101;
    if (y1_new_plot < 101) y1_new_plot = 101;
    if (y_old_plot < 101) y_old_plot = 101;
    if (y_old2_plot < 101) y_old2_plot = 101;

    if (x1 > 188 && x1 < 330) {
      if (y_new_plot < 120) y_new_plot = 120;
      if (y1_new_plot < 120) y1_new_plot = 120;
      if (y_old_plot < 120) y_old_plot = 120;
      if (y_old2_plot < 120) y_old2_plot = 120;
    }

    // Erase the old spectrum, and draw the new spectrum.
    tft.drawLine(x1 + 1, y_old2_plot, x1 + 1, y_old_plot, RA8875_BLACK);   // Erase old...
    tft.drawLine(x1 + 1, y1_new_plot, x1 + 1, y_new_plot, RA8875_YELLOW);  // Draw new

    //  What is the actual spectrum at this time?  It's a combination of the old and new spectrums.
    //  In the case of a CW interrupt, the array pixelnew should be saved as the actual spectrum.
    pixelCurrent[x1] = pixelnew[x1];  //  This is the actual "old" spectrum!  This is required due to CW interrupts.  pixelCurrent gets copied to pixelold by the FFT function.  KF5N

    if (x1 < 253) {                                                                              //AFP 09-01-22
      if (keyPressedOn == 1) {                                                                   //AFP 09-01-22
        return;                                                                                  //AFP 09-01-22
      } else {                                                                                   //AFP 09-01-22
        tft.drawFastVLine(BAND_INDICATOR_X - 8 + x1, SPECTRUM_BOTTOM - 116, 115, RA8875_BLACK);  //AFP Erase old AUDIO spectrum line
        if (audioYPixel[x1] != 0) {
          if (audioYPixel[x1] > CLIP_AUDIO_PEAK)  // audioSpectrumHeight = 118
            audioYPixel[x1] = CLIP_AUDIO_PEAK;
          if (x1 == middleSlice) {
            smeterLength = y_new;
          }
          tft.drawFastVLine(BAND_INDICATOR_X - 8 + x1, AUDIO_SPECTRUM_BOTTOM - audioYPixel[x1] - 1, audioYPixel[x1] - 2, RA8875_MAGENTA);  //AFP draw new AUDIO spectrum line
        }
        tft.drawFastHLine(SPECTRUM_LEFT_X - 1, SPECTRUM_TOP_Y + SPECTRUM_HEIGHT, MAX_WATERFALL_WIDTH, RA8875_YELLOW);
        // The following lines calculate the position of the Filter bar below the spectrum display
        // and then draw the Audio spectrum in its own container to the right of the Main spectrum display

        filterLoPositionMarker = map(bands[currentBand].FLoCut, 0, 6000, 0, 256);
        filterHiPositionMarker = map(bands[currentBand].FHiCut, 0, 6000, 0, 256);
        //Draw Fiter indicator lines on audio plot AFP 10-30-22
        tft.drawLine(BAND_INDICATOR_X - 6 + abs(filterLoPositionMarker), SPECTRUM_BOTTOM - 3, BAND_INDICATOR_X - 6 + abs(filterLoPositionMarker), SPECTRUM_BOTTOM - 112, RA8875_LIGHT_GREY);
        tft.drawLine(BAND_INDICATOR_X - 7 + abs(filterHiPositionMarker), SPECTRUM_BOTTOM - 3, BAND_INDICATOR_X - 7 + abs(filterHiPositionMarker), SPECTRUM_BOTTOM - 112, RA8875_LIGHT_GREY);

        if (filterLoPositionMarker != filterLoPositionMarkerOld || filterHiPositionMarker != filterHiPositionMarkerOld) {
          DrawBandWidthIndicatorBar();
        }
        filterLoPositionMarkerOld = filterLoPositionMarker;
        filterHiPositionMarkerOld = filterHiPositionMarker;
      }
    }

    int test1;
    test1 = -y_new_plot + 230;  // Nudged waterfall towards blue.  KF5N July 23, 2023
    if (test1 < 0) test1 = 0;
    if (test1 > 117) test1 = 117;
    waterfall[x1] = gradient[test1];  // Try to put pixel values in middle of gradient array.  KF5N
    tft.writeTo(L1);
  }
  // End for(...) Draw MAX_WATERFALL_WIDTH spectral points
  // Use the Block Transfer Engine (BTE) to move waterfall down a line

  if (keyPressedOn == 1) {
    return;
  } else {
    tft.BTE_move(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE, MAX_WATERFALL_WIDTH, MAX_WATERFALL_ROWS - 2, WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, 1, 2);
    while (tft.readStatus())  // Make sure it is done.  Memory moves can take time.
      ;
    // Now bring waterfall back to the beginning of the 2nd row.
    tft.BTE_move(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, MAX_WATERFALL_WIDTH, MAX_WATERFALL_ROWS - 2, WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, 2);
    while (tft.readStatus())  // Make sure it's done.
      ;
  }
  // Then write new row data into the missing top row to get a scroll effect using display hardware, not the CPU.
  tft.writeRect(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE, MAX_WATERFALL_WIDTH, 1, waterfall);
}

/*****
  Purpose: show filter bandwidth near center of spectrum and and show sample rate in corner

  Parameter list:
    void

  Return value;
    void
        // AudioNoInterrupts();
        // M = demod_mode, FU & FL upper & lower frequency
        // this routine prints the frequency bars under the spectrum display
        // and displays the bandwidth bar indicating demodulation bandwidth
*****/
void ShowBandwidth() {
  char buff[10];
  int centerLine = (MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2;
  int pos_left;
  float32_t pixel_per_khz;

  if (spectrum_zoom != SPECTRUM_ZOOM_1)
    spectrum_pos_centre_f = 128 * xExpand - 1;  //AFP
  else
    spectrum_pos_centre_f = 64 * xExpand;  //AFP

  pixel_per_khz = 0.0055652173913043;  // Al: I factored this constant: 512/92000;
  //pixel_per_khz = ((1 << spectrum_zoom) * SPECTRUM_RES * 1000.0 / SR[SampleRate].rate) ;
  pos_left = centerLine + ((int)(bands[currentBand].FLoCut / 1000.0 * pixel_per_khz));
  if (pos_left < spectrum_x) {
    pos_left = spectrum_x;
  }

  // Need tto add in code for zoom factor here AFP 10-20-22

  filterWidthX = pos_left + newCursorPosition - centerLine;
  tft.writeTo(L2);
  tft.setFontScale((enum RA8875tsize)0);
  tft.setTextColor(RA8875_LIGHT_GREY);
  if (switchFilterSideband == 0)
    tft.setTextColor(RA8875_WHITE);
  else if (switchFilterSideband == 1)
    tft.setTextColor(RA8875_LIGHT_GREY);

  MyDrawFloat((float)(bands[currentBand].FLoCut / 1000.0f), 1, FILTER_PARAMETERS_X, FILTER_PARAMETERS_Y, buff);

  tft.print("kHz");
  if (switchFilterSideband == 1)
    tft.setTextColor(RA8875_WHITE);
  else if (switchFilterSideband == 0)
    tft.setTextColor(RA8875_LIGHT_GREY);
  MyDrawFloat((float)(bands[currentBand].FHiCut / 1000.0f), 1, FILTER_PARAMETERS_X + 80, FILTER_PARAMETERS_Y, buff);
  tft.print("kHz");

  tft.setTextColor(RA8875_WHITE);  // set text color to white for other print routines not to get confused ;-)
  tft.writeTo(L1);
}

//DB2OO, 30-AUG-23: this variable determines the pixels per S step. In the original code it was 12.2 pixels !?
#ifdef TCVSDR_SMETER
const float pixels_per_s = 12;
#else
const float pixels_per_s = 12.2;
#endif
/*****
  Purpose: DrawSMeterContainer()
  Parameter list:
    void
  Return value;
    void
*****/
void DrawSMeterContainer() {
  int i;
  // DB2OO, 30-AUG-23: the white line must only go till S9
  tft.drawFastHLine(SMETER_X, SMETER_Y - 1, 9 * pixels_per_s, RA8875_WHITE);
  tft.drawFastHLine(SMETER_X, SMETER_Y + SMETER_BAR_HEIGHT+2, 9 * pixels_per_s, RA8875_WHITE);  // changed 6 to 20

  for (i = 0; i < 10; i++) {                                                // Draw tick marks for S-values
#ifdef TCVSDR_SMETER
    //DB2OO, 30-AUG-23: draw wider tick marks in the style of the Teensy Convolution SDR
    tft.drawRect(SMETER_X + i * pixels_per_s, SMETER_Y - 6-(i%2)*2, 2, 6+(i%2)*2, RA8875_WHITE);
#else      
    tft.drawFastVLine(SMETER_X + i * 12.2, SMETER_Y - 6, 7, RA8875_WHITE);
#endif    
  }

  // DB2OO, 30-AUG-23: the green line must start at S9
  tft.drawFastHLine(SMETER_X + 9*pixels_per_s, SMETER_Y - 1, SMETER_BAR_LENGTH+2-9*pixels_per_s, RA8875_GREEN);
  tft.drawFastHLine(SMETER_X + 9*pixels_per_s, SMETER_Y + SMETER_BAR_HEIGHT+2, SMETER_BAR_LENGTH+2-9*pixels_per_s, RA8875_GREEN);

  for (i = 1; i <= 3; i++) {                                                     // Draw tick marks for s9+ values in 10dB steps
#ifdef TCVSDR_SMETER
    //DB2OO, 30-AUG-23: draw wider tick marks in the style of the Teensy Convolution SDR
    tft.drawRect(SMETER_X + 9*pixels_per_s + i * pixels_per_s*10.0/6.0, SMETER_Y - 8+(i%2)*2, 2, 8-(i%2)*2, RA8875_GREEN);
#else      
    tft.drawFastVLine(SMETER_X + 9*pixels_per_s + i * pixels_per_s*10.0/6.0, SMETER_Y - 6, 7, RA8875_GREEN);  
#endif
  }

  tft.drawFastVLine(SMETER_X, SMETER_Y - 1, SMETER_BAR_HEIGHT+3, RA8875_WHITE);  
  tft.drawFastVLine(SMETER_X + SMETER_BAR_LENGTH+2, SMETER_Y - 1, SMETER_BAR_HEIGHT+3, RA8875_GREEN);

  tft.setFontScale((enum RA8875tsize)0);

  tft.setTextColor(RA8875_WHITE);
  //DB2OO, 30-AUG-23: moved single digits a bit to the right, to align 
  tft.setCursor(SMETER_X - 8, SMETER_Y - 25);
  tft.print("S");
  tft.setCursor(SMETER_X + 8, SMETER_Y - 25);
  tft.print("1");
  tft.setCursor(SMETER_X + 32, SMETER_Y - 25);  // was 28, 48, 68, 88, 120 and -15 changed to -20
  tft.print("3");
  tft.setCursor(SMETER_X + 56, SMETER_Y - 25);
  tft.print("5");
  tft.setCursor(SMETER_X + 80, SMETER_Y - 25);
  tft.print("7");
  tft.setCursor(SMETER_X + 104, SMETER_Y - 25);
  tft.print("9");
  //DB2OO, 30-AUG-23 +20dB needs to get more left
  tft.setCursor(SMETER_X + 133, SMETER_Y - 25);
  tft.print("+20dB");

  DrawFrequencyBarValue();
  ShowSpectrumdBScale();
}

/*****
  Purpose: ShowSpectrumdBScale()
  Parameter list:
    void
  Return value;
    void
*****/
void ShowSpectrumdBScale() {
  tft.setFontScale((enum RA8875tsize)0);

  tft.fillRect(SPECTRUM_LEFT_X + 1, SPECTRUM_TOP_Y + 10, 33, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(SPECTRUM_LEFT_X + 5, SPECTRUM_TOP_Y + 10);
  tft.setTextColor(RA8875_WHITE);
  tft.print(displayScale[currentScale].dbText);
}

/*****
  Purpose: This function draws spectrum display container
  Parameter list:
    void
  Return value;
    void
    // This function draws the frequency bar at the bottom of the spectrum scope, putting markers at every graticule and the full frequency
*****/
void DrawSpectrumDisplayContainer() {
  tft.drawRect(SPECTRUM_LEFT_X - 1, SPECTRUM_TOP_Y, MAX_WATERFALL_WIDTH + 2, SPECTRUM_HEIGHT, RA8875_YELLOW);  // Spectrum box
}

/*****
  Purpose: This function draws the frequency bar at the bottom of the spectrum scope, putting markers at every
            graticule and the full frequency

  Parameter list:
    void

  Return value;
    void
*****/
void DrawFrequencyBarValue() {
  char txt[16];

  int bignum;
  int centerIdx;
  int pos_help;
  float disp_freq;

  float freq_calc;
  float grat;
  int centerLine = MAX_WATERFALL_WIDTH / 2 + SPECTRUM_LEFT_X;
  // positions for graticules: first for spectrum_zoom < 3, then for spectrum_zoom > 2
  const static int idx2pos[2][9] = {
    { -43, 21, 50, 250, 140, 250, 232, 250, 315 },  //AFP 10-30-22
    { -43, 21, 50, 85, 200, 200, 232, 218, 315 }    //AFP 10-30-22
  };

  grat = (float)(SR[SampleRate].rate / 8000.0) / (float)(1 << spectrum_zoom);  // 1, 2, 4, 8, 16, 32, 64 . . . 4096

  tft.writeTo(L2);  // Not writing to correct layer?  KF5N.  July 31, 2023
  tft.setTextColor(RA8875_WHITE);
  tft.setFontScale((enum RA8875tsize)0);
  tft.fillRect(WATERFALL_LEFT_X, WATERFALL_TOP_Y, MAX_WATERFALL_WIDTH + 5, tft.getFontHeight(), RA8875_BLACK);  // 4-16-2022 JACK

  freq_calc = (float)(centerFreq / NEW_SI5351_FREQ_MULT);  // get current frequency in Hz

  if (activeVFO == VFO_A) {
    currentFreqA = TxRxFreq;
  } else {
    currentFreqB = TxRxFreq;
  }

  if (spectrum_zoom == 0) {
    freq_calc += (float32_t)SR[SampleRate].rate / 4.0;
  }

  if (spectrum_zoom < 5) {
    freq_calc = roundf(freq_calc / 1000);  // round graticule frequency to the nearest kHz
  } else if (spectrum_zoom < 5) {
    freq_calc = roundf(freq_calc / 100) / 10;  // round graticule frequency to the nearest 100Hz
    // === AFP 10-30-22
    // } else if (spectrum_zoom == 5) {              // 32x
    //  freq_calc = roundf(freq_calc / 50) / 20;    // round graticule frequency to the nearest 50Hz
    //} else if (spectrum_zoom < 8) {
    //  freq_calc = roundf(freq_calc / 10) / 100 ;  // round graticule frequency to the nearest 10Hz
    // } else {
    //  freq_calc = roundf(freq_calc) / 1000;       // round graticule frequency to the nearest 1Hz
    // ============
  }

  if (spectrum_zoom != 0)
    centerIdx = 0;
  else
    centerIdx = -2;

  /**************************************************************************************************
    CENTER FREQUENCY PRINT
  **************************************************************************************************/
  ultoa((freq_calc + (centerIdx * grat)), txt, DEC);
  disp_freq = freq_calc + (centerIdx * grat);
  bignum = (int)disp_freq;
  itoa(bignum, txt, DEC);  // Make into a string
  //=================== AFP 10-21-22 =====
  tft.setTextColor(RA8875_GREEN);

  //  ========= AFP 1-21-22 ======
  if (spectrum_zoom == 0) {
    tft.setCursor(centerLine - 140, WATERFALL_TOP_Y);  //AFP 10-20-22
  } else {
    tft.setCursor(centerLine - 20, WATERFALL_TOP_Y);  //AFP 10-20-22
  }
  //  ========= AFP 1-21-22 ====
  tft.print(txt);
  tft.setTextColor(RA8875_WHITE);
  /**************************************************************************************************
     PRINT ALL OTHER FREQUENCIES (NON-CENTER)
   **************************************************************************************************/
  // snprint() extremely memory inefficient. replaced with simple str?? functions JJP
  for (int idx = -4; idx < 5; idx++) {
    pos_help = idx2pos[spectrum_zoom < 3 ? 0 : 1][idx + 4];
    if (idx != centerIdx) {
      ultoa((freq_calc + (idx * grat)), txt, DEC);
      //================== AFP 10-21-22 =============
      if (spectrum_zoom == 0) {
        tft.setCursor(WATERFALL_LEFT_X + pos_help * xExpand + 40, WATERFALL_TOP_Y);  // AFP 10-20-22
      } else {
        tft.setCursor(WATERFALL_LEFT_X + pos_help * xExpand + 40, WATERFALL_TOP_Y);  // AFP 10-20-22
      }
      // ============  AFP 10-21-22
      tft.print(txt);
      if (idx < 4) {
        tft.drawFastVLine((WATERFALL_LEFT_X + pos_help * xExpand + 60), WATERFALL_TOP_Y - 5, 7, RA8875_YELLOW);  // Tick marks depending on zoom
      } else {
        tft.drawFastVLine((WATERFALL_LEFT_X + (pos_help + 9) * xExpand + 60), WATERFALL_TOP_Y - 5, 7, RA8875_YELLOW);
      }
    }
    if (spectrum_zoom > 2 || freq_calc > 1000) {
      idx++;
    }
  }
  tft.writeTo(L1);  // Always leave on layer 1.  KF5N.  July 31, 2023
  tft.setFontScale((enum RA8875tsize)1);
  ShowBandwidth();
}
/*****
  Purpose: void ShowAnalogGain()

  Parameter list:
    void

  Return value;
    void
    // This function draws the frequency bar at the bottom of the spectrum scope, putting markers at every graticule and the full frequency
*****/
void ShowAnalogGain() {
  static uint8_t RF_gain_old = 0;
  static uint8_t RF_att_old = 0;
  const uint16_t col = RA8875_GREEN;
  if ((((bands[currentBand].RFgain != RF_gain_old) || (attenuator != RF_att_old)) && twinpeaks_tested == 1) || write_analog_gain) {
    tft.setFontScale((enum RA8875tsize)0);
    tft.setCursor(pos_x_time - 40, pos_y_time + 26);
    tft.print((float)(RF_gain_old * 1.5));
    tft.setTextColor(col);
    tft.print("dB -");

    tft.setTextColor(RA8875_BLACK);
    tft.print("dB -");
    tft.setTextColor(RA8875_BLACK);
    tft.print("dB");
    tft.setTextColor(col);
    tft.print("dB = ");

    tft.setFontScale((enum RA8875tsize)0);

    tft.setTextColor(RA8875_BLACK);
    tft.print("dB");
    tft.setTextColor(RA8875_WHITE);
    tft.print("dB");
    RF_gain_old = bands[currentBand].RFgain;
    RF_att_old = attenuator;
    write_analog_gain = 0;
  }
}

/*****
  Purpose: To display the current transmission frequency, band, mode, and sideband above the spectrum display

  Parameter list:
    void

  Return value;
    void

*****/
void BandInformation()  // SSB or CW
{
  float CWFilterPosition = 0.0;

  tft.setFontScale((enum RA8875tsize)0);
  tft.setTextColor(RA8875_GREEN);

  tft.setCursor(5, FREQUENCY_Y + 30);
  tft.setTextColor(RA8875_WHITE);
  tft.print("Center Freq");
  tft.fillRect(100, FREQUENCY_Y + 30, 300, tft.getFontHeight(), RA8875_BLACK);  // Clear volume field
  tft.setCursor(100, FREQUENCY_Y + 30);
  tft.setTextColor(RA8875_LIGHT_ORANGE);
  if (spectrum_zoom == SPECTRUM_ZOOM_1) {  // AFP 11-02-22
    tft.print(centerFreq + 48000);
  } else {
    tft.print(centerFreq);
  }
  tft.fillRect(OPERATION_STATS_X + 50, FREQUENCY_Y + 30, 300, tft.getFontHeight(), RA8875_BLACK);  // Clear volume field
  tft.setTextColor(RA8875_LIGHT_ORANGE);
  tft.setCursor(OPERATION_STATS_X + 50, FREQUENCY_Y + 30);
  if (activeVFO == VFO_A) {
    tft.print(bands[currentBandA].name);  // Show band -- 40M
  } else {
    tft.print(bands[currentBandB].name);  // Show band -- 40M
  }

  tft.fillRect(OPERATION_STATS_X + 90, FREQUENCY_Y + 30, 70, tft.getFontHeight(), RA8875_BLACK);  //AFP 10-18-22
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(OPERATION_STATS_X + 90, FREQUENCY_Y + 30);  //AFP 10-18-22

  //================  AFP 10-19-22
  if (xmtMode == CW_MODE) {
    tft.fillRect(OPERATION_STATS_X + 85, FREQUENCY_Y + 30, 70, tft.getFontHeight(), RA8875_BLACK);
    tft.print("CW ");
    tft.setCursor(OPERATION_STATS_X + 115, FREQUENCY_Y + 30);  //AFP 10-18-22
    tft.writeTo(L2);                                           // Moved to L2 here to properly refresh the CW filter bandwidth.  KF5N July 30, 2023
    tft.print(CWFilter[CWFilterIndex]);                        //AFP 10-18-22
    switch (CWFilterIndex) {
      case 0:
        CWFilterPosition = 35.7;  // 0.84 * 42.5;
        break;
      case 1:
        CWFilterPosition = 42.5;
        break;
      case 2:
        CWFilterPosition = 55.25;  // 1.3 * 42.5;
        break;
      case 3:
        CWFilterPosition = 76.5;  // 1.8 * 42.5;
        break;
      case 4:
        CWFilterPosition = 85.0;  // 2.0 * 42.5;
        break;
      case 5:
        CWFilterPosition = 0.0;
        break;
    }

    tft.fillRect(BAND_INDICATOR_X - 8, AUDIO_SPECTRUM_TOP, CWFilterPosition, 120, MAROON);
    tft.drawFastVLine(BAND_INDICATOR_X - 8 + CWFilterPosition, AUDIO_SPECTRUM_BOTTOM - 118, 118, RA8875_LIGHT_GREY);

    tft.writeTo(L1);
    //================  AFP 10-19-22 =========
  } else {
    tft.fillRect(OPERATION_STATS_X + 90, FREQUENCY_Y + 30, 70, tft.getFontHeight(), RA8875_BLACK);
    tft.print("SSB");  // Which mode
  }

  tft.fillRect(OPERATION_STATS_X + 160, FREQUENCY_Y + 30, tft.getFontWidth() * 11, tft.getFontHeight(), RA8875_BLACK);  // AFP 11-01-22 Clear top-left menu area
  tft.setCursor(OPERATION_STATS_X + 160, FREQUENCY_Y + 30);                                                             // AFP 11-01-22
  tft.setTextColor(RA8875_WHITE);


  switch (bands[currentBand].mode) {
    case DEMOD_LSB:
      if (activeVFO == VFO_A) {
        tft.print(DEMOD[bands[currentBandA].mode].text);  // Which sideband //AFP 09-22-22
      } else {
        tft.print(DEMOD[bands[currentBandB].mode].text);  // Which sideband //AFP 09-22-22
      }
      break;
      ;

    case DEMOD_USB:
      if (activeVFO == VFO_A) {
        tft.print(DEMOD[bands[currentBandA].mode].text);  // Which sideband //AFP 09-22-22
      } else {
        tft.print(DEMOD[bands[currentBandB].mode].text);  // Which sideband //AFP 09-22-22
      }
      break;
    case DEMOD_AM:
      tft.setTextColor(RA8875_WHITE);
      tft.print("(AM)");  //AFP 09-22-22
      break;
    case DEMOD_SAM:         //AFP 11-01-22
      tft.print("(SAM) ");  //AFP 11-01-22
      break;
  }
  ShowCurrentPowerSetting();
}

/*****
  Purpose: Display current power setting

  Parameter list:
    void

  Return value;
    void
*****/
void ShowCurrentPowerSetting() {
  tft.fillRect(OPERATION_STATS_X + 275, FREQUENCY_Y + 30, tft.getFontWidth() * 11, tft.getFontHeight(), RA8875_BLACK);  // Clear top-left menu area
  tft.setCursor(OPERATION_STATS_X + 275, FREQUENCY_Y + 30);
  tft.setTextColor(RA8875_RED);
  tft.print(transmitPowerLevel, 1);  // Power output is a float
  tft.print(" Watts");
  tft.setTextColor(RA8875_WHITE);
}

/*****
  Purpose: Format frequency for printing
  Parameter list:
    void
  Return value;
    void
    // show frequency
*****/
void FormatFrequency(long freq, char *freqBuffer) {
  char outBuffer[15];
  int i;
  int len;
  ltoa((long)freq, outBuffer, 10);
  len = strlen(outBuffer);

  switch (len) {
    case 6:  // below 530.999 KHz
      freqBuffer[0] = outBuffer[0];
      freqBuffer[1] = outBuffer[1];
      freqBuffer[2] = outBuffer[2];
      freqBuffer[3] = FREQ_SEP_CHARACTER;  // Add separation charcter
      for (i = 4; i < len; i++) {
        freqBuffer[i] = outBuffer[i - 1];  // Next 3 digit chars
      }
      freqBuffer[i] = '0';       // trailing 0
      freqBuffer[i + 1] = '\0';  // Make it a string
      break;

    case 7:  // 1.0 - 9.999 MHz
      freqBuffer[0] = outBuffer[0];
      freqBuffer[1] = FREQ_SEP_CHARACTER;  // Add separation charcter
      for (i = 2; i < 5; i++) {
        freqBuffer[i] = outBuffer[i - 1];  // Next 3 digit chars
      }
      freqBuffer[5] = FREQ_SEP_CHARACTER;  // Add separation charcter
      for (i = 6; i < 9; i++) {
        freqBuffer[i] = outBuffer[i - 2];  // Last 3 digit chars
      }
      freqBuffer[i] = '\0';  // Make it a string
      break;

    case 8:  // 10 MHz - 30MHz
      freqBuffer[0] = outBuffer[0];
      freqBuffer[1] = outBuffer[1];
      freqBuffer[2] = FREQ_SEP_CHARACTER;  // Add separation charcter
      for (i = 3; i < 6; i++) {
        freqBuffer[i] = outBuffer[i - 1];  // Next 3 digit chars
      }
      freqBuffer[6] = FREQ_SEP_CHARACTER;  // Add separation charcter
      for (i = 7; i < 10; i++) {
        freqBuffer[i] = outBuffer[i - 2];  // Last 3 digit chars
      }
      freqBuffer[i] = '\0';  // Make it a string
      break;
  }

  /*

  if (freq < 10000000L) {                                                   // 40 or 80M?
    freqBuffer[0] = outBuffer[0];
    freqBuffer[1] = FREQ_SEP_CHARACTER;        // Add separation charcter
    for (i = 2; i < 5; i++) {
      freqBuffer[i] = outBuffer[i - 1];        // Next 3 digit chars
    }
    freqBuffer[5] = FREQ_SEP_CHARACTER;        // Add separation charcter
    for (i = 6; i < 9; i++) {
      freqBuffer[i] = outBuffer[i - 2];        // Last 3 digit chars
    }
    freqBuffer[i] = '\0';                      // Make it a string
  } else {                                                                  // 30M or higher
    freqBuffer[0] = outBuffer[0];
    freqBuffer[1] = outBuffer[1];
    freqBuffer[2] = FREQ_SEP_CHARACTER;        // Add separation charcter
    for (i = 3; i < 6; i++) {
      freqBuffer[i] = outBuffer[i - 1];            // Next 3 digit chars
    } 
    freqBuffer[6] = FREQ_SEP_CHARACTER;        // Add separation charcter
    for (i = 7; i < 10; i++) {
      freqBuffer[i] = outBuffer[i - 2];        // Last 3 digit chars
    }
    freqBuffer[i] = '\0';                      // Make it a string
  }
  */
  /*
  strcpy(outBuffer, freqBuffer);
  freqBuffer[2] = FREQ_SEP_CHARACTER;        // Add separation charcter
  for (i = 3; i < 6; i++) {
    freqBuffer[i] = outBuffer[i - 1];        // Next 3 digit chars
  }
  freqBuffer[6] = FREQ_SEP_CHARACTER;                       // Add separation charcter
  for (i = 7; i < 10; i++) {
    freqBuffer[i] = outBuffer[i - 2];        // Last 3 digit chars
  }
  freqBuffer[i] = '\0';                      // Make it a string
*/
}

/*****
  Purpose: show Main frequency display at top

  Parameter list:
    void

  Return value;
    void
    // show frequency
*****/
FASTRUN void ShowFrequency() {
  char freqBuffer[15];
  if (activeVFO == VFO_A) {  // Needed for edge checking
    currentBand = currentBandA;
  } else {
    currentBand = currentBandB;
  }

  if (activeVFO == VFO_A) {
    FormatFrequency(TxRxFreq, freqBuffer);
    //tft.setFont(&FreeMonoBold24pt7b);               // Large font KF5N
    //tft.setFontScale(2, 3);                         // JJP 7/15/23
    tft.setFontScale(3, 2);  // JJP 7/15/23
    if (TxRxFreq < bands[currentBandA].fBandLow || TxRxFreq > bands[currentBandA].fBandHigh) {
      tft.setTextColor(RA8875_RED);  // Out of band
    } else {
      tft.setTextColor(RA8875_GREEN);  // In US band
    }
    tft.fillRect(0, FREQUENCY_Y - 14, tft.getFontWidth() * 11, tft.getFontHeight(), RA8875_BLACK);  // JJP 7/15/23
    tft.setCursor(0, FREQUENCY_Y - 17);                                                             // To adjust for Greg's font change jjp 7/14/23
    tft.print(freqBuffer);                                                                          // Show VFO_A
    //tft.setFont(&FreeMonoBold18pt7b);               // KF5N
    //    tft.setFontScale(1.5);                      // KF5N
    tft.setFontScale(1, 2);  // JJP 7/15/23
    tft.setTextColor(RA8875_LIGHT_GREY);
    tft.setCursor(FREQUENCY_X_SPLIT + 60, FREQUENCY_Y - 15);
    FormatFrequency(currentFreqB, freqBuffer);
    tft.print(freqBuffer);
  } else {  // Show VFO_B
    FormatFrequency(TxRxFreq, freqBuffer);
    //tft.setFont(&FreeMonoBold24pt7b);             KF5N
    //tft.setFontScale(2);   //KF5N   JJP 7/15/23
    tft.setFontScale(3, 2);                                                                                              // JJP 7/15/23
                                                                                                                         //  tft.fillRect(FREQUENCY_X_SPLIT - 60, FREQUENCY_Y - 12, VFOB_PIXEL_LENGTH, FREQUENCY_PIXEL_HI, RA8875_BLACK);  //JJP 7/15/23
    tft.fillRect(FREQUENCY_X_SPLIT - 60, FREQUENCY_Y - 14, tft.getFontWidth() * 10, tft.getFontHeight(), RA8875_BLACK);  //JJP 7/15/23
    tft.setCursor(FREQUENCY_X_SPLIT - 60, FREQUENCY_Y - 12);
    if (TxRxFreq < bands[currentBandB].fBandLow || TxRxFreq > bands[currentBandB].fBandHigh) {
      tft.setTextColor(RA8875_RED);
    } else {
      tft.setTextColor(RA8875_GREEN);
    }
    tft.print(freqBuffer);  // Show VFO_A
    // tft.setFont(&FreeMonoBold18pt7b);          // KF5N
    // tft.setFontScale(1.5);                     // KF5N   JJP 7/15/23
    tft.setFontScale(1, 2);  // JJP 7/15/23
    FormatFrequency(TxRxFreq, freqBuffer);
    tft.fillRect(0, FREQUENCY_Y - 14, tft.getFontWidth() * 10, tft.getFontHeight(), RA8875_BLACK);  // JJP 7/15/23
    tft.setTextColor(RA8875_LIGHT_GREY);
    tft.setCursor(20, FREQUENCY_Y - 17);
    FormatFrequency(currentFreqA, freqBuffer);
    tft.print(freqBuffer);  // Show VFO_A
  }
  tft.setFontDefault();
}
/*****
  Purpose: Display dBm
  Parameter list:
    void
  Return value;
    void
*****/
void DisplaydbM() {
  char buff[10];
  const char *unit_label;
  int16_t smeterPad;
#ifdef TCVSDR_SMETER
  const float32_t slope = 10.0;
  const float32_t cons = -92;
#else
  float32_t audioLogAveSq;
#endif

  //DB2OO, 30-AUG-23: the S-Meter bar and the dBm value were inconsistent, as they were using different base values.
  // Moreover the bar could go over the limits of the S-meter box, as the map() function, does not constrain the values
  // with TCVSDR_SMETER defined the S-Meter bar will be consistent with the dBm value and the S-Meter bar will always be restricted to the box
  tft.fillRect(SMETER_X + 1, SMETER_Y + 1, SMETER_BAR_LENGTH, SMETER_BAR_HEIGHT, RA8875_BLACK);   //AFP 09-18-22  Erase old bar
#ifdef TCVSDR_SMETER
  //DB2OO, 9-OCT_23: dbm_calibration set to -22 in SDT.ino; gainCorrection is a value between -2 and +6 to compensate the frequency dependant pre-Amp gain
  // attenuator is 0 and could be set in a future HW revision; RFgain is initialized to 1 in the bands[] init in SDT.ino; cons=-92; slope=10
  dbm = dbm_calibration + bands[currentBand].gainCorrection + (float32_t)attenuator + slope * log10f_fast(audioMaxSquaredAve) + 
        cons - (float32_t)bands[currentBand].RFgain * 1.5 - rfGainAllBands; //DB2OO, 08-OCT-23; added rfGainAllBands
#else
  //DB2OO, 9-OCT-23: audioMaxSquaredAve is proportional to the input power. With rfGainAllBands=0 it is approx. 40 for -73dBm @ 14074kHz with the V010 boards and the pre-Amp fed by 12V
  // for audioMaxSquaredAve=40 audioLogAveSq will be 26
  audioLogAveSq = 10 * log10f_fast(audioMaxSquaredAve) + 10;                                      //AFP 09-18-22
  //DB2OO, 9-OCT-23: calculate dBm value from audioLogAveSq and ignore band gain differences and a potential attenuator like in original code
  dbm = audioLogAveSq - 100;

//DB2OO, 9-OCT-23: this is the orginal code, that will map a 30dB difference (35-5) to 60 (635-575) pixels, i.e. 5 S steps to 5*12 pixels
// SMETER_X is 528 --> X=635 would be 107 pixels / 12pixels per S step --> approx. S9
//  smeterPad = map(audioLogAveSq, 5, 35, 575, 635);                                                //AFP 09-18-22
//  tft.fillRect(SMETER_X + 1, SMETER_Y + 1, smeterPad - SMETER_X, SMETER_BAR_HEIGHT, RA8875_RED);  //AFP 09-18-22
#endif
  // determine length of S-meter bar, limit it to the box and draw it
  smeterPad = map(dbm, -73.0-9*6.0 /*S1*/, -73.0 /*S9*/, 0, 9*pixels_per_s);
  //DB2OO; make sure, that it does not extend beyond the field
  smeterPad = max(0, smeterPad);
  smeterPad = min(SMETER_BAR_LENGTH, smeterPad);
  tft.fillRect(SMETER_X + 1, SMETER_Y + 2, smeterPad, SMETER_BAR_HEIGHT-2, RA8875_RED); //DB2OO: bar 2*1 pixel smaller than the field

  tft.setTextColor(RA8875_WHITE);

  //DB2OO, 17-AUG-23: create PWM analog output signal on the "HW_SMETER" output. This is scaled for a 250uA  S-meter full scale, 
  // connected to HW_SMTER output via a 8.2kOhm resistor and a 4.7kOhm resistor and 10uF capacitor parallel to the S-Meter
#ifdef HW_SMETER
  { int hw_s;
    hw_s = map((int)dbm-3, -73-(8*6), -73+60, 0, 228);
    hw_s = max(0, min(hw_s, 255));
    analogWrite(HW_SMETER, hw_s);
  }
#endif

// DB2OO, 13-OCT-23: if DEBUG_SMETER defined debug messages on S-Meter variables will be put out 
//#define DEBUG_SMETER

#ifdef DEBUG_SMETER
//added, to debug S-Meter display problems
  Serial.printf("DisplaydbM(): dbm=%.1f, dbm_calibration=%.1f, bands[currentBand].gainCorrection=%.1f, attenuator=%d, bands[currentBand].RFgain=%d, rfGainAllBands=%d\n", 
                                dbm, dbm_calibration, bands[currentBand].gainCorrection, attenuator, bands[currentBand].RFgain, rfGainAllBands);
  Serial.printf("\taudioMaxSquaredAve=%.4f, audioLogAveSq=%.1f\n", audioMaxSquaredAve, audioLogAveSq);
#endif

  unit_label = "dBm";
  tft.setFontScale((enum RA8875tsize)0);

  tft.fillRect(SMETER_X + 185, SMETER_Y, 80, tft.getFontHeight(), RA8875_BLACK);  // The dB figure at end of S 
  //DB2OO, 29-AUG-23: consider no decimals in the S-meter dBm value as it is very busy with decimals
  MyDrawFloat(dbm, /*0*/ 1, SMETER_X + 184, SMETER_Y, buff);
  tft.setTextColor(RA8875_GREEN);
  tft.print(unit_label);
}

/*****
  Purpose: Display the current temperature and load figures for T4.1

  Parameter list:
    int notchF        the notch to use
    int MODE          the current MODE

  Return value;
    void
*****/
void ShowTempAndLoad() {
  char buff[10];
  int valueColor = RA8875_GREEN;
  double block_time;
  double processor_load;
  elapsed_micros_mean = elapsed_micros_sum / elapsed_micros_idx_t;

  block_time = 128.0 / (double)SR[SampleRate].rate;  // one audio block is 128 samples and uses this in seconds
  block_time = block_time * N_BLOCKS;

  block_time *= 1000000.0;                                  // now in µseconds
  processor_load = elapsed_micros_mean / block_time * 100;  // take audio processing time divide by block_time, convert to %

  if (processor_load >= 100.0) {
    processor_load = 100.0;
    valueColor = RA8875_RED;
  }

  tft.setFontScale((enum RA8875tsize)0);

  CPU_temperature = TGetTemp();

  tft.fillRect(TEMP_X_OFFSET, TEMP_Y_OFFSET, MAX_WATERFALL_WIDTH, tft.getFontHeight(), RA8875_BLACK);  // Erase current data
  tft.setCursor(TEMP_X_OFFSET, TEMP_Y_OFFSET);
  tft.setTextColor(RA8875_WHITE);
  tft.print("Temp:");
  tft.setCursor(TEMP_X_OFFSET + 120, TEMP_Y_OFFSET);
  tft.print("Load:");

  tft.setTextColor(valueColor);
  MyDrawFloat(CPU_temperature, 1, TEMP_X_OFFSET + tft.getFontWidth() * 3, TEMP_Y_OFFSET, buff);

  tft.drawCircle(TEMP_X_OFFSET + 80, TEMP_Y_OFFSET + 5, 3, RA8875_GREEN);
  MyDrawFloat(processor_load, 1, TEMP_X_OFFSET + 150, TEMP_Y_OFFSET, buff);
  tft.print("%");
  elapsed_micros_idx_t = 0;
  elapsed_micros_sum = 0;
  elapsed_micros_mean = 0;
  tft.setTextColor(RA8875_WHITE);
}

/*****
  Purpose: format a floating point number

  Parameter list:
    float val         the value to format
    int decimals      the number of decimal places
    int x             the x coordinate for display
    int y                 y          "

  Return value;
    void
*****/
void MyDrawFloat(float val, int decimals, int x, int y, char *buff) {
  dtostrf(val, FLOAT_PRECISION, decimals, buff);  // Use 8 as that is the max prevision on a float

  tft.fillRect(x + 15, y, 12 * sizeof(buff), 15, RA8875_BLACK);
  tft.setCursor(x, y);

  tft.print(buff);
}


/*****
  Purpose: Shows the startup settings for the information displayed int he lower-right box.

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateInfoWindow() {
  tft.fillRect(INFORMATION_WINDOW_X - 8, INFORMATION_WINDOW_Y, 250, 170, RA8875_BLACK);    // Clear fields
  tft.drawRect(BAND_INDICATOR_X - 10, BAND_INDICATOR_Y - 2, 260, 200, RA8875_LIGHT_GREY);  // Redraw Info Window Box

  tft.setFontScale((enum RA8875tsize)1);
  UpdateVolumeField();
  UpdateAGCField();

  tft.setFontScale((enum RA8875tsize)0);
  DisplayIncrementField();
  UpdateCompressionField();
  UpdateDecoderField();
  UpdateEEPROMSyncIndicator(syncEEPROM);
  UpdateNoiseField();
  UpdateNotchField();
  UpdateSDIndicator(sdCardPresent);
  UpdateWPMField();
  UpdateZoomField();
}

/*****
  Purpose: Updates the Volume setting on the display

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateVolumeField() {
  tft.setFontScale((enum RA8875tsize)1);

  tft.setCursor(BAND_INDICATOR_X + 20, BAND_INDICATOR_Y);  // Volume
  tft.setTextColor(RA8875_WHITE);
  tft.print("Vol:");
  tft.setTextColor(RA8875_GREEN);
  tft.fillRect(BAND_INDICATOR_X + 90, BAND_INDICATOR_Y, tft.getFontWidth() * 3 + 2, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(FIELD_OFFSET_X, BAND_INDICATOR_Y);
  tft.print(audioVolume);
}


/*****
  Purpose: Updates the AGC on the display.  Long option added. G0ORX September 6, 2023

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateAGCField() {
  tft.setFontScale((enum RA8875tsize)1);
  tft.fillRect(AGC_X_OFFSET, AGC_Y_OFFSET, tft.getFontWidth() * 6, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(BAND_INDICATOR_X + 150, BAND_INDICATOR_Y);
  switch (AGCMode) {  // The opted for AGC
    case 0:           // Off
      tft.setTextColor(DARKGREY);
      tft.print("AGC");
      tft.setFontScale((enum RA8875tsize)0);
      tft.setCursor(BAND_INDICATOR_X + 200, BAND_INDICATOR_Y + 15);
      tft.print(" off");
      tft.setFontScale((enum RA8875tsize)1);
      break;

    case 1:  // Long
      tft.setTextColor(RA8875_YELLOW);
      tft.print("AGC L");
      break;

    case 2:  // Slow
      tft.setTextColor(RA8875_WHITE);
      tft.print("AGC S");
      break;

    case 3:  // Medium
      tft.setTextColor(ORANGE);
      tft.print("AGC M");
      break;

    case 4:  // Fast
      tft.setTextColor(RA8875_GREEN);
      tft.print("AGC F");
      break;

    default:
      break;
  }
}


/*****
  Purpose: Updates the increment setting on the display

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateIncrementField() {
  long selectFT[] = { 10, 50, 250, 500 };
  static int selectFTIndex = 0;  // JJP 6/16/23

  for (int i = 0; i < 4; i++) {  // Get index for current value
    if (stepFineTune == selectFT[i]) {
      selectFTIndex = i;
      break;
    }
  }
  selectFTIndex++;  // JJP 6/16/23
  if (selectFTIndex > 3) {
    selectFTIndex = 0;
  }
  stepFineTune = selectFT[selectFTIndex];

  UpdateEEPROMSyncIndicator(0);
  DisplayIncrementField();
}
/*****
  Purpose: Updates the increment setting on the display

  Parameter list:
    void

  Return value;
    void
*****/
void DisplayIncrementField() {
  tft.setFontScale((enum RA8875tsize)0);
  tft.setTextColor(RA8875_WHITE);  // Frequency increment
  tft.setCursor(INCREMENT_X, INCREMENT_Y);
  tft.print("Increment: ");
  tft.setCursor(INCREMENT_X + 148, INCREMENT_Y);
  tft.print("FT Inc: ");

  tft.fillRect(INCREMENT_X + 90, INCREMENT_Y, tft.getFontWidth() * 7, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(FIELD_OFFSET_X - 3, INCREMENT_Y);
  tft.setTextColor(RA8875_GREEN);
  tft.print(freqIncrement);

  tft.fillRect(INCREMENT_X + 210, INCREMENT_Y, tft.getFontWidth() * 4, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(FIELD_OFFSET_X + 120, INCREMENT_Y);
  tft.setTextColor(RA8875_GREEN);
  tft.print(stepFineTune);
}
/*****
  Purpose: Updates the notch value on the display

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateNotchField() {
  tft.setFontScale((enum RA8875tsize)0);

  if (NR_first_time == 0) {  // Notch setting
    tft.setTextColor(RA8875_LIGHT_GREY);
  } else {
    tft.setTextColor(RA8875_WHITE);
  }
  tft.fillRect(NOTCH_X + 60, NOTCH_Y, 150, tft.getFontHeight() + 5, RA8875_BLACK);
  tft.setCursor(NOTCH_X - 32, NOTCH_Y);
  tft.print("AutoNotch:");
  tft.setCursor(FIELD_OFFSET_X, NOTCH_Y);
  tft.setTextColor(RA8875_GREEN);
  if (ANR_notchOn == 0) {
    tft.print("Off");
  } else {
    tft.print("On");
    ANR_notchOn = 1;  //AFP 10-21-22
  }
}

/*****
  Purpose: Updates the zoom setting on the display

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateZoomField() {
  tft.setFontScale((enum RA8875tsize)0);

  tft.fillRect(ZOOM_X, ZOOM_Y, 100, tft.getFontHeight(), RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE);  // Display zoom factor
  tft.setCursor(ZOOM_X, ZOOM_Y);
  tft.print("Zoom:");
  tft.setCursor(FIELD_OFFSET_X, ZOOM_Y);
  tft.setTextColor(RA8875_GREEN);
  tft.print(zoomOptions[zoomIndex]);
}


/*****
  Purpose: Updates the compression setting in Info Window

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateCompressionField()  // JJP 8/26/2023
{
  tft.fillRect(COMPRESSION_X, COMPRESSION_Y, 200, 15, RA8875_BLACK);
  tft.setFontScale((enum RA8875tsize)0);
  tft.setTextColor(RA8875_WHITE);  // Display zoom factor
  tft.setCursor(COMPRESSION_X, COMPRESSION_Y);
  tft.print("Compress:");
  tft.setCursor(FIELD_OFFSET_X, COMPRESSION_Y);
  tft.setTextColor(RA8875_GREEN);
  if (compressorFlag == 1) {  // JJP 8/26/2023
    tft.print("On  ");
    tft.print(currentMicThreshold);
  } else {
    tft.setCursor(FIELD_OFFSET_X, COMPRESSION_Y);
    tft.print("Off");
  }
}


/*****
  Purpose: Updates whether the decoder is on or off

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateDecoderField() {
  tft.setFontScale((enum RA8875tsize)0);

  tft.setTextColor(RA8875_WHITE);  // Display zoom factor
  tft.setCursor(DECODER_X, DECODER_Y);
  tft.print("Decoder:");
  tft.setTextColor(RA8875_GREEN);
  tft.fillRect(DECODER_X + 60, DECODER_Y, tft.getFontWidth() * 20, tft.getFontHeight() + 5, RA8875_BLACK);
  tft.setCursor(FIELD_OFFSET_X, DECODER_Y);
  if (decoderFlag == DECODE_ON) {  // AFP 09-27-22
    tft.print("On ");
  } else {
    tft.print("Off");
  }
  if (xmtMode == CW_MODE && decoderFlag == DECODE_ON) {  // In CW mode with decoder on? AFP 09-27-22
    tft.setFontScale((enum RA8875tsize)0);
    tft.setTextColor(RA8875_LIGHT_GREY);
    tft.setCursor(FIELD_OFFSET_X, DECODER_Y + 15);
    tft.print("CW Fine Adjust");
  } else {
    tft.fillRect(FIELD_OFFSET_X, DECODER_Y + 15, tft.getFontWidth() * 15, tft.getFontHeight(), RA8875_BLACK);
  }
}


/*****
  Purpose: Updates the WPM setting on the display

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateWPMField() {
  tft.setFontScale((enum RA8875tsize)0);

  tft.setTextColor(RA8875_WHITE);  // Display zoom factor
  tft.setCursor(WPM_X, WPM_Y);
  tft.print("Keyer:");
  tft.setTextColor(RA8875_GREEN);
  tft.fillRect(WPM_X + 60, WPM_Y, tft.getFontWidth() * 15, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(FIELD_OFFSET_X, WPM_Y);
  EEPROMData.currentWPM = currentWPM;
  if (EEPROMData.keyType == KEYER) {
    //tft.print("Paddles -- "); // KD0RC
    // KD0RC start
    tft.print("Paddles ");
    if (paddleFlip == 0) {
      tft.print("R");
    } else {
      tft.print("L");
    }
    tft.print(" ");
    // KD0RC end
    tft.print(EEPROMData.currentWPM);
  } else {
    tft.print("Straight Key");
  }
}


/*****
  Purpose: Updates the noise field on the display

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateNoiseField() {
  const char *filter[] = { "Off", "Kim", "Spectral", "LMS" };  //AFP 09-19-22

  tft.setFontScale((enum RA8875tsize)0);

  tft.fillRect(FIELD_OFFSET_X, NOISE_REDUCE_Y, 70, tft.getFontHeight(), RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE);  // Noise reduction
  tft.setCursor(NOISE_REDUCE_X, NOISE_REDUCE_Y);
  tft.print("Noise:");
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(FIELD_OFFSET_X, NOISE_REDUCE_Y);
  tft.print(filter[nrOptionSelect]);
}

/*****
  Purpose: Used to save a favortie frequency to EEPROM

  Parameter list:

  Return value;
    void
*****/
void SetFavoriteFrequencies() {
  int index;
  int offset;
  unsigned long currentFreqs[MAX_FAVORITES];

  EEPROMStuffFavorites(currentFreqs);  // Read current values

  tft.setFontScale((enum RA8875tsize)0);

  offset = tft.getFontHeight();

  tft.fillRect(BAND_INDICATOR_X - 10, BAND_INDICATOR_Y - 2, 260, 200, RA8875_BLACK);  // Clear volume field
  for (index = 0; index < MAX_FAVORITES; index++) {
    tft.setTextColor(RA8875_GREEN);
    tft.setCursor(BAND_INDICATOR_X - 5, BAND_INDICATOR_Y + (offset * index) + 5);
    if (index < 9)
      tft.print(" ");
    tft.print(index + 1);
    tft.print(". ");
    if (currentFreqs[index] < 10000000UL)
      tft.print(" ");
    tft.setTextColor(RA8875_WHITE);
    tft.print(currentFreqs[index]);
  }
  //==================

  int startValue = 0;
  int currentValue, lastValue, oldIndex;

  lastValue = startValue;
  currentValue = -1;

  index = 0;
  oldIndex = -1;
  tft.fillRect(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * index) + 5, 70, 15, RA8875_MAGENTA);
  // tft.setTextColor(RA8875_WHITE);
  tft.setTextColor(RA8875_BLACK);  // JJP 7/17/23

  while (true) {
    //MyDelay(ENCODER_DELAY);
    if (currentValue != lastValue) {
      oldIndex = index;
      if (currentValue > lastValue) {  // Adjust frequency index
        index++;
      } else {
        index--;
      }
      if (index < 0)  // Check for over/underflow
        index = 0;
      if (index == MAX_FAVORITES)
        index = MAX_FAVORITES - 1;

      lastValue = currentValue;  // Reset for another pass?

      tft.fillRect(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * oldIndex) + 5, 70, 15, RA8875_BLACK);
      tft.setCursor(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * oldIndex) + 5);
      tft.print(currentFreqs[oldIndex]);

      tft.fillRect(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * index) + 5, 70, 15, RA8875_MAGENTA);
      tft.setCursor(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * index) + 5);
      tft.print(currentFreq);
    }
    selectExitMenues.update();  // Exit submenu button
    if (selectExitMenues.fallingEdge()) {
      EEPROMData.favoriteFreqs[index] = currentFreq;  // Update the EEPROM value
      EEPROMWrite();                                  // Save it
      break;
    }
  }
  tft.drawRect(BAND_INDICATOR_X - 10, BAND_INDICATOR_Y - 1, 260, 185, RA8875_LIGHT_GREY);
  tft.fillRect(BAND_INDICATOR_X - 9, BAND_INDICATOR_Y + 1, 180, 178, RA8875_BLACK);  // Clear volume field
  DrawAudioSpectContainer();
  UpdateInfoWindow();
}

/*****
  Purpose: Implement the notch filter

  Parameter list:
    int notchF        the notch to use
    int MODE          the current MODE

  Return value;
    void
*****/
void ShowNotch() {
  tft.fillRect(NOTCH_X, NOTCH_Y + 30, 150, tft.getFontHeight() + 5, RA8875_BLACK);
  if (NR_first_time == 0)
    tft.setTextColor(RA8875_LIGHT_GREY);
  else
    tft.setTextColor(RA8875_WHITE);
  tft.setCursor(NOTCH_X - 6, NOTCH_Y);
  tft.print("Auto Notch:");
  tft.setCursor(NOTCH_X + 90, NOTCH_Y);
  tft.setTextColor(RA8875_GREEN);

  if (ANR_notchOn) {
    tft.print("On");
  } else {
    tft.print("Off");
  }

}  // end void show_notch



/*****
  Purpose: This function draws the Info Window frame

  Parameter list:
    void

  Return value;
    void
*****/
void DrawInfoWindowFrame() {
  tft.drawRect(BAND_INDICATOR_X - 10, BAND_INDICATOR_Y - 2, 260, 200, RA8875_LIGHT_GREY);
  tft.fillRect(TEMP_X_OFFSET, TEMP_Y_OFFSET + 80, 80, tft.getFontHeight() + 10, RA8875_BLACK);  // Clear volume field
}


/*****
  Purpose: This function redraws the entire display screen where the equalizers appeared

  Parameter list:
    void

  Return value;
    void
*****/
void RedrawDisplayScreen() {
  tft.fillWindow();
  DisplayIncrementField();
  AGCPrep();
  UpdateAGCField();
  EncoderVolume();
  SetBand();
  BandInformation();
  ShowCurrentPowerSetting();
  ShowFrequency();
  SetFreq();
  SetBandRelay(HIGH);
  SpectralNoiseReductionInit();
  UpdateNoiseField();
  SetI2SFreq(SR[SampleRate].rate);
  DrawBandWidthIndicatorBar();
  ShowName();
  ShowTransmitReceiveStatus();
  DrawSMeterContainer();
  DrawAudioSpectContainer();
  DrawSpectrumDisplayContainer();
  DrawFrequencyBarValue();
  ShowSpectrumdBScale();

  UpdateInfoWindow();
}

/*****
  Purpose: Draw Tuned Bandwidth on Spectrum Plot // AFP 03-27-22 Layers

  Parameter list:

  Return value;
    void
*****/
FASTRUN void DrawBandWidthIndicatorBar()  // AFP 10-30-22
{
  float zoomMultFactor = 0.0;
  float Zoom1Offset = 0.0;

  switch (zoomIndex) {
    case 0:
      zoomMultFactor = 0.5;
      Zoom1Offset = 24000 * 0.0053333;
      break;

    case 1:
      zoomMultFactor = 1.0;
      Zoom1Offset = 0;
      break;

    case 2:
      zoomMultFactor = 2.0;
      Zoom1Offset = 0;
      break;

    case 3:
      zoomMultFactor = 4.0;
      Zoom1Offset = 0;
      break;

    case 4:
      zoomMultFactor = 8.0;
      Zoom1Offset = 0;
      break;
  }
  newCursorPosition = (int)(NCOFreq * 0.0053333) * zoomMultFactor - Zoom1Offset;  // AFP 10-28-22

  tft.writeTo(L2);
  //  tft.clearMemory();              // This destroys the CW filter graphics, removed.  KF5N July 30, 2023
  //  tft.clearScreen(RA8875_BLACK);  // This causes an audio hole in fine tuning.  KF5N 7-16-23

  pixel_per_khz = ((1 << spectrum_zoom) * SPECTRUM_RES * 1000.0 / SR[SampleRate].rate);
  filterWidth = (int)(((bands[currentBand].FHiCut - bands[currentBand].FLoCut) / 1000.0) * pixel_per_khz * 1.06);  // AFP 10-30-22
  //======================= AFP 09-22-22

  switch (bands[currentBand].mode) {
    case DEMOD_LSB:
      tft.fillRect(centerLine - filterWidth + oldCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth * 1.0, SPECTRUM_HEIGHT - 20, RA8875_BLACK);  // Was 0.96.  KF5N July 31, 2023
      tft.fillRect(centerLine - filterWidth + newCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, FILTER_WIN);
      //      tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_BLACK);         // Yep. Erase old, draw new...
      //     tft.drawFastVLine(centerLine + newCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_CYAN); //AFP 10-20-22
      break;

    case DEMOD_USB:
      tft.fillRect(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_BLACK);  //AFP 03-27-22 Layers
      tft.fillRect(centerLine + newCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, FILTER_WIN);    //AFP 03-27-22 Layers
                                                                                                                           //      tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_BLACK); // Yep. Erase old, draw new...//AFP 03-27-22 Layers
                                                                                                                           //      tft.drawFastVLine(centerLine + newCursorPosition , SPECTRUM_TOP_Y + 20, h - 10, RA8875_CYAN); //AFP 03-27-22 Layers
      break;

    case DEMOD_AM:
      tft.fillRect(centerLine - filterWidth / 2 + oldCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_BLACK);                //AFP 10-30-22
      tft.fillRect(centerLine - (filterWidth / 2) * 0.93 + newCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth * 0.95, SPECTRUM_HEIGHT - 20, FILTER_WIN);  //AFP 10-30-22
                                                                                                                                                           //      tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_BLACK);                 // AFP 10-30-22
                                                                                                                                                           //      tft.drawFastVLine(centerLine + newCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_CYAN);                 //AFP 10-30-22*/
      break;

    case DEMOD_SAM:
      tft.fillRect(centerLine - filterWidth / 2 + oldCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_BLACK);                //AFP 10-30-22
      tft.fillRect(centerLine - (filterWidth / 2) * 0.93 + newCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth * 0.95, SPECTRUM_HEIGHT - 20, FILTER_WIN);  //AFP 10-30-22
                                                                                                                                                           //      tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_BLACK);                 // AFP 10-30-22
                                                                                                                                                           //      tft.drawFastVLine(centerLine + newCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_CYAN);                 //AFP 10-30-22*/
      break;
  }
  tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_BLACK);  // refactored from above JJP 7/12/23
  tft.drawFastVLine(centerLine + newCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_CYAN);
  //  BandInformation();  //AFP 03-27-22 Layers
  oldCursorPosition = newCursorPosition;

  tft.writeTo(L1);  //AFP 03-27-22 Layers
}

/*****
  Purpose: This function removes the spectrum display container

  Parameter list:
    void

  Return value;
    void
*****/
void EraseSpectrumDisplayContainer() {
  tft.fillRect(SPECTRUM_LEFT_X - 2, SPECTRUM_TOP_Y - 1, MAX_WATERFALL_WIDTH + 6, SPECTRUM_HEIGHT + 8, RA8875_BLACK);  // Spectrum box
}
/*****
  Purpose: This function erases the contents of the spectrum display

  Parameter list:
    void

  Return value;
    void
*****/
void EraseSpectrumWindow() {
  tft.fillRect(SPECTRUM_LEFT_X, SPECTRUM_TOP_Y, MAX_WATERFALL_WIDTH, SPECTRUM_HEIGHT, RA8875_BLACK);  // Spectrum box
}
/*****
  Purpose: To erase both primary and secondary menus from display

  Parameter list:

  Return value;
    void
*****/
void EraseMenus() {
  tft.fillRect(PRIMARY_MENU_X, MENUS_Y, BOTH_MENU_WIDTHS, CHAR_HEIGHT + 1, RA8875_BLACK);  // Erase menu choices
  menuStatus = NO_MENUS_ACTIVE;                                                            // Change menu state
}
/*****
  Purpose: To erase primary menu from display

  Parameter list:

  Return value;
    void
*****/
void ErasePrimaryMenu() {
  tft.fillRect(PRIMARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT + 1, RA8875_BLACK);  // Erase menu choices
  menuStatus = NO_MENUS_ACTIVE;                                                           // Change menu state
}
/*****
  Purpose: To erase secondary menu from display

  Parameter list:

  Return value;
    void
*****/
void EraseSecondaryMenu() {
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT + 1, RA8875_BLACK);  // Erase menu choices
  menuStatus = NO_MENUS_ACTIVE;                                                             // Change menu state
}

/*****
  Purpose: Shows transmit (red) and receive (green) mode

  Parameter list:

  Return value;
    void
*****/
void ShowTransmitReceiveStatus() {
  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_BLACK);
  if (xrState == TRANSMIT_STATE) {
    tft.fillRect(X_R_STATUS_X, X_R_STATUS_Y, 55, 25, RA8875_RED);
    tft.setCursor(X_R_STATUS_X + 4, X_R_STATUS_Y - 5);
    tft.print("XMT");
  } else {
    tft.fillRect(X_R_STATUS_X, X_R_STATUS_Y, 55, 25, RA8875_GREEN);
    tft.setCursor(X_R_STATUS_X + 4, X_R_STATUS_Y - 5);
    tft.print("REC");
  }
}


/*****
  Purpose: Provides a display indicator that SD card is in the system

  Parameter list:
    int present                    0 = no SD card, 1 = SD

  Return value;
    void

*****/
void UpdateSDIndicator(int present) {
  return;  // Just go home   JJP  7/25/23
  /*
  tft.setFontScale( (enum RA8875tsize) 0);
  if (present == 1) {                                                   // SD card present
    tft.fillRect(SD_X, SD_Y, tft.getFontWidth() * 7, tft.getFontHeight(), RA8875_GREEN);
    tft.setTextColor(RA8875_BLACK);
  } else {                                                                  // Erase message
    tft.fillRect(SD_X, SD_Y, tft.getFontWidth() * 7, tft.getFontHeight(), RA8875_RED);
    tft.setTextColor(RA8875_WHITE);
  }
  tft.setCursor(SD_X, SD_Y);
  tft.print("  SD  ");  
  */
}
