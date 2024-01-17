
#ifndef BEENHERE
#include "SDT.h"
#endif


/*****
  Purpose: Function is designed to route program control to the proper execution point in response to
           a button press.

  Parameter list:
    int vsl               the value from analogRead in loop()

  Return value;
    void
*****/
void ExecuteButtonPress(int val)
{
  int h = SPECTRUM_HEIGHT + 3;    // Al changed but didn't annotate. Jack added 12/29/21
  int TxRxFreqTruncated;

  switch (val) {
    case MENU_OPTION_SELECT:                                     // 0
      if (secondaryMenuIndex == -1) {                           // Doing primary menu
        /* int (*functionPtr[])()                = {&CWOptions, &ButtonDisplayOptions, &SpectrumOptions, &AGCOptions,
                                                    &NROptions, &IQOptions, &EqualizerRecOptions, &EqualizerXmtOptions,
                                                    &MicOptions, &FrequencyOptions, &NBOptions, &RFOptions,
                                                    &PostProcessorAudio, &VFOSelect, &EEPROMOptions
                                                   };
          //  const char *topMenus[]    = { "CW", "Display Choices", "Spectrum Set", "AGC", "Noise Reduction",
          //                                "IQ Manual", "EQ Rec Set", "EQ Xmt Set", "Mic Comp", "Freq Cal",
          //                                "NB Set", "RF Set", "Audio Post Proc", "VFO Select", "EEPROM"      // Assume no EEPROM for now, "EEPROM" 12 elements  JJP 1-28-21
          //                              };
        */
        secondaryMenuChoiceMade = functionPtr[mainMenuIndex]();   // These are processed in MenuProcessing.cpp

        secondaryMenuIndex = -1;                                  // Reset secondary menu
      }
      tft.fillRect(0, 0, RIGNAME_X_OFFSET - 2, CHAR_HEIGHT + 1, RA8875_BLACK);         // Erase menu choices
      break;

    case MAIN_MENU_UP:                                            // 1
      ButtonMenuIncrease();
      if (secondaryMenuIndex == -1) {           // Doing primary menu
        ShowMenu(&topMenus[mainMenuIndex], PRIMARY_MENU);
      }
      break;

    case BAND_UP:
      filterWidth = FILTER_WIDTH;
      ShowSpectrum();                                           //Now calls ProcessIQData and Encoders calls
      digitalWrite(bandswitchPins[currentBand], LOW);
      ButtonBandIncrease();
      digitalWrite(bandswitchPins[currentBand], HIGH);
      BandInformation();
      break;

    case ZOOM:                                                    // 3
      ButtonZoom();
      break;

    case MAIN_MENU_DN:                                            // 4
      ButtonMenuDecrease();
      if (secondaryMenuIndex == -1) {           // Doing primary menu
        ShowMenu(&topMenus[mainMenuIndex], PRIMARY_MENU);
      }
      break;

    case BAND_DN:                                                 // 5
      filterWidth = FILTER_WIDTH;
      ShowSpectrum();                                           //Now calls ProcessIQData and Encoders calls
      digitalWrite(bandswitchPins[currentBand], LOW);
      ButtonBandDecrease();
      digitalWrite(bandswitchPins[currentBand], HIGH);
      BandInformation();
      break;

    case FILTER:                                                  // 6
      ButtonFilter();
      break;

    case DEMODULATION:                                            // 7
      ButtonDemodMode();
      break;

    case SET_MODE:                                                //  8
      ButtonMode();
      break;

    case NOISE_REDUCTION:                                         // 9
      ButtonNR();
      break;

    case NOTCH_FILTER:                                             // 10
      ButtonNotchFilter();
      UpdateNotchField();
      break;

    case DISPLAY_OPTIONS:                                         //  11
      ButtonDisplayOptions();
      break;

    case INCREMENT:                                               // 12
      ButtonFreqIncrement();
      break;

    case NOISE_FLOOR:                                             // 13
      ButtonSetNoiseFloor();
      break;

    case UNUSED_1:                                                // 14

      TxRxFreqTruncated = TxRxFreq / 10000;
      centerFreq = TxRxFreqTruncated * 10000;
      NCO_FREQ = 0;
      TxRxFreq = centerFreq + NCO_FREQ;
      newCursorPosition = 256;
      fastTuneEncoder.write(newCursorPosition);
      tft.drawFastVLine(oldCursorPosition, SPECTRUM_TOP_Y + 20, h - 30, RA8875_BLACK);  // Yep. Erase old, draw new...
      tft.drawFastVLine(newCursorPosition , SPECTRUM_TOP_Y + 20, h - 30, RA8875_RED); //
      oldCursorPosition = 256;
      SetFreq();
      ShowFrequency();
      break;

    case UNUSED_2:
      ResetHistograms();     // JJP 1/28/22
      break;

    default:
      break;
  }
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonDecrease()
{
  if (secondaryMenuIndex == -1) {                   // We're working on main menu
    mainMenuIndex--;
    if (mainMenuIndex < 0) {                        // At last menu option, so...
      mainMenuIndex = TOP_MENU_COUNT - 1;           // ...wrap around to first menu option
    }
  } else {
    secondaryMenuIndex--;
    if (secondaryMenuIndex < 0) {                   // Same here...
      secondaryMenuIndex = subMenuMaxOptions - 1;
    }
  }
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonIncrease()
{
  if (secondaryMenuIndex == -1) {                   // We're working on main menu
    mainMenuIndex++;
    if (mainMenuIndex == TOP_MENU_COUNT) {          // At last menu option, so...
      mainMenuIndex = 0;                            // ...wrap around to first menu option
    }
  } else {
    secondaryMenuIndex++;
    if (secondaryMenuIndex == subMenuMaxOptions) {  // Same here...
      secondaryMenuIndex = 0;
    }
  }
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void BandDecrease()
{
  currentBand--;
  if (currentBand < FIRST_BAND) {
    currentBand = LAST_BAND; // cycle thru radio bands
  }
  if (currentBand > LAST_BAND)  {
    currentBand = FIRST_BAND;
  }
  AudioNoInterrupts();
  freq_flag[1] = 0;
  SetBand();
  ControlFilterF();
  FilterBandwidth();
//  UpdateIncrementField();
  DrawSMeterContainer();
  SetFreq();
  AGCPrep();
  AudioInterrupts();
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void BandIncrease()
{
  AudioNoInterrupts();
  currentBand++;
  if (currentBand > LAST_BAND) {
    currentBand = FIRST_BAND; // cycle thru radio bands
  }
  freq_flag[1] = 0;
  SetBand();
  UpdateIncrementField();
  FilterBandwidth();
  DrawSMeterContainer();
  AGCPrep();
  SetFreq();
  MyDelay(1L);
  AudioInterrupts();
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonFreqIncrement()           
{
  long incrementValues[] = {10, 50, 100, 250, 1000, 10000};
  tuneIndex--;
  if (tuneIndex < 0)
    tuneIndex = 5;
  freqIncrement = incrementValues[tuneIndex];
  UpdateIncrementField();
}

/*****
  Purpose: Check for UI button press. If pressed, return the ADC value

  Parameter list:
    int vsl               the value from analogRead in loop()\

  Return value;
    int                   -1 if not valid push button, ADC value if valid
*****/
int ReadSelectedPushButton()
{
  minPinRead = 0;
  int buttonReadOld = 1024;
                                                  // do exponential averaging to smooth out the button response
  while (abs(minPinRead - buttonReadOld) > 3 ) {
    minPinRead = analogRead(BUSY_ANALOG_PIN);
    buttonRead = .1 * minPinRead + (1 - .1) * buttonReadOld; // See expected values in next function.
    buttonReadOld = buttonRead;
  }
  if (buttonRead > NOTHING_TO_SEE_HERE) {
    return -1;
  }
  minPinRead = buttonRead;
  delay(100);
  return minPinRead;
}

/*****
  Purpose: Determine which UI button was pressed

  Parameter list:
    int valPin            the ADC value from analogRead()

  Return value;
    int                   -1 if not valid push button, index of push button if valid
*****/
int ProcessButtonPress(int valPin)
{
  int switchIndex;

  if (valPin == -1) {                  // Not valid press
    return -1;
  }
  
  for (switchIndex = 0; switchIndex < TOP_MENU_COUNT; switchIndex++)
  {
    if (abs(valPin - EEPROMData.switchValues[switchIndex]) < WIGGLE_ROOM)    // ...because ADC does return exact values every time
    {
      MyDelay(100L);
      return switchIndex;
    }
  }
  return -1;                                              // Really should never do this
}
