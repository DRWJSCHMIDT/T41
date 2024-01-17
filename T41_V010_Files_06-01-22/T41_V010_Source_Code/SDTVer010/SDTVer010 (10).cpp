#ifndef BEENHERE
#include "SDT.h"
#endif


/*****
  Purpose: EncoderTune
  Parameter list:
    void
  Return value;
    void
*****/
void EncoderTune()
{
  long tuneChange = 1L;
  long old_freq;
  long newFreq;
  unsigned char result = tuneEncoder.process();   // Read the encoder
  int audio_volumeOld = audio_volume;

  if (result == 0)                                // Nothing read
    return;

  if (xmtMode == CW_MODE && decoderFlag == true)         // No reason to reset if we're not doing decoded CW
    ResetHistograms();

  old_freq = centerFreq;

  switch (result) {
    case DIR_CW:                                  // Turned it clockwise, 16
      tuneChange = 1L;
      audio_volume    = 0;
      MyDelay(10);                        //Mute audio during tuning change AFP 12-17-21
      break;

    case DIR_CCW:                                 // Turned it counter-clockwise
      tuneChange = -1L;
      audio_volume    = 0;
      MyDelay(10);
      break;
  }


  newFreq = (long)freqIncrement * tuneChange;
  centerFreq += newFreq;                // tune the master vfo and check boundaries
  if (centerFreq > F_MAX) {
    centerFreq = F_MAX;
  } else if (centerFreq < F_MIN) {
    centerFreq = F_MIN;
  }
  if (centerFreq != old_freq) {            // If the frequency has changed...
    Q_in_L.clear();
    Q_in_R.clear();
    //    ResetHistograms();
    TxRxFreq = centerFreq + NCO_FREQ;
    SetFreq();
    audio_volume    = audio_volumeOld;  //Restore audio volume AFP12-17-21
    ShowFrequency();
    return;
  }
}

/*****
  Purpose: Encoder volume control

  Parameter list:
    void

  Return value;
    void
*****/
void EncoderVolume()   //AFP 1-28-21
{
  static long volume_pos  = 0, last_volume_pos  = 0;
  volume_pos = volumeEncoder.read();
  long encoderVolumeChange;

  if (volume_pos != last_volume_pos || (volume_pos == 0 && last_volume_pos == 0)) {
    encoderVolumeChange = (volume_pos - last_volume_pos);
    last_volume_pos = volume_pos;
    audio_volume    = audio_volume + encoderVolumeChange * ENCODER_FACTOR;
    if (audio_volume < 0)
      audio_volume = 0;
    else if (audio_volume > 100)
      audio_volume = 100;

    tft.setFontScale( (enum RA8875tsize) 1);

    tft.setCursor(BAND_INDICATOR_X + 20, BAND_INDICATOR_Y);
    tft.setTextColor(RA8875_WHITE);
    tft.print("Vol:");
    tft.setTextColor(RA8875_GREEN);
    tft.fillRect(BAND_INDICATOR_X + 90, BAND_INDICATOR_Y, tft.getFontWidth() * 4, tft.getFontHeight(), RA8875_BLACK);
    tft.setCursor(BAND_INDICATOR_X + 90, BAND_INDICATOR_Y);
    tft.print(audio_volume);
  }
}


/*****
  Purpose: Use the encoder to change the value of a number

  Parameter list:
    int minValue                the lowest value allowed
    int maxValue                the largest value allowed
    int startValue              the numeric value to begin the count
    int increment               the amount by which each increment changes the value

  Return value;
    int                         the new value
*****/
int GetEncoderValue(int minValue, int maxValue, int startValue, int increment)
{
  static long currentValue  = startValue;
  static long lastValue     = 0L;
  int val;

  filterEncoder.write(startValue);

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.setTextColor(RA8875_BLACK);
  tft.fillRect(250, 0, 130, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setCursor(252, 1);
  tft.print(startValue);
  while (true) {
    currentValue = filterEncoder.read();
    MyDelay(150L);
    MyDelay(ENCODER_DELAY);
    if (currentValue != lastValue) {
      currentValue += increment;
      if (currentValue < minValue)
        currentValue = minValue;
      else if (currentValue > maxValue)
        currentValue = maxValue;
      lastValue = currentValue;

      //      tft.fillRect(135, 0, 130, 15, RA8875_GREEN);
      //      tft.setCursor(137, 1);
      tft.fillRect(250, 0, 130, CHAR_HEIGHT, RA8875_GREEN);
      tft.setCursor(252, 1);
      tft.print(currentValue);
    }

    val = ReadSelectedPushButton();                     // Read the ladder value
    MyDelay(100L);
    if (val != -1) {
      val = ProcessButtonPress(val);                    // Use ladder value to get menu choice
      if (val == MENU_OPTION_SELECT) {                  // Make a choice??
        EEPROMData.powerLevel = currentValue;           // Yep, save it to EEPROM structure
      }
    }
    return lastValue;
  }
}

/*****
  Purpose: Allows quick setting of WPM without going through a menu

  Parameter list:
    void

  Return value;
    int           the current WPM
*****/
int SetWPM()
{
  int val;
  static long lastWPM     = currentWPM;
  long encoderWPMChange;

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(251, 0, 250, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(252, 1);
  tft.print("current WPM:");
  tft.setCursor(450, 1);
  tft.print(currentWPM);

  while (true) {
    currentWPM = filterEncoder.read();
    if (currentWPM != lastWPM) {
      encoderWPMChange = (currentWPM - lastWPM);    // How much the number changed
      lastWPM += encoderWPMChange;               // Add change, + or -, to existing value
      if (lastWPM < 0)
        lastWPM = 5;
      else if (lastWPM > MAX_WPM)
        lastWPM = MAX_WPM;

      filterEncoder.write(lastWPM);

      //tft.fillRect(115, 0, 30, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.fillRect(450, 0, 50, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(450, 1);
      tft.print(lastWPM);
    }

    val = ReadSelectedPushButton();                                  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);
    if (val == MENU_OPTION_SELECT) {                             // Make a choice??
      tft.fillRect(251, 0, 250, CHAR_HEIGHT, RA8875_BLACK);
      currentWPM = lastWPM;
      EEPROMData.wordsPerMinute = lastWPM;
      UpdateWPMField();
      break;
    }
  }
  tft.fillRect(251, 0, 250, CHAR_HEIGHT, RA8875_BLACK);

  filterEncoder.write(currentWPM);
  return currentWPM;
}


/*****
  Purpose: EncoderTune

  Parameter list:
    void

  Return value;
    void
    Modified AFP21-12-15
*****/
void EncoderFilterAndWPM() {  //AFP 1-28-21
  static long filter_pos  = 0, last_filter_pos  = 0;
  long filter_change;


  //switchFilterSideband==1;  // uncomment to Test opposite sideband filter selection
  ////////////////////////////////////////////////  Encoder 2 Filter
  filter_pos = filterEncoder.read();

  if (filter_pos == currentWPM && xmtMode == CW_MODE)
    return;

  if (xmtMode == CW_MODE) {                             // CW
    SetWPM();
    last_filter_pos = currentWPM;
  } else {                                              // SSB
    if (filter_pos != last_filter_pos) {
      tft.fillRect((MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2 - filterWidth, SPECTRUM_TOP_Y + 17, filterWidth, SPECTRUM_HEIGHT - 20, RA8875_BLACK); // Erase old filter background
      filter_change = (filter_pos - last_filter_pos);
      if (filter_change >= 1) {
        filterWidth--;
        if (filterWidth < 10)
          filterWidth = 10;
      }
      if (filter_change <= -1) {
        filterWidth++;
        if (filterWidth > 100)
          filterWidth = 50;
      }

      last_filter_pos = filter_pos;
      if (bands[currentBand].mode == DEMOD_LSB) {
        // "0" = normal, "1" means change opposite filter
        if (switchFilterSideband == 0)      // "0" = normal, "1" means change opposite filter
        {
          bands[currentBand].FLoCut = bands[currentBand].FLoCut + filter_change * 50 * ENCODER_FACTOR;
          FilterBandwidth();
        } else if (switchFilterSideband == 1) {
          //if (abs(bands[currentBand].FHiCut) < 500) {
          bands[currentBand].FHiCut = bands[currentBand].FHiCut + filter_change * 50 * ENCODER_FACTOR;
          // } else {
          //   bands[currentBand].FHiCut = bands[currentBand].FHiCut + filter_change * 100 * ENCODER_FACTOR;
          // }
        }
      } else if (bands[currentBand].mode == DEMOD_USB) {
        if (switchFilterSideband == 0) {
          bands[currentBand].FHiCut = bands[currentBand].FHiCut - filter_change * 50 * ENCODER_FACTOR;
          FilterBandwidth();
        } else if (switchFilterSideband == 1) {
          bands[currentBand].FLoCut = bands[currentBand].FLoCut - filter_change * 50 * ENCODER_FACTOR;
        }
      }
      ControlFilterF();
      Menu2 = MENU_F_LO_CUT;      // set Menu2 to MENU_F_LO_CUT
      FilterBandwidth();
      SetFreq();
      //      ShowFrequency(bands[currentBand].freq, 1);
      ShowFrequency();
    }
    notchPosOld = filter_pos;
  }
} // end encoder2 was turned
