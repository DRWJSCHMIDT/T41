#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: Create I and Q signals from Mic input

  Parameter list:

  Return value;
    void
    Notes:
    There are several actions in this function
    1.  Read in the data from the ADC into the Left Channel at 192KHz
    2.  Format the L data and Decimate (downsample and filter)the sampled data by x8
          - the new effective sampling rate is now 24KHz
    3.  Process the L data through the 7 EQ filters and combine to a single data stream
    4.  Copy the L channel to the R channel
    5.  Process the R and L through two Hilbert Transformers - L 0deg phase shift and R 90 deg ph shift
          - This create the I (L) and Q(R) channels
    6.  Interpolate 8x (upsample and filter) the data stream to 192KHz sample rate
    7.  Output the data stream thruogh the DACs at 192KHz
*****/
void ExciterIQData()
{
  //  int transmitEQ_Flag = 0;
  uint32_t N_BLOCKS_EX                         = N_B_EX;

  /**********************************************************************************  AFP 12-31-20
        Get samples from queue buffers
        Teensy Audio Library stores ADC data in two buffers size=128, Q_in_L and Q_in_R as initiated from the audio lib.
        Then the buffers are  read into two arrays sp_L and sp_R in blocks of 128 up to N_BLOCKS.  The arrarys are
        of size BUFFER_SIZE*N_BLOCKS.  BUFFER_SIZE is 128.
        N_BLOCKS = FFT_L / 2 / BUFFER_SIZE * (uint32_t)DF; // should be 16 with DF == 8 and FFT_L = 512
        BUFFER_SIZE*N_BLOCKS = 2024 samples
     **********************************************************************************/
  // are there at least N_BLOCKS buffers in each channel available ?
  if ( (uint32_t) Q_in_L.available() > N_BLOCKS_EX + 0 && (uint32_t) Q_in_R.available() > N_BLOCKS_EX + 0 ) {

    // get audio samples from the audio  buffers and convert them to float
    // read in 32 blocks á 128 samples in I and Q
    for (unsigned i = 0; i < N_BLOCKS_EX; i++) {
      sp_L = Q_in_L.readBuffer();
      sp_R = Q_in_R.readBuffer();

      /**********************************************************************************  AFP 12-31-20
          Using arm_Math library, convert to float one buffer_size.
          Float_buffer samples are now standardized from > -1.0 to < 1.0
      **********************************************************************************/
      arm_q15_to_float (sp_L, &float_buffer_L_EX[BUFFER_SIZE * i], BUFFER_SIZE); // convert int_buffer to float 32bit
      arm_q15_to_float (sp_R, &float_buffer_R_EX[BUFFER_SIZE * i], BUFFER_SIZE); // convert int_buffer to float 32bit
      Q_in_L.freeBuffer();
      Q_in_R.freeBuffer();
    }

    //AFP 12-24-21
    float exciteMaxL = 0;

    /**********************************************************************************  AFP 12-31-20
              Decimation is the process of downsampling the data stream and LP filtering
              Decimation is done in two stages to prevent reversal of the spectrum, which occure with each even
              Decimation.  First select every 4th asmple and then every 2nd sample, yielding 8x downsampling
              192KHz/8 = 24KHz, with 8xsmaller sample sizes
     **********************************************************************************/

    //192KHz effective sample rate here
    // decimation-by-4 in-place!
    arm_fir_decimate_f32(&FIR_dec1_EX_I, float_buffer_L_EX, float_buffer_L_EX, BUFFER_SIZE * N_BLOCKS_EX );
    arm_fir_decimate_f32(&FIR_dec1_EX_Q, float_buffer_R_EX, float_buffer_R_EX, BUFFER_SIZE * N_BLOCKS_EX );
    //48KHz effective sample rate here
    // decimation-by-2 in-place
    arm_fir_decimate_f32(&FIR_dec2_EX_I, float_buffer_L_EX, float_buffer_L_EX, 512);
    arm_fir_decimate_f32(&FIR_dec2_EX_Q, float_buffer_R_EX, float_buffer_R_EX, 512);

    if (xmtMode == CW_MODE) {
//      int numCycles;
//      numCycles = 7;
//      sineTone(numCycles);
      arm_copy_f32(sinBuffer2, float_buffer_L_EX, FFT_length / 2);
      arm_copy_f32(sinBuffer2, float_buffer_R_EX, FFT_length / 2);
      arm_scale_f32 (float_buffer_L_EX, .1, float_buffer_L_EX, FFT_length / 2);
      arm_scale_f32 (float_buffer_R_EX, .1, float_buffer_R_EX, FFT_length / 2);

      if (digitalRead(KEYER_DIT_INPUT_TIP) == LOW || digitalRead(KEYER_DAH_INPUT_RING) == LOW) {
        arm_scale_f32 (float_buffer_L_EX, 1, float_buffer_L_EX, FFT_length / 2);
        arm_scale_f32 (float_buffer_R_EX, 1, float_buffer_R_EX, FFT_length / 2);
      } else if (digitalRead(KEYER_DIT_INPUT_TIP) == HIGH || digitalRead(KEYER_DAH_INPUT_RING) == HIGH) {
        arm_scale_f32 (float_buffer_L_EX, 0, float_buffer_L_EX, FFT_length / 2);
        arm_scale_f32 (float_buffer_R_EX, 0, float_buffer_R_EX, FFT_length / 2);
      }
      arm_copy_f32 (float_buffer_L_EX, float_buffer_R_EX, 256);
    } else if(xmtMode == SSB_MODE){
      arm_copy_f32 (float_buffer_L_EX, float_buffer_R_EX, 256);
    }

    //--------------  Hilbert Transformers
    //arm_copy_f32 (float_buffer_L_EX, float_buffer_R_EX, 256);


    /**********************************************************************************
             R and L channels are processed though the two Hilbert Transformers, L at 0 deg and R at 90 deg
             Tthe result are the quadrature data streans, I and Q necessary for Phasing calculations to
             create the SSB signals.
             Two Hilbert Transformers are used to preserve eliminate the relative time delays created during processing of the data
    **********************************************************************************/
    arm_fir_f32(&FIR_Hilbert_L, float_buffer_L_EX, float_buffer_L_EX, 256);
    arm_fir_f32(&FIR_Hilbert_R, float_buffer_R_EX, float_buffer_R_EX, 256);

    /**********************************************************************************
              Additional scaling, if nesessary to compensate for down-stream gain variations
     **********************************************************************************/
    if (bands[currentBand].mode == DEMOD_LSB) { //AFP 12-27-21
      //arm_scale_f32 (float_buffer_L_EX, IQ_Xamplitude_correction_factor, float_buffer_L_EX, 256);
      //arm_scale_f32 (float_buffer_L_EX, -1.012, float_buffer_L_EX, 256);
      arm_scale_f32 (float_buffer_L_EX, -1.015, float_buffer_L_EX, 256);
      //IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, IQ_Xphase_correction_factor, 256);
      IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, .013, 256);
      //IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, 0, 256);
    }
    else if (bands[currentBand].mode == DEMOD_USB) { //AFP 12-27-21
      //arm_scale_f32 (float_buffer_L_EX, -1.005, float_buffer_L_EX, 256);
      //arm_scale_f32 (float_buffer_L_EX, -.965, float_buffer_L_EX, 256);
      arm_scale_f32 (float_buffer_L_EX, -.965, float_buffer_L_EX, 256);
      IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, -0.12, 256);
    }
    arm_scale_f32 (float_buffer_R_EX, 1.00, float_buffer_R_EX, 256);

    exciteMaxL = 0;
    for (int k = 0; k < 256; k++) {
      if (float_buffer_L_EX[k] > exciteMaxL) {
        exciteMaxL = float_buffer_L_EX[k];
      }
    }
    /**********************************************************************************
              Interpolate (upsample the data streams by 8X to create the 192KHx sample rate for output
              Requires a LPF FIR 48 tap 10KHz and 8KHz
     **********************************************************************************/
    //24KHz effective sample rate here
    arm_fir_interpolate_f32(&FIR_int1_EX_I, float_buffer_L_EX, float_buffer_LTemp, 256);
    arm_fir_interpolate_f32(&FIR_int1_EX_Q, float_buffer_R_EX, float_buffer_RTemp, 256);

    // interpolation-by-4,  48KHz effective sample rate here
    arm_fir_interpolate_f32(&FIR_int2_EX_I, float_buffer_LTemp, float_buffer_L_EX, 512);
    arm_fir_interpolate_f32(&FIR_int2_EX_Q, float_buffer_RTemp, float_buffer_R_EX, 512);
    //  192KHz effective sample rate here
    arm_scale_f32(float_buffer_L_EX, 20, float_buffer_L_EX, 2048); //Scale to compensate for losses in Interpolation
    arm_scale_f32(float_buffer_R_EX, 20, float_buffer_R_EX, 2048);


    /**********************************************************************************  AFP 12-31-20
      CONVERT TO INTEGER AND PLAY AUDIO
    **********************************************************************************/
    for (unsigned  i = 0; i < N_BLOCKS_EX; i++) {
      sp_L = Q_out_L.getBuffer();
      sp_R = Q_out_R.getBuffer();
      arm_float_to_q15 (&float_buffer_L_EX[BUFFER_SIZE * i], sp_L, BUFFER_SIZE);
      arm_float_to_q15 (&float_buffer_R_EX[BUFFER_SIZE * i], sp_R, BUFFER_SIZE);
      Q_out_L.playBuffer(); // play it !
      Q_out_R.playBuffer(); // play it !
    }
  }
}

/*****
  Purpose: Set the current band relay ON or OFF

  Parameter list:
    int state             OFF = 0, ON = 1

  Return value;
    void

*****/
void SetBandRelay(int state)
{
  digitalWrite(bandswitchPins[currentBand], state); //Set current band relay "on" AFP 12-16-21
  digitalWrite(RXTX, state);                        //Set Tx/Rx to HIGH = Rx on    AFP 12-16-21
}
