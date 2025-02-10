#ifndef BEENHERE
#include "test.h"
#endif

/*****
  Purpose: Set up the display for the encoder tests

  Parameter list:
    void

  Return value;
    void
*****/
void DoEncodersTest() 
{
  int value = 1010;
  DrawEncoderTestScreen();

  while (value > 1000) {
    value = analogRead(BUSY_ANALOG_PIN);
    delay(500L);                          // Let things settle down
  }  
  tft.fillScreen(RA8875_BLACK);
  tft.setFontScale(2); 
  tft.setCursor(360, 240);
  tft.setTextColor(RA8875_RED, RA8875_BLACK);
  tft.print("DONE");
}

/*****
  Purpose: ISR for manu/filter encoder
  Parameter list:
    void

  Return value;
    void
*****/
void EncoderFilter()
{
  volatile char result;

  result = filterEncoder.process();  // Read the encoder
  if (result == 0) {  // Nothing read
    return;
  }
  switch (result) {
    case DIR_CW:  // Turned it clockwise, 16
      adjustEncoder = 1;
      break;

    case DIR_CCW:  // Turned it counter-clockwise
      adjustEncoder = -1;
      break;
  }
  menuSetting += adjustEncoder;

  if (menuSetting < 0)
    menuSetting = 0;
  if (menuSetting == TOP_MENU_COUNT)
    menuSetting = TOP_MENU_COUNT - 1;

  tft.setTextColor(RA8875_YELLOW, RA8875_BLACK);
  tft.setCursor(FILTER_LOCATION_X, FILTER_LOCATION_Y);
  tft.print("    ");                                      // Clear old value
  tft.setCursor(FILTER_LOCATION_X, FILTER_LOCATION_Y);
  tft.print(menuSetting);                                 // SHow new value

}

/*****
  Purpose: ISR for main tuning

  Parameter list:
    void

  Return value;
    void
*****/
void DrawEncoderTestScreen()
{
  tft.fillScreen(RA8875_BLACK);

  tft.setFontScale(1); 
  tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
  tft.setCursor(600, 200);
  tft.print("Press");
  tft.setCursor(590, 230);
  tft.setTextColor(RA8875_YELLOW, RA8875_BLACK);
  tft.print(" Select");
  tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
  tft.setCursor(600, 260);
  tft.print("to end");

  tft.drawRect(10, 10, 450, 500, RA8875_MAGENTA);    // Draw outline and controls
  tft.drawCircle(100, 100, 70, RA8875_GREEN);
  tft.drawCircle(350, 100, 70, RA8875_GREEN);
  tft.drawCircle(100, 350, 70, RA8875_GREEN);
  tft.drawCircle(350, 350, 70, RA8875_GREEN);

  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);     // Set control titles
  
  tft.setCursor(50, 180);
  tft.print("Volume");
  tft.setCursor(250, 180);
  tft.print("Menu/Filter");
  tft.setCursor(30, 430);
  tft.print("Center Tune");
  tft.setCursor(280, 430);
  tft.print("Fine Tune");


    tft.setTextColor(RA8875_YELLOW, RA8875_BLACK); 
    tft.setCursor(VOLUME_LOCATION_X, VOLUME_LOCATION_Y);
    tft.print(volumeSetting);
    tft.setCursor(FILTER_LOCATION_X, FILTER_LOCATION_Y);
    tft.print(menuSetting);
    tft.setCursor(MAIN_TUNE_LOCATION_X, MAIN_TUNE_LOCATION_Y);
    tft.print(centerTune);
    tft.setCursor(FINE_TUNE_LOCATION_X, FINE_TUNE_LOCATION_Y);
    tft.print(fineTune);

  delay(500L);
}
/*****
  Purpose: ISR for main tuning

  Parameter list:
    void

  Return value;
    void
*****/
void EncoderTune()
{
  char result;

  result = tuneEncoder.process();  // Read the encoder

  if (result == 0) {  // Nothing read
    return;
  }
  switch (result) {
    case DIR_CW:  // Turned it clockwise, 16
      adjustEncoder = 1;
      break;

    case DIR_CCW:  // Turned it counter-clockwise
      adjustEncoder = -1;
      break;
  }
  centerTune += adjustEncoder;

  tft.setCursor(MAIN_TUNE_LOCATION_X, MAIN_TUNE_LOCATION_Y);
  tft.print("    ");                                      // Clear old value
  tft.setCursor(MAIN_TUNE_LOCATION_X, MAIN_TUNE_LOCATION_Y);
  tft.print(centerTune);                                  // SHow new value

}


/*****
  Purpose: ISR for fine tune encoder
  Parameter list:
    void

  Return value;
    void
*****/
void EncoderFineTune()
{
  char result;

  result = fineTuneEncoder.process();  // Read the encoder

  if (result == 0) {  // Nothing read
    return;
  }
  switch (result) {
    case DIR_CW:  // Turned it clockwise, 16
      adjustEncoder = 1;
      break;

    case DIR_CCW:  // Turned it counter-clockwise
      adjustEncoder = -1;
      break;
  }
  fineTune += adjustEncoder;


  tft.setCursor(FINE_TUNE_LOCATION_X, FINE_TUNE_LOCATION_Y);
  tft.print("    ");                                      // Clear old value
  tft.setCursor(FINE_TUNE_LOCATION_X, FINE_TUNE_LOCATION_Y);
  tft.print(fineTune);                                    // SHow new value

}
/*****
  Purpose: Encoder volume control

  Parameter list:
    void

  Return value;
    int               0 means encoder didn't move; otherwise it moved
*****/
void EncoderVolume()
{
  char result;

  result = volumeEncoder.process();  // Read the encoder

  if (result == 0) {  // Nothing read
    return;
  }
  switch (result) {
    case DIR_CW:  // Turned it clockwise, 16
      adjustEncoder = 1;
      break;

    case DIR_CCW:  // Turned it counter-clockwise
      adjustEncoder = -1;
      break;
  }
  volumeSetting += adjustEncoder;
  if (volumeSetting < 0)
    volumeSetting = 0;
  if (volumeSetting > 100)
    volumeSetting = 100;

  tft.setCursor(VOLUME_LOCATION_X, VOLUME_LOCATION_Y);
  tft.print("    ");                                      // Clear old value
  tft.setCursor(VOLUME_LOCATION_X, VOLUME_LOCATION_Y);
  tft.print(volumeSetting);                               // SHow new value
}
