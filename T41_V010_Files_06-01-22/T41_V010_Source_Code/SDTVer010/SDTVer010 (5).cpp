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
void ButtonMenuIncrease()
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
  Purpose: To process a menu decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonMenuDecrease()
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
  Purpose: Chnage the horizontal scale of the frequency display

  Parameter list:
    void

  Return value:
    int             index of the option selected
*****/
void ButtonZoom()
{
  zoomIndex++;

  if (zoomIndex == MAX_ZOOM_ENTRIES) {
    zoomIndex = 0;
  }
  if (zoomIndex == 0)
    spectrum_zoom = 0;
  else
    spectrum_zoom = zoomIndex;

  ZoomFFTPrep();
  UpdateZoomField();
  DrawFrequencyBarValue();
//  BandInformation();
  ShowBandwidth();
}

/*****
  Purpose: To process a filter button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonFilter()
{
  switchFilterSideband = !switchFilterSideband;
  ControlFilterF();
  FilterBandwidth();
  SetFreq();
  ShowFrequency();
}

/*****
  Purpose: Process demodulation mode

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonDemodMode()
{
  bands[currentBand].mode++;
  if (bands[currentBand].mode > DEMOD_MAX)
    bands[currentBand].mode = DEMOD_MIN;            // cycle thru demod modes

  AudioNoInterrupts();
  SetupMode(bands[currentBand].mode);
  ShowFrequency();
  ControlFilterF();
  FilterBandwidth();
  DrawSMeterContainer();
  ShowAnalogGain();
  AudioInterrupts();
}


/*****
  Purpose: Set transmission mode for SSB or CW

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonMode()
{
  if (xmtMode == CW_MODE) {
    xmtMode = SSB_MODE;
  }
  else if (xmtMode == SSB_MODE) {
    xmtMode = CW_MODE;
  }

  BandInformation();

  if (xmtMode == CW_MODE) {
    modeSelectOutL.gain(0, 5);
    modeSelectOutR.gain(0, 5);
    FLoCutOld = bands[currentBand].FLoCut;
    FHiCutOld = bands[currentBand].FHiCut;
    if (currentBand < 2) {
      bands[currentBand].FLoCut = -3000;
      bands[currentBand].FHiCut = -200;
    }
    else if (currentBand >= 2) {
      bands[currentBand].FLoCut = 200;
      bands[currentBand].FHiCut = 3000;
    }
  }
  if (xmtMode == SSB_MODE) {
    modeSelectOutL.gain(0, 1.0);
    modeSelectOutR.gain(0, 1.0);
    if (currentBand < 2) {
      bands[currentBand].FLoCut =  FLoCutOld;
      bands[currentBand].FHiCut = FHiCutOld;
    }
    else if (currentBand >= 2) {
      bands[currentBand].FLoCut = FLoCutOld;
      bands[currentBand].FHiCut = FHiCutOld;
    }
  }
  ControlFilterF();
  FilterBandwidth();
  SetFreq();
  ShowFrequency();
}

/*****
  Purpose: To process select noise reduction

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonNR()  //AFP 2-10-21
{
  NR_Index++;
  if (NR_Index > 3) {
    NR_Index = 0;
  }
  UpdateNoiseField();
}

/*****
  Purpose: To set the notch filter

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonNotchFilter()
{
  ANR_notchOn = !ANR_notchOn;
  MyDelay(100L);
}

/*****
  Purpose: How to present the display

  Parameter list:
    void

  Return value:
    int                       the display option
*****/
int ButtonDisplayOptions() 
{
  const char *displayChoices[] = {"Spectrum", "Waterfall", "Both", "None", "Cancel"};
  int currentDisplayMode = displayMode;

  currentDisplayMode = SubmenuSelect(displayChoices, 5, displayMode);
  switch (currentDisplayMode) {
    case SPECTRUM_ONLY:                               // Spectrum only
      displayMode = SPECTRUM_ONLY;
      RefreshMainDisplay(displayMode);
      break;

    case WATERFALL_ONLY:                              // Waterfall only
      displayMode = WATERFALL_ONLY;
      RefreshMainDisplay(displayMode);
      break;

    case BOTH_DISPLAYS:                               // Both displays active
      displayMode = BOTH_DISPLAYS;
      RefreshMainDisplay(displayMode);
      break;

    case NO_DISPLAY:                                  // Neither spectrum or waterfall active
      displayMode = NO_DISPLAY;
      EraseSpectrumDisplayContainer();
      break;

    case 4:                                           // Set noise floor
      ButtonSetNoiseFloor();
      break;

    case 5:                                           // Cancel
    default:
      displayMode = -1;
      break;
  }
  RefreshMainDisplay(displayMode);
  return displayMode;
}

/*****
  Purpose: Allows quick setting of noise floor in spectrum display

  Parameter list:
    void

  Return value;
    int           the current noise floor value
*****/
int ButtonSetNoiseFloor()
{
  int currentNoiseFloor = spectrumNoiseFloor;
  int lastNoiseFloor;
  int val;

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(251, 0, 300, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(252, 1);
  tft.print("Current floor:");
  tft.setCursor(480, 1);
  lastNoiseFloor = filterEncoder.read();
  tft.print(lastNoiseFloor);

  while (true) {
    currentNoiseFloor = filterEncoder.read();
    if (currentNoiseFloor != lastNoiseFloor) {
      if (currentNoiseFloor - lastNoiseFloor > 0)     // How much the number changed
        spectrumNoiseFloor--;                         // It moves up the display, or lower values for Y
      else if (currentNoiseFloor - lastNoiseFloor < 0)
        spectrumNoiseFloor++;                         // It moves down the display, or higher values for Y
      if (currentNoiseFloor < SPECTRUM_BOTTOM)
        spectrumNoiseFloor = SPECTRUM_BOTTOM;
      else if (currentNoiseFloor > SPECTRUM_TOP_Y)
        spectrumNoiseFloor = SPECTRUM_TOP_Y;
      lastNoiseFloor = currentNoiseFloor;
      tft.fillRect(470, 0, 100, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(480, 1);
      tft.print(currentNoiseFloor);
    }

    val = ReadSelectedPushButton();              // Get ADC value
    val = ProcessButtonPress(val);
    if (val == MENU_OPTION_SELECT)               // If they made a choice...
    {
      spectrumNoiseFloor -= currentNoiseFloor;
      if (spectrumNoiseFloor < SPECTRUM_TOP_Y) {
        spectrumNoiseFloor = SPECTRUM_TOP_Y;
      } else {
        if (currentNoiseFloor > SPECTRUM_BOTTOM) {
          spectrumNoiseFloor = SPECTRUM_BOTTOM;
        }
      }
      EEPROMData.spectrumNoiseFloor = spectrumNoiseFloor;
      EEPROMWrite();
      break;
    }
  }
  tft.fillRect(0, 0, 590, CHAR_HEIGHT + 4, RA8875_BLACK);
  tft.fillRect(SPECTRUM_LEFT_X - 1, SPECTRUM_TOP_Y, MAX_WATERFALL_WIDTH + 2, SPECTRUM_HEIGHT - 1,  RA8875_BLACK);
  DrawSpectrumDisplayContainer();
  ShowSpectrum();
  CenterFastTune();
  return spectrumNoiseFloor;
}
