#ifndef G0ORXFRONTPANEL_H
#define G0ORXFRONTPANEL_H

#define LED1 0
#define LED2 1

#define LED_ON  1
#define LED_OFF 0



extern void FrontPanelInit();
extern int FrontPanelCheck(int led_state);
extern void FrontPanelSetLed(int led, uint8_t state);

#endif