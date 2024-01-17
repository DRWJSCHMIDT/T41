#ifndef BEENHERE
#include "SDT.h"
#endif

// Updates by KF5N to CalibrateOptions() function. July 20, 2023
// Updated receive calibration code to clean up graphics.  KF5N August 3, 2023

// ==============  AFP 10-22-22 ==================
/*****
  Purpose: Present the Calibrate options available and return the selection

  Parameter list:
    void

  Return value
   void
*****/
int CalibrateOptions(int IQChoice) {
  int val;
  int32_t increment = 100L;
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH + 30, CHAR_HEIGHT, RA8875_BLACK);
  //  float transmitPowerLevelTemp;  //AFP 05-11-23
  switch (IQChoice) {

    case 0:  // Calibrate Frequency  - uses WWV
      freqCorrectionFactor = GetEncoderValueLive(-200000, 200000, freqCorrectionFactor, increment, (char *)"Freq Cal: ");
      if (freqCorrectionFactor != freqCorrectionFactorOld) {
        si5351.init(SI5351_CRYSTAL_LOAD_10PF, Si_5351_crystal, freqCorrectionFactor);
        si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);  // KF5N July 10 2023
        si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);  // KF5N July 10 2023
        SetFreq();
        MyDelay(10L);
        freqCorrectionFactorOld = freqCorrectionFactor;
      }
      val = ReadSelectedPushButton();
      if (val != BOGUS_PIN_READ) {        // Any button press??
        val = ProcessButtonPress(val);    // Use ladder value to get menu choice
        if (val == MENU_OPTION_SELECT) {  // Yep. Make a choice??
          tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH + 35, CHAR_HEIGHT, RA8875_BLACK);
          EEPROMWrite();
          calibrateFlag = 0;
          IQChoice = 5;
          return IQChoice;
        }
      }
      break;

    case 1:  // CW PA Cal
      if (keyPressedOn == 1 && xmtMode == CW_MODE) {
        //================  CW Transmit Mode Straight Key ===========
        if (digitalRead(KEYER_DIT_INPUT_TIP) == LOW && xmtMode == CW_MODE && keyType == 0) {  //Straight Key
          powerOutCW[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * CWPowerCalibrationFactor[currentBand];
          CW_ExciterIQData();
          xrState = TRANSMIT_STATE;
          ShowTransmitReceiveStatus();
          SetFreq();                 //  AFP 10-02-22
          digitalWrite(MUTE, HIGH);  //   Mute Audio  (HIGH=Mute)
          modeSelectInR.gain(0, 0);
          modeSelectInL.gain(0, 0);
          modeSelectInExR.gain(0, 0);
          modeSelectOutL.gain(0, 0);
          modeSelectOutR.gain(0, 0);
          modeSelectOutExL.gain(0, 0);
          modeSelectOutExR.gain(0, 0);
        }
      }
      CWPowerCalibrationFactor[currentBand] = GetEncoderValueLive(-2.0, 2.0, CWPowerCalibrationFactor[currentBand], 0.001, (char *)"CW PA Cal: ");
      powerOutCW[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * CWPowerCalibrationFactor[currentBand];  // AFP 10-21-22
      val = ReadSelectedPushButton();
      if (val != BOGUS_PIN_READ) {        // Any button press??
        val = ProcessButtonPress(val);    // Use ladder value to get menu choice
        if (val == MENU_OPTION_SELECT) {  // Yep. Make a choice??
          tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH + 35, CHAR_HEIGHT, RA8875_BLACK);
          EEPROMData.CWPowerCalibrationFactor[currentBand] = CWPowerCalibrationFactor[currentBand];
          EEPROMWrite();
          calibrateFlag = 0;
          IQChoice = 5;
          return IQChoice;
        }
      }
      break;
    case 2:                  // IQ Receive Cal - Gain and Phase
      DoReceiveCalibrate();  // This function was significantly revised.  KF5N August 16, 2023
      IQChoice = 5;
      break;
    case 3:               // IQ Transmit Cal - Gain and Phase  //AFP 2-21-23
      DoXmitCalibrate();  // This function was significantly revised.  KF5N August 16, 2023
      IQChoice = 5;
      break;
    case 4:  // SSB PA Cal
      SSBPowerCalibrationFactor[currentBand] = GetEncoderValueLive(-2.0, 2.0, SSBPowerCalibrationFactor[currentBand], 0.001, (char *)"SSB PA Cal: ");
      powerOutSSB[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * SSBPowerCalibrationFactor[currentBand];  // AFP 10-21-22
      val = ReadSelectedPushButton();
      if (val != BOGUS_PIN_READ) {        // Any button press??
        val = ProcessButtonPress(val);    // Use ladder value to get menu choice
        if (val == MENU_OPTION_SELECT) {  // Yep. Make a choice??
          tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH + 35, CHAR_HEIGHT, RA8875_BLACK);
          EEPROMWrite();
          calibrateFlag = 0;
          IQChoice = 5;
          return IQChoice;
        }
      }
      break;  // Missing break.  KF5N August 12, 2023

    case 5:
      //EraseMenus();
      RedrawDisplayScreen();
      currentFreq = TxRxFreq = centerFreq + NCOFreq;
      DrawBandWidthIndicatorBar();  // AFP 10-20-22
      ShowFrequency();
      BandInformation();
      calibrateFlag = 0;
      // centerTuneFlag = 1;  Not used in revised tuning scheme.  July 22, 2023
      modeSelectOutExL.gain(0, 0);
      modeSelectOutExR.gain(0, 0);
      //   ShowSpectrum();  KF5N
      break;

    default:  // Cancelled choice
      micChoice = -1;
      break;
  }
  return 1;
}
// ==============  AFP 10-22-22 ==================
/*****
  Purpose: Present the CW options available and return the selection

  Parameter list:
    void

  Return value
    int           an index into the band array
*****/
int CWOptions()  // new option for Sidetone and Delay JJP 9/1/22
{
  const char *cwChoices[] = { "WPM", "Key Type", "CW Filter", "Paddle Flip", "Sidetone Volume", "Transmit Delay", "Cancel" };  // AFP 10-18-22
  int CWChoice = 0;

  CWChoice = SubmenuSelect(cwChoices, 7, 0);

  switch (CWChoice) {
    case 0:  // WPM
      SetWPM();
      SetTransmitDitLength(currentWPM);  //Afp 09-22-22     // JJP 8/19/23
      break;

    case 1:          // Type of key:
      SetKeyType();  // Straight key or keyer? Stored in EEPROMData.keyType; no heap/stack variable
      SetKeyPowerUp();
      UpdateWPMField();
      break;

    case 2:              // CW Filter BW:      // AFP 10-18-22
      SelectCWFilter();  // in CWProcessing    // AFP 10-18-22
      break;             // AFP 10-18-22

    case 3:            // Flip paddles
      DoPaddleFlip();  // Stored in EEPROM; variables paddleDit and paddleDah
      break;

    case 4:  // Sidetone volume
    //  SetSidetoneVolume();
      SetSideToneVolume();
      break;

    case 5:                // new function JJP 9/1/22
      SetTransmitDelay();  // Transmit relay hold delay
      break;

    default:  // Cancel
      break;
  }
  UpdateEEPROMSyncIndicator(0);
  return CWChoice;
}

/*****
  Purpose: Set the loudness of the sidetone.

  Parameter list:
    void

  Return value
    void
*****/
void SetSidetoneVolume() {
  const char *loudness[] = { "Whisper", "Low", "Medium", "Loud", "Cancel" };
  int retVal;
  const float32_t sidetoneParameter[] = {0.1, 0.25, 0.5,  1.0, 0.0};  // Louder sidetone. G0ORX.  Old values -> { 0.0005, 0.001, 0.002, 0.004, 0.0 };  //  AFP 10-01-22

  retVal = SubmenuSelect(loudness, 4, sidetoneVolume);
  if (retVal == 4)  // Did they make a choice?
    return;         // Nope.

  sidetoneVolume = sidetoneParameter[retVal];

  EEPROMData.sidetoneVolume = sidetoneVolume;
  EEPROMWrite();
  RedrawDisplayScreen();
  ShowSpectrumdBScale();
}
/*****
  Purpose: Show the list of scales for the spectrum divisions

  Parameter list:
    void

  Return value
    int           an index into displayScale[] array, or -1 on cancel
*****/
int SpectrumOptions() { /*
  dispSc displayScale[] =  //r *dbText,dBScale, pixelsPerDB, baseOffset, offsetIncrement
  {
    {"20 dB/", 10.0,   2,  24, 1.00},
    {"10 dB/", 20.0,   4,  10, 0.50},  // JJP 7/14/23
    {"5 dB/",  40.0,   8,  58, 0.25},
    {"2 dB/",  100.0, 20, 120, 0.10},
    {"1 dB/",  200.0, 40, 200, 0.05}
  };
  */
  const char *spectrumChoices[] = { "20 dB/unit", "10 dB/unit", "5 dB/unit", "2 dB/unit", "1 dB/unit", "Cancel" };
  int spectrumSet = EEPROMData.currentScale;  // JJP 7/14/23

  spectrumSet = SubmenuSelect(spectrumChoices, 6, spectrumSet);
  if (strcmp(spectrumChoices[spectrumSet], "Cancel") == 0) {
    return currentScale;  // Nope.
  }
  currentScale = spectrumSet;  // Yep...
  EEPROMData.currentScale = currentScale;
  EEPROMWrite();
  RedrawDisplayScreen();
  ShowSpectrumdBScale();
  return spectrumSet;
}
/*****
  Purpose: Present the bands available and return the selection

  Parameter list:
    void

  Return value
    int           an index into the band array
*****/
int AGCOptions() {
  const char *AGCChoices[] = { "Off", "Long", "Slow", "Medium", "Fast", "Cancel" }; // G0ORX (Added Long) September 5, 2023

  AGCMode = SubmenuSelect(AGCChoices, 6, AGCMode); // G0ORX
  if (AGCMode == 5) {
    return AGCMode;  // Nope.
  }

  AGCLoadValues(); // G0ORX September 5, 2023

  EEPROMData.AGCMode = AGCMode;                 // Store in EEPROM and...
  EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);  // ...save it
  UpdateAGCField();
  return AGCMode;
}
/*****
  Purpose: IQ Options

  Parameter list:
    void

  Return value
*****/
int IQOptions()  //============================== AFP 10-22-22  All new
{
  calibrateFlag = 1;
  const char *IQOptions[] = { "Freq Cal", "CW PA Cal", "Rec Cal", "Xmit Cal", "SSB PA Cal", "Cancel" };  //AFP 10-21-22
  //const char *IQOptions[] = {"Rec Cal", "Xmit Cal", "Freq Cal", "SSB PA Cal", "CW PA Cal", "Cancel"}; //AFP 10-21-22
  IQChoice = SubmenuSelect(IQOptions, 6, 0);  //AFP 10-21-22
  CalibrateOptions(IQChoice);
  return IQChoice;
}
/*****
  Purpose: To process the graphics for the 14 chan equalizar otpion

  Parameter list:
    int array[]         the peoper array to fill in
    char *title             the equalizer being set
  Return value
    void
*****/
void ProcessEqualizerChoices(int EQType, char *title) {
  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
  }
  const char *eqFreq[] = { " 200", " 250", " 315", " 400", " 500", " 630", " 800",
                           "1000", "1250", "1600", "2000", "2500", "3150", "4000" };
  int yLevel[EQUALIZER_CELL_COUNT];

  int columnIndex;
  int iFreq;
  int newValue;
  int xOrigin = 50;
  int xOffset;
  int yOrigin = 50;
  int wide = 700;
  int high = 300;
  int barWidth = 46;
  int barTopY;
  int barBottomY;
  int val;

  for (iFreq = 0; iFreq < EQUALIZER_CELL_COUNT; iFreq++) {
    if (EQType == 0) {
      yLevel[iFreq] = EEPROMData.equalizerRec[iFreq];
    } else {
      if (EQType == 1) {
        yLevel[iFreq] = EEPROMData.equalizerXmt[iFreq];
      }
    }
  }
  tft.writeTo(L2);
  tft.clearMemory();
  tft.writeTo(L1);
  tft.fillWindow(RA8875_BLACK);

  tft.fillRect(xOrigin - 50, yOrigin - 25, wide + 50, high + 50, RA8875_BLACK);  // Clear data area
  tft.setTextColor(RA8875_GREEN);
  tft.setFontScale((enum RA8875tsize)1);
  tft.setCursor(200, 0);
  tft.print(title);

  tft.drawRect(xOrigin - 4, yOrigin, wide + 4, high, RA8875_BLUE);
  tft.drawFastHLine(xOrigin - 4, yOrigin + (high / 2), wide + 4, RA8875_RED);  // Print center zero line center
  tft.setFontScale((enum RA8875tsize)0);

  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(xOrigin - 4 - tft.getFontWidth() * 3, yOrigin + tft.getFontHeight());
  tft.print("+12");
  tft.setCursor(xOrigin - 4 - tft.getFontWidth() * 3, yOrigin + (high / 2) - tft.getFontHeight());
  tft.print(" 0");
  tft.setCursor(xOrigin - 4 - tft.getFontWidth() * 3, yOrigin + high - tft.getFontHeight() * 2);
  tft.print("-12");

  barTopY = yOrigin + (high / 2);                // 50 + (300 / 2) = 200
  barBottomY = barTopY + DEFAULT_EQUALIZER_BAR;  // Default 200 + 100

  for (iFreq = 0; iFreq < EQUALIZER_CELL_COUNT; iFreq++) {
    tft.fillRect(xOrigin + (barWidth + 4) * iFreq, barTopY - (yLevel[iFreq] - DEFAULT_EQUALIZER_BAR), barWidth, yLevel[iFreq], RA8875_CYAN);
    tft.setCursor(xOrigin + (barWidth + 4) * iFreq, yOrigin + high - tft.getFontHeight() * 2);
    tft.print(eqFreq[iFreq]);
    tft.setCursor(xOrigin + (barWidth + 4) * iFreq + tft.getFontWidth() * 1.5, yOrigin + high + tft.getFontHeight() * 2);
    tft.print(yLevel[iFreq]);
  }

  columnIndex = 0;  // Get ready to set values for columns
  newValue = 0;
  while (columnIndex < EQUALIZER_CELL_COUNT) {
    xOffset = xOrigin + (barWidth + 4) * columnIndex;   // Just do the math once
    tft.fillRect(xOffset,                               // Indent to proper bar...
                 barBottomY - yLevel[columnIndex] - 1,  // Start at red line
                 barWidth,                              // Set bar width
                 newValue + 1,                          // Erase old bar
                 RA8875_BLACK);

    tft.fillRect(xOffset,                           // Indent to proper bar...
                 barBottomY - yLevel[columnIndex],  // Start at red line
                 barWidth,                          // Set bar width
                 yLevel[columnIndex],               // Draw new bar
                 RA8875_MAGENTA);
    while (true) {
      newValue = yLevel[columnIndex];  // Get current value
      if (filterEncoderMove != 0) {

        tft.fillRect(xOffset,                    // Indent to proper bar...
                     barBottomY - newValue - 1,  // Start at red line
                     barWidth,                   // Set bar width
                     newValue + 1,               // Erase old bar
                     RA8875_BLACK);
        newValue += (PIXELS_PER_EQUALIZER_DELTA * filterEncoderMove);  // Find new bar height. OK since filterEncoderMove equals 1 or -1
        tft.fillRect(xOffset,                                          // Indent to proper bar...
                     barBottomY - newValue,                            // Start at red line
                     barWidth,                                         // Set bar width
                     newValue,                                         // Draw new bar
                     RA8875_MAGENTA);
        yLevel[columnIndex] = newValue;

        tft.fillRect(xOffset + tft.getFontWidth() * 1.5 - 1, yOrigin + high + tft.getFontHeight() * 2,  // Update bottom number
                     barWidth, CHAR_HEIGHT, RA8875_BLACK);
        tft.setCursor(xOffset + tft.getFontWidth() * 1.5, yOrigin + high + tft.getFontHeight() * 2);
        tft.print(yLevel[columnIndex]);
        if (newValue < DEFAULT_EQUALIZER_BAR) {  // Repaint red center line if erased
          tft.drawFastHLine(xOrigin - 4, yOrigin + (high / 2), wide + 4, RA8875_RED);
          ;  // Clear hole in display center
        }
      }
      filterEncoderMove = 0;
      MyDelay(200L);

      val = ReadSelectedPushButton();  // Read the ladder value

      if (val != -1 && val < (EEPROMData.switchValues[0] + WIGGLE_ROOM)) {
        val = ProcessButtonPress(val);  // Use ladder value to get menu choice
        MyDelay(100L);

        tft.fillRect(xOffset,                // Indent to proper bar...
                     barBottomY - newValue,  // Start at red line
                     barWidth,               // Set bar width
                     newValue,               // Draw new bar
                     RA8875_GREEN);

        if (EQType == 0) {
          recEQ_Level[columnIndex] = newValue;
          EEPROMData.equalizerRec[columnIndex] = recEQ_Level[columnIndex];
        } else {
          if (EQType == 1) {
            xmtEQ_Level[columnIndex] = newValue;
            ;
            EEPROMData.equalizerXmt[columnIndex] = xmtEQ_Level[columnIndex];
          }
        }

        filterEncoderMove = 0;
        columnIndex++;
        break;
      }
      //recEQ_Level[columnIndex] = (float)array[columnIndex];  //AFP 08-09-22
      //EEPROMData.equalizerRec[columnIndex] = recEQ_Level[columnIndex];
    }
  }

  EEPROMWrite();
}

/*****
  Purpose: Receive EQ set

  Parameter list:
    void

  Return value
    int           an index into the band array
*****/
int EqualizerRecOptions() {
  const char *RecEQChoices[] = { "On", "Off", "EQSet", "Cancel" };  // Add code practice oscillator
  int EQChoice = 0;

  EQChoice = SubmenuSelect(RecEQChoices, 4, 0);

  switch (EQChoice) {
    case 0:
      receiveEQFlag = ON;
      break;
    case 1:
      receiveEQFlag = OFF;
      break;
    case 2:
      for (int iFreq = 0; iFreq < EQUALIZER_CELL_COUNT; iFreq++) {
      }
      ProcessEqualizerChoices(0, (char *)"Receive Equalizer");
      EEPROMWrite();
      RedrawDisplayScreen();
      break;
    case 3:
      break;
  }
  return 0;
}

/*****
  Purpose: Xmit EQ options

  Parameter list:
    void

  Return value
    int           an index into the band array
*****/
int EqualizerXmtOptions() {
  const char *XmtEQChoices[] = { "On", "Off", "EQSet", "Cancel" };  // Add code practice oscillator
  int EQChoice = 0;

  EQChoice = SubmenuSelect(XmtEQChoices, 4, 0);

  switch (EQChoice) {
    case 0:
      xmitEQFlag = ON;
      break;
    case 1:
      xmitEQFlag = OFF;
      break;
    case 2:
      ProcessEqualizerChoices(1, (char *)"Transmit Equalizer");
      EEPROMWrite();
      RedrawDisplayScreen();
      break;
    case 3:
      break;
  }
  return 0;
}


/*****
  Purpose: Set Mic level

  Parameter list:
    void

  Return value
    int           an index into the band array
*****/
int MicGainSet() {
  //=====
  const char *micGainChoices[] = { "Set Mic Gain", "Cancel" };
  micGainChoice = SubmenuSelect(micGainChoices, 2, micGainChoice);
  switch (micGainChoice) {
    case 0:
      int val;
      currentMicGain = EEPROMData.currentMicGain;  // AFP 09-22-22
      tft.setFontScale((enum RA8875tsize)1);
      tft.fillRect(SECONDARY_MENU_X - 50, MENUS_Y, EACH_MENU_WIDTH + 50, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setTextColor(RA8875_WHITE);
      tft.setCursor(SECONDARY_MENU_X - 48, MENUS_Y + 1);
      tft.print("Mic Gain:");
      tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
      tft.print(currentMicGain);
      while (true) {
        if (filterEncoderMove != 0) {
          currentMicGain += ((float)filterEncoderMove);
          if (currentMicGain < -40)
            currentMicGain = -40;
          else if (currentMicGain > 30)  // 100% max
            currentMicGain = 30;
          tft.fillRect(SECONDARY_MENU_X + 180, MENUS_Y, 80, CHAR_HEIGHT, RA8875_MAGENTA);
          tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
          tft.print(currentMicGain);
          filterEncoderMove = 0;
        }
        val = ReadSelectedPushButton();  // Read pin that controls all switches
        val = ProcessButtonPress(val);
        //MyDelay(150L);
        if (val == MENU_OPTION_SELECT) {  // Make a choice??
          EEPROMData.currentMicGain = currentMicGain;
          EEPROMWrite();
          break;
        }
      }
    case 1:
      break;
  }
  return micGainChoice;
  //  EraseMenus();
}
/*****
  Purpose: Turn mic compression on and set the level

  Parameter list:
    void

  Return value
    int           an index into the band array
*****/
int MicOptions()  // AFP 09-22-22 All new
{
  const char *micChoices[] = { "On", "Off", "Set Threshold", "Set Comp_Ratio", "Set Attack", "Set Decay", "Cancel" };

  micChoice = SubmenuSelect(micChoices, 7, micChoice);
  switch (micChoice) {
    case 0:                // On
      compressorFlag = 1;  // AFP 09-22-22
      UpdateCompressionField();     // JJP 8/26/2023
      break;
    case 1:  // Off
      compressorFlag = 0;
      UpdateCompressionField();     // JJP 8/26/2023
      break;
    case 2:
      SetCompressionLevel();
      break;
    case 3:
      SetCompressionRatio();
      break;
    case 4:
      SetCompressionAttack();
      break;
    case 5:
      SetCompressionRelease();
      break;
    case 6:
      break;
    default:  // Cancelled choice
      micChoice = -1;
      break;
  }
  secondaryMenuIndex = -1;
  return micChoice;
}

/*****
  Purpose: Present the bands available and return the selection

  Parameter list:
    void

  Return value12
    int           an index into the band array
*****/
int RFOptions() {
  const char *rfOptions[] = { "Power level", "Gain", "Cancel" };
  int rfSet = 0;
  int returnValue = 0;

  rfSet = SubmenuSelect(rfOptions, 3, rfSet);

  switch (rfSet) {
    case 0:  // AFP 10-21-22
      transmitPowerLevel = (float)GetEncoderValue(1, 20, transmitPowerLevel, 1, (char *)"Power: ");
      if (xmtMode == CW_MODE) {                                                                                                                                      //AFP 10-13-22
        powerOutCW[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * CWPowerCalibrationFactor[currentBand];  //  afp 10-21-22

        EEPROMData.powerOutCW[currentBand] = powerOutCW[currentBand];  //AFP 10-21-22
        //EEPROMWrite();//AFP 10-21-22
      } else {  //AFP 10-13-22
        if (xmtMode == SSB_MODE) {
          powerOutSSB[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * SSBPowerCalibrationFactor[currentBand];  // afp 10-21-22
          EEPROMData.powerOutSSB[currentBand] = powerOutSSB[currentBand];                                                                                                //AFP 10-21-22
        }
      }
      EEPROMData.powerLevel = transmitPowerLevel;  //AFP 10-21-22
      EEPROMWrite();                               //AFP 10-21-22

      BandInformation();
      break;

    case 1:                                                                                  // Gain
      rfGainAllBands = GetEncoderValue(-60, 10, rfGainAllBands, 5, (char *)"RF Gain dB: ");  // Argument: min, max, start, increment
      EEPROMData.rfGainAllBands = rfGainAllBands;
      EEPROMWrite();
      returnValue = rfGainAllBands;
      break;
  }
  return returnValue;
}


/*****
  Purpose: This option reverses the dit and dah paddles on the keyer

  Parameter list:
    void

  Return value
    void
*****/
void DoPaddleFlip() {
  const char *paddleState[] = { "Right paddle = dah", "Right paddle = dit" };
  int choice, lastChoice;
  int pushButtonSwitchIndex;
  int valPin;

  paddleDah = KEYER_DAH_INPUT_RING;  // Defaults
  paddleDit = KEYER_DIT_INPUT_TIP;
  choice = lastChoice = 0;

  tft.setTextColor(RA8875_BLACK);
  tft.fillRect(SECONDARY_MENU_X - 100, MENUS_Y, EACH_MENU_WIDTH + 100, CHAR_HEIGHT, RA8875_GREEN);
  tft.setCursor(SECONDARY_MENU_X - 93, MENUS_Y + 1);
  tft.print(paddleState[choice]);  // Show the default (right paddle = dah

  while (true) {
    MyDelay(150L);
    valPin = ReadSelectedPushButton();                     // Poll buttons
    if (valPin != -1) {                                    // button was pushed
      pushButtonSwitchIndex = ProcessButtonPress(valPin);  // Winner, winner...chicken dinner!
      if (pushButtonSwitchIndex == MAIN_MENU_UP || pushButtonSwitchIndex == MAIN_MENU_DN) {
        choice = !choice;  // Reverse the last choice
        tft.fillRect(SECONDARY_MENU_X - 100, MENUS_Y, EACH_MENU_WIDTH + 100, CHAR_HEIGHT, RA8875_GREEN);
        tft.setCursor(SECONDARY_MENU_X - 93, MENUS_Y + 1);
        tft.print(paddleState[choice]);
      }
      if (pushButtonSwitchIndex == MENU_OPTION_SELECT)
      {  // Made a choice??
        if (choice)
        {  // Means right-paddle dit
          paddleDit = KEYER_DAH_INPUT_RING;
          paddleDah = KEYER_DIT_INPUT_TIP;
          paddleFlip = 1; // KD0RC
        }
        else
        {
          paddleDit = KEYER_DIT_INPUT_TIP;
          paddleDah = KEYER_DAH_INPUT_RING;
          paddleFlip = 0;  // KD0RC
        }
        EEPROMData.paddleDit = paddleDit;
        EEPROMData.paddleDah = paddleDah;
        EraseMenus();
        UpdateWPMField(); // KD0RC
        return;
      }
    }
  }
}


/*****
  Purpose: Used to change the currently active VFO

  Parameter list:
    void

  Return value
    int             // the currently active VFO, A = 1, B = 0
*****/
int VFOSelect() {
  const char *VFOOptions[] = { "VFO A", "VFO B", "Split", "Cancel" };
  int toggle;
  int choice, lastChoice;

  choice = lastChoice = toggle = activeVFO;
  splitOn = 0;

  tft.setTextColor(RA8875_BLACK);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_GREEN);
  tft.setCursor(SECONDARY_MENU_X + 7, MENUS_Y + 1);
  tft.print(VFOOptions[choice]);  // Show the default (right paddle = dah

  choice = SubmenuSelect(VFOOptions, 4, 0);
  MyDelay(10);
  NCOFreq = 0L;
  switch (choice) {
    case VFO_A:  // VFO A
      centerFreq = TxRxFreq = currentFreqA;
      activeVFO = VFO_A;
      currentBand = currentBandA;
      tft.fillRect(FILTER_PARAMETERS_X + 180, FILTER_PARAMETERS_Y, 150, 20, RA8875_BLACK);  // Erase split message
      splitOn = 0;
      break;

    case VFO_B:  // VFO B
      centerFreq = TxRxFreq = currentFreqB;
      activeVFO = VFO_B;
      currentBand = currentBandB;
      tft.fillRect(FILTER_PARAMETERS_X + 180, FILTER_PARAMETERS_Y, 150, 20, RA8875_BLACK);  // Erase split message
      splitOn = 0;
      break;

    case VFO_SPLIT:  // Split
      DoSplitVFO();
      splitOn = 1;
      break;

    default:  // Cancel
      return activeVFO;
      break;
  }
  bands[currentBand].freq = TxRxFreq;
  SetBand();           // KF5N July 12, 2023
  SetBandRelay(HIGH);  // Required when switching VFOs. KF5N July 12, 2023
  SetFreq();
  RedrawDisplayScreen();
  BandInformation();
  ShowBandwidth();
  FilterBandwidth();
  EEPROMData.activeVFO = activeVFO;

  tft.fillRect(FREQUENCY_X_SPLIT, FREQUENCY_Y - 12, VFOB_PIXEL_LENGTH, FREQUENCY_PIXEL_HI, RA8875_BLACK);  // delete old digit
  tft.fillRect(FREQUENCY_X, FREQUENCY_Y - 12, VFOA_PIXEL_LENGTH, FREQUENCY_PIXEL_HI, RA8875_BLACK);        // delete old digit  tft.setFontScale( (enum RA8875tsize) 0);
  ShowFrequency();
  // Draw or not draw CW filter graphics to audio spectrum area.  KF5N July 30, 2023
  tft.writeTo(L2);
  tft.clearMemory();
  if (xmtMode == CW_MODE) BandInformation();
  DrawBandWidthIndicatorBar();
  DrawFrequencyBarValue();
  return activeVFO;
}

/*****
  Purpose: Allow user to set current EEPROM values or restore default settings

  Parameter list:
    void

  Return value
    int           the user's choice
*****/
int EEPROMOptions() {
  const char *EEPROMOpts[] = { "Save Current", "Set Defaults", "Get Favorite", "Set Favorite",
                               "Copy EEPROM-->SD", "Copy SD-->EEPROM", "SD EEPROM Dump", "Cancel" };
  int defaultOpt = 0;

  defaultOpt = SubmenuSelect(EEPROMOpts, 8, defaultOpt);
  switch (defaultOpt) {
    case 0:  // Save current values
      EEPROMWrite();
      UpdateEEPROMSyncIndicator(1);  //  JJP 7/25/23
      break;

    case 1:
      EEPROMSaveDefaults2();  // Restore defaults
      break;

    case 2:
      GetFavoriteFrequency();  // Get a stored frequency and store in active VFO
      break;

    case 3:
      SetFavoriteFrequency();  // Set favorites
      break;

    case 4:
      CopyEEPROMToSD();  // Save current EEPROM value to SD
      break;

    case 5:
      CopySDToEEPROM();  // Copy from SD to EEPROM
      EEPROMRead();      // KF5N
      tft.writeTo(L2);   // This is specifically to clear the bandwidth indicator bar.  KF5N August 7, 2023
      tft.clearMemory();
      tft.writeTo(L1);
      RedrawDisplayScreen();  // Assume there are lots of changes and do a heavy-duty refresh.  KF5N August 7, 2023
      break;

    case 6:
      SDEEPROMDump();  // Show SD data
      break;

    default:
      defaultOpt = -1;  // No choice made
      break;
  }
  return defaultOpt;
}


/*****
  Purpose: To select an option from a submenu

  Parameter list:
    char *options[]           submenus
    int numberOfChoices       choices available
    int defaultState          the starting option

  Return value
    int           an index into the band array
const char *topMenus[] = { "Bearing", "CW Options", "RF Set", "VFO Select",
                           "EEPROM", "AGC", "Spectrum Options",
                           "Noise Floor", "Mic Gain", "Mic Comp",
                           "EQ Rec Set", "EQ Xmt Set", "Calibrate" };

int (*functionPtr[])() = { &BearingMaps, &CWOptions, &RFOptions, &VFOSelect,
                           &EEPROMOptions, &AGCOptions, &SpectrumOptions,
                           &ButtonSetNoiseFloor, &MicGainSet, &MicOptions,
                           &EqualizerRecOptions, &EqualizerXmtOptions, &IQOptions

};
  const char *labels[]        = {"Select",       "Menu Up",  "Band Up",
                               "Zoom",         "Menu Dn",  "Band Dn",
                               "Filter",       "DeMod",    "Mode",
                               "NR",           "Notch",    "Noise Floor",
                               "Fine Tune",    "Decoder",  "Tune Increment",
                               "User 1",       "User 2",   "User 3"
                              };

*****/
int SubmenuSelect(const char *options[], int numberOfChoices, int defaultStart) {
  int refreshFlag = 0;
  int val;
  int encoderReturnValue;

  tft.setTextColor(RA8875_BLACK);
  encoderReturnValue = defaultStart;  // Start the options using this option

  tft.setFontScale((enum RA8875tsize)1);
  if (refreshFlag == 0) {
    tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_GREEN);  // Show the option in the second field
    tft.setCursor(SECONDARY_MENU_X + 1, MENUS_Y + 1);
    tft.print(options[encoderReturnValue]);  // Secondary Menu
    refreshFlag = 1;
  }
  MyDelay(150L);

  while (true) {
    val = ReadSelectedPushButton();  // Read the ladder value
    MyDelay(150L);
    if (val != -1 && val < (EEPROMData.switchValues[0] + WIGGLE_ROOM)) {
      val = ProcessButtonPress(val);  // Use ladder value to get menu choice
      if (val > -1) {                 // Valid choice?
        switch (val) {
          case MENU_OPTION_SELECT:  // They made a choice
            tft.setTextColor(RA8875_WHITE);
            EraseMenus();
            return encoderReturnValue;
            break;

          case MAIN_MENU_UP:
            encoderReturnValue++;
            if (encoderReturnValue >= numberOfChoices)
              encoderReturnValue = 0;
            break;

          case MAIN_MENU_DN:
            encoderReturnValue--;
            if (encoderReturnValue < 0)
              encoderReturnValue = numberOfChoices - 1;
            break;

          default:
            encoderReturnValue = -1;  // An error selection
            break;
        }
        if (encoderReturnValue != -1) {
          tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_GREEN);  // Show the option in the second field
          tft.setTextColor(RA8875_BLACK);
          tft.setCursor(SECONDARY_MENU_X + 1, MENUS_Y + 1);
          tft.print(options[encoderReturnValue]);
          MyDelay(50L);
          refreshFlag = 0;
        }
      }
    }
  }
}
