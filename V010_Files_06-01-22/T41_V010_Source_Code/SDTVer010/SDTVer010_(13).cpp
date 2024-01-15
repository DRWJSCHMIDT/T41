#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: void FilterBandwidth()  Parameter list:
    void
  Return value;
    void
*****/
void FilterBandwidth()
{
  AudioNoInterrupts();

  CalcCplxFIRCoeffs(FIR_Coef_I, FIR_Coef_Q, m_NumTaps, (float32_t)bands[currentBand].FLoCut, (float32_t)bands[currentBand].FHiCut, (float)SR[SampleRate].rate / DF);
  InitFilterMask();

  // also adjust IIR AM filter
  int filter_BW_highest = bands[currentBand].FHiCut;
  if (filter_BW_highest < -bands[currentBand].FLoCut) 
    filter_BW_highest = -bands[currentBand].FLoCut;
  SetIIRCoeffs((float32_t)filter_BW_highest, 1.3, (float32_t)SR[SampleRate].rate / DF, 0); // 1st stage
  for (int i = 0; i < 5; i++)
  {
    biquad_lowpass1_coeffs[i] = coefficient_set[i];
  }

  // and adjust decimation and interpolation filters
  SetDecIntFilters();
  ShowBandwidth();

  MyDelay(1L);
  AudioInterrupts();
} // end filter_bandwidth

/*****
  Purpose: InitFilterMask()

  Parameter list:
    void

  Return value;
    void
*****/
void InitFilterMask()
{

  /****************************************************************************************
     Calculate the FFT of the FIR filter coefficients once to produce the FIR filter mask
  ****************************************************************************************/
  // the FIR has exactly m_NumTaps and a maximum of (FFT_length / 2) + 1 taps = coefficients, so we have to add (FFT_length / 2) -1 zeros before the FFT
  // in order to produce a FFT_length point input buffer for the FFT
  // copy coefficients into real values of first part of buffer, rest is zero

  for (unsigned i = 0; i < m_NumTaps; i++)
  {
    // try out a window function to eliminate ringing of the filter at the stop frequency
    //             sd.FFT_Samples[i] = (float32_t)((0.53836 - (0.46164 * arm_cos_f32(PI*2 * (float32_t)i / (float32_t)(FFT_IQ_BUFF_LEN-1)))) * sd.FFT_Samples[i]);
    FIR_filter_mask[i * 2] = FIR_Coef_I [i];
    FIR_filter_mask[i * 2 + 1] = FIR_Coef_Q [i];
  }

  for (unsigned i = FFT_length + 1; i < FFT_length * 2; i++)
  {
    FIR_filter_mask[i] = 0.0;
  }

  // FFT of FIR_filter_mask
  // perform FFT (in-place), needs only to be done once (or every time the filter coeffs change)
  arm_cfft_f32(maskS, FIR_filter_mask, 0, 1);

} // end init_filter_mask

/*****
  Purpose: void control_filter_f()
  Parameter list:
    void
  Return value;
    void
*****/
void ControlFilterF() 
{

  // low Fcut must never be larger than high Fcut and vice versa
  if (bands[currentBand].FHiCut < bands[currentBand].FLoCut)
    bands[currentBand].FHiCut = bands[currentBand].FLoCut;
  if (bands[currentBand].FLoCut > bands[currentBand].FHiCut)
    bands[currentBand].FLoCut = bands[currentBand].FHiCut;
    
                                                                // calculate maximum possible FHiCut
  float32_t sam = (SR[SampleRate].rate / (DF * 2.0)) - 100.0;
                                                                // clamp FHicut and Flowcut to max / min values
  if (bands[currentBand].FHiCut > (int)(sam)) {
    bands[currentBand].FHiCut = (int)sam;
  } else {
    if (bands[currentBand].FHiCut < -(int)(sam - 100.0)) {
      bands[currentBand].FHiCut = -(int)(sam - 100.0);\
    }
  }

  if (bands[currentBand].FLoCut > (int)(sam - 100.0)) {
    bands[currentBand].FLoCut = (int)(sam - 100.0);
  } else {
    if (bands[currentBand].FLoCut < -(int)(sam)) {
      bands[currentBand].FLoCut = -(int)(sam);
    }
  }

  switch (bands[currentBand].mode) {
    case DEMOD_IQ:
      bands[currentBand].FLoCut = - bands[currentBand].FHiCut;
      break;
    case DEMOD_LSB:
      if (bands[currentBand].FHiCut > 0) bands[currentBand].FHiCut = 0;
      break;
    case DEMOD_USB:
      if (bands[currentBand].FLoCut < 0) bands[currentBand].FLoCut = 0;
      break;
  }
}
/*****
  Purpose: void SetDecIntFilters()
  Parameter list:
    void
  Return value;
    void
*****/
void SetDecIntFilters()
{
  /****************************************************************************************
     Recalculate decimation and interpolation FIR filters
  ****************************************************************************************/
  int filter_BW_highest = bands[currentBand].FHiCut;
  
  if (filter_BW_highest < - bands[currentBand].FLoCut) {
    filter_BW_highest = - bands[currentBand].FLoCut;
  }
  LP_F_help = filter_BW_highest;

  if (LP_F_help > 10000) {
    LP_F_help = 10000;
  }
  CalcFIRCoeffs(FIR_dec1_coeffs, n_dec1_taps, (float32_t)(LP_F_help), n_att, 0, 0.0, (float32_t)(SR[SampleRate].rate));
  CalcFIRCoeffs(FIR_dec2_coeffs, n_dec2_taps, (float32_t)(LP_F_help), n_att, 0, 0.0, (float32_t)(SR[SampleRate].rate / DF1));

  CalcFIRCoeffs(FIR_int1_coeffs, 48, (float32_t)(LP_F_help), n_att, 0, 0.0, (float32_t)(SR[SampleRate].rate / DF1));
  CalcFIRCoeffs(FIR_int2_coeffs, 32, (float32_t)(LP_F_help), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);
  bin_BW = 1.0 / (DF * FFT_length) * (float32_t)SR[SampleRate].rate;
}
