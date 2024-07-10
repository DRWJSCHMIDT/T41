#include "test.h"

// ==================================================== Globals  =======================
int adjustEncoder;
int audioVolume;

int buttonRead;
volatile int16_t curVal[6] = { -1, -1, -1, -1, -1, -1 };
int minPinRead;
volatile int16_t oldVal[6] = { 0, 0, 0, 0, 0, 0 };
const int16_t posx[6] = { 63, 193, 323, 453, 583, 713 };
const int16_t posy[6] = { 63, 63, 63, 63, 63, 63 };
const int16_t radius[6] = { 63, 63, 63, 63, 63, 63 };
//const uint8_t analogIn[6] = {A0, A1, A2, A3, A8, A9};
const uint16_t needleColors[6] = { RA8875_GREEN, RA8875_CYAN, RA8875_MAGENTA, RA8875_YELLOW, RA8875_LIGHT_ORANGE, RA8875_RED };
const uint8_t degreesVal[6][2] = {
  { 150, 240 },
  { 150, 240 },
  { 150, 240 },
  { 150, 240 },
  { 150, 240 },
  { 150, 240 },
};



#define RA8875_CS TFT_CS
#define RA8875_RESET TFT_DC  // any pin or nothing!

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);  // Instantiate the display object

Rotary volumeEncoder   = Rotary(VOLUME_ENCODER_A, VOLUME_ENCODER_B);          //( 2,  3)
Rotary tuneEncoder     = Rotary(TUNE_ENCODER_A, TUNE_ENCODER_B);              //(16, 17)
Rotary filterEncoder   = Rotary(FILTER_ENCODER_A, FILTER_ENCODER_B);          //(15, 14)
Rotary fineTuneEncoder = Rotary(FINETUNE_ENCODER_A, FINETUNE_ENCODER_B);      //( 4,  5)

int volumeSetting = 30;
int menuSetting   = 0;
int centerTune    = 7030;
int fineTune      = 10;


void setup() {
  // put your setup code here, to run once:


  Serial.begin(38400);
  tft.begin(RA8875_800x480, 8, 20000000UL, 4000000UL);  // parameter list from library code
  tft.setRotation(0);

  tuneEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(TUNE_ENCODER_A), EncoderTune, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TUNE_ENCODER_B), EncoderTune, CHANGE);
  volumeEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(VOLUME_ENCODER_A), EncoderVolume, CHANGE);
  attachInterrupt(digitalPinToInterrupt(VOLUME_ENCODER_B), EncoderVolume, CHANGE);
  filterEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(FILTER_ENCODER_A), EncoderFilter, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FILTER_ENCODER_B), EncoderFilter, CHANGE);
  fineTuneEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(FINETUNE_ENCODER_A), EncoderFineTune, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FINETUNE_ENCODER_B), EncoderFineTune, CHANGE);

  pinMode(BUSY_ANALOG_PIN, INPUT);
  digitalWrite(BUSY_ANALOG_PIN, HIGH);

  DoDisplayTest();
  DoSwitchTest();
  DoEncodersTest();
}

void loop() {
  // put your main code here, to run repeatedly:
}

/*****
  Purpose: Check for UI button press. If pressed, return the ADC value

  Parameter list:
    int vsl               the value from analogRead in loop()\

  Return value;
    int                   -1 if not valid push button, ADC value if valid
*****/
int ReadSelectedPushButton() {
  minPinRead = 0;
  int buttonReadOld = 1023;

  while (abs(minPinRead - buttonReadOld) > 3) {  // do averaging to smooth out the button response
    minPinRead = analogRead(BUSY_ANALOG_PIN);
    buttonRead = .1 * minPinRead + .9 * buttonReadOld;  // See expected values in next function.
    buttonReadOld = buttonRead;
  }

  minPinRead = buttonRead;
  delay(100L);

  return minPinRead;
}