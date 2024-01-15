#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: Kim1_NR()
  Parameter list:
    void
  Return value;
    void
*****/
void Kim1_NR() 
{
  /**********************************************************************************
      EXPERIMENTAL STATION FOR SPECTRAL NOISE REDUCTION
      FFT - iFFT Convolution

     thanks a lot for your support, Michael DL2FW !
   **********************************************************************************/
  NR_Kim=1;
  if (NR_Kim == 1)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // this is exactly the implementation by
    // Kim & Ruwisch 2002 - 7th International Conference on Spoken Language Processing Denver, Colorado, USA
    // with two exceptions:
    // 1.) we use power instead of magnitude for X
    // 2.) we need to clamp for negative gains . . .
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    // perform a loop two times (each time process 128 new samples)
    // FFT 256 points
    // frame step 128 samples
    // half-overlapped data buffers

    uint8_t VAD_low = 0;
    uint8_t VAD_high = 127;
    float32_t lf_freq; // = (offset - width/2) / (12000 / NR_FFT_L); // bin BW is 46.9Hz [12000Hz / 256 bins] @96kHz
    float32_t uf_freq;
    if (bands[currentBand].FLoCut <= 0 && bands[currentBand].FHiCut >= 0) {
      lf_freq = 0.0;
      uf_freq = fmax(-(float32_t)bands[currentBand].FLoCut, (float32_t)bands[currentBand].FHiCut);
    } else {
      if (bands[currentBand].FLoCut > 0) {
        lf_freq = (float32_t)bands[currentBand].FLoCut;
        uf_freq = (float32_t)bands[currentBand].FHiCut;
      } else {
        uf_freq = -(float32_t)bands[currentBand].FLoCut;
        lf_freq = -(float32_t)bands[currentBand].FHiCut;
      }
    }
    lf_freq /= ((SR[SampleRate].rate / DF) / NR_FFT_L); // bin BW is 46.9Hz [12000Hz / 256 bins] @96kHz
    uf_freq /= ((SR[SampleRate].rate / DF) / NR_FFT_L);

    VAD_low = (int)lf_freq;
    VAD_high = (int)uf_freq;
    if (VAD_low == VAD_high) {
      VAD_high++;
    }
    if (VAD_low < 1) {
      VAD_low = 1;
    } else if (VAD_low > NR_FFT_L / 2 - 2) {
      VAD_low = NR_FFT_L / 2 - 2;
    }
    if (VAD_high < 1) {
      VAD_high = 1;
    } else if (VAD_high > NR_FFT_L / 2) {
      VAD_high = NR_FFT_L / 2;
    }

    for (int k = 0; k < 2; k++) {
      // NR_FFT_buffer is 512 floats big
      // interleaved r, i, r, i . . .
      // fill first half of FFT_buffer with last events audio samples
      for (int i = 0; i < NR_FFT_L / 2; i++) {
        NR_FFT_buffer[i * 2] = NR_last_sample_buffer_L[i]; // real
        NR_FFT_buffer[i * 2 + 1] = 0.0; // imaginary
      }
      // copy recent samples to last_sample_buffer for next time!
      for (int i = 0; i < NR_FFT_L  / 2; i++) {
        NR_last_sample_buffer_L [i] = float_buffer_L[i + k * (NR_FFT_L / 2)];
      }
      // now fill recent audio samples into second half of FFT_buffer
      for (int i = 0; i < NR_FFT_L / 2; i++) {
        NR_FFT_buffer[NR_FFT_L + i * 2] = float_buffer_L[i + k * (NR_FFT_L / 2)]; // real
        NR_FFT_buffer[NR_FFT_L + i * 2 + 1] = 0.0;
      }
      // perform windowing on 256 real samples in the NR_FFT_buffer
      for (int idx = 0; idx < NR_FFT_L; idx++)  {                               // Hann window
        float32_t temp_sample = 0.5 * (float32_t)(1.0 - (cosf(PI * 2.0 * (float32_t)idx / (float32_t)((NR_FFT_L) - 1))));
        NR_FFT_buffer[idx * 2] *= temp_sample;
      }


#if 0     // Odd way to comment something out. Not sure why they did this. JJP

      // perform windowing on 256 real samples in the NR_FFT_buffer
      for (int idx = 0; idx < NR_FFT_L; idx++) { // sqrt Hann window
        NR_FFT_buffer[idx * 2] *= sqrtHann[idx];
      }
#endif

      arm_cfft_f32(NR_FFT, NR_FFT_buffer, 0, 1);
      for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) { // take first 128 bin values of the FFT result
        // it seems that taking power works better than taking magnitude . . . !?
        //NR_X[bindx][NR_X_pointer] = sqrtf(NR_FFT_buffer[bindx * 2] * NR_FFT_buffer[bindx * 2] + NR_FFT_buffer[bindx * 2 + 1] * NR_FFT_buffer[bindx * 2 + 1]);
        NR_X[bindx][NR_X_pointer] = (NR_FFT_buffer[bindx * 2] * NR_FFT_buffer[bindx * 2] + NR_FFT_buffer[bindx * 2 + 1] * NR_FFT_buffer[bindx * 2 + 1]);
      }

      for (int bindx = VAD_low; bindx < VAD_high; bindx++) { // take first 128 bin values of the FFT result
        NR_sum = 0.0;
        for (int j = 0; j < NR_L_frames; j++)
        { // sum up the L_frames |X|
          NR_sum = NR_sum + NR_X[bindx][j];
        }
        // divide sum of L_frames |X| by L_frames to calculate the average and save in NR_E
        NR_E[bindx][NR_E_pointer] = NR_sum / (float32_t)NR_L_frames;
      }
      for (int bindx = VAD_low; bindx < VAD_high; bindx++) { // take first 128 bin values of the FFT result
                                            // we have to reset the minimum value to the first E value every time we start with a bin
        NR_M[bindx] = NR_E[bindx][0];
                                            // therefore we start with the second E value (index j == 1)
        for (uint8_t j = 1; j < NR_N_frames; j++) {
          if (NR_E[bindx][j] < NR_M[bindx]) {
            NR_M[bindx] = NR_E[bindx][j];
          }
        }
      }
      for (int bindx = VAD_low; bindx < VAD_high; bindx++) { // take first 128 bin values of the FFT result
        NR_T = NR_X[bindx][NR_X_pointer] / NR_M[bindx]; // dies scheint mir besser zu funktionieren !
        if (NR_T > NR_PSI) {
          NR_lambda[bindx] = NR_M[bindx];
        } else {
          NR_lambda[bindx] = NR_E[bindx][NR_E_pointer];
        }
      }

      for (int bindx = VAD_low; bindx < VAD_high; bindx++) { // take first 128 bin values of the FFT result
        if (NR_use_X) {
          NR_G[bindx] = 1.0 - (NR_lambda[bindx] * NR_KIM_K / NR_X[bindx][NR_X_pointer]);
          if (NR_G[bindx] < 0.0)
            NR_G[bindx] = 0.0;
        } else {
          NR_G[bindx] = 1.0 - (NR_lambda[bindx] * NR_KIM_K / NR_E[bindx][NR_E_pointer]);
          if (NR_G[bindx] < 0.0)
            NR_G[bindx] = 0.0;
        }

        // time smoothing
        NR_Gts[bindx][0] = NR_alpha * NR_Gts[bindx][1] + (NR_onemalpha) * NR_G[bindx];
        NR_Gts[bindx][1] = NR_Gts[bindx][0]; // copy for next FFT frame
      }

      // NR_G is always positive, however often 0.0
      for (int bindx = 1; bindx < ((NR_FFT_L / 2) - 1); bindx++) {// take first 128 bin values of the FFT result
        NR_G[bindx] = NR_beta * NR_Gts[bindx - 1][0] + NR_onemtwobeta * NR_Gts[bindx][0] + NR_beta * NR_Gts[bindx + 1][0];
      }
                                                                              // take care of bin 0 and bin NR_FFT_L/2 - 1
      NR_G[0] = (NR_onemtwobeta + NR_beta) * NR_Gts[0][0] + NR_beta * NR_Gts[1][0];
      NR_G[(NR_FFT_L / 2) - 1] = NR_beta * NR_Gts[(NR_FFT_L / 2) - 2][0] + (NR_onemtwobeta + NR_beta) * NR_Gts[(NR_FFT_L / 2) - 1][0];
      for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) {                                      // try 128:
        NR_FFT_buffer[bindx * 2] = NR_FFT_buffer [bindx * 2] * NR_G[bindx];                     // real part
        NR_FFT_buffer[bindx * 2 + 1] = NR_FFT_buffer [bindx * 2 + 1] * NR_G[bindx];             // imag part
        NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 2] = NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 2] * NR_G[bindx]; // real part conjugate symmetric
        NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 1] = NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 1] * NR_G[bindx]; // imag part conjugate symmetric
      }
      NR_X_pointer = NR_X_pointer + 1;
      if (NR_X_pointer >= NR_L_frames) {
        NR_X_pointer = 0;
      }
      // 3b ++NR_E_pointer
      NR_E_pointer = NR_E_pointer + 1;
      if (NR_E_pointer >= NR_N_frames) {
        NR_E_pointer = 0;
      }


#if 0
      for (int idx = 1; idx < 20; idx++) {      // bins 2 to 29 attenuated set real values to 0.1 of their original value
        NR_iFFT_buffer[idx * 2] *= 0.1;
        NR_iFFT_buffer[NR_FFT_L * 2 - ((idx + 1) * 2)] *= 0.1; //NR_iFFT_buffer[idx] * 0.1;
        NR_iFFT_buffer[idx * 2 + 1] *= 0.1; //NR_iFFT_buffer[idx] * 0.1;
        NR_iFFT_buffer[NR_FFT_L * 2 - ((idx + 1) * 2) + 1] *= 0.1; //NR_iFFT_buffer[idx] * 0.1;
      }
#endif
      arm_cfft_f32(NR_iFFT, NR_FFT_buffer, 1, 1);

#if 0
      // perform windowing on 256 real samples in the NR_FFT_buffer
      for (int idx = 0; idx < NR_FFT_L; idx++) { // sqrt Hann window
        NR_FFT_buffer[idx * 2] *= sqrtHann[idx];
      }
#endif
      for (int i = 0; i < NR_FFT_L / 2; i++) { // take real part of first half of current iFFT result and add to 2nd half of last iFFT_result
        NR_output_audio_buffer[i + k * (NR_FFT_L / 2)] = NR_FFT_buffer[i * 2] + NR_last_iFFT_result[i];
      }

      for (int i = 0; i < NR_FFT_L / 2; i++) {
        NR_last_iFFT_result[i] = NR_FFT_buffer[NR_FFT_L + i * 2];
      }
    }

    for (int i = 0; i < NR_FFT_L; i++) {
      float_buffer_L[i] = NR_output_audio_buffer[i]; // * 9.0; // * 5.0;
      float_buffer_R[i] = float_buffer_L[i];
    }
  } // end of Kim et al. 2002 algorithm

}
/*****
  Purpose:   void xanr
  Parameter list:
    void
  Return value;
    void
*****/
void Xanr() // variable leak LMS algorithm for automatic notch or noise reduction
{ // (c) Warren Pratt wdsp library 2016
  int idx;
  float32_t c0, c1;
  float32_t y, error, sigma, inv_sigp;
  float32_t nel, nev;

  for (int i = 0; i < ANR_buff_size; i++) {
    ANR_d[ANR_in_idx] = float_buffer_L[i];

    y = 0;
    sigma = 0;

    for (int j = 0; j < ANR_taps; j++) {
      idx = (ANR_in_idx + j + ANR_delay) & ANR_mask;
      y += ANR_w[j] * ANR_d[idx];
      sigma += ANR_d[idx] * ANR_d[idx];
    }
    inv_sigp = 1.0 / (sigma + 1e-10);
    error = ANR_d[ANR_in_idx] - y;

    if (ANR_notch)
      float_buffer_R[i] = error;                            // NOTCH FILTER
    else
      float_buffer_R[i] = y;                                // NOISE REDUCTION

    if ((nel = error * (1.0 - ANR_two_mu * sigma * inv_sigp)) < 0.0)
      nel = -nel;
    if ((nev = ANR_d[ANR_in_idx] - (1.0 - ANR_two_mu * ANR_ngamma) * y - ANR_two_mu * error * sigma * inv_sigp) < 0.0)
      nev = -nev;
    if (nev < nel) {
      if ((ANR_lidx += ANR_lincr) > ANR_lidx_max)
        ANR_lidx = ANR_lidx_max;
      else if ((ANR_lidx -= ANR_ldecr) < ANR_lidx_min)
        ANR_lidx = ANR_lidx_min;
    }
    ANR_ngamma = ANR_gamma * (ANR_lidx * ANR_lidx) * (ANR_lidx * ANR_lidx) * ANR_den_mult;

    c0 = 1.0 - ANR_two_mu * ANR_ngamma;
    c1 = ANR_two_mu * error * inv_sigp;

    for (int j = 0; j < ANR_taps; j++) {
      idx = (ANR_in_idx + j + ANR_delay) & ANR_mask;
      ANR_w[j] = c0 * ANR_w[j] + c1 * ANR_d[idx];
    }
    ANR_in_idx = (ANR_in_idx + ANR_mask) & ANR_mask;
  }
}

/*****
  Purpose: spectral_noise_reduction
  Parameter list:
    void
  Return value;
    void
*****/
void SpectralNoiseReduction()
/************************************************************************************************************

      Noise reduction with spectral subtraction rule
      based on Romanin et al. 2009 & Schmitt et al. 2002
      and MATLAB voicebox
      and Gerkmann & Hendriks 2002
      and Yao et al. 2016

   STAND: UHSDR github 14.1.2018
   ************************************************************************************************************/
{
  static uint8_t NR_init_counter = 0;
  uint8_t VAD_low = 0;
  uint8_t VAD_high = 127;
  float32_t lf_freq; // = (offset - width/2) / (12000 / NR_FFT_L); // bin BW is 46.9Hz [12000Hz / 256 bins] @96kHz
  float32_t uf_freq; //= (offset + width/2) / (12000 / NR_FFT_L);

  const float32_t tinc = 0.00533333;  // frame time 5.3333ms
  const float32_t tax = 0.0239;       // noise output smoothing time constant = -tinc/ln(0.8)
  const float32_t tap = 0.05062;      // speech prob smoothing time constant = -tinc/ln(0.9) tinc = frame time (5.33ms)
  const float32_t psthr = 0.99;       // threshold for smoothed speech probability [0.99]
  const float32_t pnsaf = 0.01;       // noise probability safety value [0.01]
  const float32_t asnr = 20;          // active SNR in dB
  const float32_t psini = 0.5;        // initial speech probability [0.5]
  const float32_t pspri = 0.5;        // prior speech probability [0.5]
  static float32_t ax;                //=0.8;       // ax=exp(-tinc/tax); % noise output smoothing factor
  static float32_t ap;                //=0.9;        // ap=exp(-tinc/tap); % noise output smoothing factor
  static float32_t xih1;              // = 31.6;
  ax = expf(-tinc / tax);
  ap = expf(-tinc / tap);
  xih1 = powf(10, (float32_t)asnr / 10.0);
  static float32_t xih1r = 1.0 / (1.0 + xih1) - 1.0;
  static float32_t pfac = (1.0 / pspri - 1.0) * (1.0 + xih1);
  float32_t snr_prio_min = powf(10, - (float32_t)20 / 20.0);
  static float32_t pslp[NR_FFT_L / 2];
  static float32_t xt[NR_FFT_L / 2];
  static float32_t xtr;
  static float32_t pre_power;
  static float32_t post_power;
  static float32_t power_ratio;
  static int16_t NN;
  const int16_t NR_width = 4;
  const float32_t power_threshold = 0.4;
  float32_t ph1y[NR_FFT_L / 2];
  static int NR_first_time_2 = 1;

  if (bands[currentBand].FLoCut <= 0 && bands[currentBand].FHiCut >= 0) {
    lf_freq = 0.0;
    uf_freq = fmax(-(float32_t)bands[currentBand].FLoCut, (float32_t)bands[currentBand].FHiCut);
  } else {
    if (bands[currentBand].FLoCut > 0) {
      lf_freq = (float32_t)bands[currentBand].FLoCut;
      uf_freq = (float32_t)bands[currentBand].FHiCut;
    } else {
      uf_freq = -(float32_t)bands[currentBand].FLoCut;
      lf_freq = -(float32_t)bands[currentBand].FHiCut;
    }
  }
  // / rate DF SR[SampleRate].rate/DF
  lf_freq /= ((SR[SampleRate].rate / DF) / NR_FFT_L); // bin BW is 46.9Hz [12000Hz / 256 bins] @96kHz
  uf_freq /= ((SR[SampleRate].rate / DF) / NR_FFT_L);


  // INITIALIZATION ONCE 1
  if (NR_first_time_2 == 1) { // TODO: properly initialize all the variables
    for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) {
      NR_last_sample_buffer_L[bindx] = 0.0;
      NR_G[bindx] = 1.0;
      //xu[bindx] = 1.0;  //has to be replaced by other variable
      NR_Hk_old[bindx] = 1.0; // old gain or xu in development mode
      NR_Nest[bindx][0] = 0.0;
      NR_Nest[bindx][1] = 1.0;
      pslp[bindx] = 0.5;
    }
    NR_first_time_2 = 2; // we need to do some more a bit later down
  }

  for (int k = 0; k < 2; k++) {
    // NR_FFT_buffer is 512 floats big
    // interleaved r, i, r, i . . .
    // fill first half of FFT_buffer with last events audio samples
    for (int i = 0; i < NR_FFT_L / 2; i++) {
      NR_FFT_buffer[i * 2] = NR_last_sample_buffer_L[i]; // real
      NR_FFT_buffer[i * 2 + 1] = 0.0; // imaginary
    }
    // copy recent samples to last_sample_buffer for next time!
    for (int i = 0; i < NR_FFT_L  / 2; i++) {
      NR_last_sample_buffer_L [i] = float_buffer_L[i + k * (NR_FFT_L / 2)];
    }
    // now fill recent audio samples into second half of FFT_buffer
    for (int i = 0; i < NR_FFT_L / 2; i++) {
      NR_FFT_buffer[NR_FFT_L + i * 2] = float_buffer_L[i + k * (NR_FFT_L / 2)]; // real
      NR_FFT_buffer[NR_FFT_L + i * 2 + 1] = 0.0;
    }
#if 1
    // perform windowing on samples in the NR_FFT_buffer
    for (int idx = 0; idx < NR_FFT_L; idx++) { // sqrt Hann window
      //float32_t temp_sample = 0.5 * (float32_t)(1.0 - (cosf(PI * 2.0 * (float32_t)idx / (float32_t)((NR_FFT_L) - 1))));
      //NR_FFT_buffer[idx * 2] *= temp_sample;
      NR_FFT_buffer[idx * 2] *= sqrtHann[idx];
    }
#endif

    // NR_FFT
    // calculation is performed in-place the FFT_buffer [re, im, re, im, re, im . . .]
    arm_cfft_f32(NR_FFT, NR_FFT_buffer, 0, 1);

    for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) {
      // this is squared magnitude for the current frame
      NR_X[bindx][0] = (NR_FFT_buffer[bindx * 2] * NR_FFT_buffer[bindx * 2] + NR_FFT_buffer[bindx * 2 + 1] * NR_FFT_buffer[bindx * 2 + 1]);
    }

    if (NR_first_time_2 == 2) { // TODO: properly initialize all the variables
      for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) {
        NR_Nest[bindx][0] = NR_Nest[bindx][0] + 0.05 * NR_X[bindx][0]; // we do it 20 times to average over 20 frames for app. 100ms only on NR_on/bandswitch/modeswitch,...
        xt[bindx] = psini * NR_Nest[bindx][0];
      }
      NR_init_counter++;
      if (NR_init_counter > 19)  { //average over 20 frames for app. 100ms
        NR_init_counter = 0;
        NR_first_time_2 = 3;  // now we did all the necessary initialization to actually start the noise reduction
      }
    }

    if (NR_first_time_2 == 3) {
      for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) { // 1. Step of NR - calculate the SNR's
        ph1y[bindx] = 1.0 / (1.0 + pfac * expf(xih1r * NR_X[bindx][0] / xt[bindx]));
        pslp[bindx] = ap * pslp[bindx] + (1.0 - ap) * ph1y[bindx];

        if (pslp[bindx] > psthr) {
          ph1y[bindx] = 1.0 - pnsaf;
        } else {
          ph1y[bindx] = fmin(ph1y[bindx] , 1.0);
        }
        xtr = (1.0 - ph1y[bindx]) * NR_X[bindx][0] + ph1y[bindx] * xt[bindx];
        xt[bindx] = ax * xt[bindx] + (1.0 - ax) * xtr;
      }
      for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) { // 1. Step of NR - calculate the SNR's
        NR_SNR_post[bindx] = fmax(fmin(NR_X[bindx][0] / xt[bindx], 1000.0), snr_prio_min); // limited to +30 /-15 dB, might be still too much of reduction, let's try it?
        NR_SNR_prio[bindx] = fmax(NR_alpha * NR_Hk_old[bindx] + (1.0 - NR_alpha) * fmax(NR_SNR_post[bindx] - 1.0, 0.0), 0.0);
      }

      VAD_low = (int)lf_freq;
      VAD_high = (int)uf_freq;
      if (VAD_low == VAD_high) {
        VAD_high++;
      }
      if (VAD_low < 1) {
        VAD_low = 1;
      } else if (VAD_low > NR_FFT_L / 2 - 2) {
        VAD_low = NR_FFT_L / 2 - 2;
      }
      if (VAD_high < 1) {
        VAD_high = 1;
      } else if (VAD_high > NR_FFT_L / 2) {
        VAD_high = NR_FFT_L / 2;
      }

      float32_t v;
      for (int bindx = VAD_low; bindx < VAD_high; bindx++) { // maybe we should limit this to the signal containing bins (filtering!!)
        {
          v = NR_SNR_prio[bindx] * NR_SNR_post[bindx] / (1.0 + NR_SNR_prio[bindx]);
          NR_G[bindx] = 1.0 / NR_SNR_post[bindx] * sqrtf((0.7212 * v + v * v));
          NR_Hk_old[bindx] = NR_SNR_post[bindx] * NR_G[bindx] * NR_G[bindx]; //
        }

        // MUSICAL NOISE TREATMENT HERE, DL2FW

        // musical noise "artefact" reduction by dynamic averaging - depending on SNR ratio
        pre_power  = 0.0;
        post_power = 0.0;
        for (int bindx = VAD_low; bindx < VAD_high; bindx++) {
          pre_power += NR_X[bindx][0];
          post_power += NR_G[bindx] * NR_G[bindx]  * NR_X[bindx][0];
        }

        power_ratio = post_power / pre_power;
        if (power_ratio > power_threshold) {
          power_ratio = 1.0;
          NN = 1;
        } else {
          NN = 1 + 2 * (int)(0.5 + NR_width * (1.0 - power_ratio / power_threshold));
        }

        for (int bindx = VAD_low + NN / 2; bindx < VAD_high - NN / 2; bindx++) {
          NR_Nest[bindx][0] = 0.0;
          for (int m = bindx - NN / 2; m <= bindx + NN / 2; m++) {
            NR_Nest[bindx][0] += NR_G[m];
          }
          NR_Nest[bindx][0] /= (float32_t)NN;
        }

        // and now the edges - only going NN steps forward and taking the average
        // lower edge
        for (int bindx = VAD_low; bindx < VAD_low + NN / 2; bindx++) {
          NR_Nest[bindx][0] = 0.0;
          for (int m = bindx; m < (bindx + NN); m++) {
            NR_Nest[bindx][0] += NR_G[m];
          }
          NR_Nest[bindx][0] /= (float32_t)NN;
        }

        // upper edge - only going NN steps backward and taking the average
        for (int bindx = VAD_high - NN; bindx < VAD_high; bindx++) {
          NR_Nest[bindx][0] = 0.0;
          for (int m = bindx; m > (bindx - NN); m--) {
            NR_Nest[bindx][0] += NR_G[m];
          }
          NR_Nest[bindx][0] /= (float32_t)NN;
        }

        // end of edge treatment

        for (int bindx = VAD_low + NN / 2; bindx < VAD_high - NN / 2; bindx++) {
          NR_G[bindx] = NR_Nest[bindx][0];
        }
        // end of musical noise reduction
      } //end of "if ts.nr_first_time == 3"

#if 1
      // FINAL SPECTRAL WEIGHTING: Multiply current FFT results with NR_FFT_buffer for 128 bins with the 128 bin-specific gain factors G
      //              for(int bindx = 0; bindx < NR_FFT_L / 2; bindx++)                 // try 128:
      for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++) {                              // try 128:
        NR_FFT_buffer[bindx * 2]                    = NR_FFT_buffer [bindx * 2] * NR_G[bindx] * NR_long_tone_gain[bindx];              // real part
        NR_FFT_buffer[bindx * 2 + 1]                = NR_FFT_buffer [bindx * 2 + 1] * NR_G[bindx] * NR_long_tone_gain[bindx];      // imag part
        NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 2] = NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 2] * NR_G[bindx] * NR_long_tone_gain[bindx]; // real part conjugate symmetric
        NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 1] = NR_FFT_buffer[NR_FFT_L * 2 - bindx * 2 - 1] * NR_G[bindx] * NR_long_tone_gain[bindx]; // imag part conjugate symmetric
      }

#endif
      /*****************************************************************
         NOISE REDUCTION CODE ENDS HERE
       *****************************************************************/
      // very interesting!
      // if I leave the FFT_buffer as is and just multiply the 19 bins below with 0.1, the audio
      // is distorted a little bit !
      // To me, this is an indicator of a problem with windowing . . .
      //

#if 0
      for (int bindx = 1; bindx < 20; bindx++)
        // bins 2 to 29 attenuated
        // set real values to 0.1 of their original value
      {
        NR_FFT_buffer[bindx * 2] *= 0.1;
        NR_FFT_buffer[bindx * 2 + 1] *= 0.1; //NR_iFFT_buffer[idx] * 0.1;
      }
#endif

      arm_cfft_f32(NR_iFFT, NR_FFT_buffer, 1, 1);

      for (int idx = 0; idx < NR_FFT_L; idx++) {
        NR_FFT_buffer[idx * 2] *= sqrtHann[idx];      // sqrt Hann window
      }

      // do the overlap & add
      for (int i = 0; i < NR_FFT_L / 2; i++) {        // take real part of first half of current iFFT result and add to 2nd half of last iFFT_result
        float_buffer_L[i + k * (NR_FFT_L / 2)] = NR_FFT_buffer[i * 2] + NR_last_iFFT_result[i];
        float_buffer_R[i + k * (NR_FFT_L / 2)] = float_buffer_L[i + k * (NR_FFT_L / 2)];
      }
      for (int i = 0; i < NR_FFT_L / 2; i++) {
        NR_last_iFFT_result[i] = NR_FFT_buffer[NR_FFT_L + i * 2];
      }
      // end of "for" loop which repeats the FFT_iFFT_chain two times !!!
    }
  }
}

/*****
  Purpose: void LMSNoiseReduction(
  
  Parameter list:
    void
    
  Return value;
    void
*****/
void LMSNoiseReduction(int16_t blockSize, float32_t *nrbuffer)
{
  static ulong    lms1_inbuf = 0, lms1_outbuf = 0;

  arm_copy_f32(nrbuffer, &LMS_nr_delay[lms1_inbuf], blockSize);  // put new data into the delay buffer

  arm_lms_norm_f32(&LMS_Norm_instance, nrbuffer, &LMS_nr_delay[lms1_outbuf], nrbuffer, LMS_errsig1, blockSize);  // do noise reduction

  lms1_inbuf += blockSize;  // bump input to the next location in our de-correlation buffer
  lms1_outbuf = lms1_inbuf + blockSize; // advance output to same distance ahead of input
  lms1_inbuf %= 512;
  lms1_outbuf %= 512;
}

/*****
  Purpose: void InitLMSNoiseReduction()
  
  Parameter list:
    void
    
  Return value;
    void
*****/
void InitLMSNoiseReduction()
{
  uint16_t  calc_taps = 96;
  float32_t mu_calc;

  LMS_Norm_instance.numTaps = calc_taps;
  LMS_Norm_instance.pCoeffs = LMS_NormCoeff_f32;
  LMS_Norm_instance.pState = LMS_StateF32;

  // Calculate "mu" (convergence rate) from user "DSP Strength" setting.  This needs to be significantly de-linearized to
  // squeeze a wide range of adjustment (e.g. several magnitudes) into a fairly small numerical range.
  mu_calc = LMS_nr_strength;   // get user setting

  // New DSP NR "mu" calculation method as of 0.0.214
  mu_calc /= 2; // scale input value
  mu_calc += 2; // offset zero value
  mu_calc /= 10;  // convert from "bels" to "deci-bels"
  mu_calc = powf(10, mu_calc);  // convert to ratio
  mu_calc = 1 / mu_calc;    // invert to fraction
  LMS_Norm_instance.mu = mu_calc;

  arm_fill_f32(0.0, LMS_nr_delay, 512 + 256);
  arm_fill_f32(0.0, LMS_StateF32, 96 + 256);

  // use "canned" init to initialize the filter coefficients
  arm_lms_norm_init_f32(&LMS_Norm_instance, calc_taps, &LMS_NormCoeff_f32[0], &LMS_StateF32[0], mu_calc, 256);

}
/*****
  Purpose:
  Parameter list:
    void
  Return value;
    void
*****/
void SpectralNoiseReductionInit()
{
  for (int bindx = 0; bindx < NR_FFT_L / 2; bindx++)
  {
    NR_last_sample_buffer_L[bindx] = 0.1;
    NR_Hk_old[bindx] = 0.1; // old gain
    NR_Nest[bindx][0] = 0.01;
    NR_Nest[bindx][1] = 0.015;
    NR_Gts[bindx][1] = 0.1;
    NR_M[bindx] = 500.0;
    NR_E[bindx][0] = 0.1;
    NR_X[bindx][1] = 0.5;
    NR_SNR_post[bindx] = 2.0;
    NR_SNR_prio[bindx] = 1.0;
    NR_first_time = 2;
    NR_long_tone_gain[bindx] = 1.0;
  }
}
