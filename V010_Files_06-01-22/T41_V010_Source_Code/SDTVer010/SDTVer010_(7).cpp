
#ifndef BEENHERE
#include "SDT.h"
#endif

const uint16_t gradient[] = {       // Color array for waterfall
  0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,
  0x10,   0x1F,  0x11F,  0x19F,  0x23F,  0x2BF,  0x33F,  0x3BF,  0x43F,  0x4BF,
  0x53F,  0x5BF,  0x63F,  0x6BF,  0x73F,  0x7FE,  0x7FA,  0x7F5,  0x7F0,  0x7EB,
  0x7E6,  0x7E2, 0x17E0, 0x3FE0, 0x67E0, 0x8FE0, 0xB7E0, 0xD7E0, 0xFFE0, 0xFFC0,
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

/*****
  Purpose: Draw Tuned Bandwidth on Spectrum Plot // AFP 03-27-22 Layers

  Parameter list:

  Return value;
    void
*****/
void DrawBandWidthIndicatorBar() {

    tft.writeTo(L2);//AFP 03-27-22 Layers
  tft.clearMemory();//AFP 03-27-22 Layers
  if (bands[currentBand].mode == DEMOD_LSB) {

    tft.fillRect(centerLine - filterWidth + oldCursorPosition , SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_BLACK); //AFP 03-27-22 Layers
    tft.fillRect(centerLine - filterWidth + newCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, FILTER_WIN); //AFP 03-27-22 Layers
   // tft.fillRect(centerLine - filterWidth + newCursorPosition, SPECTRUM_TOP_Y + 20, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_YELLOW); //AFP 03-27-22 Layers
    tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 20, RA8875_BLACK); // Yep. Erase old, draw new...//AFP 03-27-22 Layers
    tft.drawFastVLine(centerLine  + newCursorPosition , SPECTRUM_TOP_Y + 20, h - 20, RA8875_CYAN); //AFP 03-27-22 Layers
  }
  else {
    if (bands[currentBand].mode == DEMOD_USB) {

      tft.fillRect(centerLine  + oldCursorPosition , SPECTRUM_TOP_Y + 17, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_BLACK); //AFP 03-27-22 Layers
      tft.fillRect(centerLine + newCursorPosition, SPECTRUM_TOP_Y + 17, filterWidth, SPECTRUM_HEIGHT - 20, FILTER_WIN); //AFP 03-27-22 Layers
      tft.drawFastVLine(centerLine + oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 10, RA8875_BLACK); // Yep. Erase old, draw new...//AFP 03-27-22 Layers
      tft.drawFastVLine(centerLine + newCursorPosition , SPECTRUM_TOP_Y + 20, h - 10, RA8875_CYAN); //AFP 03-27-22 Layers
    }
  }
  oldCursorPosition = newCursorPosition;

  tft.writeTo(L1); //AFP 03-27-22 Layers
}

/*****
  Purpose: Draw audio spectrum box  AFP added 3-14-21

  Parameter list:

  Return value;
    void
*****/
void DrawAudioSpectContainer() {
  tft.drawRect(BAND_INDICATOR_X - 9 , SPECTRUM_BOTTOM - 118, 255, 118, RA8875_GREEN);
  for (int k = 0; k < 7; k++) {
    tft.drawFastVLine(BAND_INDICATOR_X - 10 + k * 42.5, SPECTRUM_BOTTOM, 15, RA8875_GREEN);
    tft.setCursor(BAND_INDICATOR_X - 14 + k * 42.5, SPECTRUM_BOTTOM + 16);
    tft.print(k); tft.print("k");
  }
}

/*****
  Purpose: Show the program name and version number

  Parameter list:
    void

  Return value;
    void
*****/
void ShowName()
{
  tft.fillRect(RIGNAME_X_OFFSET, 0, XPIXELS - RIGNAME_X_OFFSET, tft.getFontHeight(), RA8875_BLACK);

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(RIGNAME_X_OFFSET, 1);
  tft.print(RIGNAME);
  tft.setFontScale(0);
  tft.print(" ");                           // Added to correct for deleted leading space 4/16/2022 JACK
  tft.print(VERSION);
}


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
void ShowSpectrum()  //AFP Extensively Modified 3-15-21 Adjusted 12-13-21 to align all elements
{
    int centerLine              =  (MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2;
    int middleSlice             = centerLine / 2;                               // Approximate center element
    int j2;
    int x1                      = 0; //AFP
    int h                       = SPECTRUM_HEIGHT + 3;
    int filterLoPositionMarker;
    int filterHiPositionMarker;

    if (displayMode == NO_DISPLAY)                      // We're done...
      return;

    for (int j = 0; j < MAX_WATERFALL_WIDTH - 1; j++) {     // map pixel colors
      j2 = map(j, 0, MAX_WATERFALL_WIDTH, 0, SPECTRUM_RES - 1);
      //pixelnew2[j] = (pixelnew[j2] + 20) - (j * 0.06);
      // pixelold2[j] = (pixelold[j2] + 20) - (j * 0.06);
      pixelnew2[j] = pixelnew[j2];
      pixelold2[j] = pixelold[j2];
    }

    tft.drawFastVLine(centerLine, SPECTRUM_TOP_Y, h, RA8875_GREEN);     // Draws centerline on spectrum display

    pixelnew[0] = 0;
    pixelnew[1] = 0;
    pixelold[0] = 0;
    pixelold[1] = 0;

    for (x1 = 1; x1 < MAX_WATERFALL_WIDTH - 1; x1++)  //AFP, JJP changed init from 0 to 1 for x1: out of bounds addressing in line 112
      //Draws the main Spectrum, Waterfall and Audio displays
    {
      if (x1 == 1) {
        updateDisplayFlag = 1;  //Set flag so the display data are saved only once during each display refresh cycle at the start of the cycle, not 512 times
      }
      else updateDisplayFlag = 0;  //  Do not save the the display data for the remainder of the

      ProcessIQData();  // Call the Audio process from within the display routine to eliminate conflicts with drawing the spectrum and waterfall displays
      EncoderTune();    //Moved the tuning encoder to reduce lag times and interference during tuning.

      if (IQCalFlag != 1) {
        EncoderFilterAndWPM();
      }

      y_new  = pixelnew[x1];
      y1_new = pixelnew[x1 - 1];
      y_old  = pixelold[x1];
      y_old2 = pixelold[x1 - 1];
      // I don't like this coding style, but do it here so more lines can be seen at once
      if (y_new > base_y)      y_new = base_y;
      if (y_old > base_y)      y_old = base_y;
      if (y_old2 > base_y)     y_old2 = base_y;
      if (y1_new > base_y)     y1_new = base_y;

      if (y_new < 0)            y_new = 0;
      if (y_old < 0)            y_old = 0;
      if (y_old2 < 0)          y_old2 = 0;
      if (y1_new < 0)          y1_new = 0;

      if (displayMode != NO_DISPLAY && displayMode != WATERFALL_ONLY) {
        tft.drawLine(x1 + 2 , spectrumNoiseFloor - y_old2, x1 + 2 , spectrumNoiseFloor - y_old , RA8875_BLACK);   // Erase old...
        tft.drawLine(x1 + 2 , spectrumNoiseFloor - y1_new, x1 + 2 , spectrumNoiseFloor - y_new , RA8875_YELLOW);  // Draw new
      }
      if (x1 < 253) {
        tft.drawFastVLine(BAND_INDICATOR_X - 8 + x1, SPECTRUM_BOTTOM - 116, 115, RA8875_BLACK);                            //AFP Erase old AUDIO spectrum line
        if (audioYPixel[x1] != 0) {
          if (audioYPixel[x1] > CLIP_AUDIO_PEAK)             // audioSpectrumHeight = 118
            audioYPixel[x1] = CLIP_AUDIO_PEAK;
          if (x1 == middleSlice) {
            smeterLength = y_new;
          }
          tft.drawFastVLine(BAND_INDICATOR_X - 8 + x1, AUDIO_SPECTRUM_BOTTOM - audioYPixel[x1] - 1, audioYPixel[x1] - 2, RA8875_MAGENTA);   //AFP draw new AUDIO spectrum line
        }

        //      if (displayMode != NO_DISPLAY || displayMode != SPECTRUM_ONLY) {
        // The following lines calculate the position of the Filter bar below the spectrum display
        // and then draw the Audio spectrum in its own container to the right of the Main spectrum display
        filterLoPositionMarker = map(bands[currentBand].FLoCut, 0, 6000, 0, 256);
        filterHiPositionMarker = map(bands[currentBand].FHiCut, 0, 6000, 0, 256);
        //Draw Fiter indicator lines on audio plot
        tft.drawLine(BAND_INDICATOR_X - 10 + abs(filterLoPositionMarker), SPECTRUM_BOTTOM, BAND_INDICATOR_X - 10 + abs(filterLoPositionMarker), SPECTRUM_BOTTOM - 115, RA8875_LIGHT_GREY);
        tft.drawLine(BAND_INDICATOR_X - 10 + abs(filterHiPositionMarker), SPECTRUM_BOTTOM, BAND_INDICATOR_X - 10 + abs(filterHiPositionMarker), SPECTRUM_BOTTOM - 115, RA8875_LIGHT_GREY);

        if (filterLoPositionMarker != filterLoPositionMarkerOld || filterHiPositionMarker != filterHiPositionMarkerOld) { // AFP 03-27-22 Layers
          DrawBandWidthIndicatorBar();// AFP 03-27-22 Layers
        }
        filterLoPositionMarkerOld = filterLoPositionMarker; // AFP 03-27-22 Layers
        filterHiPositionMarkerOld = filterHiPositionMarker; // AFP 03-27-22 Layers

      }
      waterfall[x1] = gradient[y_new - 20];
      tft.writeTo(L1);
    }//AFP 03-27-22 Layers
    //  }
    // End for(...) Draw MAX_WATERFALL_WIDTH spectral points
    if (displayMode != NO_DISPLAY && displayMode != SPECTRUM_ONLY) {
      // Use the Block Transfer Engine to move waterfall down a line
      tft.BTE_move(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE, MAX_WATERFALL_WIDTH, MAX_WATERFALL_ROWS - 2, WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, 1, 2);
      while (tft.readStatus())                                  // Make sure it is done.  Memory moves can take time.
        ;                                                       // Now bring waterfall back to the beginning of the 2nd row.
      tft.BTE_move(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, MAX_WATERFALL_WIDTH, MAX_WATERFALL_ROWS - 2, WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, 2);
      while (tft.readStatus())                                  // Make sure it's done.
        ;
      // Then write new row data into the missing top row to get a scroll effect using display hardware, not the CPU.
      tft.writeRect(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE, MAX_WATERFALL_WIDTH, 1, waterfall);
    }
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
  void ShowBandwidth()
  {
    char buff[10];
    int centerLine  = (MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2;

    if (spectrum_zoom != SPECTRUM_ZOOM_1)
      spectrum_pos_centre_f = 128 * xExpand - 1;      //AFP
    else
      spectrum_pos_centre_f = 64 * xExpand;           //AFP

    float32_t pixel_per_khz = ((1 << spectrum_zoom) * SPECTRUM_RES * 1000.0 / SR[SampleRate].rate) ; //AFP
    int pos_left            = centerLine + (int)(bands[currentBand].FLoCut / 1000.0 * pixel_per_khz);
    if (pos_left < spectrum_x) {
      pos_left = spectrum_x;
    }

    filterWidthX = pos_left + newCursorPosition - centerLine;

    tft.setFontScale( (enum RA8875tsize) 0);
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

    tft.setCursor(FILTER_PARAMETERS_X + 250, FILTER_PARAMETERS_Y - 50);
    tft.setCursor(FILTER_PARAMETERS_X + 180, FILTER_PARAMETERS_Y);
    tft.print("Sample Rate: ");
    tft.print(SR[SampleRate].text);                                                     // Example: 192k

    //  UpdateIncrementField();
    tft.setTextColor(RA8875_WHITE); // set text color to white for other print routines not to get confused ;-)
  }

  /*****
    Purpose: DrawSMeterContainer()
    Parameter list:
      void
    Return value;
      void
  *****/
  void DrawSMeterContainer()
  {
    int i;

    tft.drawFastHLine (SMETER_X, SMETER_Y - 1, 12 * s_w, RA8875_WHITE);
    tft.drawFastHLine (SMETER_X, SMETER_Y + 20, 12 * s_w, RA8875_WHITE);          // changed 6 to 20

    for (i = 0; i < 10; i++) {                                                    // Draw tick marks
      tft.drawFastVLine (SMETER_X + i * 12.2, SMETER_Y - 6, 7, RA8875_WHITE);     // charge 8 to 18
    }

    tft.drawFastHLine (SMETER_X + 120, SMETER_Y - 1,  62, RA8875_GREEN);
    tft.drawFastHLine (SMETER_X + 120, SMETER_Y + 20, 62, RA8875_GREEN);

    for (i = 0; i < 5; i++) {                                                     // Draw tick marks
      tft.drawFastVLine (SMETER_X + 120 + i * 12, SMETER_Y - 6, 7, RA8875_GREEN); // charge 8 to 18
    }

    tft.drawFastVLine (SMETER_X,       SMETER_Y - 1, 20, RA8875_WHITE);           // charge 8 to 18
    tft.drawFastVLine (SMETER_X + 182, SMETER_Y - 1, 20, RA8875_GREEN);


    tft.setFontScale( (enum RA8875tsize) 0);

    tft.setTextColor(RA8875_WHITE);
    tft.setCursor(SMETER_X - 10,  SMETER_Y - 25);
    tft.print("S");
    tft.setCursor(SMETER_X + 7,   SMETER_Y - 25);
    tft.print("1");
    tft.setCursor(SMETER_X + 31,  SMETER_Y - 25);    // was 28, 48, 68, 88, 120 and -15 changed to -20
    tft.print("3");
    tft.setCursor(SMETER_X + 55,  SMETER_Y - 25);
    tft.print("5");
    tft.setCursor(SMETER_X + 79,  SMETER_Y - 25);
    tft.print("7");
    tft.setCursor(SMETER_X + 103, SMETER_Y - 25);
    tft.print("9");
    tft.setCursor(SMETER_X + 145, SMETER_Y - 25);
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
  void ShowSpectrumdBScale()
  {
    tft.setFontScale( (enum RA8875tsize) 0);

    tft.fillRect(SPECTRUM_LEFT_X, SPECTRUM_TOP_Y + 10, 33, tft.getFontHeight(), RA8875_BLACK);
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
  void DrawSpectrumDisplayContainer()
  {
    if (displayMode == BOTH_DISPLAYS || displayMode == SPECTRUM_ONLY) {
      tft.drawRect(SPECTRUM_LEFT_X - 1, SPECTRUM_TOP_Y, MAX_WATERFALL_WIDTH + 2, SPECTRUM_HEIGHT,  RA8875_YELLOW);  // Spectrum box
    }
  }


  /*****
    Purpose: This function draws the frequency bar at the bottom of the spectrum scope, putting markers at every
              graticule and the full frequency

    Parameter list:
      void
    Return value;
      void
      // This function draws the frequency bar at the bottom of the spectrum scope, putting markers at every graticule and the full frequency
  *****/
  void DrawFrequencyBarValue()
  {
    char txt[16];
    char temp[16];

    int bignum;
    int centerIdx;
    int pos_help;
    int smallnum;
    float disp_freq;

    float freq_calc;
    float grat;
    int centerLine =  MAX_WATERFALL_WIDTH / 2 + SPECTRUM_LEFT_X;

    // positions for graticules: first for spectrum_zoom < 3, then for spectrum_zoom > 2
    const static int idx2pos[2][9] = {
      { -43, 21, 52, 250, 250, 250, 232, 250, 315},
      { -43, 21, 50,  86, 200, 200, 232, 218, 323}
    };

    grat = (float)(SR[SampleRate].rate / 8000.0) / (float)(1 << spectrum_zoom);     // 1, 2, 4, 8, 16, 32, 64 . . . 4096

    tft.setTextColor(RA8875_WHITE);
    tft.setFontScale( (enum RA8875tsize) 0);
    tft.fillRect(WATERFALL_LEFT_X, WATERFALL_TOP_Y, MAX_WATERFALL_WIDTH, tft.getFontHeight() + 5, RA8875_BLACK);  // 4-16-2022 JACK

    freq_calc = (float)(centerFreq / NEW_SI5351_FREQ_MULT);      // get current frequency in Hz

    if (activeVFO == VFO_A) {
      currentFreqA = TxRxFreq;
    } else {
      currentFreqB = TxRxFreq;
    }

    if (spectrum_zoom == 0) {
      freq_calc += (float32_t)SR[SampleRate].rate / 4.0;
    }

    if (spectrum_zoom < 3) {
      freq_calc = roundf(freq_calc / 1000);       // round graticule frequency to the nearest kHz
    } else if (spectrum_zoom < 5) {
      freq_calc = roundf(freq_calc / 100) / 10;   // round graticule frequency to the nearest 100Hz
    } else if (spectrum_zoom == 5) {              // 32x
      freq_calc = roundf(freq_calc / 50) / 20;    // round graticule frequency to the nearest 50Hz
    } else if (spectrum_zoom < 8) {
      freq_calc = roundf(freq_calc / 10) / 100 ;  // round graticule frequency to the nearest 10Hz
    } else {
      freq_calc = roundf(freq_calc) / 1000;       // round graticule frequency to the nearest 1Hz
    }

    if (spectrum_zoom != 0)
      centerIdx = 0;
    else
      centerIdx = -2;

    /**************************************************************************************************
      CENTER FREQUENCY PRINT
    **************************************************************************************************/
    if (spectrum_zoom < 3) {
      //    snprintf(txt, 16, "  %lu  ", (ulong)(freq_calc + (centerIdx * grat))); // build string for center frequency precision 1khz
      ultoa((freq_calc + (centerIdx * grat)), txt, DEC);
    } else {
      disp_freq = freq_calc + (centerIdx * grat);
      bignum = (int)disp_freq;
      itoa(bignum, txt, DEC);                               // Make into a string
      if (spectrum_zoom < 8) {
        smallnum = (int)roundf((disp_freq - bignum) * 100);
        //      snprintf(txt, 16, "  %u.%02u  ", bignum, smallnum); // build string for center frequency precision 100Hz/10Hz/1Hz
      } else {
        smallnum = (int)roundf((disp_freq - bignum) * 1000);
        //     snprintf(txt, 16, "  %u.%03u  ", bignum, smallnum); // build string for center frequency precision 100Hz/10Hz/1Hz
      }
      itoa(smallnum, temp, DEC);
      strcat(txt, ".");
      strcat(txt, temp);
    }
    //i = centerIdx2pos[centerIdx + 2] - ((strlen(txt) - 4) * 4);             // calculate position of center frequency text
    tft.setTextColor(RA8875_GREEN);
    tft.setCursor(centerLine - 35, WATERFALL_TOP_Y ); //AFP 3-13-21
    tft.print(txt);
    tft.setTextColor(RA8875_WHITE);
    /**************************************************************************************************
       PRINT ALL OTHER FREQUENCIES (NON-CENTER)
     **************************************************************************************************/
    // snprint() extremely memory inefficient. replaced with simple str?? functions JJP
    for (int idx = -4; idx < 5; idx++) {
      pos_help = idx2pos[spectrum_zoom < 3 ? 0 : 1][idx + 4];
      if (idx != centerIdx) {
        if (spectrum_zoom < 3) {
          //   snprintf(txt, 16, " %lu ", (ulong)(freq_calc + (idx * grat))); // build string for middle-left frequency (1khz precision)
          //        c = &txt[strlen(txt) - 3]; // point at 2nd character from the end
          ultoa((freq_calc + (idx * grat)), txt, DEC);
        } else {
          disp_freq = freq_calc + (idx * grat);
          bignum = (int)disp_freq;
          itoa(bignum, txt, DEC);
          if (spectrum_zoom < 8) {
            smallnum = (int)roundf((disp_freq - bignum) * 100);
            //     snprintf(txt, 16, "  %u.%02u  ", bignum, smallnum); // build string for center frequency precision 100Hz/10Hz/1Hz
          } else {
            smallnum = (int)roundf((disp_freq - bignum) * 1000);
            //    snprintf(txt, 16, "  %u.%03u  ", bignum, smallnum); // build string for center frequency precision 100Hz/10Hz/1Hz
          }
          itoa(smallnum, temp, DEC);
          strcat(txt, ".");
          strcat(txt, temp);
        }
        tft.setCursor(WATERFALL_LEFT_X + pos_help * xExpand + 35, WATERFALL_TOP_Y ); //
        tft.print(txt);
        if (idx < 4) {
          tft.drawFastVLine((WATERFALL_LEFT_X + pos_help * xExpand + 60), WATERFALL_TOP_Y - 5, 7, RA8875_YELLOW);    // Tick marks depending on zoom
        } else {
          tft.drawFastVLine((WATERFALL_LEFT_X + (pos_help + 9) * xExpand + 60), WATERFALL_TOP_Y - 5, 7, RA8875_YELLOW);
        }
      }
      if (spectrum_zoom > 2 || freq_calc > 1000) {
        idx++;
      }
    }

    tft.setFontScale( (enum RA8875tsize) 1);
    //  BandInformation();
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
  void ShowAnalogGain()
  {
    static uint8_t RF_gain_old = 0;
    static uint8_t RF_att_old  = 0;
    const uint16_t col = RA8875_GREEN;
    if ((((bands[currentBand].RFgain != RF_gain_old) || (RF_attenuation != RF_att_old)) && twinpeaks_tested == 1) || write_analog_gain)
    {
      tft.setFontScale( (enum RA8875tsize) 0);
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

      tft.setFontScale( (enum RA8875tsize) 0);

      tft.setTextColor(RA8875_BLACK);
      tft.print("dB");
      tft.setTextColor(RA8875_WHITE);
      tft.print("dB");
      RF_gain_old = bands[currentBand].RFgain;
      RF_att_old = RF_attenuation;
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
  void BandInformation() // SSB or CW
  {
    tft.setFontScale( (enum RA8875tsize) 0);
    tft.setTextColor(RA8875_GREEN);

    tft.setCursor(5, FREQUENCY_Y + 30);
    tft.setTextColor(RA8875_WHITE);
    tft.print("Center Freq");
    tft.fillRect(100, FREQUENCY_Y + 30, 300, tft.getFontHeight(), RA8875_BLACK);  // Clear volume field
    tft.setCursor(100, FREQUENCY_Y + 30);
    tft.setTextColor(RA8875_LIGHT_ORANGE);
    tft.print(centerFreq);
    tft.fillRect(OPERATION_STATS_X + 50, FREQUENCY_Y + 30, 300, tft.getFontHeight(), RA8875_BLACK); // Clear volume field
    tft.setTextColor(RA8875_LIGHT_ORANGE);
    tft.setCursor(OPERATION_STATS_X + 50, FREQUENCY_Y + 30);
    tft.print(bands[currentBand].name);                                     // Show band -- 40M

    tft.fillRect(OPERATION_STATS_X + 50 + 60, FREQUENCY_Y + 30, 60, tft.getFontHeight(), RA8875_BLACK);
    tft.setTextColor(RA8875_GREEN);
    tft.setCursor(OPERATION_STATS_X + 50 + 60, FREQUENCY_Y + 30);              // 277,212
    if (xmtMode == CW_MODE) {
      tft.print("CW");
    } else {
      tft.print("SSB");
    }
    tft.fillRect(OPERATION_STATS_X + 50 + 120, FREQUENCY_Y + 30, tft.getFontWidth() * 5, tft.getFontHeight(), RA8875_BLACK);        // Clear top-left menu area
    tft.setCursor(OPERATION_STATS_X + 50 + 120, FREQUENCY_Y + 30);
    tft.setTextColor(RA8875_WHITE);
    tft.print(DEMOD[bands[currentBand].mode].text);               // For example: LSB
  }

  /*****
    Purpose: Format frequency for printing
    Parameter list:
      void
    Return value;
      void
      // show frequency
  *****/
  void FormatFrequency(long freq, char *freqBuffer)
  {
    char outBuffer[15];
    int i;
    ltoa((long)freq, outBuffer, 10);

    if (freq < 10000000) {
      freqBuffer[0] = ' ';                      // Pad frequency display if less than 20M
      strcpy(&freqBuffer[1], outBuffer);
    } else {
      strcpy(freqBuffer, outBuffer);
    }

    strcpy(outBuffer, freqBuffer);
    freqBuffer[2] = '.';                      // Add separation charcter
    for (i = 3; i < 6; i++) {
      freqBuffer[i] = outBuffer[i - 1];       // Next 3 digit chars
    }
    freqBuffer[6] = ' ';                      // Add separation charcter
    for (i = 7; i < 10; i++) {
      freqBuffer[i] = outBuffer[i - 2];       // Last 3 digit chars
    }
    freqBuffer[i] = '\0';                     // Make it a string
  }

  /*****
    Purpose: show frequency

    Parameter list:
      void

    Return value;
      void
      // show frequency
  *****/
  void ShowFrequency()
  {
    char freqBuffer[15];

    if (activeVFO == VFO_A) {
      FormatFrequency(currentFreqA, freqBuffer);
      tft.setFont(&FreeMonoBold24pt7b);
      tft.setTextColor(RA8875_GREEN);
      tft.fillRect(FREQUENCY_X, FREQUENCY_Y - 12, VFOA_PIXEL_LENGTH, FREQUENCY_PIXEL_HI, RA8875_BLACK); // delete old freq
      tft.setCursor(FREQUENCY_X, FREQUENCY_Y);

      tft.print(freqBuffer);                                    // Show VFO_A

      tft.setFont(&FreeMonoBold18pt7b);
      tft.setTextColor(RA8875_LIGHT_GREY);
      tft.setCursor(FREQUENCY_X_SPLIT + 20, FREQUENCY_Y + 6);
      FormatFrequency(currentFreqB, freqBuffer);
      tft.print(freqBuffer);
    } else {                                                    // Show VFO_B
      FormatFrequency(currentFreqB, freqBuffer);
      tft.fillRect(FREQUENCY_X_SPLIT, FREQUENCY_Y - 12, VFOB_PIXEL_LENGTH, FREQUENCY_PIXEL_HI, RA8875_BLACK);
      tft.setCursor(FREQUENCY_X_SPLIT, FREQUENCY_Y);
      tft.setFont(&FreeMonoBold24pt7b);
      tft.setTextColor(RA8875_GREEN);
      tft.print(freqBuffer);                                    // Show VFO_A

      tft.setFont(&FreeMonoBold18pt7b);
      FormatFrequency(currentFreqA, freqBuffer);
      tft.setTextColor(RA8875_LIGHT_GREY);
      tft.setCursor(FREQUENCY_X, FREQUENCY_Y + 6);
      tft.print(freqBuffer);                                    // Show VFO_A
    }
    tft.setFontDefault();

  } // END VOID SHOW-FREQUENCY



  /*****
    Purpose: Display dBm
    Parameter list:
      void
    Return value;
      void
  *****/
  void DisplaydbM()
  {
    char buff[10];
    const char* unit_label;
    uint8_t display_something = 0;
    int16_t smeterPad;
    float32_t val_dbm = 0.0;
    float32_t dbuv;

    switch (display_dbm)
    {
      case DISPLAY_S_METER_DBM:
        display_something = 1;
        val_dbm = dbm;
        unit_label = "dBm";
        break;
      case DISPLAY_S_METER_DBMHZ:
        display_something = 0;
        val_dbm = dbmhz;
        unit_label = "dBm/Hz";
        break;
    }
    if ( abs(dbm - dbm_old) < 0.1) {
      display_something = 0;
    }
    if (display_something == 1)
    {
      tft.setTextColor(RA8875_WHITE);

      tft.setFontScale( (enum RA8875tsize) 0);

      tft.fillRect(SMETER_X + 185, SMETER_Y, 80, tft.getFontHeight(), RA8875_BLACK);  // The dB figure at end of S meter
      MyDrawFloat(val_dbm, 1, SMETER_X + 185, SMETER_Y, buff);
      tft.setTextColor(RA8875_GREEN);
      tft.print(unit_label);
      dbm_old = dbm;

      float32_t s = 9.0 + ((dbm + 73.0) / 6.0);
      if (s < 0.0)
        s = 0.0;
      if ( s > 9.0)
      {
        dbuv = dbm + 73.0;
        s = 9.0;
      } else {
        dbuv = 0.0;
      }

      smeterPad = map(smeterLength, 31, 0, 533, 580);
      tft.fillRect(SMETER_X + 1, SMETER_Y + 1, SMETER_BAR_LENGTH,    SMETER_BAR_HEIGHT, RA8875_BLACK);// Erase old bar
      tft.fillRect(SMETER_X + 1, SMETER_Y + 1, smeterPad - SMETER_X, SMETER_BAR_HEIGHT, RA8875_RED);

      if (dbuv > 30)
        dbuv = 30;
    }

    tft.setFontScale( (enum RA8875tsize) 1);

    tft.fillRect(BAND_INDICATOR_X + 180, BAND_INDICATOR_Y, 50, 30 , RA8875_BLACK);     //AFP

    if (agc_action == 1) {
      tft.setTextColor(RA8875_LIGHT_GREY);
    } else {
      tft.setTextColor(RA8875_RED);
    }
    tft.setCursor(BAND_INDICATOR_X + 180, BAND_INDICATOR_Y);
    tft.print("AGC");
  }

  /*****
    Purpose: Display the current temperature and load figures for T4.1

    Parameter list:
      int notchF        the notch to use
      int MODE          the current MODE

    Return value;
      void
  *****/
  void ShowTempAndLoad()
  {
    char buff[10];
    int valueColor = RA8875_GREEN;
    double block_time;
    double processor_load;
    elapsed_micros_mean = elapsed_micros_sum / elapsed_micros_idx_t;

    block_time = 128.0 / (double)SR[SampleRate].rate;           // one audio block is 128 samples and uses this in seconds
    block_time = block_time * N_BLOCKS;

    block_time *= 1000000.0;                                    // now in µseconds
    processor_load = elapsed_micros_mean / block_time * 100;    // take audio processing time divide by block_time, convert to %

    if (processor_load >= 100.0) {
      processor_load = 100.0;
      valueColor = RA8875_RED;
    }

    tft.setFontScale( (enum RA8875tsize) 0);

    CPU_temperature = TGetTemp();

    tft.fillRect(TEMP_X_OFFSET, TEMP_Y_OFFSET, MAX_WATERFALL_WIDTH, tft.getFontHeight(), RA8875_BLACK);    // Erase current data
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
  void MyDrawFloat(float val, int decimals, int x, int y, char *buff)
  {
    dtostrf(val, FLOAT_PRECISION, decimals, buff);  // Use 8 as that is the max prevision on a float

    tft.fillRect(x + 15, y, 12 * sizeof(buff), 15, RA8875_BLACK);
    tft.setCursor(x, y);

    tft.print(buff);
  }

  //===================================

  /*****
    Purpose:  Draw the EQ  display axies and tick marks

    Paramter list:
      void

    Return value:
      int
  *****/
  int PostProcessorAudio() {
    int columnIndex;
    int newValue;
    // int oldValue;
    int oldPosition;
    int newPosition;
    int xOrigin = 100;
    int yOrigin = 50;
    int wide    = 400;
    int high    = 300;
    int barWidth;
    //  int barHeight;
    int barTopY;
    int barBottomY;

    int eqFreq[] = {150, 240, 370, 590, 900, 1300, 2000, 3300};
    int yLevel[] = {100, 100, 100, 100, 100, 100, 100, 100};

    tft.fillWindow(RA8875_BLACK);

    tft.fillRect(xOrigin - 50, yOrigin - 25, wide + 50, high + 50, RA8875_BLACK); // Clear data area
    // tft.setTextSize(2);
    // tft.setFont();
    // tft.setTextColor(RA8875_WHITE);
    // tft.setCursor(0, 0);

    tft.drawRect(xOrigin, yOrigin, wide, high, RA8875_BLUE);
    tft.drawFastHLine(xOrigin + tft.getFontWidth(), yOrigin + (high / 2), wide - tft.getFontWidth() * 2, RA8875_RED);    // Clear hole in display center
    //drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    tft.setFontScale( (enum RA8875tsize) 0);

    //  tft.setCursor(xOrigin - tft.getFontWidth() * 2, yOrigin + tft.getFontHeight());
    tft.setTextColor(RA8875_WHITE);
    tft.setCursor(xOrigin - tft.getFontWidth() * 3, yOrigin + tft.getFontHeight());
    tft.print("+12");
    tft.setCursor(xOrigin - tft.getFontWidth() * 3, yOrigin + (high / 2) - tft.getFontHeight());
    tft.print(" 0");
    tft.setCursor(xOrigin - tft.getFontWidth() * 3, yOrigin + high - tft.getFontHeight() * 2);
    tft.print("-12");
    tft.drawFastHLine(xOrigin + tft.getFontWidth() * 2, yOrigin + high - tft.getFontHeight() * 2 - 10, wide - tft.getFontWidth() * 2, RA8875_WHITE);    // Clear hole in display center

    //  tft.drawFastVLine(xOrigin + 11, yOrigin - 230, 229, RA8875_WHITE); //Draw Y Axis
    //  tft.drawLine(xOrigin + 11, yOrigin, xOrigin + 240, yOrigin, RA8875_WHITE); //Draw X axis
    barWidth  = 35;
    //  barHeight = high / 2 - 10;
    barTopY   = yOrigin + (high / 2);
    barBottomY = barTopY + DEFAULT_EQUALIZER_BAR;       // Default is 100

    for (int iFreq = 0; iFreq < 8; iFreq++) {
      //    tft.drawRect(xOrigin  + barWidth * iFreq + tft.getFontWidth() * 2, yOrigin + (high / 2), barWidth, yLevel[iFreq], RA8875_BLUE);
      tft.fillRect(xOrigin  + (barWidth + 10) * iFreq + tft.getFontWidth() * 3, barTopY, barWidth, yLevel[iFreq], RA8875_CYAN);
      tft.setCursor(xOrigin + (barWidth + 10) * iFreq + tft.getFontWidth() * 3, yOrigin + high - tft.getFontHeight() * 2);
      //  tft.setFontScale( (enum RA8875tsize) 0);
      tft.print(eqFreq[iFreq]);
      tft.setCursor(xOrigin + (barWidth + 10) * iFreq + tft.getFontWidth() * 3, yOrigin + high + tft.getFontHeight() * 2);
      tft.print(yLevel[iFreq]);
    }
    columnIndex = 0;                                // Get ready to set values for columns
    //  oldValue = yLevel[columnIndex];
    oldPosition = 0;
    while (columnIndex < 8) {
      while (true) {
        newValue = yLevel[columnIndex];             // Get current value
        newPosition = fastTuneEncoder.read();              // Read menu encoder for new value
        if (newPosition != oldPosition) {
          tft.fillRect(xOrigin  + (barWidth + 10) * columnIndex + tft.getFontWidth() * 3,   // Indent to proper bar...
                       barBottomY - newValue - 1,                                            // Start at red line
                       barWidth,                                                             // Set bar width
                       newValue + 1,                                                         // Draw new bar
                       RA8875_BLACK);
          if (newPosition - oldPosition > 0) {
            newValue++;
          } else {
            newValue--;
          }

          tft.fillRect(xOrigin  + (barWidth + 10) * columnIndex + tft.getFontWidth() * 3,   // Indent to proper bar...
                       barBottomY - newValue,                        // Start at red line
                       barWidth,                                     // Set bar width
                       newValue,                                     // Draw new bar
                       RA8875_GREEN);
          yLevel[columnIndex] = newValue;
          tft.fillRect(xOrigin + (barWidth + 10) * columnIndex + tft.getFontWidth() * 3 - 1, yOrigin + high + tft.getFontHeight() * 2, tft.getFontWidth() * 5, CHAR_HEIGHT, RA8875_BLACK);
          tft.setCursor(xOrigin + (barWidth + 10) * columnIndex + tft.getFontWidth() * 3, yOrigin + high + tft.getFontHeight() * 2);
          tft.print(yLevel[columnIndex]);
          oldPosition = newPosition;
          if (newValue < DEFAULT_EQUALIZER_BAR) {                   // Repaint red center line if erased
            tft.drawFastHLine(xOrigin + tft.getFontWidth(), yOrigin + (high / 2), wide - tft.getFontWidth() * 2, RA8875_RED);    // Clear hole in display center
          }
        }
        selectExitMenues.update();                  // Exit submenu button
        if (selectExitMenues.fallingEdge()) {
          tft.fillRect(xOrigin  + (barWidth + 10) * columnIndex + tft.getFontWidth() * 3,   // Indent to proper bar...
                       barBottomY - newValue,                        // Start at red line
                       barWidth,                                     // Set bar width
                       newValue,                                     // Draw new bar
                       ORANGE);
          columnIndex++;
          break;
        }
      }
    }
    tft.fillWindow(RA8875_BLACK);
    RefreshMainDisplay(displayMode);
    ShowSpectrum();
    return columnIndex;                       // just a filler for function pointer
  }

  /*****
    Purpose: Shows the startup settings for the information displayed int he lower-right box.

    Parameter list:
      void

    Return value;
      void
  *****/
  void ShowDefaultSettings()
  {

    tft.setFontScale( (enum RA8875tsize) 1);
    UpdateVolumeField();
    UpdateAGCField();

    tft.setFontScale( (enum RA8875tsize) 0);
    UpdateIncrementField();
    UpdateNotchField();
    UpdateNoiseField();
    UpdateZoomField();
    UpdateCompressionField();
    UpdateWPMField();
    UpdateDecoderField();
    //  UpdateNRField();
  }

  /*****
    Purpose: Updates the Volume setting on the display

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateVolumeField()
  {

    tft.setFontScale( (enum RA8875tsize) 1);

    tft.setCursor(BAND_INDICATOR_X + 20, BAND_INDICATOR_Y);       // Volume
    tft.setTextColor(RA8875_WHITE);
    tft.print("Vol:");
    tft.setTextColor(RA8875_GREEN);
    tft.fillRect(BAND_INDICATOR_X + 90, BAND_INDICATOR_Y, tft.getFontWidth() * 4, tft.getFontHeight(), RA8875_BLACK);
    tft.setCursor(FIELD_OFFSET_X, BAND_INDICATOR_Y);
    tft.print(audio_volume);
  }
  /*****
    Purpose: Updates the AGC on the display

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateAGCField()
  {

    tft.setFontScale( (enum RA8875tsize) 1);

    if (agc_action == 1) {                                          // AGC box
      tft.setTextColor(RA8875_LIGHT_GREY);
    } else {
      tft.setTextColor(RA8875_RED);
    }
    tft.setCursor(BAND_INDICATOR_X + 180, BAND_INDICATOR_Y);
    tft.print("AGC");

  }
  /*****
    Purpose: Updates the increment setting on the display

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateIncrementField()
  {

    tft.setFontScale( (enum RA8875tsize) 0);

    tft.setTextColor(RA8875_WHITE);                                 // Frequency increment
    tft.setCursor(INCREMENT_X, INCREMENT_Y);
    tft.print("Increment: ");
    tft.fillRect(INCREMENT_X + 90, INCREMENT_Y, tft.getFontWidth() * 6, tft.getFontHeight(), RA8875_BLACK);
    tft.setCursor(FIELD_OFFSET_X, INCREMENT_Y);
    tft.setTextColor(RA8875_GREEN);
    tft.print(freqIncrement);
  }
  /*****
    Purpose: Updates the notch value on the display

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateNotchField()
  {
    tft.setFontScale( (enum RA8875tsize) 0);

    if (NR_first_time == 0) {                                        // Notch setting
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
    }
  }

  /*****
    Purpose: Updates the zoom setting on the display

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateZoomField()
  {

    tft.setFontScale( (enum RA8875tsize) 0);

    tft.fillRect(ZOOM_X, ZOOM_Y, 100, CHAR_HEIGHT, RA8875_BLACK);
    tft.setTextColor(RA8875_WHITE);                                 // Display zoom factor
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
  void UpdateCompressionField()
  {
    tft.fillRect(251, 0, 270, CHAR_HEIGHT, RA8875_BLACK);

    tft.setFontScale( (enum RA8875tsize) 0);
    tft.setTextColor(RA8875_WHITE);                                 // Display zoom factor
    tft.setCursor(COMPRESSION_X, COMPRESSION_Y);
    tft.print("Compress: ");
    tft.setCursor(FIELD_OFFSET_X, COMPRESSION_Y);
    tft.setTextColor(RA8875_GREEN);
    tft.print(micCompression);
  }
  /*****
    Purpose: Updates whether the decoder is on or off

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateDecoderField()
  {

    tft.setFontScale( (enum RA8875tsize) 0);

    tft.setTextColor(RA8875_WHITE);                                 // Display zoom factor
    tft.setCursor(DECODER_X, DECODER_Y);
    tft.print("Decoder:");
    tft.setTextColor(RA8875_GREEN);
    tft.fillRect(DECODER_X + 60, DECODER_Y, tft.getFontWidth() * 15, tft.getFontHeight(), RA8875_BLACK);
    tft.setCursor(FIELD_OFFSET_X, DECODER_Y);
    if (decoderFlag) {
      tft.print("On ");
    } else {
      tft.print("Off");
    }
  }


  /*****
    Purpose: Updates the WPM setting on the display

    Parameter list:
      void

    Return value;
      void
  *****/
  void UpdateWPMField()
  {
    tft.setFontScale( (enum RA8875tsize) 0);

    tft.setTextColor(RA8875_WHITE);                                 // Display zoom factor
    tft.setCursor(WPM_X, WPM_Y);
    tft.print("Keyer:");
    tft.setTextColor(RA8875_GREEN);
    tft.fillRect(WPM_X + 60, WPM_Y, tft.getFontWidth() * 15, tft.getFontHeight(), RA8875_BLACK);
    tft.setCursor(FIELD_OFFSET_X, WPM_Y);
    if (EEPROMData.keyType == KEYER) {
      tft.print(EEPROMData.wordsPerMinute);
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
  void UpdateNoiseField()
  {

    tft.setFontScale( (enum RA8875tsize) 0);

    tft.fillRect(FIELD_OFFSET_X, NOISE_REDUCE_Y, 100, CHAR_HEIGHT, RA8875_BLACK);
    tft.setTextColor(RA8875_WHITE);                                 // Noise reduction
    tft.setCursor(NOISE_REDUCE_X, NOISE_REDUCE_Y);
    tft.print("Noise:");
    tft.setTextColor(RA8875_GREEN);
    tft.setCursor(FIELD_OFFSET_X, NOISE_REDUCE_Y);
    switch (NR_Index) {
      case 0:                               // Off
        tft.print("Off");
        break;
      case 1:                               // Kim NR
        tft.print("Kim");
        break;
      case 2:                               // Spectral NR
        tft.print("Spectral");
        break;
      case 3:                               // LMS NR
        tft.print("LMS");
        break;
    }

  }

  /*****
    Purpose: Used to save a favortie frequency to EEPROM

    Parameter list:

    Return value;
      void
  *****/
  void SetFavoriteFrequencies()
  {
    int index;
    int offset;
    unsigned long currentFreqs[MAX_FAVORITES];

    EEPROMStuffFavorites(currentFreqs);                     // Read current values

    tft.setFontScale( (enum RA8875tsize) 0);

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

    fastTuneEncoder.write(startValue);
    index = 0;
    oldIndex = -1;
    tft.fillRect(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * index) + 5, 70, 15, RA8875_MAGENTA);
    tft.setTextColor(RA8875_WHITE);
    while (true) {
      currentValue = filterEncoder.read();

      MyDelay(ENCODER_DELAY);
      if (currentValue != lastValue) {
        oldIndex = index;
        if (currentValue > lastValue) {           // Adjust frequency index
          index++;
        } else {
          index--;
        }
        if (index < 0)                            // Check for over/underflow
          index = 0;
        if (index == MAX_FAVORITES)
          index = MAX_FAVORITES - 1;

        lastValue = currentValue;                 // Reset for another pass?

        tft.fillRect(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * oldIndex) + 5, 70, 15, RA8875_BLACK);
        tft.setCursor(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * oldIndex) + 5);
        tft.print(currentFreqs[oldIndex]);

        tft.fillRect(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * index) + 5, 70, 15, RA8875_MAGENTA);
        tft.setCursor(BAND_INDICATOR_X + 30, BAND_INDICATOR_Y + (offset * index) + 5);
        tft.print(currentFreq);
      }
      selectExitMenues.update();                            // Exit submenu button
      if (selectExitMenues.fallingEdge()) {
        EEPROMData.favoriteFreqs[index] = currentFreq;      // Update the EEPROM value
        EEPROMWrite();                                      // Save it
        break;
      }
    }
    tft.drawRect(BAND_INDICATOR_X - 10, BAND_INDICATOR_Y - 1, 260, 185, RA8875_LIGHT_GREY);

    tft.fillRect(BAND_INDICATOR_X - 9, BAND_INDICATOR_Y + 1, 180, 178, RA8875_BLACK);  // Clear volume field
    DrawAudioSpectContainer();
    ShowDefaultSettings();
  }

  /*****
    Purpose: Implement the notch filter

    Parameter list:
      int notchF        the notch to use
      int MODE          the current MODE

    Return value;
      void
  *****/
  void ShowNotch()
  {

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

  } // end void show_notch

  /*****
    Purpose: This function removes the spectrum display container

    Parameter list:
      void

    Return value;
      void
  *****/
  void EraseSpectrumDisplayContainer()
  {
    tft.fillRect(SPECTRUM_LEFT_X - 2, SPECTRUM_TOP_Y - 1, MAX_WATERFALL_WIDTH + 6, SPECTRUM_HEIGHT + 8,  RA8875_BLACK); // Spectrum box
  }


  /*****
    Purpose: This function draws the Info Window frame

    Parameter list:
      void

    Return value;
      void
  *****/
  void DrawInfoWindowFrame()
  {
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
  void RedrawDisplayScreen()
  {
    tft.fillWindow();
    DrawSpectrumDisplayContainer();
    DrawFrequencyBarValue();
    CenterFastTune();                                     // Put fast Tune line next to centerline for tuning
    DrawInfoWindowFrame();
    UpdateIncrementField();
    AGCPrep();
    EncoderVolume();
    ShowDefaultSettings();
    SetBand();
    ControlFilterF();
    BandInformation();
    FilterBandwidth();
    DrawSMeterContainer();
    DrawAudioSpectContainer();
    SetAttenuator(RF_attenuation);
    SetFreq();
    SetBandRelay(HIGH);
    SpectralNoiseReductionInit();
    SetI2SFreq(SR[SampleRate].rate);
  }
