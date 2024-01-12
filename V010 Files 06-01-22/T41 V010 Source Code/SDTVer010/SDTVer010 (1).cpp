#ifndef BEENHERE
#include "SDT.h"
#endif


/*****
  Purpose: SetFrequency

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/
void SetFreq() {

  // Changes for Bobs Octave Filters:  18 March 2018  W7PUA <<<<<<
  // http://www.janbob.com/electron/FilterBP1/FiltBP1.html

  // NEVER USE AUDIONOINTERRUPTS HERE: that introduces annoying clicking noise with every frequency change

  // SI5351_FREQ_MULT is 100ULL, MASTER_CLK_MULT is 4;
  hilfsf = ((centerFreq * SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT * 1000000000;           // AFP 12-30-21

  if (IQ_RecCalFlag == 0) { //IQ receive Test flag
    hilfsfEx = ((TxRxFreq + CWFreqShift) * SI5351_FREQ_MULT) * MASTER_CLK_MULT * 1000000000;                                   //Xmit Clock CLK1
  } else {
    if (IQ_RecCalFlag == 1) {
      hilfsfEx = ((7180000 * SI5351_FREQ_MULT)) * 1 * 1000000000;   //create test tone at 7.180MHZ for IQ calibration
    }
  }

  if (CWOnState == 1) {
    hilfsfEx = (TxRxFreq + CWFreqShift) * SI5351_FREQ_MULT * MASTER_CLK_MULT * 1000000000; //AFP 05-17-22
  }
  hilfsf = hilfsf / calibration_factor;
  hilfsfEx = hilfsfEx / calibration_factor;

  si5351.set_freq(hilfsf, SI5351_CLK2);     // this is actually SI5351_CLK2
  si5351.set_freq(hilfsfEx, SI5351_CLK1);     // this is actually SI5351_CLK1

  if (xmtMode == CW_MODE && decoderFlag == true) {        // No reason to reset if we're not doing decoded CW
    ResetHistograms();
  }
  DrawFrequencyBarValue();
}

/*****
  Purpose: Set Frequency Display Cursor for Fast Tune

  Parameter list:

  Return value;
    void
    Extensively modified 12-23-21 AFP
    Note:  Fast tune frequency offset is determined in FreqShift2()
*****/
void SetFastTuneFrequency()
{
  int offset = MAX_WATERFALL_WIDTH / 2 - 5;
  float cursorDiff;
  float cursorFraction;
  newCursorPosition = 0;
  //return; //AFP 05-17-22
  while (true) {
    cursorDiff        = fineEncoderRead - 256;            // AFP 04-16-22
    cursorFraction    = cursorDiff / ZOOM_2X_BIN_COUNT;   // AFP 12-10-21
    newCursorPosition = (float)stepFT2 * cursorFraction;  // AFP 03-27-22 Layers

    if (newCursorPosition == oldCursorPosition) {         // Was encoder changed?
      return;                                             // Nope, go home...
    }
    fastTuneActive = 1;

    if (newCursorPosition >= offset) {
      newCursorPosition = offset;
    }
    if (newCursorPosition <= -offset) {
      newCursorPosition = -offset;
    }
    if (newCursorPosition != oldCursorPosition) {
      DrawBandWidthIndicatorBar();  // Did fast tume cursor change? // AFP 03-27-22 Layers
    }

    TxRxFreq = centerFreq + NCO_FREQ;

    SetFreq();
    if (xmtMode == CW_MODE && decoderFlag == true)  // No reason to reset if we're not doing decoded CW
      ResetHistograms();
  }
  if (displayMode == NO_DISPLAY || displayMode == WATERFALL_ONLY) {   // Don't show a Spectrum
    return;
  }
  tft.writeTo(L1);//AFP 03-27-22 Layers
}

/*****
  Purpose: Places the Fast Tune cursor in the center of the spectrum display

  Parameter list:

  Return value;
    void
*****/
void CenterFastTune()
{
  oldCursorPosition = newCursorPosition = 256;    // AFP 12-10-21
  fastTuneEncoder.write(newCursorPosition);
  tft.drawFastVLine(oldCursorPosition, SPECTRUM_TOP_Y + 20, SPECTRUM_HEIGHT - 27, RA8875_BLACK);
  tft.drawFastVLine(newCursorPosition , SPECTRUM_TOP_Y + 20, SPECTRUM_HEIGHT - 27, RA8875_RED);
}

/*****
  Purpose: Purpose is to sety VFOa to receive frequency and VFOb to the transmit frequency

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/
void DoSplitVFO()
{

}
