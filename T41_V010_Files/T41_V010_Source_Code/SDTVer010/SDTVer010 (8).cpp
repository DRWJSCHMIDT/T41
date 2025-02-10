#ifndef BEENHERE
#include "SDT.h"
#endif


#define debug_alternate_NR
#define boundary_blank 14                           //14 // for first trials very large!!!!
#define impulse_length NB_impulse_samples           // 7 // has to be odd!!!! 7 / 3 should be enough
#define PL             (impulse_length - 1) / 2     //6 // 3 has to be (impulse_length-1)/2 !!!!


/*****
  Purpose: void noiseblanker
  Parameter list:
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
*****/
//alt noise blanking is trying to localize some impulse noise within the samples and after that
//trying to replace corrupted samples by linear predicted samples.
//therefore, first we calculate the lpc coefficients which represent the actual status of the
//speech or sound generating "instrument" (in case of speech this is an estimation of the current
//filter-function of the voice generating tract behind our lips :-) )
//after finding this function we inverse filter the actual samples by this function
//so we are eliminating the speech, but not the noise. Then we do a matched filtering an thereby detecting impulses
//After that we threshold the remaining samples by some
//level and so detecting impulse noise's positions within the current frame - if one (or more) impulses are there.
//finally some area around the impulse position will be replaced by predicted samples from both sides (forward and
//backward prediction)
//hopefully we have enough processor power left....
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
      impulse_positions[impulse_count] = search_pos - order; // save the impulse positions and correct it by the filter delay
      impulse_count++;
      search_pos += PL; //  set search_pos a bit away, cause we are already repairing this area later
      //  and the next impulse should not be that close
    }

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
  bin_BW =  SR[SampleRate].rate/16; // sample rate/2/8 
                                                // calculate notch centre bin for FFT512
  notchCenterBin = roundf(notchFreq / bin_BW);
  // calculate bins  for deletion of bins in the iFFT_buffer
  // set iFFT_buffer[notch_L] to iFFT_buffer[notch_R] to zero
}

// ================= AGC

/*****
  Purpose: Setup AGC()
  Parameter list:
    void
  Return value;
    void
*****/
void AGCPrep()
{
  float32_t tmp;
  float32_t sample_rate = (float32_t)SR[SampleRate].rate / DF;
  // Start variables taken from wdsp

  tau_attack      = 0.001;                // tau_attack
  //    tau_decay = 0.250;                // tau_decay
  n_tau           = 1;                    // n_tau

  //    max_gain = 1000.0; // 1000.0; max gain to be applied??? or is this AGC threshold = knee level?
  fixed_gain            = 0.7; // if AGC == OFF
  max_input             = 2.0; //
  out_targ              = 0.3; // target value of audio after AGC
  //    var_gain = 32.0;  // slope of the AGC --> this is 10 * 10^(slope / 20) --> for 10dB slope, this is 30
  var_gain              = powf (10.0, (float32_t)agc_slope / 200.0); // 10 * 10^(slope / 20)

  tau_fast_backaverage  = 0.250;      // tau_fast_backaverage
  tau_fast_decay        = 0.005;      // tau_fast_decay
  pop_ratio             = 5.0;        // pop_ratio
  hang_enable           = 0;          // hang_enable
  tau_hang_backmult     = 0.500;      // tau_hang_backmult
  hangtime              = 0.250;      // hangtime
  hang_thresh           = 0.250;      // hang_thresh
  tau_hang_decay        = 0.100;      // tau_hang_decay

  //calculate internal parameters
  if (agc_switch_mode)
  {
    switch (AGCMode)
    {
      case 0:                                           //agcOFF
        break;
        
      case 1: //agcFrank
        hang_enable = 0;
        hang_thresh = 0.100;                            // from which level on should hang be enabled
        hangtime = 2.000;                               // hang time, if enabled
        tau_hang_backmult = 0.500;                      // time constant exponential averager
        agc_decay = 4000;                               // time constant decay long
        tau_fast_decay = 0.05;                          // tau_fast_decay
        tau_fast_backaverage = 0.250;                   // time constant exponential averager
        break;      
        
      case 2:                                           //agcLONG     {"Off", "Frank", "Long", "Slow", "Medium", "Fast"};
       // hangtime = 2.000;
        hangtime = 4.000;
        agc_decay = 4000;
        break;
        
      case 3:                                           //agcSLOW
        //hangtime = 1.000;
         hangtime = 4.000;
        //agc_decay = 500;
       agc_decay = 4000;
        break;
        
      case 4:                                           //agcMED
        hang_thresh = 1.0;
        //hangtime = 0.000;
       // agc_decay = 250;
         hangtime = 4.000;
        agc_decay = 4000;
        break;
        
      case 5:                                           //agcFAST
       // hang_thresh = 1.0;
        //hangtime = 0.000;
         hangtime = 4.000;
        agc_decay = 4000;
        //agc_decay = 50;
        break;

      default:
        break;
    }
    agc_switch_mode = 0;
  }
  tau_decay = (float32_t)agc_decay / 1000.0;
  //  max_gain = powf (10.0, (float32_t)agc_thresh / 20.0);
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
            volts += (ring_max - volts) * decay_mult*.05;
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
   
#ifdef USE_LOG10FAST
    mult = (out_target - slope_constant * min (0.0, log10f_fast(inv_max_input * volts))) / volts;
#else
    mult = (out_target - slope_constant * min (0.0, log10f(inv_max_input * volts))) / volts;
#endif
    iFFT_buffer[FFT_length + 2 * i + 0] = out_sample[0] * mult;
    iFFT_buffer[FFT_length + 2 * i + 1] = out_sample[1] * mult;
  }
}

// ========== AM-Decode stuff



/*****
  Purpose: AMDecodeSAM()
  Parameter list:
    void
  Return value;
    void
  Notes:  Synchronous AM detection.  Determines the carrier frequency, adjusts freq and replaces the received carrier with a steady signal to prevent fading.
  This alogorithm works best of those implimented
      // taken from Warren Pratt´s WDSP, 2016
  // http://svn.tapr.org/repos_sdr_hpsdr/trunk/W5WC/PowerSDR_HPSDR_mRX_PS/Source/wdsp/
*****/
void AMDecodeSAM() {
  // taken from Warren Pratt´s WDSP, 2016
  // http://svn.tapr.org/repos_sdr_hpsdr/trunk/W5WC/PowerSDR_HPSDR_mRX_PS/Source/wdsp/

  int k;

  for (unsigned i = 0; i < FFT_length / 2; i++)
  {
    float32_t Sin, Cos;
    if (atan2_approx) {
      Sin = arm_sin_f32(phzerror);
      Cos = arm_cos_f32(phzerror);
    } else {
      Sin = sin(phzerror);
      Cos = cos(phzerror);
    }
    ai = Cos * iFFT_buffer[FFT_length + i * 2];
    bi = Sin * iFFT_buffer[FFT_length + i * 2];
    aq = Cos * iFFT_buffer[FFT_length + i * 2 + 1];
    bq = Sin * iFFT_buffer[FFT_length + i * 2 + 1];

    if (bands[currentBand].mode != DEMOD_SAM) {
      a[0] = dsI;
      b[0] = bi;
      c[0] = dsQ;
      d[0] = aq;
      dsI = ai;
      dsQ = bq;

      for (int j = 0; j < SAM_PLL_HILBERT_STAGES; j++) {
        k = 3 * j;
        a[k + 3] = c0[j] * (a[k] - a[k + 5]) + a[k + 2];
        b[k + 3] = c1[j] * (b[k] - b[k + 5]) + b[k + 2];
        c[k + 3] = c0[j] * (c[k] - c[k + 5]) + c[k + 2];
        d[k + 3] = c1[j] * (d[k] - d[k + 5]) + d[k + 2];
      }
      ai_ps = a[OUT_IDX];
      bi_ps = b[OUT_IDX];
      bq_ps = c[OUT_IDX];
      aq_ps = d[OUT_IDX];

      for (int j = OUT_IDX + 2; j > 0; j--) {
        a[j] = a[j - 1];
        b[j] = b[j - 1];
        c[j] = c[j - 1];
        d[j] = d[j - 1];
      }
    }
    corr[0] = +ai + bq;
    corr[1] = -bi + aq;
    switch (bands[currentBand].mode) {
      case DEMOD_SAM:
        audio = corr[0];
        break;

    }
    if (fade_leveler) {
      dc = mtauR * dc + onem_mtauR * audio;
      dc_insert = mtauI * dc_insert + onem_mtauI * corr[0];
      audio = audio + dc_insert - dc;
    }
    float_buffer_L[i] = audio;
    if (bands[currentBand].mode == DEMOD_SAM_STEREO) {
      if (fade_leveler) {
        dcu = mtauR * dcu + onem_mtauR * audiou;
        dc_insertu = mtauI * dc_insertu + onem_mtauI * corr[0];
        audiou = audiou + dc_insertu - dcu;
      }
      float_buffer_R[i] = audiou;
    }
    if (atan2_approx) {
      det = arm_atan2_f32(corr[1], corr[0]);
    } else {
      // Why??
    }
    del_out = fil_out;
    omega2 = omega2 + g2 * det;
    if (omega2 < omega_min)
      omega2 = omega_min;
    else if (omega2 > omega_max)
      omega2 = omega_max;
    fil_out = g1 * det + omega2;
    phzerror = phzerror + del_out;
    // wrap round 2PI, modulus
    while (phzerror >= TPI)
      phzerror -= TPI;
    while (phzerror < 0.0)
      phzerror += TPI;
  }
  if (bands[currentBand].mode != DEMOD_SAM_STEREO) {
    arm_copy_f32(float_buffer_L, float_buffer_R, FFT_length / 2);
  }
  //        SAM_display_count++;
  //        if(SAM_display_count > 50) // to display the exact carrier frequency that the PLL is tuned to
  //        if(0)
  // in the small frequency display
  // we calculate carrier offset here and the display function is
  // then called in main loop every 100ms
  // to make this smoother, a simple lowpass/exponential averager here . . .
  
  SAM_carrier             = 0.08 * (omega2 * SR[SampleRate].rate) / (DF * TPI);
  SAM_carrier             = SAM_carrier + 0.92 * SAM_lowpass;
  SAM_carrier_freq_offset = (int)SAM_carrier;
  SAM_lowpass             = SAM_carrier;
  
//  ShowFrequency(bands[currentBand].freq, 0);
  ShowFrequency();
}

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
  Purpose: AM_Demod_AM2
  
  Parameter list:
    void
    
  Return value;
    void
    
  Notes:   // // E(t) = sqrtf(I*I + Q*Q) --> highpass IIR 1st order for DC removal --> lowpass IIR 2nd order
    Calc. magnitude of Signals and applies DC and low freq filtering using IIR filter.  Works well
*****/
void AMDemodAM2() {
  // // E(t) = sqrtf(I*I + Q*Q) --> highpass IIR 1st order for DC removal --> lowpass IIR 2nd order
  for (unsigned i = 0; i < FFT_length / 2; i++) { //
    audiotmp = sqrtf(iFFT_buffer[FFT_length + (i * 2)] * iFFT_buffer[FFT_length + (i * 2)]
                     + iFFT_buffer[FFT_length + (i * 2) + 1] * iFFT_buffer[FFT_length + (i * 2) + 1]);
                                                                                                        // DC removal filter -------
    w = audiotmp + wold * 0.9999f; // yes, I want a superb bass response ;-)
    float_buffer_L[i] = w - wold;

    wold = w;
  }
  arm_biquad_cascade_df1_f32 (&biquad_lowpass1, float_buffer_L, float_buffer_R, FFT_length / 2);
  arm_copy_f32(float_buffer_R, float_buffer_L, FFT_length / 2);
}

/*****
  Purpose: Allow user to set the mic compression level between 0.0 and 1.0
  
  Parameter list:
    void
    
  Return value;
    void
    
  Notes:   // // E(t) = sqrtf(I*I + Q*Q) --> highpass IIR 1st order for DC removal --> lowpass IIR 2nd order
    Calc. magnitude of Signals and applies DC and low freq filtering using IIR filter.  Works well
*****/
void SetCompressionLevel()
{
  int val;
  static float lastCompression;
  float currentCompression = micCompression;

  tft.setFontScale( (enum RA8875tsize) 1);

  tft.fillRect(251, 0, 270, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(252, 1);
  tft.print("Compression:");
  tft.setCursor(450, 1);
  tft.print(micCompression);

  filterEncoder.write(micCompression * 100);
  while (true) {
    val = filterEncoder.read();
    currentCompression += ((float) val) * 0.05;
    if (currentCompression != lastCompression) {

      if (currentCompression < 0.0)
        currentCompression = 0.0;
      else if (currentCompression > 1.0)                 // 100% max
        currentCompression = 1.0;

      filterEncoder.write(currentCompression);

      tft.fillRect(450, 0, 50, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(450, 1);
      tft.print(currentCompression);
    }

    val = ReadSelectedPushButton();                                  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);

    if (val == MENU_OPTION_SELECT) {                             // Make a choice??
      lastCompression = micCompression = currentCompression;
      EEPROMData.micCompression = micCompression;
      UpdateCompressionField();   
      break;
    }
  }
}
