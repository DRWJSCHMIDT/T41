#ifndef BEENHERE
#include "test.h"
#endif


/*****
  Purpose: To exercise the RA8875 display and see if it is working properly

  Argument List:
    void

  Return Value:
    void
*****/
void DoDisplayTest() {
  int test;

  tft.fillWindow(RA8875_BLACK);
  tft.setFontScale((enum RA8875tsize)2);
  tft.setTextColor(RA8875_MAGENTA, RA8875_BLACK);
  tft.setCursor(400 - (12 * tft.getFontWidth()) / 2, 150);
  tft.print("Display Test");
  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_GREEN, RA8875_BLACK);
  tft.setCursor(150, 220 + tft.getFontHeight() + 5);
  tft.print("You will see 6 circular gauges");
  tft.setCursor(150, 255 + tft.getFontHeight() + 5);
  tft.print(" displaying bogus test values.");
  tft.setCursor(135, 280 + tft.getFontHeight() + 20);
  tft.print("The screen guages shouold appear");
  tft.setCursor(200, 310 + tft.getFontHeight() + 20);
  tft.print("within about 5 seconds.");

  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(135, 440);
  tft.print("Press ");
  tft.setTextColor(RA8875_RED);
  tft.print("Select");
  tft.setTextColor(RA8875_WHITE);
  tft.print(" pushbutton to end test");

  delay(5000L);                                 // Given them time to read the message

  for (uint8_t i = 0; i < 6; i++) {
    drawGauge(posx[i], posy[i], radius[i]);
  }


  while (1) {
    for (uint8_t i = 0; i < 6; i++) {
      curVal[i] = random(255);
      drawNeedle(i, RA8875_BLACK);
    }
    delay(100);

    test = analogRead(BUSY_ANALOG_PIN);
    if (test < 1000) {
      return;
    }
  }

}

/*****
  Purpose: To draw the guages on the display

  Argument List:
    void

  Return Value:
    void
*****/
void drawGauge(uint16_t x, uint16_t y, uint16_t r) {
  tft.drawCircle(x, y, r, RA8875_WHITE);                       //draw instrument container
  tft.roundGaugeTicker(x, y, r, 150, 390, 1.3, RA8875_WHITE);  //draw major ticks
  if (r > 15)
    tft.roundGaugeTicker(x, y, r, 165, 375, 1.1, RA8875_WHITE);  //draw minor ticks
}

/*****
  Purpose: To draw the needles on the guages

  Argument List:
    void

  Return Value:
    void
*****/
void drawNeedle(uint8_t index, uint16_t bcolor) {
  uint16_t i;
  if (curVal[index] > 255) return;  //curVal[index] = 255;
  if (curVal[index] < 0) return;    //curVal[index] = 0;
  if (oldVal[index] != curVal[index]) {
    if (curVal[index] > oldVal[index]) {
      for (i = oldVal[index]; i <= curVal[index]; i++) {
        if (i > 0) drawPointerHelper(index, i - 1, posx[index], posy[index], radius[index], bcolor);
        drawPointerHelper(index, i, posx[index], posy[index], radius[index], needleColors[index]);
        if ((curVal[index] - oldVal[index]) < (128)) delay(1);  //ballistic
      }
    } else {
      for (i = oldVal[index]; i > curVal[index]; i--) {
        drawPointerHelper(index, i + 1, posx[index], posy[index], radius[index], bcolor);
        drawPointerHelper(index, i, posx[index], posy[index], radius[index], needleColors[index]);
        //ballistic
        if ((oldVal[index] - curVal[index]) >= 128) {
          delay(1);
        } else {
          delay(2);
        }
      }
    }
    oldVal[index] = curVal[index];
  }
}

/*****
  Purpose: To determine the angles for the guage needles

  Argument List:
    void

  Return Value:
    void
*****/
void drawPointerHelper(uint8_t index, int16_t val, uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
  float dsec;
  const int16_t minValue = 0;
  const int16_t maxValue = 255;
  dsec = (((float)(uint16_t)(val - minValue) / (float)(uint16_t)(maxValue - minValue) * degreesVal[index][1]) + degreesVal[index][0]) * (PI / 180);
  uint16_t w = (uint16_t)(1 + x + (cos(dsec) * (r / 1.35)));
  uint16_t h = (uint16_t)(1 + y + (sin(dsec) * (r / 1.35)));
  tft.drawLine(x, y, w, h, color);
  tft.fillCircle(x, y, 2, color);
}
