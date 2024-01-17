#ifndef BEENHERE
#include "SDT.h"
#endif

// Updates to DoReceiveCalibration() and DoXmitCalibrate() functions by KF5N.  July 20, 2023
// Updated PlotCalSpectrum() function to clean up graphics.  KF5N August 3, 2023
// Major clean-up of calibration.  KF5N August 16, 2023

int val;
int corrChange;
float correctionIncrement;  //AFP 2-7-23
int userScale, userZoomIndex, userXmtMode;
int transmitPowerLevelTemp;
/*****
  Purpose: Set up prior to IQ calibrations.  New function.  KF5N August 14, 2023
  These things need to be saved here and restored in the prologue function:
  Vertical scale in dB  (set to 10 dB during calibration)
  Zoom, set to 1X in receive and 4X in transmit calibrations.
  Transmitter power, set to 5W during both calibrations.
   Parameter List:
      int setZoom   (This parameter should be 0 for receive (1X) and 2 (4X) for transmit)

   Return value:
      void
 *****/
void CalibratePreamble(int setZoom) {
  calOnFlag = 1;
  corrChange = 0;
  correctionIncrement = 0.01;  //AFP 2-7-23
  IQCalType = 0;
  radioState = CW_TRANSMIT_STRAIGHT_STATE;      // KF5N
  transmitPowerLevelTemp = transmitPowerLevel;  //AFP 05-11-23
  transmitPowerLevel = 5;                       //AFP 02-09-23
  powerOutCW[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * CWPowerCalibrationFactor[currentBand];
  modeSelectOutExL.gain(0, powerOutCW[currentBand]);  //AFP 10-21-22
  modeSelectOutExR.gain(0, powerOutCW[currentBand]);  //AFP 10-21-22
  userXmtMode = xmtMode;          // Store the user's mode setting.  KF5N July 22, 2023
  userZoomIndex = spectrum_zoom;  // Save the zoom index so it can be reset at the conclusion.  KF5N August 12, 2023
  zoomIndex = setZoom - 1;
  ButtonZoom();
  tft.writeTo(L2);  // Erase the bandwidth bar.  KF5N August 16, 2023
  tft.clearMemory();
  tft.writeTo(L1);
  tft.setFontScale((enum RA8875tsize)0);
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(350, 160);
  tft.print("user1 - Gain/Phase");
  tft.setCursor(350, 175);
  tft.print("User2 - Incr");
  tft.setTextColor(RA8875_CYAN);
  tft.fillRect(350, 125, 100, tft.getFontHeight(), RA8875_BLACK);
  tft.fillRect(0, 272, 517, 399, RA8875_BLACK);  // Erase waterfall.  KF5N August 14, 2023
  tft.setCursor(400, 125);
  tft.print("dB");
  tft.setCursor(350, 110);
  tft.print("Incr= ");
  tft.setCursor(400, 110);
  tft.print(correctionIncrement, 3);
  userScale = currentScale;  //  Remember user preference so it can be reset when done.  KF5N
  currentScale = 1;          //  Set vertical scale to 10 dB during calibration.  KF5N
  updateDisplayFlag = 0;
  digitalWrite(MUTE, LOW);  //turn off mute
  xrState = RECEIVE_STATE;
  T41State = CW_RECEIVE;
  modeSelectInR.gain(0, 1);
  modeSelectInL.gain(0, 1);
  modeSelectInExR.gain(0, 0);
  modeSelectInExL.gain(0, 0);
  modeSelectOutL.gain(0, 1);
  modeSelectOutR.gain(0, 1);
  modeSelectOutL.gain(1, 0);
  modeSelectOutR.gain(1, 0);
  modeSelectOutExL.gain(0, 1);
  modeSelectOutExR.gain(0, 1);
  centerFreq = TxRxFreq;
  NCOFreq = 0L;
  xrState = TRANSMIT_STATE;
  digitalWrite(MUTE, HIGH);  //  Mute Audio  (HIGH=Mute)
  digitalWrite(RXTX, HIGH);  // Turn on transmitter.
  ShowTransmitReceiveStatus();
  ShowSpectrumdBScale();
}

/*****
  Purpose: Shut down and clean up after IQ calibrations.  New function.  KF5N August 14, 2023

   Parameter List:
      void

   Return value:
      void
 *****/
void CalibratePrologue() {
  digitalWrite(RXTX, LOW);  // Turn off the transmitter.
  updateDisplayFlag = 0;
  xrState = RECEIVE_STATE;
  ShowTransmitReceiveStatus();
  T41State = CW_RECEIVE;
  // Clear queues to reduce transient.
  Q_in_L.clear();
  Q_in_R.clear();
  centerFreq = TxRxFreq;
  NCOFreq = 0L;
  xrState = RECEIVE_STATE;
  calibrateFlag = 0;  // KF5N
  calFreqShift = 0;
  currentScale = userScale;                     //  Restore vertical scale to user preference.  KF5N
  ShowSpectrumdBScale();
  xmtMode = userXmtMode;   // Restore the user's floor setting.  KF5N July 27, 2023
  transmitPowerLevel = transmitPowerLevelTemp;  // Restore the user's transmit power level setting.  KF5N August 15, 2023
  EEPROMWrite();                                // Save calibration numbers and configuration.  KF5N August 12, 2023
  zoomIndex = userZoomIndex - 1;
  ButtonZoom();     // Restore the user's zoom setting.  Note that this function also modifies spectrum_zoom.
  EEPROMWrite();                                // Save calibration numbers and configuration.  KF5N August 12, 2023
  tft.writeTo(L2);  // Clear layer 2.  KF5N July 31, 2023
  tft.clearMemory();
  tft.writeTo(L1);  // Exit function in layer 1.  KF5N August 3, 2023
  RedrawDisplayScreen();
  IQChoice = 5;
  calOnFlag = 0;
  radioState = CW_RECEIVE_STATE;  // KF5N
  SetFreq();                      // Return Si5351 to normal operation mode.  KF5N
  lastState = 1111;  // This is required due to the function deactivating the receiver.  This forces a pass through the receiver set-up code.  KF5N October 16, 2023
  return;
}

/*****
  Purpose: Combined input/ output for the purpose of calibrating the receive IQ

   Parameter List:
      void

   Return value:
      void
 *****/
void DoReceiveCalibrate() {
  int task = -1;
  int lastUsedTask = -2;
  CalibratePreamble(0);                                                   // Set zoom to 1X.
  if (bands[currentBand].mode == DEMOD_LSB) calFreqShift = 24000 - 2000;  //  LSB offset.  KF5N
  if (bands[currentBand].mode == DEMOD_USB) calFreqShift = 24000 + 2250;  //  USB offset.  KF5N
  SetFreqCal();
  calTypeFlag = 0;  // RX cal
  // Receive calibration loop
  while (true) {
    ShowSpectrum2();
    val = ReadSelectedPushButton();
    if (val != BOGUS_PIN_READ) {
      val = ProcessButtonPress(val);
      if (val != lastUsedTask && task == -100) task = val;
      else task = BOGUS_PIN_READ;
    }
    switch (task) {
        // Toggle gain and phase
      case UNUSED_1:
        IQCalType = !IQCalType;
        break;
        // Toggle increment value
      case BEARING:  // UNUSED_2 is now called BEARING
        corrChange = !corrChange;
        if (corrChange == 1) {
          correctionIncrement = 0.001;  //AFP 2-7-23
        } else {                        //if (corrChange == 0)                   // corrChange is a toggle, so if not needed JJP 2/5/23
          correctionIncrement = 0.01;   //AFP 2-7-23
        }
        tft.setFontScale((enum RA8875tsize)0);
        tft.fillRect(400, 110, 50, tft.getFontHeight(), RA8875_BLACK);
        tft.setCursor(400, 110);
        tft.print(correctionIncrement, 3);
        break;
      case MENU_OPTION_SELECT:
        tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH + 35, CHAR_HEIGHT, RA8875_BLACK);
        EEPROMData.IQAmpCorrectionFactor[currentBand] = IQAmpCorrectionFactor[currentBand];
        EEPROMData.IQPhaseCorrectionFactor[currentBand] = IQPhaseCorrectionFactor[currentBand];
        IQChoice = 6;
        break;
      default:
        break;
    }  // End switch
    if (task != -1) lastUsedTask = task;  //  Save the last used task.
    task = -100;                          // Reset task after it is used.
    if (IQCalType == 0) {  // AFP 2-11-23
      IQAmpCorrectionFactor[currentBand] = GetEncoderValueLive(-2.0, 2.0, IQAmpCorrectionFactor[currentBand], correctionIncrement, (char *)"IQ Gain");
    } else {
      IQPhaseCorrectionFactor[currentBand] = GetEncoderValueLive(-2.0, 2.0, IQPhaseCorrectionFactor[currentBand], correctionIncrement, (char *)"IQ Phase");
    }
    if (IQChoice == 6) break;  // Exit the while loop.
  }                            // End while loop
  CalibratePrologue();
}

/*****
  Purpose: Combined input/ output for the purpose of calibrating the transmit IQ

   Parameter List:
      void

   Return value:
      void
 *****/
void DoXmitCalibrate() {
  int task = -1;
  int lastUsedTask = -2;
  CalibratePreamble(2);  // Set zoom to 4X.
  calTypeFlag = 1;       // TX cal
  calFreqShift = 750;
  SetFreqCal();
  tft.writeTo(L1);
  // Transmit Calibration Loop
  while (true) {
    ShowSpectrum2();
    val = ReadSelectedPushButton();
    if (val != BOGUS_PIN_READ) {
      val = ProcessButtonPress(val);
      if (val != lastUsedTask && task == -100) task = val;
      else task = BOGUS_PIN_READ;
    }
    switch (task) {
      // Toggle gain and phase
      case UNUSED_1:
        IQCalType = !IQCalType;
        break;
      // Toggle increment value
      case BEARING:  // UNUSED_2 is now called BEARING
        corrChange = !corrChange;
        if (corrChange == 1) {          // Toggle increment value
          correctionIncrement = 0.001;  // AFP 2-11-23
        } else {
          correctionIncrement = 0.01;  // AFP 2-11-23
        }
        tft.setFontScale((enum RA8875tsize)0);
        tft.fillRect(400, 110, 50, tft.getFontHeight(), RA8875_BLACK);
        tft.setCursor(400, 110);
        tft.print(correctionIncrement, 3);
        break;
      case (MENU_OPTION_SELECT):  // Save values and exit calibration.
        tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH + 35, CHAR_HEIGHT, RA8875_BLACK);
        EEPROMData.IQXAmpCorrectionFactor[currentBand] = IQAmpCorrectionFactor[currentBand];
        EEPROMData.IQXPhaseCorrectionFactor[currentBand] = IQPhaseCorrectionFactor[currentBand];
        IQChoice = 6;  // AFP 2-11-23
        break;
      default:
        break;
    }  // end switch
    if (task != -1) lastUsedTask = task;  //  Save the last used task.
    task = -100;                          // Reset task after it is used.
    //  Read encoder and update values.
    if (IQCalType == 0) {
      IQXAmpCorrectionFactor[currentBand] = GetEncoderValueLive(-2.0, 2.0, IQXAmpCorrectionFactor[currentBand], correctionIncrement, (char *)"IQ Gain X");
    } else {
      IQXPhaseCorrectionFactor[currentBand] = GetEncoderValueLive(-2.0, 2.0, IQXPhaseCorrectionFactor[currentBand], correctionIncrement, (char *)"IQ Phase X");
    }
    if (IQChoice == 6) break;  //  Exit the while loop.
  }                            // end while
  CalibratePrologue();
}


/*****
  Purpose: Signal processing for th purpose of calibrating the transmit IQ

   Parameter List:
      void

   Return value:
      void
 *****/
void ProcessIQData2() {
  float bandCouplingFactor[7] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };  // AFP 2-11-23  KF5N uniform values
  float bandOutputFactor;                                               // AFP 2-11-23
  float rfGainValue;                                                    // AFP 2-11-23
  float recBandFactor[7] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };       // AFP 2-11-23  KF5N uniform values
  /**********************************************************************************  AFP 12-31-20
        Get samples from queue buffers
        Teensy Audio Library stores ADC data in two buffers size=128, Q_in_L and Q_in_R as initiated from the audio lib.
        Then the buffers are read into two arrays sp_L and sp_R in blocks of 128 up to N_BLOCKS.  The arrarys are
        of size BUFFER_SIZE * N_BLOCKS.  BUFFER_SIZE is 128.
        N_BLOCKS = FFT_LENGTH / 2 / BUFFER_SIZE * (uint32_t)DF; // should be 16 with DF == 8 and FFT_LENGTH = 512
        BUFFER_SIZE*N_BLOCKS = 2024 samples
     **********************************************************************************/

  bandOutputFactor = bandCouplingFactor[currentBand] * CWPowerCalibrationFactor[currentBand] / CWPowerCalibrationFactor[1];  //AFP 2-7-23

  // Generate I and Q for the transmit or receive calibration.  KF5N
  if (IQChoice == 2 || IQChoice == 3) {                                   // KF5N
    arm_scale_f32(cosBuffer3, bandOutputFactor, float_buffer_L_EX, 256);  // AFP 2-11-23 Use pre-calculated sin & cos instead of Hilbert
    arm_scale_f32(sinBuffer3, bandOutputFactor, float_buffer_R_EX, 256);  // AFP 2-11-23 Sidetone = 3000
  }

  if (bands[currentBand].mode == DEMOD_LSB) {
    arm_scale_f32(float_buffer_L_EX, -IQXAmpCorrectionFactor[currentBand], float_buffer_L_EX, 256);       //Adjust level of L buffer // AFP 2-11-23
    IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, IQXPhaseCorrectionFactor[currentBand], 256);  // Adjust phase
  } else {
    if (bands[currentBand].mode == DEMOD_USB) {
      arm_scale_f32(float_buffer_L_EX, IQXAmpCorrectionFactor[currentBand], float_buffer_L_EX, 256);  // AFP 2-11-23
      IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, IQXPhaseCorrectionFactor[currentBand], 256);
    }
  }
  //24KHz effective sample rate here
  arm_fir_interpolate_f32(&FIR_int1_EX_I, float_buffer_L_EX, float_buffer_LTemp, 256);
  arm_fir_interpolate_f32(&FIR_int1_EX_Q, float_buffer_R_EX, float_buffer_RTemp, 256);

  // interpolation-by-4,  48KHz effective sample rate here
  arm_fir_interpolate_f32(&FIR_int2_EX_I, float_buffer_LTemp, float_buffer_L_EX, 512);
  arm_fir_interpolate_f32(&FIR_int2_EX_Q, float_buffer_RTemp, float_buffer_R_EX, 512);

  // are there at least N_BLOCKS buffers in each channel available ?
  if ((uint32_t)Q_in_L.available() > N_BLOCKS + 0 && (uint32_t)Q_in_R.available() > N_BLOCKS + 0) {

    // Revised I and Q calibration signal generation using large buffers.  Greg KF5N June 4 2023
    q15_t q15_buffer_LTemp[2048];  //KF5N
    q15_t q15_buffer_RTemp[2048];  //KF5N
    Q_out_L_Ex.setBehaviour(AudioPlayQueue::NON_STALLING);
    Q_out_R_Ex.setBehaviour(AudioPlayQueue::NON_STALLING);
    arm_float_to_q15(float_buffer_L_EX, q15_buffer_LTemp, 2048);
    arm_float_to_q15(float_buffer_R_EX, q15_buffer_RTemp, 2048);
    Q_out_L_Ex.play(q15_buffer_LTemp, 2048);
    Q_out_R_Ex.play(q15_buffer_RTemp, 2048);
    Q_out_L_Ex.setBehaviour(AudioPlayQueue::ORIGINAL);
    Q_out_R_Ex.setBehaviour(AudioPlayQueue::ORIGINAL);

    usec = 0;
    // get audio samples from the audio  buffers and convert them to float
    // read in 32 blocks á 128 samples in I and Q
    for (unsigned i = 0; i < N_BLOCKS; i++) {
      sp_L1 = Q_in_R.readBuffer();
      sp_R1 = Q_in_L.readBuffer();

      /**********************************************************************************  AFP 12-31-20
          Using arm_Math library, convert to float one buffer_size.
          Float_buffer samples are now standardized from > -1.0 to < 1.0
      **********************************************************************************/
      arm_q15_to_float(sp_L1, &float_buffer_L[BUFFER_SIZE * i], BUFFER_SIZE);  // convert int_buffer to float 32bit
      arm_q15_to_float(sp_R1, &float_buffer_R[BUFFER_SIZE * i], BUFFER_SIZE);  // convert int_buffer to float 32bit
      Q_in_L.freeBuffer();
      Q_in_R.freeBuffer();
    }

    rfGainValue = pow(10, (float)rfGainAllBands / 20);                                   //AFP 2-11-23
    arm_scale_f32(float_buffer_L, rfGainValue, float_buffer_L, BUFFER_SIZE * N_BLOCKS);  //AFP 2-11-23
    arm_scale_f32(float_buffer_R, rfGainValue, float_buffer_R, BUFFER_SIZE * N_BLOCKS);  //AFP 2-11-23

    /**********************************************************************************  AFP 12-31-20
      Scale the data buffers by the RFgain value defined in bands[currentBand] structure
    **********************************************************************************/
    arm_scale_f32(float_buffer_L, recBandFactor[currentBand], float_buffer_L, BUFFER_SIZE * N_BLOCKS);  //AFP 2-11-23
    arm_scale_f32(float_buffer_R, recBandFactor[currentBand], float_buffer_R, BUFFER_SIZE * N_BLOCKS);  //AFP 2-11-23

    // Manual IQ amplitude correction
    if (bands[currentBand].mode == DEMOD_LSB) {
      arm_scale_f32(float_buffer_L, -IQAmpCorrectionFactor[currentBand], float_buffer_L, BUFFER_SIZE * N_BLOCKS);  //AFP 04-14-22
      IQPhaseCorrection(float_buffer_L, float_buffer_R, IQPhaseCorrectionFactor[currentBand], BUFFER_SIZE * N_BLOCKS);
    } else {
      if (bands[currentBand].mode == DEMOD_USB) {
        arm_scale_f32(float_buffer_L, -IQAmpCorrectionFactor[currentBand], float_buffer_L, BUFFER_SIZE * N_BLOCKS);  //AFP 04-14-22 KF5N changed sign
        IQPhaseCorrection(float_buffer_L, float_buffer_R, IQPhaseCorrectionFactor[currentBand], BUFFER_SIZE * N_BLOCKS);
      }
    }
    FreqShift1();  // Why done here? KF5N

    if (spectrum_zoom == SPECTRUM_ZOOM_1) {  // && display_S_meter_or_spectrum_state == 1)
      zoom_display = 1;
      CalcZoom1Magn();  //AFP Moved to display function
    }

    if (spectrum_zoom != SPECTRUM_ZOOM_1) {
      //AFP  Used to process Zoom>1 for display
      ZoomFFTExe(BUFFER_SIZE * N_BLOCKS);  // there seems to be a BUG here, because the blocksize has to be adjusted according to magnification,
      // does not work for magnifications > 8
    }

    if (auto_codec_gain == 1) {
      Codec_gain();
    }
  }
}

/*****
  Purpose: Show Spectrum display modified for IQ calibration.
           This is similar to the function used for normal reception, however, it has
           been simplified and streamlined for calibration.

  Parameter list:
    void

  Return value;
    void
*****/
void ShowSpectrum2()  //AFP 2-10-23
{
  int x1 = 0;
  float adjdB = 0.0;
  int capture_bins = 10;  // Sets the number of bins to scan for signal peak.
  //=========== // AFP 2-11-23
//  tft.drawFastVLine(centerLine, SPECTRUM_TOP_Y, h + 14, RA8875_GREEN);  // Centerline not required for calibration.  KF5N August 16, 2023

  pixelnew[0] = 0;
  pixelnew[1] = 0;
  pixelold[0] = 0;
  pixelold[1] = 0;

  //  This is the "spectra scanning" for loop.  During calibration, only small areas of the spectrum need to be examined.
  //  If the entire 512 wide spectrum is used, the calibration loop will be slow and unresponsive.
  //  The scanning areas are determined by receive versus transmit calibration, and LSB or USB.  Thus there are 4 different scanning zones.
  //  All calibrations use a 0 dB reference signal and an "undesired sideband" signal which is to be minimized relative to the reference.
  //  Thus there is a target "bin" for the reference signal and another "bin" for the undesired sideband.
  //  The target bin locations are used by the for-loop to sweep a small range in the FFT.  A maximum finding function finds the peak signal strength.
  int cal_bins[2] = {0, 0};
  if (calTypeFlag == 0 && bands[currentBand].mode == DEMOD_LSB) {
    cal_bins[0] = 310;
    cal_bins[1] = 460;
  }  // Receive calibration, LSB.  KF5N
  if (calTypeFlag == 0 && bands[currentBand].mode == DEMOD_USB) {
    cal_bins[0] = 65;
    cal_bins[1] = 192;
  }  // Receive calibration, USB.  KF5N
  if (calTypeFlag == 1 && bands[currentBand].mode == DEMOD_LSB) {
    cal_bins[0] = 240;
    cal_bins[1] = 305;
  }  // Transmit calibration, LSB.  KF5N
  if (calTypeFlag == 1 && bands[currentBand].mode == DEMOD_USB) {
    cal_bins[0] = 209;
    cal_bins[1] = 273;
  }  // Transmit calibration, USB.  KF5N

  // Draw vertical markers for the reference and undesired sideband locations.  For debugging only!
  //  tft.drawFastVLine(cal_bins[0], SPECTRUM_TOP_Y, h, RA8875_GREEN);
  //  tft.drawFastVLine(cal_bins[1], SPECTRUM_TOP_Y, h, RA8875_GREEN);

  //  There are 2 for-loops, one for the reference signal and another for the undesired sideband.
  for (x1 = cal_bins[0] - capture_bins; x1 < cal_bins[0] + capture_bins; x1++) adjdB = PlotCalSpectrum(x1, cal_bins, capture_bins);
  for (x1 = cal_bins[1] - capture_bins; x1 < cal_bins[1] + capture_bins; x1++) adjdB = PlotCalSpectrum(x1, cal_bins, capture_bins);
  // Plot carrier during transmit cal, do not return a dB value:
  if (calTypeFlag == 1)
    for (x1 = cal_bins[0] + 20; x1 < cal_bins[1] - 20; x1++) PlotCalSpectrum(x1, cal_bins, capture_bins);

  // Finish up:
  //= AFP 2-11-23
  tft.fillRect(350, 125, 50, tft.getFontHeight(), RA8875_BLACK);
  tft.setCursor(350, 125);  // 350, 125
  tft.print(adjdB, 1);

  //  At least a partial waterfall is necessary.  It seems to provide some important timing function.  KF5N August 14, 2023
  tft.BTE_move(WATERFALL_LEFT_X, FIRST_WATERFALL_LINE, MAX_WATERFALL_WIDTH, MAX_WATERFALL_ROWS - 2, WATERFALL_LEFT_X, FIRST_WATERFALL_LINE + 1, 1, 2);
  while (tft.readStatus())
    ;
}

/*****
  Purpose:  Plot Calibration Spectrum   //  KF5N 7/2/2023
            This function plots a partial spectrum during calibration only.
            This is intended to increase the efficiency and therefore the responsiveness of the calibration encoder.
            This function is called by ShowSpectrum2() in two for-loops.  One for-loop is for the refenence signal,
            and the other for-loop is for the undesired sideband.
  Parameter list:
    int x1, where x1 is the FFT bin.
    cal_bins[2] locations of the desired and undesired signals
    capture_bins width of the bins used to display the signals
  Return value;
    float returns the adjusted value in dB
*****/
float PlotCalSpectrum(int x1, int cal_bins[2], int capture_bins) {
  float adjdB = 0.0;
  int16_t adjAmplitude = 0;  // Was float; cast to float in dB calculation.  KF5N
  int16_t refAmplitude = 0;  // Was float; cast to float in dB calculation.  KF5N
  uint32_t index_of_max;     // This variable is not currently used, but it is required by the ARM max function.  KF5N

  if (x1 == (cal_bins[0] - capture_bins)) {  // Set flag at revised beginning.  KF5N
    updateDisplayFlag = 1;                   //Set flag so the display data are saved only once during each display refresh cycle at the start of the cycle, not 512 times
    ShowBandwidth();                         // Without this call, the calibration value in dB will not be updated.  KF5N
  } else updateDisplayFlag = 0;              //  Do not save the the display data for the remainder of the

  ProcessIQData2();  // Call the Audio process from within the display routine to eliminate conflicts with drawing the spectrum and waterfall displays

  y_new = pixelnew[x1];
  y1_new = pixelnew[x1 - 1];
  y_old = pixelold[x1];
  y_old2 = pixelold[x1 - 1];

  // Find the maximums of the desired and undesired signals.
  if (bands[currentBand].mode == DEMOD_LSB) {
    arm_max_q15(&pixelnew[(cal_bins[0] - capture_bins)], capture_bins * 2, &refAmplitude, &index_of_max);
    arm_max_q15(&pixelnew[(cal_bins[1] - capture_bins)], capture_bins * 2, &adjAmplitude, &index_of_max);
  }
  if (bands[currentBand].mode == DEMOD_USB) {
    arm_max_q15(&pixelnew[(cal_bins[0] - capture_bins)], capture_bins * 2, &adjAmplitude, &index_of_max);
    arm_max_q15(&pixelnew[(cal_bins[1] - capture_bins)], capture_bins * 2, &refAmplitude, &index_of_max);
  }

  //=== // AFP 2-11-23
  if (y_new > base_y) y_new = base_y;
  if (y_old > base_y) y_old = base_y;
  if (y_old2 > base_y) y_old2 = base_y;
  if (y1_new > base_y) y1_new = base_y;

  if (y_new < 0) y_new = 0;
  if (y_old < 0) y_old = 0;
  if (y_old2 < 0) y_old2 = 0;
  if (y1_new < 0) y1_new = 0;

  // Erase the old spectrum and draw the new spectrum.
  tft.drawLine(x1, spectrumNoiseFloor - y_old2, x1, spectrumNoiseFloor - y_old, RA8875_BLACK);   // Erase old...
  tft.drawLine(x1, spectrumNoiseFloor - y1_new, x1, spectrumNoiseFloor - y_new, RA8875_YELLOW);  // Draw new
  pixelCurrent[x1] = pixelnew[x1];                                                               //  This is the actual "old" spectrum!  This is required due to CW interrupts.  Copied to pixelold by the FFT function.

  if (calTypeFlag == 0) {  // Receive Cal
    adjdB = ((float)adjAmplitude - (float)refAmplitude) / 1.95;
    tft.writeTo(L2);
    if (bands[currentBand].mode == DEMOD_LSB) {
      tft.fillRect(450, SPECTRUM_TOP_Y + 20, 20, h - 6, DARK_RED);     // SPECTRUM_TOP_Y = 100
      tft.fillRect(300, SPECTRUM_TOP_Y + 20, 20, h - 6, RA8875_BLUE);  // h = SPECTRUM_HEIGHT + 3
    } else {                                                           // SPECTRUM_HEIGHT = 150 so h = 153
      tft.fillRect(55, SPECTRUM_TOP_Y + 20, 20, h - 6, DARK_RED);
      tft.fillRect(182, SPECTRUM_TOP_Y + 20, 20, h - 6, RA8875_BLUE);
    }
  } else {                                                       //Transmit Cal
    adjdB = ((float)adjAmplitude - (float)refAmplitude) / 1.95;  // Cast to float and calculate the dB level.  KF5N
    tft.writeTo(L2);
    if (bands[currentBand].mode == DEMOD_LSB) {
      tft.fillRect(295, SPECTRUM_TOP_Y + 20, 20, h - 6, DARK_RED);  // Adjusted height due to other graphics changes.  KF5N August 3, 2023
      tft.fillRect(230, SPECTRUM_TOP_Y + 20, 20, h - 6, RA8875_BLUE);      
    } else {
      if (bands[currentBand].mode == DEMOD_USB) {  //mode == DEMOD_LSB
        tft.fillRect(199, SPECTRUM_TOP_Y + 20, 20, h - 6, DARK_RED);
        tft.fillRect(263, SPECTRUM_TOP_Y + 20, 20, h - 6, RA8875_BLUE);
      }
    }
  }
  tft.writeTo(L1);
  return adjdB;
}
