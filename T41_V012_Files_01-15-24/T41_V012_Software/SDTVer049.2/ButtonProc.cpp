#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: To process a menu increase button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonMenuIncrease() {
  if (menuStatus == PRIMARY_MENU_ACTIVE) {
    mainMenuIndex++;
    if (mainMenuIndex == TOP_MENU_COUNT) {  // At last menu option, so...
      mainMenuIndex = 0;                    // ...wrap around to first menu option
    }
  } else {
    if (menuStatus == SECONDARY_MENU_ACTIVE) {
      secondaryMenuIndex++;
      if (secondaryMenuIndex == subMenuMaxOptions) {  // Same here...
        secondaryMenuIndex = 0;
      }
    }
  }
}

/*****
  Purpose: To process a menu decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonMenuDecrease() {
  if (menuStatus == PRIMARY_MENU_ACTIVE) {
    mainMenuIndex--;
    if (mainMenuIndex < 0) {               // At last menu option, so...
      mainMenuIndex = TOP_MENU_COUNT - 1;  // ...wrap around to first menu option
    }
  } else {
    if (menuStatus == SECONDARY_MENU_ACTIVE) {
      secondaryMenuIndex--;
      if (secondaryMenuIndex < 0) {  // Same here...
        secondaryMenuIndex = subMenuMaxOptions - 1;
      }
    }
  }
}
//==================  AFP 09-27-22

/*****
  Purpose: To process a band increase button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonBandIncrease() {
  int tempIndex;
  tempIndex = currentBandA;
  if (currentBand == NUMBER_OF_BANDS) {  // Incremented too far?
    currentBand = 0;                     // Yep. Roll to list front.
  }
  NCOFreq = 0L;
  switch (activeVFO) {
    case VFO_A:
      tempIndex = currentBandA;
      if (save_last_frequency == 1) {
        lastFrequencies[tempIndex][VFO_A] = TxRxFreq;
      } else {
        if (save_last_frequency == 0) {
          if (directFreqFlag == 1) {
            lastFrequencies[tempIndex][VFO_A] = TxRxFreqOld;
          } else {
            if (directFreqFlag == 0) {
              lastFrequencies[tempIndex][VFO_A] = TxRxFreq;
            }
          }
          TxRxFreqOld = TxRxFreq;
        }
      }
      currentBandA++;
      if (currentBandA == NUMBER_OF_BANDS) {  // Incremented too far?
        currentBandA = 0;                     // Yep. Roll to list front.
      }
      currentBand = currentBandA;
      centerFreq = TxRxFreq = currentFreqA = lastFrequencies[currentBandA][VFO_A] + NCOFreq;
      break;

    case VFO_B:
      tempIndex = currentBandB;
      if (save_last_frequency == 1) {
        lastFrequencies[tempIndex][VFO_B] = TxRxFreq;
      } else {
        if (save_last_frequency == 0) {
          if (directFreqFlag == 1) {
            lastFrequencies[tempIndex][VFO_B] = TxRxFreqOld;
          } else {
            if (directFreqFlag == 0) {
              lastFrequencies[tempIndex][VFO_B] = TxRxFreq;
            }
          }
          TxRxFreqOld = TxRxFreq;
        }
      }
      currentBandB++;
      if (currentBandB == NUMBER_OF_BANDS) {  // Incremented too far?
        currentBandB = 0;                     // Yep. Roll to list front.
      }
      currentBand = currentBandB;
      centerFreq = TxRxFreq = currentFreqB = lastFrequencies[currentBandB][VFO_B] + NCOFreq;
      break;

    case VFO_SPLIT:
      DoSplitVFO();
      break;
  }
  directFreqFlag = 0;
  EraseSpectrumDisplayContainer();
  DrawSpectrumDisplayContainer();
  SetBand();
  SetFreq();
  ShowFrequency();
  ShowSpectrumdBScale();
  MyDelay(1L);
  AudioInterrupts();
  EEPROMWrite();
  // Draw or not draw CW filter graphics to audio spectrum area.  KF5N July 30, 2023
  tft.writeTo(L2);
  tft.clearMemory();
  tft.writeTo(L1);
  if(xmtMode == CW_MODE) BandInformation(); 
  DrawBandWidthIndicatorBar();
  DrawFrequencyBarValue();
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonBandDecrease() {
  int tempIndex = currentBand;
  ;
  //  NCOFreq = 0L;

  currentBand--;  // decrement band index

  if (currentBand < 0) {                // decremented too far?
    currentBand = NUMBER_OF_BANDS - 1;  // Yep. Roll to list end.
  }

  switch (activeVFO) {
    case VFO_A:
      if (save_last_frequency == 1) {
        lastFrequencies[tempIndex][VFO_A] = TxRxFreq;
      } else {
        if (save_last_frequency == 0) {
          if (directFreqFlag == 1) {
            lastFrequencies[tempIndex][VFO_A] = TxRxFreqOld;

          } else {
            if (directFreqFlag == 0) {
              lastFrequencies[tempIndex][VFO_A] = TxRxFreq;
            }
          }
          TxRxFreqOld = TxRxFreq;
        }
      }
      currentBandA--;
      if (currentBandA == NUMBER_OF_BANDS) {  // decremented too far?
        currentBandA = 0;                     // Yep. Roll to list front.
      }
      if (currentBandA < 0) {                // Incremented too far?
        currentBandA = NUMBER_OF_BANDS - 1;  // Yep. Roll to list front.
      }
      currentBand = currentBandA;
      centerFreq = TxRxFreq = currentFreqA = lastFrequencies[currentBandA][VFO_A] + NCOFreq;
      break;

    case VFO_B:
      if (save_last_frequency == 1) {
        lastFrequencies[tempIndex][VFO_B] = TxRxFreq;
      } else {
        if (save_last_frequency == 0) {
          if (directFreqFlag == 1) {
            lastFrequencies[tempIndex][VFO_B] = TxRxFreqOld;

          } else {
            if (directFreqFlag == 0) {
              lastFrequencies[tempIndex][VFO_B] = TxRxFreq;
            }
          }
          TxRxFreqOld = TxRxFreq;
        }
      }
      currentBandB--;
      if (currentBandB == NUMBER_OF_BANDS) {  // Incremented too far?
        currentBandB = 0;                     // Yep. Roll to list front.
      }
      if (currentBandB < 0) {                // Incremented too far?
        currentBandB = NUMBER_OF_BANDS - 1;  // Yep. Roll to list front.
      }
      currentBand = currentBandB;
      centerFreq = TxRxFreq = currentFreqB = lastFrequencies[currentBandB][VFO_B] + NCOFreq;
      break;

    case VFO_SPLIT:
      DoSplitVFO();
      break;
  }
  directFreqFlag = 0;
  EraseSpectrumDisplayContainer();
  DrawSpectrumDisplayContainer();
  SetBand();
  SetFreq();
  ShowFrequency();
  MyDelay(1L);
  ShowSpectrumdBScale();
  AudioInterrupts();
  EEPROMWrite();
  // Draw or not draw CW filter graphics to audio spectrum area.  KF5N July 30, 2023
  tft.writeTo(L2);
  tft.clearMemory();
  tft.writeTo(L1);
  if(xmtMode == CW_MODE) BandInformation(); 
  DrawBandWidthIndicatorBar();
  DrawFrequencyBarValue();
}


//================ AFP 09-27-22
/*****
  Purpose: Chnage the horizontal scale of the frequency display

  Parameter list:
    void

  Return value:
    int             index of the option selected
*****/
void ButtonZoom() {
  zoomIndex++;

  if (zoomIndex == MAX_ZOOM_ENTRIES) {
    zoomIndex = 0;
  }
  if (zoomIndex <= 0)
    spectrum_zoom = 0;
  else
    spectrum_zoom = zoomIndex;
  ZoomFFTPrep();
  UpdateZoomField();
  tft.writeTo(L2);  // Clear layer 2.  KF5N July 31, 2023
  tft.clearMemory();
  tft.writeTo(L1);  // Always exit function in L1.  KF5N August 15, 2023
  DrawBandWidthIndicatorBar();
  ShowSpectrumdBScale();
  DrawFrequencyBarValue();
  ShowFrequency();
  ShowBandwidth();
  ResetTuning();  // AFP 10-11-22
}

/*****
  Purpose: To process a filter button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonFilter() {
  switchFilterSideband = !switchFilterSideband;
  ControlFilterF();
  FilterBandwidth();
  //SetFreq();
  ShowFrequency();
}

/*****
  Purpose: Process demodulation mode

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonDemodMode() {
  bands[currentBand].mode++;
  if (bands[currentBand].mode > DEMOD_MAX) {
    bands[currentBand].mode = DEMOD_MIN;  // cycle thru demod modes
  }
  //AudioNoInterrupts();
  BandInformation();
  SetupMode(bands[currentBand].mode);
  ShowFrequency();
  ControlFilterF();
  tft.writeTo(L2);  // Destroy the bandwidth indicator bar.  KF5N July 30, 2023
  tft.clearMemory();
  if(xmtMode == CW_MODE) BandInformation(); 
  DrawBandWidthIndicatorBar();  // Restory the bandwidth indicator bar.  KF5N July 30, 2023
  FilterBandwidth();
  DrawSMeterContainer();
  ShowAnalogGain();
  AudioInterrupts();
  SetFreq();  // Must update frequency, for example moving from SSB to CW, the RX LO is shifted.  KF5N
}


/*****
  Purpose: Set transmission mode for SSB or CW

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonMode()  //====== Changed AFP 10-05-22  =================
{
  if (xmtMode == CW_MODE) {  // Toggle the current mode
    xmtMode = SSB_MODE;
  } else {
    xmtMode = CW_MODE;
  }
  //fLoCutOld = bands[currentBand].FLoCut;
  //fHiCutOld = bands[currentBand].FHiCut;
  SetFreq();  // Required due to RX LO shift from CW to SSB modes.  KF5N
  //tft.fillWindow();  // This was erasing the waterfall when switching modes.  Removed by KF5N.
  DrawSpectrumDisplayContainer();
  DrawFrequencyBarValue();
  DrawInfoWindowFrame();
  DisplayIncrementField();
  AGCPrep();
  UpdateAGCField();
  EncoderVolume();
  UpdateInfoWindow();
  ControlFilterF();
  BandInformation();
  FilterBandwidth();
  DrawSMeterContainer();
  DrawAudioSpectContainer();
  SpectralNoiseReductionInit();
  UpdateNoiseField();
  ShowName();
  ShowSpectrumdBScale();
  ShowTransmitReceiveStatus();
  ShowFrequency();
  // Draw or not draw CW filter graphics to audio spectrum area.  KF5N July 30, 2023
  if(xmtMode == SSB_MODE) {
  tft.writeTo(L2);
  tft.clearMemory();
  } else BandInformation();
  DrawBandWidthIndicatorBar();
}

/*****
  Purpose: To process select noise reduction

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonNR()  //AFP 09-19-22 update
{
  nrOptionSelect++;
  if (nrOptionSelect > 3) {
    nrOptionSelect = 0;
  }
  NROptions();  //AFP 09-19-22
  UpdateNoiseField();
}

/*****
  Purpose: To set the notch filter

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonNotchFilter() {
  ANR_notchOn = !ANR_notchOn;
  MyDelay(100L);
}


/*****
  Purpose: Allows quick setting of noise floor in spectrum display

  Parameter list:
    void

  Return value;
    int           the current noise floor value
*****/
int ButtonSetNoiseFloor() {
  int floor = currentNoiseFloor[currentBand];   // KF5N
  int val;

  tft.setFontScale((enum RA8875tsize)1);
  ErasePrimaryMenu();
  tft.fillRect(SECONDARY_MENU_X - 100, MENUS_Y, EACH_MENU_WIDTH + 120, CHAR_HEIGHT, RA8875_MAGENTA);
  //tft.setTextColor(RA8875_WHITE);
  tft.setTextColor(RA8875_BLACK);     // JJP 7/17/23
  tft.setCursor(SECONDARY_MENU_X - 98, MENUS_Y + 1);
  tft.print("Pixels above axis:");
  tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
  tft.print(currentNoiseFloor[currentBand]);
  MyDelay(150L);

  while (true) {
    if (filterEncoderMove != 0) {
      floor += filterEncoderMove;  // It moves the display
      /* JJP 7/17/23
      if (floor < -15) {           //AFP 09-22-22                        // Don't exceed limits
        floor = -15;               //AFP 09-22-22
      } else {
        if (floor > SPECTRUM_HEIGHT) {
          floor = SPECTRUM_HEIGHT;
        }
      }
      */
      EraseSpectrumWindow();
      floor = DrawNewFloor(floor);
      tft.fillRect(SECONDARY_MENU_X + 190, MENUS_Y, 80, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
      tft.print(floor);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();  // Get ADC value
    MyDelay(100L);
    val = ProcessButtonPress(val);
    if (val == MENU_OPTION_SELECT)  // If they made a choice...
    {
      /* JJP 7/17/23
      if (floor > SPECTRUM_BOTTOM + 15) {  //AFP 09-22-22
        floor = SPECTRUM_BOTTOM + 15;      //AFP 09-22-22
      } else {
        if (floor < SPECTRUM_BOTTOM - 50)
          floor = SPECTRUM_BOTTOM - 50;
      }
      */
      currentNoiseFloor[currentBand]             = floor;
      EEPROMData.spectrumNoiseFloor              = floor;
      EEPROMData.currentNoiseFloor[currentBand]  = floor;
      EEPROMWrite();
      break;
    }
  }
  EraseMenus();
  EraseSpectrumDisplayContainer();
  DrawSpectrumDisplayContainer();
  tft.setTextColor(RA8875_WHITE);
  DrawSpectrumDisplayContainer();
  ShowSpectrumdBScale();
  ShowSpectrum();
  tft.writeTo(L2);
  DrawFrequencyBarValue();
  tft.writeTo(L1);
  return spectrumNoiseFloor;
}

/*****
  Purpose: Draw in a red line at the new floor position

  Parameter list:
    int floor     the pixel position for the new floor

  Return value;
    int           the current noise floor value
*****/
int DrawNewFloor(int floor)
{
  static int oldY = SPECTRUM_BOTTOM;

  if (floor < 0) {
    floor = 0;
    oldY = SPECTRUM_BOTTOM - floor;
    return floor;
  }

  tft.drawFastHLine(SPECTRUM_LEFT_X + 30, oldY - floor, 100, RA8875_BLACK);
  tft.drawFastHLine(SPECTRUM_LEFT_X + 30, oldY - floor - 1, 100, RA8875_BLACK);
  tft.drawFastHLine(SPECTRUM_LEFT_X + 30, oldY - floor, 100, RA8875_RED);
  tft.drawFastHLine(SPECTRUM_LEFT_X + 30, oldY - floor - 1, 100, RA8875_RED);
  oldY = SPECTRUM_BOTTOM - floor;
  return floor;
}

/*****
  Purpose: The next 3 functions are "empty" user-defined function stubs that can be filled in by the user with
           "real" code.

  Parameter list:
    void

  Return value;
    int           the current noise floor value
*****/
int Unused1() {
  return -1;
}
int Unused2() {
  return -1;
}
int Unused3() {
  return -1;
}
/*****
  Purpose: Reset Zoom to zoomIndex

  Parameter list:
    void

  Return value;
    int           the current noise floor value
*****/
void ResetZoom(int zoomIndex1) {
  if (zoomIndex1 == MAX_ZOOM_ENTRIES) {
    zoomIndex1 = 0;
  }
  if (zoomIndex1 <= 0)
    spectrum_zoom = 0;
  else
    spectrum_zoom = zoomIndex1;

  ZoomFFTPrep();
  UpdateZoomField();
  DrawBandWidthIndicatorBar();
  //ShowSpectrumdBScale();
  DrawFrequencyBarValue();
  ShowFrequency();
  ShowBandwidth();
  //ResetTuning();
  RedrawDisplayScreen();
}

/*****
  Purpose: Direct Frequrncy Entry

  Parameter list:
    void

  Return value;
    void
    Base Code courtesy of Harry  GM3RVL
*****/
/*****
  Purpose: Direct Frequency Entry

  Parameter list:
    void

  Return value;
    void
    Base Code courtesy of Harry  GM3RVL
*****/
void ButtonFrequencyEntry() {
  TxRxFreqOld = TxRxFreq;

#define show_FEHelp
  bool doneFE = false;                         // set to true when a valid frequency is entered
  long enteredF = 0L;                          // desired frequency
  char strF[6] = { ' ', ' ', ' ', ' ', ' ' };  // container for frequency string during entry
  String stringF;
  int valPin;
  int key;
  int numdigits = 0;  // number of digits entered
  int pushButtonSwitchIndex;
  lastFrequencies[currentBand][activeVFO] = TxRxFreq;
  //save_last_frequency = false;                    // prevents crazy frequencies when you change bands/save_last_frequency = true;
  // Arrays for allocating values associated with keys and switches - choose whether USB keypad or analogue switch matrix
  // USB keypad and analogue switch matrix
  const char *DE_Band[] = {"80m","40m","20m","17m","15m","12m","10m"};
  const char *DE_Flimit[] = {"4.5","9","16","26","26","30","30"};
  int numKeys[] = { 0x0D, 0x7F, 0x58,  // values to be allocated to each key push
                    0x37, 0x38, 0x39,
                    0x34, 0x35, 0x36,
                    0x31, 0x32, 0x33,
                    0x30, 0x7F, 0x7F,
                    0x7F, 0x7F, 0x99 };
  EraseMenus();
#ifdef show_FEHelp
  int keyCol[] = { YELLOW, RED, RED,
                   RA8875_BLUE, RA8875_GREEN, RA8875_GREEN,
                   RA8875_BLUE, RA8875_BLUE, RA8875_BLUE,
                   RED, RED, RED,
                   RED, RA8875_BLACK, RA8875_BLACK,
                   YELLOW, YELLOW, RA8875_BLACK };
  int textCol[] = { RA8875_BLACK, RA8875_WHITE, RA8875_WHITE,
                    RA8875_WHITE, RA8875_BLACK, RA8875_BLACK,
                    RA8875_WHITE, RA8875_WHITE, RA8875_WHITE,
                    RA8875_WHITE, RA8875_WHITE, RA8875_WHITE,
                    RA8875_WHITE, RA8875_WHITE, RA8875_WHITE,
                    RA8875_BLACK, RA8875_BLACK, RA8875_WHITE };
  const char *key_labels[] = { "<", "", "X",
                               "7", "8", "9",
                               "4", "5", "6",
                               "1", "2", "3",
                               "0", "D", "",
                               "", "", "S" };

#define KEYPAD_LEFT 350
#define KEYPAD_TOP SPECTRUM_TOP_Y + 35
#define KEYPAD_WIDTH 150
#define KEYPAD_HEIGHT 300
#define BUTTONS_LEFT KEYPAD_LEFT + 30
#define BUTTONS_TOP KEYPAD_TOP + 30
#define BUTTONS_SPACE 45
#define BUTTONS_RADIUS 15
#define TEXT_OFFSET -8

  tft.writeTo(L1);
  tft.fillRect(WATERFALL_LEFT_X, SPECTRUM_TOP_Y + 1, MAX_WATERFALL_WIDTH, WATERFALL_BOTTOM - SPECTRUM_TOP_Y, RA8875_BLACK);  // Make space for FEInfo
  tft.fillRect(MAX_WATERFALL_WIDTH, WATERFALL_TOP_Y - 10, 15, 30, RA8875_BLACK);
  tft.writeTo(L2);

  tft.fillRect(WATERFALL_LEFT_X, SPECTRUM_TOP_Y + 1, MAX_WATERFALL_WIDTH, WATERFALL_BOTTOM - SPECTRUM_TOP_Y, RA8875_BLACK);

  tft.setCursor(centerLine - 140, WATERFALL_TOP_Y);
  tft.drawRect(SPECTRUM_LEFT_X - 1, SPECTRUM_TOP_Y, MAX_WATERFALL_WIDTH + 2, 360, RA8875_YELLOW);  // Spectrum box

  // Draw keypad box
  tft.fillRect(KEYPAD_LEFT, KEYPAD_TOP, KEYPAD_WIDTH, KEYPAD_HEIGHT, DARKGREY);
  // put some circles
  tft.setFontScale((enum RA8875tsize)1);
  for (unsigned i = 0; i < 6; i++) {
    for (unsigned j = 0; j < 3; j++) {
      tft.fillCircle(BUTTONS_LEFT + j * BUTTONS_SPACE, BUTTONS_TOP + i * BUTTONS_SPACE, BUTTONS_RADIUS, keyCol[j + 3 * i]);
      tft.setCursor(BUTTONS_LEFT + j * BUTTONS_SPACE + TEXT_OFFSET, BUTTONS_TOP + i * BUTTONS_SPACE - 18);
      tft.setTextColor(textCol[j + 3 * i]);
      tft.print(key_labels[j + 3 * i]);
    }
  }
  tft.setFontScale((enum RA8875tsize)0);
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 50);
  tft.setTextColor(RA8875_WHITE);
  tft.print("Direct Frequency Entry");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 100);
  tft.print("<   Apply entered frequency");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 130);
  tft.print("X   Exit without changing frequency");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 160);
  tft.print("D   Delete last digit");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 190);
  tft.print("S   Save Direct to Last Freq. ");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 240);
  tft.print("Direct Entry was called from "); 
  tft.print(DE_Band[currentBand]);
  tft.print(" band");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 270);
  tft.print("Frequency response limited above "); 
  tft.print(DE_Flimit[currentBand]);
  tft.print("MHz");
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 300);
  tft.print("For widest direct entry frequency range"); 
  tft.setCursor(WATERFALL_LEFT_X + 20, SPECTRUM_TOP_Y + 330);
  tft.print("call from 12m or 10m band"); 

#endif

  tft.writeTo(L2);

  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(10, 0);
  tft.print("Enter Frequency");

  tft.fillRect(SECONDARY_MENU_X + 20, MENUS_Y, EACH_MENU_WIDTH + 10, CHAR_HEIGHT, RA8875_MAGENTA);
  //tft.setTextColor(RA8875_WHITE);
  tft.setTextColor(RA8875_BLACK);       // JJP 7/17/23
  tft.setCursor(SECONDARY_MENU_X + 21, MENUS_Y + 1);
  tft.print("kHz or MHz:");
  tft.setFontScale((enum RA8875tsize)0);
  tft.setCursor(WATERFALL_LEFT_X + 50, SPECTRUM_TOP_Y + 260);
  tft.print("Save Direct to Last Freq.= ");
  tft.setCursor(WATERFALL_LEFT_X + 270, SPECTRUM_TOP_Y + 190);
  if (save_last_frequency == 0) {
    tft.setTextColor(RA8875_MAGENTA);
    tft.print("Off");
  } else {
    if (save_last_frequency == 1) {
      tft.setTextColor(RA8875_GREEN);
      tft.print("On");
    }
  }

  while (doneFE == false) {
    valPin = ReadSelectedPushButton();                     // Poll UI push buttons
    if (valPin != BOGUS_PIN_READ) {                        // If a button was pushed...
      pushButtonSwitchIndex = ProcessButtonPress(valPin);  // Winner, winner...chicken dinner!
      key = numKeys[pushButtonSwitchIndex];
      switch (key) {
        case 0x7F:  // erase last digit =127
          if (numdigits != 0) {
            numdigits--;
            strF[numdigits] = ' ';
          }
          break;
        case 0x58:  // Exit without updating frequency =88
          doneFE = true;
          break;
        case 0x0D:  // Apply the entered frequency (if valid) =13
          stringF = String(strF);
          enteredF = stringF.toInt();
          if ((numdigits == 1) || (numdigits == 2)) {
            enteredF = enteredF * 1000000;
          }
          if ((numdigits == 4) || (numdigits == 5)) {
            enteredF = enteredF * 1000;
          }
          if ((enteredF > 30000000) || (enteredF < 1250000)) {
            stringF = "     ";  // 5 spaces
            stringF.toCharArray(strF, stringF.length());
            numdigits = 0;
          } else {
            doneFE = true;
          }
          break;
        case 0x99:
          save_last_frequency = !save_last_frequency;
          tft.setFontScale((enum RA8875tsize)0);
          tft.fillRect(WATERFALL_LEFT_X + 269, SPECTRUM_TOP_Y + 190, 50, CHAR_HEIGHT, RA8875_BLACK);
          tft.setCursor(WATERFALL_LEFT_X + 260, SPECTRUM_TOP_Y + 190);
          if (save_last_frequency == 0) {
            tft.setTextColor(RA8875_MAGENTA);
            tft.print("Off");
            save_last_frequency = 0;
            break;
          } else {
            if (save_last_frequency == 1) {
              tft.setTextColor(RA8875_GREEN);
              tft.print("On");
              save_last_frequency = 1;
            }
          }
          break;
        default:
          if ((numdigits == 5) || ((key == 0x30) & (numdigits == 0))) {
          } else {
            strF[numdigits] = char(key);
            numdigits++;
          }
          break;
      }
      tft.setTextColor(RA8875_WHITE);
      tft.setFontScale((enum RA8875tsize)1);
      tft.fillRect(SECONDARY_MENU_X + 195, MENUS_Y + 1, 85, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
      tft.print(strF);
      MyDelay(250);  // only for analogue switch matrix
    }
  }
  if (key != 0x58) {

    TxRxFreq = enteredF;
  }
  NCOFreq = 0L;
  directFreqFlag = 1;
  centerFreq = TxRxFreq;
  centerTuneFlag = 1;  // Put back in so tuning bar is refreshed.  KF5N July 31, 2023
  SetFreq();  // Used here instead of centerTuneFlag.  KF5N July 22, 2023
  //}
  if (save_last_frequency == 1) {
    lastFrequencies[currentBand][activeVFO] = enteredF;
  } else {
    if (save_last_frequency == 0) {
      lastFrequencies[currentBand][activeVFO] = TxRxFreqOld;
    }
  }
  tft.fillRect(0, 0, 799, 479, RA8875_BLACK);   // Clear layer 2  JJP 7/23/23
  tft.writeTo(L1);
  EraseSpectrumDisplayContainer();
  DrawSpectrumDisplayContainer();
  DrawFrequencyBarValue();
  SetBand();
  SetFreq();
  ShowFrequency();
  MyDelay(1L);
  ShowSpectrumdBScale();
  AudioInterrupts();
  EEPROMWrite();
  // Draw or not draw CW filter graphics to audio spectrum area.  KF5N July 30, 2023
  tft.writeTo(L2);
  tft.clearMemory();
  if(xmtMode == CW_MODE) BandInformation(); 
  DrawBandWidthIndicatorBar();
  RedrawDisplayScreen(); // KD0RC
}
