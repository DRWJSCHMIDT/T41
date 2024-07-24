#ifndef BEENHERE
#include "SDT.h"
#endif

#if !defined(EXCLUDE_BODE)
/*==================================
Notes about the Bode Plotter code.
* Most  of the Bode code is stand-alone, not using most of the resources of the T41 for things like Plotting, drawing the plot container, processing the signal, etc.
* Resource that are used include the four encoders, Setting the Si Quadrature clocks, Front panel buttons.
*The signal processing is quite straightfroward:
  * signal are read in using the V12 RF board hardware and digitized in the normal fashion  using 
  the ADCs on the Main board.
  * The signals are then filtered using a 6 pole Chebyshev DSP BPF operatinfg at 192KHz, with a center frequency of 19.2 KHz and a bandwidth of about 7KHz. 
   This narrow band filter eliminates much of the unwanted non-signal noise and eliminates unwanted responses from the Si5351 harmonics. In order to utilize the filter, 
   the Si output signal to the filter under test is offset by about 17KHz as compared to the Receive quadrature clocks.
  * After filtering, the signals are then scaled and the RMS value determined using one of the arm DSP functions.
  * UI is accomplished using the T41 Menu system, all four T41 encoders, and several of the front panel switch matrix.
  * Plot speed is kept slow to allow the filters under test time to settle between each frequency change.
  * In this addition, no plots are saved, although this would be a nice enhancement.
  * Useful dynamic range is approximately 70 dB.
  * T41 V12 Rf board attenuators can be used the reduce the signal level to suitable levels, however, the code to activate the attenuators is not yet impleiented as of version 49.7.
*/
//=================== AFP 03-30-24 V012 Bode Plot
/*****
  Purpose: Bode Menu Options
  Parameter list:
  Return value;
    int           Return value has no real meaning. Done to be consistent with pointer to function array.
*****/
int BodeOptions() 
{
  switch (secondaryMenuIndex) {
    case 0:  // Start Bode Plot
      BodePlotFlag = 1;
      BodePLotter();
      break;
    case 1:  // Set Start Frequency
      freqStartBode = 3000000;
      SetBodeStart();
      break;
    case 2:  // Set Start Frequency
      freqStopBode = 30000000;
      SetBodeStop();
      break;

    case 3:  // Set Ref Level
      DrawPlots();
      break;

    case 4:  // Set Ref Level
      BodePlotFlag = 0;
      break;
  }
  return 1;
}
/*****
  Purpose: DoBodePlot

  Parameter list:
    float freqStart, freqStop, long numBodePoints, float refLevel

  Return value;
    void

*****/
void BodePLotter() {

  int buttonIndex = 0;
  doneViewing = 0;
//  int nQ;
//  int N;
//  int rem;

  float freqIncrement = (float)(freqStopBode - freqStartBode) / numBodePoints;
//  int yBodePosition;
//  int xBodePosition;
//  int pushButtonSwitchIndex = -1;
//  int valPin;
//  float bodeResultROld;
//  float bodeResultROld2;
//  float bodeResultRSum = 0.0;
 freqBodeChangeFlag = 0;
  BodeValuesMax = -60;
  modeSelectInR.gain(0, 1);
  modeSelectInL.gain(0, 1);
  digitalWrite(33, HIGH);
  digitalWrite(34, LOW);
  digitalWrite(38, LOW);
  DrawBodePlotContainer();
 
  int k = 0;
  si5351.reset();                                                                // KF5N.  Moved Si5351 start-up to setup. JJP  7/14/23
  si5351.init(SI5351_CRYSTAL_LOAD_10PF, Si_5351_crystal, freqCorrectionFactor);  //JJP  7/14/23
  si5351.set_ms_source(SI5351_CLK2, SI5351_PLLB);                                //  Allows CLK1 and CLK2 to exceed 100 MHz simultaneously.
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);                          //AFP 10-13-22
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
  while (doneViewing != 1) {
    ;
   


//=== for testing the attenuator
/*
numBodePoints=2;
for(int j=0;j<63;j++){
 SetRF_OutAtten(j);
 MyDelay(10000);
}
*/
//=================

//Serial.print("currentRF_OutAtten=  ");Serial.println((float)currentRF_OutAtten);
    for (int iFreq1 = 0; iFreq1 < numBodePoints; iFreq1++) {
      EncoderCenterTune();
      if (freqBodeChangeFlag == 1) {
        DrawBodePlotContainer();
        freqBodeChangeFlag = 0;
      }

//      int pushButtonSwitchIndex = -1;
      int valPin;
      freqIncrement = (freqStopBode - freqStartBode) / (numBodePoints);
    centerFreqBode = freqStartBode + freqIncrement * iFreq1;
//centerFreqBode =7000000L;
      Clk2SetFreq = ((centerFreqBode + 17500.) * SI5351_FREQ_MULT);

      si5351.set_freq(Clk2SetFreq, SI5351_CLK2);

      SetFreqBode();

      ProcessIQDataBode();

      centerFreqBode = float(centerFreqBode);
      BodeValuesRaw[iFreq1] = bodeResultR;
      BodeFreqRaw[iFreq1] = centerFreqBode;
      bodeResultRdB = (20.0 * log10f_fast(bodeResultR));
      bodeResultRdB = ((float)refLevelBode + bodeResultRdB);

      float freqStart2 = freqStartBode / 1000.0;
      float freqStop2 = freqStopBode / 1000.0;
      float centerFreq2 = (centerFreqBode) / 1000.0;
      BodeValues[iFreq1] = bodeResultRdB;
      BodeFreq[iFreq1] = centerFreq2;
      BodePlotValues[iFreq1] = (map(int(bodeResultRdB * 10), -600, 100, 400, 50));
      if (BodePlotValues[iFreq1] > 399) {
        BodePlotValues[iFreq1] = 399;
      }
      BodePlotFreq[iFreq1] = map(centerFreq2, freqStart2, freqStop2, 50, 750);
      tft.writeTo(L2);
      if ((centerFreqBode >= 6850000 + freqIncrement * 11) && (centerFreqBode < 6850000 + freqIncrement * 13)) {  //Removes spurious data caused by Di5351 multiplier change during sweep.
        BodePlotValues[iFreq1] = 479;
      }
      if ((centerFreqBode >= 9500000 + freqIncrement * 11) && (centerFreqBode < 9500000 + freqIncrement * 13)) {
        BodePlotValues[iFreq1] = 475;
      }
      if ((centerFreqBode >= 13600000 + freqIncrement * 11) && (centerFreqBode < 13600000 + freqIncrement * 13)) {
        BodePlotValues[iFreq1] = 475;
      }
      if ((centerFreqBode >= 17500000 + freqIncrement * 11) && (centerFreqBode < 17500000 + freqIncrement * 13)) {
        BodePlotValues[iFreq1] = 479;
      }
      if ((centerFreqBode >= 25000000 + freqIncrement * 11) && (centerFreqBode < 25000000 + freqIncrement * 13)) {
        BodePlotValues[iFreq1] = 479;
      }
      tft.fillCircle(BodePlotFreqOld[iFreq1], BodePlotValuesOld[iFreq1], 2, RA8875_BLACK);
      if (BodePlotValues[iFreq1] > 399) {
        tft.fillCircle(BodePlotFreq[iFreq1], BodePlotValues[iFreq1], 2, RA8875_BLACK);
      } else {

        tft.fillCircle(BodePlotFreq[iFreq1], BodePlotValues[iFreq1], 2, RA8875_YELLOW);
      }
      if (plotBodeRefFlag == 1) {
        //.println("Ref plot on");
        tft.fillCircle(BodePlotFreqSave[iFreq1], BodePlotValuesSave[iFreq1], 2, RA8875_LIGHT_GREY);
      }

      BodePlotValuesOld[iFreq1] = BodePlotValues[iFreq1];
      BodePlotFreqOld[iFreq1] = BodePlotFreq[iFreq1];
      if (BodeValues[iFreq1] >= BodeValuesMax) {
        BodeValuesMax = BodeValues[iFreq1];
        BodeValuesMaxFreq = BodeFreq[iFreq1];
      } else {
        BodeValuesMax = BodeValuesMax;
        BodeValuesMaxFreq = BodeValuesMaxFreq;
      }

      MoveStopFreqBode();

      if (replotFlag == 1) {
        tft.fillRect(40, 50, 760, 430, RA8875_BLACK);
        DrawBodePlotContainer();
        replotFlag = 0;
        return;
      }

      MoveBodeCursor();
      tft.writeTo(L1);
      //============  Print Max value and freq
      tft.setFontScale((enum RA8875tsize)0);
      tft.fillRect(685, 118, 60, 50, RA8875_BLACK);
      tft.setCursor(695, 120);
      tft.print(BodeValuesMaxFreq / 1000, 3);
      tft.setCursor(695, 140);
      tft.print(BodeValuesMax, 1);

      tft.setFontScale((enum RA8875tsize)1);
      valPin = ReadSelectedPushButton();  // Poll UI push buttons

      if (valPin != BOGUS_PIN_READ) {  // If a button was pushed...
        buttonIndex = ProcessButtonPress(valPin);

        switch (buttonIndex) {
          case BAND_UP:
            EraseMenus();
            if (currentBand < 5) digitalWrite(bandswitchPins[currentBand], LOW);  // Added if so unused GPOs will not be touched.  KF5N October 16, 2023.
            ButtonBandIncrease();
            if (currentBand < 5) digitalWrite(bandswitchPins[currentBand], HIGH);
            DrawBodePlotContainer();
            tft.setTextColor(RA8875_LIGHT_ORANGE);
            tft.setCursor(700, 20);
            tft.print(bands[currentBandA].name);
            break;
          case BAND_DN:
            DrawBodePlotContainer();
            if (currentBand < 5) digitalWrite(bandswitchPins[currentBand], LOW);  // Added if so unused GPOs will not be touched.  KF5N October 16, 2023.
            ButtonBandIncrease();
            if (currentBand < 5) digitalWrite(bandswitchPins[currentBand], HIGH);
            DrawBodePlotContainer();
            tft.setTextColor(RA8875_LIGHT_ORANGE);
            tft.setCursor(700, 20);
            tft.print(bands[currentBandA].name);
            break;
          case UNUSED_1:  // Pressed pushbutton 14 save current plot
            for (int iFreq1 = 0; iFreq1 < numBodePoints; iFreq1++) {
              BodePlotValuesSave[iFreq1] = BodePlotValues[iFreq1];
              BodePlotFreqSave[iFreq1] = BodePlotFreq[iFreq1];
            }
            break;
          case UNUSED_3:  // Pressed pushbutton 14 save current plot
            if (plotBodeBandFlag == 0) {
              DrawBodePlotContainer();
              plotBodeBandFlag = 1;
            } else if (plotBodeBandFlag == 1) {
              plotBodeBandFlag = 0;
              DrawBodePlotContainer();
              DrawBands();
            }
            //DrawPlots();
            break;
          case UNUSED_2:  // Pressed pushbutton 14 save current plot
            if (plotBodeRefFlag == 0) {
              plotBodeRefFlag = 1;
            } else if (plotBodeRefFlag == 1) {
              plotBodeRefFlag = 0;
              DrawBodePlotContainer();
            }

            //DrawPlots();
            break;
          case UNUSED_4:              // Pressed pushbutton 18
          case MENU_OPTION_SELECT:    // All done
            doneViewing = 1;
            BodePlotFlag = 0;
            break;
          default:
            break;
        }
      }
      if (doneViewing == 1) {
        break;
      }
    }
    k = k + 1;
    if (doneViewing == 1) {
      doneViewing = 0;
      break;
    }
  }
}

//stopFreqOld = freqStopBode;
//}

/*****
  Purpose: DrawPlots()

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/

void DrawPlots() {
  int buttonIndex = 0;
  doneViewing = 0;
  DrawBodePlotContainer();
  tft.writeTo(L2);
  while (doneViewing != 1) {
    for (int iFreq1 = 0; iFreq1 < numBodePoints; iFreq1++) {

      tft.fillCircle(BodePlotFreqSave[iFreq1], BodePlotValuesSave[iFreq1], 2, RA8875_LIGHT_GREY);
    }

    //MyDelay(100L);
    valPin = ReadSelectedPushButton();  // Poll UI push buttons

    //MyDelay(100L);
    if (valPin != BOGUS_PIN_READ) {  // If a button was pushed...
      buttonIndex = ProcessButtonPress(valPin);

      if (buttonIndex == UNUSED_4) {  // Pressed pushbutton 18
        doneViewing = 1;
        break;
      }
    }
  }
}


/*****
  Purpose: SetFrequency

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/

void SetFreqBode() 
{  //AFP
//  long freqBode;
  Clk1SetFreq = (((centerFreqBode)*SI5351_FREQ_MULT));
  multiple = EvenDivisor(Clk1SetFreq / SI5351_FREQ_MULT);

  pll_freq = Clk1SetFreq * multiple;
// freqBode = pll_freq / multiple;
  si5351.pll_reset(SI5351_PLLA);
  si5351.pll_reset(SI5351_PLLB);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);  // KF5N July 10 2023
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
  si5351.set_freq_manual(Clk1SetFreq, pll_freq, SI5351_CLK0);
  si5351.set_freq_manual(Clk1SetFreq, pll_freq, SI5351_CLK1);
  si5351.set_phase(SI5351_CLK0, 0);
  si5351.set_phase(SI5351_CLK1, multiple);
}

/*****
  Purpose: DrawDoBodePlotContainer

  Parameter list:
    float freqStart, freqStop, long numBodePoints, float refLevel

  Return value;
    void

 
*****/
void DrawBodePlotContainer() {
  tft.writeTo(L1);
  tft.fillRect(0, 0, 799, 479, RA8875_BLACK);
  tft.writeTo(L2);
  tft.fillRect(0, 0, 799, 479, RA8875_BLACK);

//  float freqIncrement = (float)(freqStopBode - freqStartBode) / float(numBodePoints);
//  int yBodePosition;
//  int xBodePosition;
//  int pushButtonSwitchIndex = -1;
//  int valPin;
//  float bodeResultROld;
//  float bodeResultROld2;
  BodeValuesMax = -600;
  modeSelectInR.gain(0, 1);
  modeSelectInL.gain(0, 1);
  //============== Draw lines on Layer 2

  tft.drawFastHLine(40, 50, 710, RA8875_GREEN);
  tft.drawFastHLine(40, 100, 10, RA8875_GREEN);
  tft.drawFastHLine(40, 150, 10, RA8875_GREEN);
  tft.drawFastHLine(40, 200, 10, RA8875_GREEN);
  tft.drawFastHLine(40, 250, 10, RA8875_GREEN);
  tft.drawFastHLine(40, 300, 10, RA8875_GREEN);
  tft.drawFastHLine(40, 350, 10, RA8875_GREEN);
  tft.drawFastVLine(50, 50, 360, RA8875_GREEN);
  tft.drawFastHLine(50, 100, 700, tft.Color565(50, 50, 50));
  tft.drawFastHLine(50, 150, 700, tft.Color565(50, 50, 50));
  tft.drawFastHLine(50, 200, 700, tft.Color565(50, 50, 50));
  tft.drawFastHLine(50, 250, 700, tft.Color565(50, 50, 50));
  tft.drawFastHLine(50, 300, 700, tft.Color565(50, 50, 50));
  tft.drawFastHLine(50, 350, 700, tft.Color565(50, 50, 50));
  tft.drawFastHLine(50, 400, 700, RA8875_GREEN);
  tft.drawFastVLine(750, 50, 360, RA8875_GREEN);

  tft.drawFastVLine(50, 50, 360, RA8875_GREEN);
  for (int k = 1; k < 10; k++) {  // Draw Freq axis tick marks and numbers
    tft.drawFastVLine(50 + 70 * k, 50, 360, tft.Color565(50, 50, 50));
  }
  tft.setTextColor(RA8875_WHITE);
  for (int k = 0; k < 11; k++) {  // Draw Freq axis tick marks and numbers
    tft.drawFastVLine(50 + 70 * k, 400, 10, RA8875_GREEN);
    tft.setCursor(37 + 70 * k, 415);
    tft.setFontScale((enum RA8875tsize)0);
    tft.print(float(freqStartBode) / 1000000.0 + k * float(freqStopBode - freqStartBode) / 10000000., 2);
  }
  tft.setFontScale((enum RA8875tsize)0);
  tft.setCursor(350, 440);
  tft.print("MHz");
  tft.setFontScale((enum RA8875tsize)0);
  tft.setCursor(630, 120);
  tft.print("Max");
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(16, 225);
  tft.print("dB");
  for (int k = 0; k < 8; k++) {  // Draw Vert axix tick marks
    tft.setCursor(10, 40 + 50 * k);
    tft.print(10 - 10 * k);
  }
  xrState = RECEIVE_STATE;
  tft.setFontScale((enum RA8875tsize)1);
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(270, 15);
  tft.print("T41 Bode Plotter");
  tft.writeTo(L1);
}

/*****
  Purpose: Draw Ham Bands

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/

void DrawBands() {
  tft.writeTo(L1);
  int BandStart;
  int BandStop;

  int freqStartBodePlot = freqStartBode / 1000;
  int freqStopBodePlot = freqStopBode / 1000;
  tft.setFontScale((enum RA8875tsize)0);
  tft.setTextColor(RA8875_LIGHT_ORANGE);
  //float freqIncrement = (float)(freqStopBode - freqStartBode) / float(numBodePoints);
  if (freqStartBode < 3500000 && freqStopBode > 4000000) {
    BandStart = map(3500, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(4000, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("80M");
  }
  if (freqStartBode < 7000000 && freqStopBode > 7300000) {
    BandStart = map(7000, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(7300, freqStartBodePlot, freqStopBodePlot, 50, 750);

    //tft.fillRect(BandStart, 50, BandStop - BandStart, 350,  tft.Color565(0, 0, 150));
    tft.fillRect(BandStart, 50, BandStop - BandStart, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("40M");
  }

  if (freqStartBode < 10100000 && freqStopBode > 10150000) {
    BandStart = map(10100, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(10150, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart + 1, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("30M");
  }

  if (freqStartBode < 14000000 && freqStopBode > 14350000) {
    BandStart = map(14000, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(14350, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("20M");
  }
  if (freqStartBode < 18068000 && freqStopBode > 18168000) {
    BandStart = map(18068, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(18168, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart + 1, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("17M");
  }
  if (freqStartBode < 21000000 && freqStopBode > 21450000) {
    BandStart = map(21000, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(21450, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("15M");
  }
  if (freqStartBode < 24800000 && freqStopBode > 24990000) {
    BandStart = map(24800, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(24899, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart + 1, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop + 2, 55);
    tft.print("12M");
  }
  if (freqStartBode < 28000000 && freqStopBode > 29700000) {
    BandStart = map(28000, freqStartBodePlot, freqStopBodePlot, 50, 750);
    BandStop = map(29700, freqStartBodePlot, freqStopBodePlot, 50, 750);
    tft.fillRect(BandStart, 50, BandStop - BandStart, 350, tft.colorInterpolation(BLUE, BLACK, 4, 8));
    tft.setCursor(BandStop - 18, 55);
    tft.print("10M");
  }
}

/*****
  Purpose: Bode Plot Encoder

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/
void MoveBodeCursor() {
//  int val;

  if (filterEncoderMove != 0) {
    tft.writeTo(L1);
    tft.setFontScale((enum RA8875tsize)0);
    tft.fillRect(685, 53, 60, 50, RA8875_BLACK);
    currentFreqPos += filterEncoderMove;
    tft.setTextColor(RA8875_WHITE);
    tft.setCursor(695, 60);
    tft.print(BodeFreq[currentFreqPos] / 1000, 3);
    tft.setCursor(695, 80);
    tft.print(BodeValues[currentFreqPos], 1);
    filterEncoderMove = 0;
    tft.writeTo(L1);
    tft.fillCircle(BodePlotFreqOld[lastCurrentFreqPos], BodePlotValuesOld[lastCurrentFreqPos], 12, RA8875_BLACK);
    tft.drawCircle(BodePlotFreq[currentFreqPos], BodePlotValues[currentFreqPos], 10, RA8875_CYAN);
    tft.drawCircle(BodePlotFreq[currentFreqPos], BodePlotValues[currentFreqPos], 9, RA8875_CYAN);
    tft.drawCircle(BodePlotFreq[currentFreqPos], BodePlotValues[currentFreqPos], 8, RA8875_CYAN);
    tft.writeTo(L2);

    lastCurrentFreqPos = currentFreqPos;
  } else {
    return;
  }
}

/*****
  Purpose: Process Bode Plot Data

   Parameter List:
      void
   Return value:
      void

 *****/
void ProcessIQDataBode() {
//  float bodeResultROld;
  if ((uint32_t)Q_in_L.available() > N_BLOCKS + 0 && (uint32_t)Q_in_R.available() > N_BLOCKS + 0) {
    usec = 0;
    for (unsigned i = 0; i < N_BLOCKS; i++) {
      sp_L1 = Q_in_R.readBuffer();
      arm_q15_to_float(sp_L1, &float_buffer_L[BUFFER_SIZE * i], BUFFER_SIZE);  // convert int_buffer to float 32bit
      Q_in_L.freeBuffer();
      Q_in_R.freeBuffer();
    }
  }

  arm_biquad_cascade_df2T_f32(&S1_BodePlotFilter, float_buffer_L, float_buffer_L, 2048);
  arm_scale_f32(float_buffer_L, 2, float_buffer_L_AudioCW, 2048);  //AFP 10-18-22
  arm_rms_f32(float_buffer_L_AudioCW, 2048, &bodeResultR);
}

// ================== Bode IIF Filter


// 6 pole Chebyshev BPF 192KSPS 19.2KHZ Fc 7.68BW // AFP 03-24-24 // Bode
float32_t BodePlotterBPFCoeffs[25] = {
  // b0, b1,b2,-a1,-a2
  -0.068121899066840616,
  0.000000000000000000,
  0.068121899066840616,
  1.560896480160454350,
  -0.885355657744309466,
  -0.067319389768690507,
  0.000000000000000000,
  0.067319389768690507,
  1.477995246851487640,
  -0.874925734955274037,
  -0.098892489793497851,
  0.000000000000000000,
  0.098892489793497851,
  1.646024467432495310,
  -0.922049998052159925,
  -0.123764255566975914,
  0.000000000000000000,
  0.123764255566975914,
  1.717009766450222410,
  -0.972374132283812753,
  -0.122368855994624118,
  0.000000000000000000,
  0.122368855994624118,
  1.417341777758437390,
  -0.961410947136866034,
};


/*****
  Purpose: Allows menu setting of Bode Start

  Parameter list:
    void

  Return value;
   long          the current start freq
*****/
long SetBodeStart() {
  int val;
  long lastfreqStartBode = freqStartBode;
  tft.setFontScale((enum RA8875tsize)1);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(SECONDARY_MENU_X + 1, MENUS_Y + 1);
  tft.print("Bode Start:");
  tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
  tft.print(freqStartBode);
  while (true) {
    if (filterEncoderMove != 0) {                     // Changed encoder?
      freqStartBode += filterEncoderMove * 1000000L;  // Yep
      lastfreqStartBode = freqStartBode;

      tft.setFontScale((enum RA8875tsize)1);
      tft.fillRect(SECONDARY_MENU_X + 200, MENUS_Y + 1, 50, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
      tft.print(lastfreqStartBode);
      filterEncoderMove = 0;
    }
    val = ReadSelectedPushButton();   // Read pin that controls all switches
    val = ProcessButtonPress(val);    ///////////////////////////////////////////////////
    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      freqStartBode = lastfreqStartBode;
      break;
    }
  }

  return freqStartBode;
}

/*****
  Purpose: Allows quick setting of Bode Stop Frequenct

  Parameter list:
    void

  Return value;
    long          the current start freq
*****/
long SetBodeStop() {
  int val;
  long lastfreqStopBode = freqStopBode;

  tft.setFontScale((enum RA8875tsize)1);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(SECONDARY_MENU_X + 1, MENUS_Y + 1);
  tft.print("Bode Stop:");
  tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
  tft.print(freqStopBode);

  while (true) {
    if (filterEncoderMove != 0) {                    // Changed encoder?
      freqStopBode += filterEncoderMove * 1000000L;  // Yep
      lastfreqStopBode = freqStopBode;
      tft.setFontScale((enum RA8875tsize)1);
      tft.fillRect(SECONDARY_MENU_X + 200, MENUS_Y + 1, 50, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 200, MENUS_Y + 1);
      tft.print(lastfreqStopBode);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();   // Read pin that controls all switches
    val = ProcessButtonPress(val);    ///////////////////////////////////////////////////
    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      freqStopBode = lastfreqStopBode;
      break;
    }
  }
  return freqStopBode;
}


/*****Purpose : Allows quick setting of Bode Stop Frequenct

                 Parameter list : void

*****/

long MoveStopFreqBode() {
  char result;

  result = fineTuneEncoder.process();  // Read the encoder
  if (result == 0) {                   // Nothing read
    fineTuneEncoderMove = 0L;
    return 0L;
  } else {
    if (result == DIR_CW) {  // 16 = CW, 32 = CCW
      fineTuneEncoderMove = 1L;
    } else {
      fineTuneEncoderMove = -1L;
    }
  }

  freqStopBode += 1000000L * fineTuneEncoderMove;

  return freqStopBode;
}

#endif // EXCLUDE_BODE