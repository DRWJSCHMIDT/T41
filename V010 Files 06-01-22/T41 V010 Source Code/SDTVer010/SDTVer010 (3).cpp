#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: To process a band increase button push

  Parameter list:
    void

  Return value:
    void

#define BAND_80M                  0
#define BAND_40M                  1
#define BAND_20M                  2
#define BAND_17M                  3
#define BAND_15M                  4
#define BAND_12M                  5
#define BAND_10M                  6
80M - Teensy Pin 30

40M- Teensy Pin 31

20M - Teensy Pin 28

15M- Teensy Pin 29
int BandswitchPins[] = {30,   // 80M
                        31,   // 40M
                        28,   // 20M
                        0,    // 17M
                        29,   // 15M
                        0,    // 12M
                        0     // 10M
                       };
*****/
void ButtonBandIncrease()
{
  AudioNoInterrupts();
  currentBand++;
  if (currentBand > LAST_BAND) {
    currentBand = FIRST_BAND; // cycle thru radio bands
  }
  if (activeVFO == VFO_A) {
    centerFreq = TxRxFreq = currentFreqA = bands[currentBand].freq;
  } else {
    centerFreq =  TxRxFreq = currentFreqB = bands[currentBand].freq;
   }
 
  freq_flag[1] = 0;
  SetBand();
  SetFreq();
  ShowFrequency();
  MyDelay(1L);
  AudioInterrupts();
}

/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonBandDecrease()
{
  AudioNoInterrupts();

  currentBand--;
  if (currentBand < FIRST_BAND) {
    currentBand = LAST_BAND; // cycle thru radio bands
  }
  if (currentBand > LAST_BAND)  {
    currentBand = FIRST_BAND;
  }
  if (activeVFO == VFO_A) {
    TxRxFreq = currentFreqA = bands[currentBand].freq;  
  } else {
    TxRxFreq = currentFreqB = bands[currentBand].freq;  
  }
  centerFreq = TxRxFreq; 
  freq_flag[1] = 0;
  SetBand();
  SetFreq();
  ShowFrequency();

  AudioInterrupts();
}

/*****
  Purpose: set Band
  Parameter list:
    void
  Return value;
    void
*****/
void SetBand() 
{
  old_demod_mode = -99; // used in setup_mode and when changing bands, so that LoCut and HiCut are not changed!
  SetupMode(bands[currentBand].mode);
  SetFreq();
  ShowFrequency();  
  FilterBandwidth();
}
