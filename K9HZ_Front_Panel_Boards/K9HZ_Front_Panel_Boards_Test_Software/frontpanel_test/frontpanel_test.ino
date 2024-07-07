#include "G0ORX_Rotary.h"
#include "G0ORXFrontPanel.h"

int led_state;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Front Panel Test");
  Serial.println("Both LEDs should be OFF they swap state with each button press");
  Serial.println("Press a button or rotate an encoder ...");
  FrontPanelInit();

}

void loop() {
  // put your main code here, to run repeatedly:
  led_state=FrontPanelCheck(led_state);
  FrontPanelSetLed(LED1,led_state);
  FrontPanelSetLed(LED2,led_state);
}
