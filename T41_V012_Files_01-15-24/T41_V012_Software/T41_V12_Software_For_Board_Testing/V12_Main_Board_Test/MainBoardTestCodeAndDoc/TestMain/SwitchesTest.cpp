#ifndef BEENHERE
#include "test.h"
#endif

/*****
  Purpose: To test the switch matrix. Start by pressing the Upper-left switch in the switch matrix
           and proceed left-to-right, top-to-bottom in the switch sequence

  Argument List:
    void

  Return Value:
    int           1 test passed, 0 test failed
*****/
int DoSwitchTest() {
  const char *labels[] = { "Select", "Menus", "Band+",
                           "Zoom", "Dis. Reset", "Band-",
                           "Mode", "Demod", "Main Inc",
                           "Noise", "Notch", "Fine Inc",
                           "Filter", "CW Resest", "User 1",
                           "User 2", "User 3", "User 4" };

  int upperLeftX = 50;
  int upperLeftY = 25;
  int boxWidth = 550,
      boxHeight = 450;
  int columnSpacing = 200;
  int rowSpacing = 75;
  int index;
  int oldValue;
  int value;
  int col, row;

  tft.fillScreen(RA8875_BLACK);
  tft.drawRect(upperLeftX, upperLeftY, boxWidth, boxHeight, RA8875_YELLOW);

  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.setCursor(upperLeftX + 10 + boxWidth, 40);
  tft.print("To start");
  tft.setCursor(upperLeftX + 10 + boxWidth, 70);
  tft.print("test, press");
  tft.setCursor(upperLeftX + 10 + boxWidth, 100);
  tft.print("the upper-");
  tft.setCursor(upperLeftX + 10 + boxWidth, 130);
  tft.print("left button");
  tft.setCursor(upperLeftX + 10 + boxWidth, 160);
  tft.print("moving left");
  tft.setCursor(upperLeftX + 10 + boxWidth, 190);
  tft.print("to right.");
  tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
  row = 0;
  col = -1;
  for (index = 0; index < NUMBER_OF_SWITCHES;) {  // Show the (assumed) switch labels
    if (col == 2) {
      col = 0;
      row++;
    } else {
      col++;
    }

    tft.setCursor(upperLeftX + 10 + (columnSpacing * col), upperLeftY + 1 + (row * rowSpacing));
    tft.print(labels[index]);
    index++;
  }
  row = 0;
  col = -1;
  for (index = 0; index < NUMBER_OF_SWITCHES;) {  // Now show the values
    if (col == 2) {
      col = 0;
      row++;
    } else {
      col++;
    }

    tft.setCursor(upperLeftX + 10 + (columnSpacing * col), upperLeftY + 38 + (row * rowSpacing));
    tft.print("(");
    tft.print(value);
    tft.print(")");
    index++;
  }

  delay(200L);
  row = 0;
  col = -1;

  oldValue = 1010;
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  for (index = 0; index < NUMBER_OF_SWITCHES;) {  // Now show the values
    while (true) {
      value = ReadSelectedPushButton();
      if (abs(value - oldValue) > WIGGLE_ROOM && value < oldValue) {
        if (col == 2) {
          col = 0;
          row++;
        } else {
          col++;
        }

        tft.setCursor(upperLeftX + 10 + (columnSpacing * col), upperLeftY + 38 + (row * rowSpacing));
        tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
        tft.print("(");
        tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
        tft.print(value);
        tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
        tft.print(")");
        oldValue = value;
        index++;
        delay(100L);
        break;
      }
    }
  }

  tft.fillRect(upperLeftX + 9 + boxWidth, 40, upperLeftX + 9 + boxWidth, 280, RA8875_BLACK);
  tft.setCursor(upperLeftX + 40 + boxWidth, 300);
  tft.print("Press");
  tft.setCursor(upperLeftX + 20 + boxWidth, 330);
  tft.setTextColor(RA8875_YELLOW, RA8875_BLACK);
  tft.print(" Select");
  tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
  tft.setCursor(upperLeftX + 30 + boxWidth, 360);
  tft.print("to end");
  delay(500L);
  while (true) {
    value = analogRead(BUSY_ANALOG_PIN);
    delay(100L);
    if (value < 1000) {
      tft.fillScreen(RA8875_BLACK);
      return 1;
    }
  }
  return 0;
}
