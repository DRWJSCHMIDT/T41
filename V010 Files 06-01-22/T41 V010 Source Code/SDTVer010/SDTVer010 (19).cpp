#ifndef BEENHERE
#include "SDT.h"
#endif

char atom, currentAtom;

/*****
  Purpose: Read audio from Teensy Audio Library
             Calculate FFT for display
             Process audio into SSB signalF
             Output audio to amplifier
             
   Parameter List:
      void

   Return value:
      void

   CAUTION: Assumes a spaces[] array is defined
 *****/
void ProcessIQData() 
{
  /**********************************************************************************  AFP 12-31-20
        Get samples from queue buffers
        Teensy Audio Library stores ADC data in two buffers size=128, Q_in_L and Q_in_R as initiated from the audio lib.
        Then the buffers are read into two arrays sp_L and sp_R in blocks of 128 up to N_BLOCKS.  The arrarys are
        of size BUFFER_SIZE * N_BLOCKS.  BUFFER_SIZE is 128.
        N_BLOCKS = FFT_LENGTH / 2 / BUFFER_SIZE * (uint32_t)DF; // should be 16 with DF == 8 and FFT_LENGTH = 512
        BUFFER_SIZE*N_BLOCKS = 2024 samples
     **********************************************************************************/
  int audioTemp  = 0;

  float goertzelMagnitude1;
  float goertzelMagnitude2;

  // are there at least N_BLOCKS buffers in each channel available ?
  if ( (uint32_t) Q_in_L.available() > N_BLOCKS + 0 && (uint32_t) Q_in_R.available() > N_BLOCKS + 0 ) {
    usec = 0;
    // get audio samples from the audio  buffers and convert them to float
    // read in 32 blocks á 128 samples in I and Q
    for (unsigned i = 0; i < N_BLOCKS; i++) {
      //sp_L = Q_in_L.readBuffer();
      //sp_R = Q_in_R.readBuffer();
      sp_L = Q_in_R.readBuffer();
      sp_R = Q_in_L.readBuffer();

      /**********************************************************************************  AFP 12-31-20
          Using arm_Math library, convert to float one buffer_size.
          Float_buffer samples are now standardized from > -1.0 to < 1.0
      **********************************************************************************/
      arm_q15_to_float (sp_L, &float_buffer_L[BUFFER_SIZE * i], BUFFER_SIZE); // convert int_buffer to float 32bit
      arm_q15_to_float (sp_R, &float_buffer_R[BUFFER_SIZE * i], BUFFER_SIZE); // convert int_buffer to float 32bit
      Q_in_L.freeBuffer();
      Q_in_R.freeBuffer();
      //     blocks_read ++;
    }

    /**********************************************************************************  AFP 12-31-20
        Remove DC offset to reduce centeral spike.  First read the Mean value of
        left and right channels.  Then fill L and R correction arrays with those Means
        and subtract the Means from the float L and R buffer data arrays.  Again use Arm_Math functions
        to manipulate the arrays.  Arrays are all BUFFER_SIZE * N_BLOCKS long
    **********************************************************************************/
    //
    arm_mean_f32(float_buffer_L, BUFFER_SIZE * N_BLOCKS, &sample_meanL);
    arm_mean_f32(float_buffer_R, BUFFER_SIZE * N_BLOCKS, &sample_meanR);

    for (uint32_t j = 0; j < BUFFER_SIZE * N_BLOCKS  ; j++) {
      L_BufferOffset [j] = -sample_meanL;
      R_BufferOffset [j] = -sample_meanR;
    }
    arm_add_f32(float_buffer_L , L_BufferOffset, float_buffer_L2 , BUFFER_SIZE * N_BLOCKS ) ;
    arm_add_f32(float_buffer_R , R_BufferOffset, float_buffer_R2 , BUFFER_SIZE * N_BLOCKS ) ;

    /**********************************************************************************  AFP 12-31-20
        Scale the data buffers by the RFgain value defined in bands[currentBand] structure
    **********************************************************************************/
    arm_scale_f32 (float_buffer_L2, bands[currentBand].RFgain + 1.0, float_buffer_L, BUFFER_SIZE * N_BLOCKS);
    arm_scale_f32 (float_buffer_R2, bands[currentBand].RFgain + 1.0, float_buffer_R, BUFFER_SIZE * N_BLOCKS);

    /**********************************************************************************  AFP 12-31-20
      Clear Buffers
      This is to prevent overfilled queue buffers during each switching event
      (band change, mode change, frequency change, the audio chain runs and fills the buffers
      if the buffers are full, the Teensy needs much more time
      in that case, we clear the buffers to keep the whole audio chain running smoothly
      **********************************************************************************/
    if (Q_in_L.available() >  25) {
      AudioNoInterrupts();
      Q_in_L.clear();
      n_clear ++; // just for debugging to check how often this occurs
      AudioInterrupts();
    }
    if (Q_in_R.available() >  25) {
      AudioNoInterrupts();
      Q_in_R.clear();
      n_clear ++; // just for debugging to check how often this occurs
      AudioInterrupts();
    }
    /**********************************************************************************  AFP 12-31-20
      IQ amplitude and phase correction.  For this scaled down version the I an Q chnnels are
      equalized and phase corrected manually. This is done by applying a correction, which is the difference, to
      the L channel only.  The phase is corrected in the IQPhaseCorrection() function.

      IQ amplitude and phase correction
    ***********************************************************************************************/


    // Manual IQ amplitude correction
    // to be honest: we only correct the amplitude of the I channel ;-)
    arm_scale_f32 (float_buffer_L, -IQ_amplitude_correction_factor, float_buffer_L, BUFFER_SIZE * N_BLOCKS); //AFP 04-14-22

    // IQ phase correction
    IQPhaseCorrection(float_buffer_L, float_buffer_R, IQ_phase_correction_factor, BUFFER_SIZE * N_BLOCKS);

    /**********************************************************************************  AFP 12-31-20
        Perform a 256 point FFT for the spectrum display on the basis of the first 256 complex values
        of the raw IQ input data this saves about 3% of processor power compared to calculating
        the magnitudes and means of the 4096 point FFT for the display

        Only go there from here, if magnification == 1
     ***********************************************************************************************/
    if (spectrum_zoom == SPECTRUM_ZOOM_1) { // && display_S_meter_or_spectrum_state == 1)
      zoom_display = 1;
      CalcZoom1Magn();  //AFP Moved to display function
    }
    display_S_meter_or_spectrum_state++;

    /**********************************************************************************  AFP 12-31-20
        Frequency translation by Fs/4 without multiplication from Lyons (2011): chapter 13.1.2 page 646
        together with the savings of not having to shift/rotate the FFT_buffer, this saves
        about 1% of processor use

        This is for +Fs/4 [moves receive frequency to the left in the spectrum display]
           float_buffer_L contains I = real values
           float_buffer_R contains Q = imaginary values
           xnew(0) =  xreal(0) + jximag(0)
               leave first value (DC component) as it is!
           xnew(1) =  - ximag(1) + jxreal(1)
    **********************************************************************************/
    FreqShift1();

    /**********************************************************************************  AFP 12-31-20
        SPECTRUM_ZOOM_2 and larger here after frequency conversion!
        Spectrum zoom displays a magnified display of the data around the translated receive frequency.
        Processing is done in the ZoomFFTExe(BUFFER_SIZE * N_BLOCKS) function.  For magnifications of 2x to 8X
        Larger magnification are not needed in practice.

        Spectrum Zoom uses the shifted spectrum, so the center "hump" around DC is shifted by fs/4
    **********************************************************************************/
    if (spectrum_zoom != SPECTRUM_ZOOM_1) {
      //AFP  Used to process Zoom>1 for display
      ZoomFFTExe(BUFFER_SIZE * N_BLOCKS); // there seems to be a BUG here, because the blocksize has to be adjusted according to magnification,
      // does not work for magnifications > 8
    }

    if (zoom_display) {
      if (show_spectrum_flag) {
        //ShowSpectrum();//AFP
      }
      zoom_display = 1;
      //zoom_sample_ptr = 0;
    }
    /**********************************************************************************  AFP 12-31-20
        S-Meter & dBm-display ?? not usually called
     **********************************************************************************/

    if (display_S_meter_or_spectrum_state == 2) {
      Calculatedbm();
    } else if (display_S_meter_or_spectrum_state == 4) {
      DisplaydbM();
      display_S_meter_or_spectrum_state = 0;
    } else if (display_S_meter_or_spectrum_state == 3) {
      // ??
    }


    /*************************************************************************************************
        freq_conv2()

        FREQUENCY CONVERSION USING A SOFTWARE QUADRATURE OSCILLATOR
        Creates a new IF frequency to allow the tuning window to be moved anywhere in the current display.
        THIS VERSION calculates the COS AND SIN WAVE on the fly - uses double precision float

        MAJOR ADVANTAGE: frequency conversion can be done for any frequency !

        large parts of the code taken from the mcHF code by Clint, KA7OEI, thank you!
          see here for more info on quadrature oscillators:
        Wheatley, M. (2011): CuteSDR Technical Manual Ver. 1.01. - http://sourceforge.net/projects/cutesdr/
        Lyons, R.G. (2011): Understanding Digital Processing. – Pearson, 3rd edition.
     *************************************************************************************************/

    FreqShift2();  //AFP 12-14-21


    /**********************************************************************************  AFP 12-31-20
        Decimation
        Resample (Decimate) the shifted time signal, first by 4, then by 2.  Each time the
        signal is decimated by an even number, the spectrum is reversed.  Resampling twice
        returns the spectrum to the correct orientation.
        Signal has now been shifted to base band, leaving aliases at higher frequencies,
        which are removed at each decimation step using the Arm combined decimate/filter function.
        If the statring sample rate is 192K SPS after the combined decimation, the sample rate is
        now 192K/8 = 24K SPS.  The array size is also reduced by 8, making FFT calculations much faster.
        The effective bandwidth (up to Nyquist frequency) is 12KHz.
     **********************************************************************************/
                                                                                              // decimation-by-4 in-place!
    arm_fir_decimate_f32(&FIR_dec1_I, float_buffer_L, float_buffer_L, BUFFER_SIZE * N_BLOCKS);
    arm_fir_decimate_f32(&FIR_dec1_Q, float_buffer_R, float_buffer_R, BUFFER_SIZE * N_BLOCKS);

                                                                                              // decimation-by-2 in-place
    arm_fir_decimate_f32(&FIR_dec2_I, float_buffer_L, float_buffer_L, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1);
    arm_fir_decimate_f32(&FIR_dec2_Q, float_buffer_R, float_buffer_R, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1);


    /**********************************************************************************  AFP 12-31-20
        Digital FFT convolution
        Filtering is accomplished by combinig (multiplying) spectra in the frequency domain.
         basis for this was Lyons, R. (2011): Understanding Digital Processing.
         "Fast FIR Filtering using the FFT", pages 688 - 694.
         Method used here: overlap-and-save.

        First, Create Complex time signal for CFFT routine.
        Fill first block with Zeros
        Then interleave RE and IM parts to create signal for FFT
     **********************************************************************************/
    // Prepare the audio signal buffers:

    //------------------------------ ONLY FOR the VERY FIRST FFT: fill first samples with zeros

    if (first_block) { // fill real & imaginaries with zeros for the first BLOCKSIZE samples
      for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF / 2.0); i++) {
        FFT_buffer[i] = 0.0;
      }
      first_block = 0;
    } else  // All other FFTs

      //------------------------------ fill FFT_buffer with last events audio samples for all other FFT instances
      for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF); i++) {
        FFT_buffer[i * 2] = last_sample_buffer_L[i]; // real
        FFT_buffer[i * 2 + 1] = last_sample_buffer_R[i]; // imaginary
      }

    for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF); i++) {   // copy recent samples to last_sample_buffer for next time!
      last_sample_buffer_L [i] = float_buffer_L[i];
      last_sample_buffer_R [i] = float_buffer_R[i];
    }

    //------------------------------ now fill recent audio samples into FFT_buffer (left channel: re, right channel: im)
    for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF); i++) {
      FFT_buffer[FFT_length + i * 2] = float_buffer_L[i]; // real
      FFT_buffer[FFT_length + i * 2 + 1] = float_buffer_R[i]; // imaginary
    }

    /**********************************************************************************  AFP 12-31-20
       Perform complex FFT on the audio time signals
       calculation is performed in-place the FFT_buffer [re, im, re, im, re, im . . .]
     **********************************************************************************/
    arm_cfft_f32(S, FFT_buffer, 0, 1);

    if (autotune_flag != 0) {                  // AUTOTUNE, slow down process in order for Si5351 to settle
      if (autotune_counter < autotune_wait)
        autotune_counter++;
      else {
        autotune_counter = 0;
        Autotune();
      }
    }

    /**********************************************************************************  AFP 12-31-20
      Continuing FFT Convolution
          Next, prepare the filter mask (done in the Filter.cpp file).  Only need to do this once for each filter setting.
          Allows efficient real-time variable LP and HP audio filters, without the overhead of time-domain convolution filtering.

          After the Filter mask in the frequency domain is created, complex multiply  filter mask with the frequency domain audio data.
          Filter mask previously calculated in setup Array of filter mask coefficients:
          FIR_filter_mask[]
     **********************************************************************************/

    arm_cmplx_mult_cmplx_f32 (FFT_buffer, FIR_filter_mask, iFFT_buffer, FFT_length);

    if (updateDisplayFlag == 1) {
      for (int k = 0; k < 1024; k++) {
        audioSpectBuffer[1024 - k] = (iFFT_buffer[k] * iFFT_buffer[k]);
      }
      for (int k = 0; k < 256; k++) {
        if (bands[currentBand].mode == 0) {
          audioYPixel[k] = 70 + map((int)displayScale[currentScale].dBScale * log10f((audioSpectBuffer[1024 - k] + audioSpectBuffer[1024 - k + 1] + audioSpectBuffer[1024 - k + 2]) / 3), 0, 100, 0, 120);
        }
        else if (bands[currentBand].mode == 1) {
          audioYPixel[k] = 70 + map((int)displayScale[currentScale].dBScale * log10f((audioSpectBuffer[k] + audioSpectBuffer[k + 1] + audioSpectBuffer[k + 2]) / 3), 0, 100, 0, 120);
        }

        if (audioYPixel[k] < 0)
          audioYPixel[k] = 0;
      }
    }

    /**********************************************************************************
          Additional Convolution Processes:
              // filter by just deleting bins - principle of Linrad
      only works properly when we have the right window function!

        (automatic) notch filter = Tone killer --> the name is stolen from SNR ;-)
        first test, we set a notch filter at 1kHz
        which bin is that?
        positive & negative frequency -1kHz and +1kHz --> delete 2 bins
        we are not deleting one bin, but five bins for the test
        1024 bins in 12ksps = 11.71Hz per bin
        SR[SampleRate].rate / 8.0 / 1024 = bin BW
        1000Hz / 11.71Hz = bin 85.333

     **********************************************************************************/

    /**********************************************************************************  AFP 12-31-20
      After the frequency domain filter mask and other processes are complete, do a
      complex inverse FFT to return to the time domain
        (if sample rate = 192kHz, we are in 24ksps now, because we decimated by 8)
        perform iFFT (in-place)  IFFT is selected by the IFFT flag=1 in the Arm CFFT function.
     **********************************************************************************/

    arm_cfft_f32(iS, iFFT_buffer, 1, 1);

    /**********************************************************************************  AFP 12-31-20
        AGC - automatic gain control

        we´re back in time domain
        AGC acts upon I & Q before demodulation on the decimated audio data in iFFT_buffer
     **********************************************************************************/
    AGC();  //AGC function works with time domain I and Q data buffers created in the last step

    //============================  Demod  ========================

    /**********************************************************************************
          Demodulation
            our time domain output is a combination of the real part (left channel) AND the imaginary part (right channel) of the second half of the FFT_buffer
            The demod mode is accomplished by selecting/combining the real and imaginary parts of the output of the IFFT process.
       **********************************************************************************/

    if (bands[currentBand].mode == DEMOD_SAM ) {
      AMDecodeSAM();                                      //Synchronous AM Detection
    } else if (bands[currentBand].mode == DEMOD_IQ) {
      DecodeIQ();                                         //Output I and Q channels together
    } else if (bands[currentBand].mode == DEMOD_AM2) {
      AMDemodAM2();                                       //AM detection
    } else if (bands[currentBand].mode == DEMOD_AM_ME2) {
      for (unsigned i = 0; i < FFT_length / 2; i++) {     // Magnitude estimation Lyons (2011): page 652 / libcsdr
        audiotmp = AlphaBetaMag(iFFT_buffer[FFT_length + (i * 2)], iFFT_buffer[FFT_length + (i * 2) + 1]);
        // DC removal filter -----------------------
        w = audiotmp + wold * 0.9999f; // yes, I want a superb bass response ;-)
        float_buffer_L[i] = w - wold;
        wold = w;
      }
      arm_biquad_cascade_df1_f32 (&biquad_lowpass1, float_buffer_L, float_buffer_R, FFT_length / 2);
      arm_copy_f32(float_buffer_R, float_buffer_L, FFT_length / 2);
    } else {
      for (unsigned i = 0; i < FFT_length / 2; i++) {
        if (bands[currentBand].mode == DEMOD_USB || bands[currentBand].mode == DEMOD_LSB ) {  // for SSB copy real part in both outputs
          float_buffer_L[i] = iFFT_buffer[FFT_length + (i * 2)];

          float_buffer_R[i] = float_buffer_L[i];
        }
      }
    }
    /**********************************************************************************
      Noise Reduction
      3 algorithms working 3-15-22
      NR_Kim
      Spectral NR
      LMS variable leak NR
    **********************************************************************************/
    switch (NR_Index) {
      case 0:                               // NR Off
        break;
      case 1:                               // Kim NR
        Kim1_NR();
        break;
      case 2:                               // Spectral NR
        SpectralNoiseReduction();
        break;
      case 3:                               // LMS NR
        ANR_notch = 0;
        Xanr();
        arm_scale_f32 (float_buffer_L, 2, float_buffer_L, FFT_length / 2);
        arm_scale_f32 (float_buffer_R, 2, float_buffer_R, FFT_length / 2);
        break;

    }
    //==================  End NR ============================
    // ===========================Aotomatic Notch ==================
    if (ANR_notchOn == 1) {
      ANR_notch = 1;
      Xanr();
    }
    // ====================End notch =================================
    /**********************************************************************************
      EXPERIMENTAL: noise blanker
      by Michael Wild
    **********************************************************************************/

    //=============================================================
    if (NB_on != 0) {
      NoiseBlanker(float_buffer_L, float_buffer_R);
      arm_copy_f32(float_buffer_R, float_buffer_L, FFT_length / 2);
    }

    //===========================  Narrow-band CW filter centered on 750Hz  =================

    //=== CW Filter ===
    // ----------------------  CW Narrow band filter -------------------------
    if (xmtMode == CW_MODE) {
      arm_biquad_cascade_df2T_f32(&S1_CW_Filter, float_buffer_R, float_buffer_R, 256);
      arm_biquad_cascade_df2T_f32(&S1_CW_Filter, float_buffer_L, float_buffer_L, 256);

      //=== end CW Filter ===

      // ----------------------  Correlation calculation  AFP 02-04-22 -------------------------

      //Calculate correlation between calc sine and incoming signal

      arm_correlate_f32  ( float_buffer_R, 256, sinBuffer, 256, float_Corr_BufferR ) ;    //AFP 02-02-22
      arm_max_f32  ( float_Corr_BufferR, 511, &corrResultR, &corrResultIndexR);    //AFP 02-02-22
      //running average of corr coeff.
      aveCorrResultR = .7 * corrResultR + .3 * aveCorrResultR;
      arm_correlate_f32  ( float_buffer_L, 256, sinBuffer, 256, float_Corr_BufferL ) ;    //AFP 02-02-22
      //get max value of correlation
      arm_max_f32  ( float_Corr_BufferL, 511, &corrResultL, &corrResultIndexL);    //AFP 02-02-22
      //running average of corr coeff.
      aveCorrResultL = .7 * corrResultL + .3 * aveCorrResultL;
      aveCorrResult = (corrResultR + corrResultL) / 2;
      // Calculate Goertzel Mahnitude of incomming signal
      goertzelMagnitude1 = goertzel_mag(256, 768, 24000, float_buffer_L);
      goertzelMagnitude2 = goertzel_mag(256, 768, 24000, float_buffer_R);
      goertzelMagnitude = (goertzelMagnitude1 + goertzelMagnitude2) / 2;
      //Combine Correlation and Gowetzel Coefficients

      combinedCoeff = 10 * aveCorrResult * 100 * goertzelMagnitude;
      combinedCoeff2 = combinedCoeff;
      // ==========   AFP 02-10-22  Changed CW decode "lock" indicator
      if (combinedCoeff > 2) {
        tft.fillRect(745, 448,  15 , 15, RA8875_GREEN);
      }
      else if (combinedCoeff < 2) {
        CWLevelTimer = millis();
        if (CWLevelTimer - CWLevelTimerOld > 2000) {
          CWLevelTimerOld = millis();
          tft.fillRect(744, 447,  17 , 17, RA8875_BLACK);
        }
      }
      combinedCoeff2Old = combinedCoeff2;
      tft.drawFastVLine(BAND_INDICATOR_X - 8 + 26, AUDIO_SPECTRUM_BOTTOM - 118, 118, RA8875_GREEN); //CW lower freq indicator
      tft.drawFastVLine(BAND_INDICATOR_X - 8 + 33, AUDIO_SPECTRUM_BOTTOM - 118, 118, RA8875_GREEN); //CW upper freq indicator
      if (combinedCoeff > 2) { // if  have a reasonable corr coeff, >.15, then we have a keeper.
        audioTemp = 1;
      } else {
        audioTemp = 0;

      }
      //==============  acquire data on CW  ================

      DoCWDecoding(audioTemp); //AFP 02-02-22  pass audio value. Either 1 or 0
    }

    // ======================================Interpolation  ================

    arm_fir_interpolate_f32(&FIR_int1_I, float_buffer_L, iFFT_buffer, BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF));   // Interpolatikon
    arm_fir_interpolate_f32(&FIR_int1_Q, float_buffer_R, FFT_buffer, BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF));

    // interpolation-by-4
    arm_fir_interpolate_f32(&FIR_int2_I, iFFT_buffer, float_buffer_L, BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF1));
    arm_fir_interpolate_f32(&FIR_int2_Q, FFT_buffer, float_buffer_R, BUFFER_SIZE * N_BLOCKS / (uint32_t)(DF1));



    /**********************************************************************************  AFP 12-31-20
      Digital Volume Control
    **********************************************************************************/
    if (mute == 1) {
      arm_scale_f32(float_buffer_L, 0.0, float_buffer_L, BUFFER_SIZE * N_BLOCKS);
      arm_scale_f32(float_buffer_R, 0.0, float_buffer_R, BUFFER_SIZE * N_BLOCKS);
    } else if (mute == 0) {
      arm_scale_f32(float_buffer_L, DF * VolumeToAmplification(audio_volume), float_buffer_L, BUFFER_SIZE * N_BLOCKS);
      arm_scale_f32(float_buffer_R, DF * VolumeToAmplification(audio_volume), float_buffer_R, BUFFER_SIZE * N_BLOCKS);
    }
    /**********************************************************************************  AFP 12-31-20
      CONVERT TO INTEGER AND PLAY AUDIO
    **********************************************************************************/

    for (unsigned  i = 0; i < N_BLOCKS; i++) {
      sp_L = Q_out_L.getBuffer();
      sp_R = Q_out_R.getBuffer();
      arm_float_to_q15 (&float_buffer_L[BUFFER_SIZE * i], sp_L, BUFFER_SIZE);
      arm_float_to_q15 (&float_buffer_R[BUFFER_SIZE * i], sp_R, BUFFER_SIZE);
      Q_out_L.playBuffer(); // play it !
      Q_out_R.playBuffer(); // play it !
    }

    if (auto_codec_gain == 1) {
      Codec_gain();
    }
    elapsed_micros_sum = elapsed_micros_sum + usec;
    elapsed_micros_idx_t++;
  } // end of if(audio blocks available)

}
