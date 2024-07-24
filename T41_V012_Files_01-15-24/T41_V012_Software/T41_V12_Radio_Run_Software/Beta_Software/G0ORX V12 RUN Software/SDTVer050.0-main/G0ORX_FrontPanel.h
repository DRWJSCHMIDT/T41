#ifndef G0ORX_FRONTPANEL_H
#define G0ORX_FRONTPANEL_H

#include <stdint.h>
#include "G0ORX_Rotary.h"

#define AUDIO_VOLUME 0
#define MIC_GAIN 1
#define AGC_GAIN 2
#define SIDETONE_VOLUME 3
#define NOISE_FLOOR_LEVEL 4
#define SQUELCH_LEVEL 5

extern G0ORX_Rotary volumeEncoder;
extern G0ORX_Rotary filterEncoder;
extern G0ORX_Rotary tuneEncoder;
extern G0ORX_Rotary fineTuneEncoder;

extern int volumeFunction;
extern int my_ptt;

extern int G0ORXButtonPressed;
extern void FrontPanelInit();
extern void FrontPanelCheck();
extern void FrontPanelSetLed(int led, uint8_t state);

extern void PTT_Interrupt();

#endif