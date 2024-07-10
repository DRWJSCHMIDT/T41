#ifndef BEENHERE
#define BEENHERE

//======================================== User section that might need to be changed ===================================
#define VERSION "V050.0"        // Change this for updates. If you make this longer than 9 characters, brace yourself for surprises
#define UPDATE_SWITCH_MATRIX 0  // 1 = Yes, redo the switch matrix values, 0 = leave switch matrix values as is from the last change
struct maps {
  char mapNames[50];
  float lat;
  float lon;
};
extern struct maps myMapFiles[];


//======================================== Library include files ========================================================
#include <Adafruit_GFX.h>
#include "Adafruit_MCP23X17.h"
#include "Fonts/FreeMonoBold24pt7b.h"
#include "Fonts/FreeMonoBold18pt7b.h"
#include "Fonts/FreeMono24pt7b.h"
#include "Fonts/FreeMono9pt7b.h"
#include <TimeLib.h>  // Part of Teensy Time library
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Metro.h>
#include <Bounce.h>
#include <arm_math.h>
#include <arm_const_structs.h>
#include "si5351.h"
//#include <si5351.h>                                 // https://github.com/etherkit/Si5351Arduino
#include <RA8875.h>  // https://github.com/mjs513/RA8875/tree/RA8875_t4
#include <Rotary.h>  // https://github.com/brianlow/Rotary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/crc16.h>  // mdrhere
//#include <utility/imxrt_hw.h>                       // for setting I2S freq, Thanks, FrankB!
#include <EEPROM.h>

#include <math.h>


//========================================= Display pins
#define BACKLIGHT_PIN                   6  // unfortunately connected to 3V3 in DO7JBHs PCB
#define TFT_DC                          9
#define TFT_CS                          10
#define TFT_MOSI                        11
#define TFT_MISO                        12
#define TFT_SCLK                        13
#define TFT_RST                         255

#define V12HWR  // selects the Si5351 direct quadruture clock output for V12 hardware.

#define RA8875_CS                       TFT_CS
#define RA8875_RESET                    TFT_DC  // any pin or nothing!

#define WIGGLE_ROOM                     20  // This is the maximum value that can added to a BUSY_ANALOG_PIN pin read value of a push
#define NUMBER_OF_SWITCHES              18  // Number of push button switches. 16 on older boards
#define TOP_MENU_COUNT                  14  // Menus to process AFP 09-27-22, JJP 7-8-23 AFP 04-12-24

#define BUSY_ANALOG_PIN                 39  // monitors switch matrix

#define VOLUME_ENCODER_A                2  //5   //2
#define VOLUME_ENCODER_B                3  //4   //3
#define FILTER_ENCODER_A                15
#define FILTER_ENCODER_B                14
#define FINETUNE_ENCODER_A              4  //17  // 4
#define FINETUNE_ENCODER_B              5  //16  // 5
#define TUNE_ENCODER_A                  16 //2  //16
#define TUNE_ENCODER_B                  17 //3  // 17

#define VOLUME_LOCATION_X               85
#define VOLUME_LOCATION_Y               115
#define FILTER_LOCATION_X               340
#define FILTER_LOCATION_Y               115
#define MAIN_TUNE_LOCATION_X            70
#define MAIN_TUNE_LOCATION_Y            350
#define FINE_TUNE_LOCATION_X            330
#define FINE_TUNE_LOCATION_Y            350

extern Adafruit_MCP23X17 mcp;

//======================================== Globals =========================================
extern RA8875 tft;

extern int adjustEncoder;
extern int audioVolume;
extern int buttonRead;
extern volatile int16_t curVal[];
extern int minPinRead;
extern volatile int16_t oldVal[];
extern const int16_t posx[];
extern const int16_t posy[];
extern const int16_t radius[];
extern int selectInterrupt;
extern const uint16_t needleColors[];
extern const uint8_t degreesVal[][2];

extern Rotary volumeEncoder;    // ( 2,  3)
extern Rotary tuneEncoder;      // (16, 17)
extern Rotary filterEncoder;    // (14, 15)
extern Rotary fineTuneEncoder;  // ( 4,  5);
extern int volumeSetting;
extern int menuSetting;
extern int centerTune;
extern int fineTune;


//======================================== Function prototypes =========================================================

//int  CheckForSwitchPress(int oldTest, int test);
void DoDisplayTest();
void DoEncodersTest();
int DoSwitchTest();
void DrawEncoderTestScreen();
void drawGauge(uint16_t x, uint16_t y, uint16_t r);
void drawNeedle(uint8_t index, uint16_t bcolor);
void drawPointerHelper(uint8_t index, int16_t val, uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void EncoderTune();
void EncoderFilter();
void EncoderFineTune();
void EncoderVolume();
//void EndOfTestMessage(int where);
int ReadSelectedPushButton();

#endif
