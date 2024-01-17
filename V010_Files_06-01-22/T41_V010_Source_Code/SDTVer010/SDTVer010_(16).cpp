#ifndef BEENHERE
#include "SDT.h"
#endif
  // Useful in understanding menues:
  //
  //  const char *topMenus[]    = { "CW", "Display Choices", "Spectrum Set", "AGC", "NR Set",
  //                                "IQ Manual", "EQ Rec Set", "EQ Xmt Set", "Mic Comp", "Freq Cal",
  //                                "NB Set", "RF Set", "Audio Post Proc", "VFO Select", "EEPROM"      // Assume no EEPROM for now, "EEPROM" 12 elements  JJP 1-28-21
  //                              };
  /*
    int (*functionPtr[])() = {&CWOptions, &DisplayOptions,      &SpectrumOptions,     &AGCOptions, &NROptions,
                              &IQOptions, &EqualizerRecOptions, &EqualizerXmtOptions, &MicOptions, &FrequencyOptions,
                              &NBOptions, &RFOptions
                             };
  */
/*****
  Purpose: void ShowMenu()

  Parameter list:
    char *menuItem          pointers to the menu
    int where               0 is a primary menu, 1 is a secondary menu

  Return value;
    void
*****/
void ShowMenu(const char *menu[], int where)
{
  static int refreshFlag    = 0;

  tft.setFontScale( (enum RA8875tsize) 1);  

  if (where == PRIMARY_MENU) {                          // Should print on left edge of top line
    tft.fillRect(30, 0, 300, CHAR_HEIGHT, RA8875_BLUE);        // Top-left of display
    tft.setCursor(35, 0);
    tft.setTextColor(RA8875_WHITE);
    tft.print(*menu);                                      // Primary Menu
    refreshFlag = 1;
  } else {
    tft.fillRect(250, 0, 300, CHAR_HEIGHT, RA8875_GREEN);        // Right of primary display
    tft.setCursor(35, 0);
    tft.setTextColor(RA8875_WHITE);
    tft.print(*menu);                                      // Primary Menu
    refreshFlag = 1;
  }

  return;

  if (where == 0) {                      // Menu --
    mainMenuIndex--;
    if (mainMenuIndex < 0) {
      mainMenuIndex = TOP_MENU_COUNT - 1;
    }
    refreshFlag = 0;
  } else {                                  // menu ++
    mainMenuIndex++;
    if (mainMenuIndex == TOP_MENU_COUNT) {
      mainMenuIndex = 0;
    }
    refreshFlag = 0;
  }

  if (refreshFlag == 0) {
    tft.fillRect(0, 0, 300, CHAR_HEIGHT, RA8875_BLUE);        // Top-left of display
    tft.setCursor(20, 1);
    tft.setTextColor(RA8875_WHITE);
    tft.print(topMenus[mainMenuIndex]);                 // Primary Menu
    refreshFlag = 1;
  }
}
