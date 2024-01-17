#ifndef BEENHERE
#include "SDT.h"
#endif


#define debug_alternate_NR
#define boundary_blank 14                           //14 // for first trials very large!!!!
#define impulse_length NB_impulse_samples           // 7 // has to be odd!!!! 7 / 3 should be enough
#define PL             (impulse_length - 1) / 2     // 6 // 3 has to be (impulse_length-1)/2 !!!!

/*****
  Purpose: Setup Teensy Mic Compressor
  Parameter list:
    void
  Return value;
    void
*****/
void SetupMyCompressors(boolean use_HP_filter1, float knee_dBFS1, float comp_ratio1, float attack_sec1, float release_sec1) {
  comp1.enableHPFilter(use_HP_filter1);   comp2.enableHPFilter(use_HP_filter1);
  comp1.setThresh_dBFS(knee_dBFS1);       comp2.setThresh_dBFS(knee_dBFS1);
  comp1.setCompressionRatio(comp_ratio1); comp2.setCompressionRatio(comp_ratio1);

  float fs_Hz = AUDIO_SAMPLE_RATE;
  comp1.setAttack_sec(attack_sec1, fs_Hz);       comp2.setAttack_sec(attack_sec1, fs_Hz);
  comp1.setRelease_sec(release_sec1, fs_Hz);     comp2.setRelease_sec(release_sec1, fs_Hz);
}


/*****
  Purpose: void noiseblanker
  Parameter list:Figure 4.
    void
  Return value;
    void
*****/
void NoiseBlanker(float32_t* inputsamples, float32_t* outputsamples)
{
  float32_t* Energy = 0;

  AltNoiseBlanking(inputsamples, NB_FFT_SIZE, Energy);

  for (unsigned k = 0; k < NB_FFT_SIZE;  k++)
  {
    outputsamples[k] = inputsamples[k];
  }

}

/*****
  Purpose: void AltNoiseBlanking(
  Parameter list:
    void
  Return value;
    void

  alt noise blanking is trying to localize some impulse noise within the samples and after that
  trying to replace corrupted samples by linear predicted samples.
  therefore, first we calculate the lpc coefficients which represent the actual status of the
  speech or sound generating "instrument" (in case of speech this is an estimation of the current
  filter-function of the voice generating tract behind our lips :-) )
  after finding this function we inverse filter the actual samples by this function
  so we are eliminating the speech, but not the noise. Then we do a matched filtering an thereby detecting impulses
  After that we threshold the remaining samples by some
  level and so detecting impulse noise's positions within the current frame - if one (or more) impulses are there.
  finally some area around the impulse position will be replaced by predicted samples from both sides (forward and
  backward prediction)
  hopefully we have enough processor power left....
*****/
void AltNoiseBlanking(float* insamp, int Nsam, float* E )
{
  int impulse_positions[20];      //we allow a maximum of 5 impulses per frame
  int search_pos    = 0;
  int impulse_count = 0;
  int order         = NB_taps;    //10 // lpc's order
  static float32_t last_frame_end[80]; //this takes the last samples from the previous frame to do the prediction within the boundaries

  arm_fir_instance_f32 LPC;
  float32_t lpcs[order + 1];                      // we reserve one more than "order" because of a leading "1"
  float32_t reverse_lpcs[order + 1];              //this takes the reversed order lpc coefficients
  float32_t firStateF32[NB_FFT_SIZE + order];
  float32_t tempsamp[NB_FFT_SIZE];
  float32_t sigma2;                               //taking the variance of the inpo
  float32_t lpc_power;
  float32_t impulse_threshold;

#ifdef debug_alternate_NR
  static int frame_count = 0; //only used for the distortion insertion - can alter be deleted
  int dist_level         = 0; //only used for the distortion insertion - can alter be deleted
#endif

  int nr_setting = 0;
  float32_t R[11];            // takes the autocorrelation results
  float32_t k, alfa;

  float32_t any[order + 1];   //some internal buffers for the levinson durben algorithm

  float32_t Rfw[impulse_length + order]; // takes the forward predicted audio restauration
  float32_t Rbw[impulse_length + order]; // takes the backward predicted audio restauration
  float32_t Wfw[impulse_length], Wbw[impulse_length]; // taking linear windows for the combination of fwd and bwd

  float32_t s;

  memset(R, 0, sizeof(float32_t) * 11);

#ifdef debug_alternate_NR  // generate test frames to test the noise blanker function
  // using the NR-setting (0..55) to select the test frame
  // 00 = noise blanker active on orig. audio; threshold factor=3
  // 01 = frame of vocal "a" undistorted
  // 02 .. 05 = frame of vocal "a" with different impulse distortion levels
  // 06 .. 09 = frame of vocal "a" with different impulse distortion levels
  //            noise blanker operating!!
  //************
  // 01..09 are now using the original received audio and applying a rythmic "click" distortion
  // 06..09 is detecting and removing the click by restoring the predicted audio!!!
  //************
  // 5 / 9 is the biggest "click" and it is slightly noticeable in the restored audio (9)
  // 10 = noise blanker active on orig. audio threshold factor=3
  // 11  = sinusoidal signal undistorted
  // 12 ..15 = sinusoidal signal with different impulse distortion levels
  // 16 ..19 = sinusoidal signal with different impulse distortion levels
  //            noise blanker operating!!
  // 20 ..50   noise blanker active on orig. audio; threshold factor varying between 3 and 0.26

  nr_setting = NB_test; //(int)ts.dsp_nr_strength;

  //*********************************from here just debug impulse / signal generation
  if ((nr_setting > 0) && (nr_setting < 10)) // we use the vocal "a" frame
  {
    //for (int i=0; i<128;i++)          // not using vocal "a" but the original signal
    //    insamp[i]=NR_test_samp[i];

    if ((frame_count > 19) && (nr_setting > 1))    // insert a distorting pulse
    {
      dist_level = nr_setting;
      if (dist_level > 5) {
        dist_level = dist_level - 4; // distortion level is 1...5
      }
      insamp[4] = insamp[4] + dist_level * 3000; // overlaying a short  distortion pulse +/-
      insamp[5] = insamp[5] - dist_level * 1000;
    }
  }

  if ((nr_setting > 10) && (nr_setting < 20))             // we use the sinus frame
  {
    for (int i = 0; i < 128; i++)
      if ((frame_count > 19) && (nr_setting > 11))        // insert a distorting pulse
      {
        dist_level = nr_setting - 10;
        if (dist_level > 5) dist_level = dist_level - 4;
        insamp[24] = insamp[24] + dist_level * 1000;      // overlaying a short  distortion pulse +/-
        insamp[25] = insamp[25] + dist_level * 500;
        insamp[26] = insamp[26] - dist_level * 200;       // overlaying a short  distortion pulse +/-
        insamp[27] = insamp[27] - dist_level * 100;
      }
  }
  frame_count++;
  if (frame_count > 20)
    frame_count = 0;

#endif
  //*****************************end of debug impulse generation

  //  start of test timing zone

  for (int i = 0; i < impulse_length; i++) // generating 2 Windows for the combination of the 2 predictors
  { // will be a constant window later!
    Wbw[i] = 1.0 * i / (impulse_length - 1);
    Wfw[impulse_length - i - 1] = Wbw[i];
  }

  // calculate the autocorrelation of insamp (moving by max. of #order# samples)
  for (int i = 0; i < (order + 1); i++)
  {
    arm_dot_prod_f32(&insamp[0], &insamp[i], Nsam - i, &R[i]); // R is carrying the crosscorrelations
  }
  // end of autocorrelation

  //alternative levinson durben algorithm to calculate the lpc coefficients from the crosscorrelation

  R[0] = R[0] * (1.0 + 1.0e-9);

  lpcs[0] = 1;   //set lpc 0 to 1

  for (int i = 1; i < order + 1; i++)
    lpcs[i] = 0;                    // fill rest of array with zeros - could be done by memfill

  alfa = R[0];

  for (int m = 1; m <= order; m++)
  {
    s = 0.0;
    for (int u = 1; u < m; u++)
      s = s + lpcs[u] * R[m - u];

    k = -(R[m] + s) / alfa;

    for (int v = 1; v < m; v++)
      any[v] = lpcs[v] + k * lpcs[m - v];

    for (int w = 1; w < m; w++)
      lpcs[w] = any[w];

    lpcs[m] = k;
    alfa = alfa * (1 - k * k);
  }

  // end of levinson durben algorithm

  for (int o = 0; o < order + 1; o++ )           //store the reverse order coefficients separately
    reverse_lpcs[order - o] = lpcs[o];    // for the matched impulse filter

  arm_fir_init_f32(&LPC, order + 1, &reverse_lpcs[0], &firStateF32[0], NB_FFT_SIZE);                                   // we are using the same function as used in freedv
  arm_fir_f32(&LPC, insamp, tempsamp, Nsam); //do the inverse filtering to eliminate voice and enhance the impulses
  arm_fir_init_f32(&LPC, order + 1, &lpcs[0], &firStateF32[0], NB_FFT_SIZE);                                   // we are using the same function as used in freedv
  arm_fir_f32(&LPC, tempsamp, tempsamp, Nsam); // do a matched filtering to detect an impulse in our now voiceless signal

  arm_var_f32(tempsamp, NB_FFT_SIZE, &sigma2); //calculate sigma2 of the original signal ? or tempsignal
  arm_power_f32(lpcs, order, &lpc_power); // calculate the sum of the squares (the "power") of the lpc's

  impulse_threshold = NB_thresh * sqrtf(sigma2 * lpc_power);  //set a detection level (3 is not really a final setting)

  search_pos = order + PL; // lower boundary problem has been solved! - so here we start from 1 or 0?
  impulse_count = 0;

  do {        //going through the filtered samples to find an impulse larger than the threshold
    if ((tempsamp[search_pos] > impulse_threshold) || (tempsamp[search_pos] < (-impulse_threshold)))
    {
      impulse_positions[impulse_count] = search_pos - order;  // save the impulse positions and correct it by the filter delay
      impulse_count++;
      search_pos += PL;                                       //  set search_pos a bit away, cause we are already repairing this area later
    }                                                         //  and the next impulse should not be that close
    search_pos++;
  } while (( (unsigned int) search_pos < NB_FFT_SIZE - (unsigned int) boundary_blank) && ( (unsigned int) impulse_count < 20U)); // avoid upper boundary

  //boundary handling has to be fixed later
  //as a result we now will not find any impulse in these areas

  // from here: reconstruction of the impulse-distorted audio part:

  // first we form the forward and backward prediction transfer functions from the lpcs
  // that is easy, as they are just the negated coefficients  without the leading "1"
  // we can do this in place of the lpcs, as they are not used here anymore and being recalculated in the next frame!

  arm_negate_f32(&lpcs[1], &lpcs[1], order);
  arm_negate_f32(&reverse_lpcs[0], &reverse_lpcs[0], order);

  for (int j = 0; j < impulse_count; j++)
  {
    for (int k = 0; k < order; k++) // we have to copy some samples from the original signal as
    { // basis for the reconstructions - could be done by memcopy

      if ((impulse_positions[j] - PL - order + k) < 0) // this solves the prediction problem at the left boundary
      {
        Rfw[k] = last_frame_end[impulse_positions[j] + k]; //take the sample from the last frame
      }
      else
      {
        Rfw[k] = insamp[impulse_positions[j] - PL - order + k]; //take the sample from this frame as we are away from the boundary
      }

      Rbw[impulse_length + k] = insamp[impulse_positions[j] + PL + k + 1];
    }     //bis hier alles ok

    for (int i = 0; i < impulse_length; i++) //now we calculate the forward and backward predictions
    {
      arm_dot_prod_f32(&reverse_lpcs[0], &Rfw[i], order, &Rfw[i + order]);
      arm_dot_prod_f32(&lpcs[1], &Rbw[impulse_length - i], order, &Rbw[impulse_length - i - 1]);
    }
    arm_mult_f32(&Wfw[0], &Rfw[order], &Rfw[order], impulse_length); // do the windowing, or better: weighing
    arm_mult_f32(&Wbw[0], &Rbw[0], &Rbw[0], impulse_length);

#ifdef debug_alternate_NR
    // in debug mode do the restoration only in some cases
    if (((nr_setting > 0) && (nr_setting < 6)) || ((nr_setting > 10) && (nr_setting < 16)))
    {
      // just let the distortion pass at setting 1...5 and 11...15
      //    arm_add_f32(&Rfw[order],&Rbw[0],&insamp[impulse_positions[j]-PL],impulse_length);
    } else {
      //finally add the two weighted predictions and insert them into the original signal - thereby eliminating the distortion
      arm_add_f32(&Rfw[order], &Rbw[0], &insamp[impulse_positions[j] - PL], impulse_length);
    }
#else
    //finally add the two weighted predictions and insert them into the original signal - thereby eliminating the distortion
    arm_add_f32(&Rfw[order], &Rbw[0], &insamp[impulse_positions[j] - PL], impulse_length);

#endif
  }

  for (int p = 0; p < (order + PL); p++)
  {
    last_frame_end[p] = insamp[NB_FFT_SIZE - 1 - order - PL + p]; // store 13 samples from the current frame to use at the next frame
  }
  //end of test timing zone
}

void CalcNotchBins()
{
  bin_BW =  SR[SampleRate].rate / 16; // sample rate/2/8
  // calculate notch centre bin for FFT512
  notchCenterBin = roundf(notchFreq / bin_BW);
  // calculate bins  for deletion of bins in the iFFT_buffer
  // set iFFT_buffer[notch_L] to iFFT_buffer[notch_R] to zero
}

// ================= AGC

// G0ORX broke this code out so can be called from other places

void AGCLoadValues() {
  float32_t tmp;
  float32_t sample_rate = (float32_t)SR[SampleRate].rate / DF;

  //calculate internal parameters
  switch (AGCMode)
  {
    case 0:                                           //agcOFF
      break;

    case 1:                                           //agcLONG
      // G0ORX 
      hangtime = 2.000;
      tau_decay = 2.000;
      break;

    case 2:                                           //agcSLOW
      // G0ORX
      hangtime = 1.000;
      tau_decay = 0.5;
      break;

    case 3:                                           //agcMED        hang_thresh = 1.0;
      // G0ORX
      hangtime = 0.000;
      tau_decay = 0.250;
      break;

    case 4:                                           //agcFAST
      hang_thresh = 1.0;
      // G0ORX
      hangtime = 0.0;
      tau_decay = 0.050;
      break;

    default:
      break;
  }
  
  max_gain = powf (10.0, (float32_t)bands[currentBand].AGC_thresh / 20.0);
  attack_buffsize = (int)ceil(sample_rate * n_tau * tau_attack);
  in_index = attack_buffsize + out_index;
  attack_mult = 1.0 - expf(-1.0 / (sample_rate * tau_attack));
  decay_mult = 1.0 - expf(-1.0 / (sample_rate * tau_decay));
  fast_decay_mult = 1.0 - expf(-1.0 / (sample_rate * tau_fast_decay));
  fast_backmult = 1.0 - expf(-1.0 / (sample_rate * tau_fast_backaverage));

  onemfast_backmult = 1.0 - fast_backmult;

  out_target = out_targ * (1.0 - expf(-(float32_t)n_tau)) * 0.9999;
  min_volts = out_target / (var_gain * max_gain);
  inv_out_target = 1.0 / out_target;

  tmp = log10f(out_target / (max_input * var_gain * max_gain));
  if (tmp == 0.0)
    tmp = 1e-16;
  slope_constant = (out_target * (1.0 - 1.0 / var_gain)) / tmp;

  inv_max_input = 1.0 / max_input;

  tmp = powf (10.0, (hang_thresh - 1.0) / 0.125);
  hang_level = (max_input * tmp + (out_target / (var_gain * max_gain)) * (1.0 - tmp)) * 0.637;

  hang_backmult = 1.0 - expf(-1.0 / (sample_rate * tau_hang_backmult));
  onemhang_backmult = 1.0 - hang_backmult;

  hang_decay_mult = 1.0 - expf(-1.0 / (sample_rate * tau_hang_decay));
}

/*****
  Purpose: Setup AGC()
  Parameter list:
    void
  Return value;
    void
*****/
void AGCPrep()
{
  // Start variables taken from wdsp

  tau_attack      = 0.001;                // tau_attack
  tau_decay       = 0.250; // G0ORX
  n_tau           = 4; // G0ORX

  // max_gain = 1000.0 to be applied??? or is this AGC threshold = knee level?
  max_gain              = 10000.0; // G0ORX
  fixed_gain            = 20.0; // G0ORX
  max_input             = 1.0; // G0ORX
  out_targ              = 1.0; // G0ORX       // target value of audio after AGC
  var_gain              = 1.5; // G0ORX

  tau_fast_backaverage  = 0.250;      // tau_fast_backaverage
  tau_fast_decay        = 0.005;      // tau_fast_decay
  pop_ratio             = 5.0;        // pop_ratio
  hang_enable           = 1; // G0ORX
  tau_hang_backmult     = 0.500;      // tau_hang_backmult
  hangtime              = 0.250;      // hangtime
  hang_thresh           = 0.250;      // hang_thresh
  tau_hang_decay        = 0.100;      // tau_hang_decay


  AGCLoadValues(); // G0ORX

}

void AGCThresholdChanged() {
  max_gain = powf (10.0, (float32_t)bands[currentBand].AGC_thresh / 20.0);
}
/*****
  Purpose: Audio AGC()
  Parameter list:
    void
  Return value;
    void
*****/
void AGC()
{

  int k;
  float32_t mult;
  if (AGCMode == 0)  // AGC OFF
  {
    for (unsigned i = 0; i < FFT_length / 2; i++)
    {
      iFFT_buffer[FFT_length + 2 * i + 0] = fixed_gain * iFFT_buffer[FFT_length + 2 * i + 0];
      iFFT_buffer[FFT_length + 2 * i + 1] = fixed_gain * iFFT_buffer[FFT_length + 2 * i + 1];
    }
    return;
  }

  for (unsigned i = 0; i < FFT_length / 2; i++)
  {
    if (++out_index >= (int)ring_buffsize)
      out_index -= ring_buffsize;
    if (++in_index >= ring_buffsize)
      in_index -= ring_buffsize;

    out_sample[0] = ring[2 * out_index + 0];
    out_sample[1] = ring[2 * out_index + 1];
    abs_out_sample = abs_ring[out_index];
    ring[2 * in_index + 0] = iFFT_buffer[FFT_length + 2 * i + 0];
    ring[2 * in_index + 1] = iFFT_buffer[FFT_length + 2 * i + 1];
    if (pmode == 0) // MAGNITUDE CALCULATION
      abs_ring[in_index] = max(fabs(ring[2 * in_index + 0]), fabs(ring[2 * in_index + 1]));
    else
      abs_ring[in_index] = sqrtf(ring[2 * in_index + 0] * ring[2 * in_index + 0] + ring[2 * in_index + 1] * ring[2 * in_index + 1]);

    fast_backaverage = fast_backmult * abs_out_sample + onemfast_backmult * fast_backaverage;
    hang_backaverage = hang_backmult * abs_out_sample + onemhang_backmult * hang_backaverage;

    if ((abs_out_sample >= ring_max) && (abs_out_sample > 0.0))
    {
      ring_max = 0.0;
      k = out_index;
      for (int j = 0; j < attack_buffsize; j++)
      {
        if (++k == (int)ring_buffsize)
          k = 0;
        if (abs_ring[k] > ring_max)
          ring_max = abs_ring[k];
      }
    }
    if (abs_ring[in_index] > ring_max)
      ring_max = abs_ring[in_index];

    if (hang_counter > 0)
      --hang_counter;

    switch (state)
    {
      case 0:
        if (ring_max >= volts) {
          volts += (ring_max - volts) * attack_mult;
        } else {
          if (volts > pop_ratio * fast_backaverage) {
            state = 1;
            volts += (ring_max - volts) * fast_decay_mult;
          } else {
            if (hang_enable && (hang_backaverage > hang_level)) {
              state = 2;
              hang_counter = (int)(hangtime * SR[SampleRate].rate / DF);
              decay_type = 1;
            } else {
              state = 3;
              volts += (ring_max - volts) * decay_mult;
              decay_type = 0;
            }
          }
        }
        break;

      case 1:
        if (ring_max >= volts) {
          state = 0;
          volts += (ring_max - volts) * attack_mult;
        } else {
          if (volts > save_volts) {
            volts += (ring_max - volts) * fast_decay_mult;
          } else {
            if (hang_counter > 0) {
              state = 2;
            } else {
              if (decay_type == 0) {
                state = 3;
                volts += (ring_max - volts) * decay_mult;
              } else {
                state = 4;
                volts += (ring_max - volts) * hang_decay_mult;
              }
            }
          }
        }
        break;

      case 2:
        if (ring_max >= volts) {
          state = 0;
          save_volts = volts;
          volts += (ring_max - volts) * attack_mult;
        } else {
          if (hang_counter == 0) {
            state = 4;
            volts += (ring_max - volts) * hang_decay_mult;
          }
        }
        break;

      case 3:
        if (ring_max >= volts) {
          state = 0;
          save_volts = volts;
          volts += (ring_max - volts) * attack_mult;
        } else {
          volts += (ring_max - volts) * decay_mult * .05;
        }
        break;

      case 4:
        if (ring_max >= volts) {
          state = 0;
          save_volts = volts;
          volts += (ring_max - volts) * attack_mult;
        } else {
          volts += (ring_max - volts) * hang_decay_mult;
        }
        break;
    }
    if (volts < min_volts) {
      volts = min_volts; // no AGC action is taking place
      agc_action = 0;
    } else {
      agc_action = 1;                           // LED indicator for AGC action
    }

    //#ifdef USE_LOG10FAST
    mult = (out_target - slope_constant * min (0.0, log10f_fast(inv_max_input * volts))) / volts;
    //#else
    //  mult = (out_target - slope_constant * min (0.0, log10f(inv_max_input * volts))) / volts;
    //#endif
    iFFT_buffer[FFT_length + 2 * i + 0] = out_sample[0] * mult;
    iFFT_buffer[FFT_length + 2 * i + 1] = out_sample[1] * mult;
  }
}

// ========== AM-Decode stuff



/*****
  Purpose: Demod IQ
  Parameter list:
    void
  Return value;
    void
*****/
void DecodeIQ() {
  for (unsigned i = 0; i < FFT_length / 2; i++) {
    float_buffer_L[i] = iFFT_buffer[FFT_length + i * 2];
    float_buffer_R[i] = iFFT_buffer[FFT_length + i * 2 + 1];
  }
}



/*****
  Purpose: Allow user to set the mic compression level

  Parameter list:
    void

  Return value;
    void
*****/
void SetCompressionLevel()
{
  int val;
  currentMicThreshold = knee_dBFS; // AFP 09-22-22

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(SECONDARY_MENU_X - 50, MENUS_Y, EACH_MENU_WIDTH + 50, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(SECONDARY_MENU_X  - 48, MENUS_Y + 1);
  tft.print("Compression:");
  tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
  tft.print(currentMicThreshold);

  while (true) {
    if (filterEncoderMove != 0) {
      currentMicThreshold += ((float) filterEncoderMove);
      if (currentMicThreshold < -60)
        currentMicThreshold = -60;
      else if (currentMicThreshold > 0)                 // 100% max
        currentMicThreshold = 0;

      tft.fillRect(SECONDARY_MENU_X + 180, MENUS_Y, 80, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
      tft.print(currentMicThreshold);
      filterEncoderMove = 0;
    }
    val = ReadSelectedPushButton();                                  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);
    if (val == MENU_OPTION_SELECT) {                             // Make a choice??
      // micCompression = currentMicThreshold;
      EEPROMData.currentMicThreshold = currentMicThreshold;
      EEPROMWrite();
      UpdateCompressionField();
      break;
    }
  }
  EraseMenus();
}

/*****
  Purpose: Allow user to set the mic compression ratio

  Parameter list:
    void

  Return value;
    void
*****/
void SetCompressionRatio()
{
  int val;

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(SECONDARY_MENU_X - 50, MENUS_Y, EACH_MENU_WIDTH + 50, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(SECONDARY_MENU_X  - 48, MENUS_Y + 1);
  tft.print("Comp Ratio:");
  tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
  tft.print(currentMicCompRatio, 1);

  while (true) {
    if (filterEncoderMove != 0) {
      currentMicCompRatio += ((float) filterEncoderMove * .1);
      if (currentMicCompRatio > 10)
        currentMicCompRatio = 10;
      else if (currentMicCompRatio < 1)                 // 100% max
        currentMicCompRatio = 1;

      tft.fillRect(SECONDARY_MENU_X + 180, MENUS_Y, 80, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
      tft.print(currentMicCompRatio, 1);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();                                  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);

    if (val == MENU_OPTION_SELECT) {                             // Make a choice??
      EEPROMData.currentMicCompRatio = currentMicCompRatio;
      EEPROMWrite();

      break;
    }
  }
  EraseMenus();
}
/*****
  Purpose: Allow user to set the mic Attack in sec

  Parameter list:
    void

  Return value;
    void
*****/
void SetCompressionAttack()
{
  int val;

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(SECONDARY_MENU_X - 50, MENUS_Y, EACH_MENU_WIDTH + 50, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(SECONDARY_MENU_X  - 48, MENUS_Y + 1);
  tft.print("Attack Sec:");
  tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
  tft.print(currentMicAttack, 1);

  while (true) {
    if (filterEncoderMove != 0) {
      currentMicAttack += ((float) filterEncoderMove * 0.1);
      if (currentMicAttack > 10)
        currentMicAttack = 10;
      else if (currentMicAttack < .1)                 // 100% max
        currentMicAttack = .1;

      tft.fillRect(SECONDARY_MENU_X + 180, MENUS_Y, 80, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
      tft.print(currentMicAttack, 1);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();                                  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);

    if (val == MENU_OPTION_SELECT) {                             // Make a choice??
      EEPROMData.currentMicAttack = currentMicAttack;
      EEPROMWrite();

      break;
    }
  }
  EraseMenus();
}

/*****
  Purpose: Allow user to set the mic compression ratio

  Parameter list:
    void

  Return value;
    void
*****/
void SetCompressionRelease()
{
  int val;

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(SECONDARY_MENU_X - 50, MENUS_Y, EACH_MENU_WIDTH + 50, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(SECONDARY_MENU_X  - 48, MENUS_Y + 1);
  tft.print("Decay Sec:");
  tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
  tft.print(currentMicRelease, 1);

  while (true) {
    if (filterEncoderMove != 0) {
      currentMicRelease += ((float) filterEncoderMove * 0.1);
      if (currentMicRelease > 10)
        currentMicRelease = 10;
      else if (currentMicRelease < 0.1)                 // 100% max
        currentMicRelease = 0.1;

      tft.fillRect(SECONDARY_MENU_X + 180, MENUS_Y, 80, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X + 180, MENUS_Y + 1);
      tft.print(currentMicRelease, 1);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();                                  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);

    if (val == MENU_OPTION_SELECT) {                             // Make a choice??
      EEPROMData.currentMicCompRatio = currentMicCompRatio;
      EEPROMWrite();

      break;
    }
  }
  EraseMenus();
}
