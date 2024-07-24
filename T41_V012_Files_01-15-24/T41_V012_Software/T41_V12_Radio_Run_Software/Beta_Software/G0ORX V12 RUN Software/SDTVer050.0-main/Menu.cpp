#ifndef BEENHERE
#include "SDT.h"
#endif
/*
const char *topMenus[] = { "CW Options", "RF Set", "VFO Select",
                           "EEPROM", "AGC", "Spectrum Options",
                           "Noise Floor", "Mic Gain", "Mic Comp",
                           "EQ Rec Set", "EQ Xmt Set", "Calibrate", "Bearing", "Bode"};  //=================== AFP 03-30-24 V012 Bode Plot

int (*functionPtr[])() = { &CWOptions, &RFOptions, &VFOSelect,
                           &EEPROMOptions, &AGCOptions, &SpectrumOptions,
                           &ButtonSetNoiseFloor, &MicGainSet, &MicOptions,
                           &EqualizerRecOptions, &EqualizerXmtOptions, &IQOptions,&BearingMaps,&BodeOptions  //=================== AFP 03-30-24 V012 Bode Plot
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
void ShowMenu(const char *menu[], int where) {
  tft.setFontScale((enum RA8875tsize)1);

  if (menuStatus == NO_MENUS_ACTIVE)  // No menu selected??
    NoActiveMenu();

  if (where == PRIMARY_MENU) {                                             // Should print on left edge of top line
    tft.fillRect(PRIMARY_MENU_X, MENUS_Y, 300, CHAR_HEIGHT, RA8875_BLUE);  // Top-left of display
    tft.setCursor(5, 0);
    tft.setTextColor(RA8875_WHITE);
    tft.print(*menu);  // Primary Menu
  } else {
    tft.fillRect(SECONDARY_MENU_X, MENUS_Y, 300, CHAR_HEIGHT, RA8875_GREEN);  // Right of primary display
    tft.setCursor(35, 0);
    tft.setTextColor(RA8875_WHITE);
    tft.print(*menu);  // Secondary Menu
  }
  return;
}

/*****
  Purpose: To present the encoder-driven menu display

  Argument List;
    void

  Return value: index number for the selected menu
*****/
/*
const char *secondaryChoices[][15]= {
  { "WPM", "Key Type", "CW Filter", "Paddle Flip", "Sidetone Vol", "Xmit Delay", "Cancel" },
  { "Power level", "Gain", "Cancel" },
  { "VFO A", "VFO B", "Split", "Cancel" },
  { "Save Current", "Set Defaults", "Get Favorite", "Set Favorite", "EEPROM-->SD", "SD-->EEPROM", "SD Dump", "Cancel" },
  { "Off", "Long", "Slow", "Medium", "Fast", "Cancel" },
  { "20 dB/unit", "10 dB/unit", " 5 dB/unit", " 2 dB/unit", " 1 dB/unit", "Cancel" },
  { "Set floor", "Cancel" },
  { "Set Mic Gain", "Cancel" },
  { "On", "Off", "Set Threshold", "Set Ratio", "Set Attack", "Set Decay", "Cancel" },
  { "On", "Off", "EQSet", "Cancel" },
  { "On", "Off", "EQSet", "Cancel" },
  { "Freq Cal", "CW PA Cal", "Rec Cal", "Xmit Cal", "SSB PA Cal", "Cancel" },
  { "Set Prefix", "Cancel" }
};

const char *secondaryFunctions[][8] = {
  { "WPM", "Key Type", "CW Filter", "Paddle Flip", "Sidetone Vol", "Xmit Delay", "Cancel" },
  { "Power level", "Gain", "Cancel" },
  { "VFO A", "VFO B", "Split", "Cancel" },
  { "Save Current", "Set Defaults", "Get Favorite", "Set Favorite", "EEPROM-->SD", "SD-->EEPROM", "SD Dump", "Cancel" },
  { "Off", "Long", "Slow", "Medium", "Fast", "Cancel" },
  { "20 dB/unit", "10 dB/unit", " 5 dB/unit", " 2 dB/unit", " 1 dB/unit", "Cancel" },
  { "Set floor", "Cancel" },
  { "Set Mic Gain", "Cancel" },
  { "On", "Off", "Set Threshold", "Set Ratio", "Set Attack", "Set Decay", "Cancel" },
  { "On", "Off", "EQSet", "Cancel" },
  { "On", "Off", "EQSet", "Cancel" },
  { "Freq Cal", "CW PA Cal", "Rec Cal", "Xmit Cal", "SSB PA Cal", "Cancel" },
  { "Set Prefix", "Cancel" }
};
*/
int DrawMenuDisplay() 
{
  int i;
  menuStatus = 0;                                                       // No primary or secondary menu set
  mainMenuIndex = 0;
  secondaryMenuIndex = 0;

  tft.writeTo(L2);                                                      // Clear layer 2.  KF5N July 31, 2023
  tft.clearMemory();
  tft.writeTo(L1);
  tft.fillRect(1,  5, 520, 460, RA8875_BLACK);          // Show Menu box
  tft.drawRect(1, 15, 520, 460, RA8875_YELLOW);

  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_WHITE);
  for (i = 0; i < TOP_MENU_COUNT; i++) {                                // Show primary menu list
    tft.setCursor(10, i * 27 + 20);
    tft.print(topMenus[i]);
  }
  tft.setTextColor(RA8875_GREEN);                                       // show currently active menu
  tft.setCursor(10, mainMenuIndex * 27 + 20);
  tft.print(topMenus[mainMenuIndex]);
  i = 0;
  tft.setTextColor(DARKGREY, RA8875_BLACK);                   
  while (strcmp(secondaryChoices[mainMenuIndex][i], "Cancel") != 0) {   // Show secondary choices
    tft.setCursor(300, i * 27 + 20);
    tft.print(secondaryChoices[mainMenuIndex][i]);
    i++;
  }
  tft.setCursor(300, i * 27 + 20);
  tft.print(secondaryChoices[mainMenuIndex][i]);

  return 0;
}

/*****
  Purpose: To select the primary menu

  Argument List;
    void

  Return value: index number for the selected primary menu 
*****/
int SetPrimaryMenuIndex() 
{
  int i;
//  int resultIndex;
  int val;

  mainMenuWindowActive = true;
  while (true) {

    if (filterEncoderMove != 0) {             // Did they move the encoder?
      tft.setTextColor(RA8875_WHITE);         // Yep. Repaint the old choice
      tft.setCursor(10, mainMenuIndex * 27 + 21);
      tft.print(topMenus[mainMenuIndex]);
#if defined(G0ORX_FRONTPANEL)
      mainMenuIndex -= filterEncoderMove;     // Change the menu index to the new value
#else
      mainMenuIndex += filterEncoderMove;     // Change the menu index to the new value
#endif // G0ORX_FRONTPANEL
      if (mainMenuIndex == TOP_MENU_COUNT) {  // Did they go past the end of the primary menu list?
        mainMenuIndex = 0;                    // Yep. Set to start of the list.
      } else {
        if (mainMenuIndex < 0) {               // Did they go past the start of the list?
          mainMenuIndex = TOP_MENU_COUNT - 1;  // Yep. Set to end of the list.
        }
      }
      tft.setTextColor(RA8875_GREEN);
      tft.setCursor(10, mainMenuIndex * 27 + 21);
      tft.print(topMenus[mainMenuIndex]);
      tft.fillRect(299, 20, 210, 279, RA8875_BLACK);         // Erase secondary menu list
      tft.setTextColor(DARKGREY);
//      resultIndex = mainMenuIndex;
      i = 0;
      while (strcmp(secondaryChoices[mainMenuIndex][i], "Cancel") != 0) {   // Have we read the last entry in secondary menu?
        tft.setTextColor(DARKGREY);                                         // Nope.
        tft.setCursor(300, i * 27 + 20);
        tft.print(secondaryChoices[mainMenuIndex][i]);
        i++;
      }
      tft.setCursor(300, i * 27 + 20);
      tft.print(secondaryChoices[mainMenuIndex][i]);
      filterEncoderMove = 0;
    }
    val = ReadSelectedPushButton();  // Read the ladder value

    MyDelay(150L);
#if defined(G0ORX_FRONTPANEL)
    if(val!=-1) {
#else
    if (val != -1 && val < (EEPROMData.switchValues[0] + WIGGLE_ROOM)) {      // Did they press Select?
#endif // G0ORX_FRONTPANEL
      val = ProcessButtonPress(val);                                          // Use ladder value to get menu choice

      if (mainMenuWindowActive == true && val == MENU_BAILOUT_VALUE) {       // Cancel out of the main menu choices
        mainMenuWindowActive = false;
        mainMenuIndex = -1;
        return mainMenuIndex;
      }
      if (val > -1) {                                                         // Valid choice?

        if (val == MENU_OPTION_SELECT) {                                      // They made a choice
          tft.setTextColor(RA8875_WHITE);
          break;
        }
        MyDelay(50L);
      }
    }

  }  // End while True
  tft.setTextColor(RA8875_WHITE);
  return mainMenuIndex;
}


/*****
  Purpose: To select the secondary menu

  Argument List;
    void

  Return value: index number for the selected primary menu 
*****/
int SetSecondaryMenuIndex() 
{
  int i = 0;
  int secondaryMenuCount = 0;
  int oldIndex = 0;
  int val;

  while (true) {                                                        // How many secondary menu options?
    if (strcmp(secondaryChoices[mainMenuIndex][i], "Cancel") != 0) {    // Have we read the last entry in secondary menu?
      i++;                                                              // Nope.  
    } else {
      secondaryMenuCount = i + 1;                                       // Add 1 because index starts with 0
      break;
    }
  }
  MyDelay(100);
  secondaryMenuIndex = 0;                                   // Change the menu index to the new value
  filterEncoderMove  = 0;
  i = 0;

  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(300, 21);
  tft.print(secondaryChoices[mainMenuIndex][0]);

  i = 0;
  while (true) {

    if (filterEncoderMove != 0) {  // Did they move the encoder?
      tft.setTextColor(DARKGREY);  // Yep. Repaint the old choice
      tft.setCursor(300, oldIndex * 27 + 21);
      tft.print(secondaryChoices[mainMenuIndex][oldIndex]);
      i += filterEncoderMove;  // Change the menu index to the new value
     
      if (i == secondaryMenuCount) {  // Did they go past the end of the primary menu list?
        i = 0;                        // Yep. Set to start of the list.
      } else {
        if (i < 0) {                  // Did they go past the start of the list?
          i = secondaryMenuCount - 1; // Yep. Set to end of the list.
        }
      }
      oldIndex = i;
      tft.setTextColor(RA8875_GREEN);
      tft.setCursor(300, i * 27 + 21);
      tft.print(secondaryChoices[mainMenuIndex][i]);
      filterEncoderMove = 0;
    }
    val = ReadSelectedPushButton();  // Read the ladder value
    MyDelay(200L);
    if (val != -1 && val < (EEPROMData.switchValues[0] + WIGGLE_ROOM)) {
      val = ProcessButtonPress(val);  // Use ladder value to get menu choice

      if (val > -1) {                 // Valid choice?
        if (val == MENU_OPTION_SELECT) {  // They made a choice
          tft.setTextColor(RA8875_WHITE);
          secondaryMenuIndex = i;
          break;
        }
        MyDelay(50L);
      }
    }
  }  // End while True

  return secondaryMenuIndex;
}