#ifndef BEENHERE
#include "SDT.h"
#endif


/*****
  Purpose: void DoReceiveEQ  Parameter list:
    void
  Return value;
    void
*****/
void DoReceiveEQ() //AFP 08-09-22
{
  for (int i = 0; i < 14; i++) {
    recEQ_LevelScale[i] = (float)EEPROMData.equalizerRec[i] / 100.0;
  }
  arm_biquad_cascade_df2T_f32(&S1_Rec, float_buffer_L, rec_EQ1_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S2_Rec, float_buffer_L, rec_EQ2_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S3_Rec, float_buffer_L, rec_EQ3_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S4_Rec, float_buffer_L, rec_EQ4_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S5_Rec, float_buffer_L, rec_EQ5_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S6_Rec, float_buffer_L, rec_EQ6_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S7_Rec, float_buffer_L, rec_EQ7_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S8_Rec, float_buffer_L, rec_EQ8_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S9_Rec, float_buffer_L, rec_EQ9_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S10_Rec, float_buffer_L, rec_EQ10_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S11_Rec, float_buffer_L, rec_EQ11_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S12_Rec, float_buffer_L, rec_EQ12_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S13_Rec, float_buffer_L, rec_EQ13_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S14_Rec, float_buffer_L, rec_EQ14_float_buffer_L, 256);

  arm_scale_f32(rec_EQ1_float_buffer_L, -recEQ_LevelScale[0], rec_EQ1_float_buffer_L, 256);
  arm_scale_f32(rec_EQ2_float_buffer_L, recEQ_LevelScale[1], rec_EQ2_float_buffer_L, 256);
  arm_scale_f32(rec_EQ3_float_buffer_L, -recEQ_LevelScale[2], rec_EQ3_float_buffer_L, 256);
  arm_scale_f32(rec_EQ4_float_buffer_L, recEQ_LevelScale[3], rec_EQ4_float_buffer_L, 256);
  arm_scale_f32(rec_EQ5_float_buffer_L, -recEQ_LevelScale[4], rec_EQ5_float_buffer_L, 256);
  arm_scale_f32(rec_EQ6_float_buffer_L, recEQ_LevelScale[5], rec_EQ6_float_buffer_L, 256);
  arm_scale_f32(rec_EQ7_float_buffer_L, -recEQ_LevelScale[6], rec_EQ7_float_buffer_L, 256);
  arm_scale_f32(rec_EQ8_float_buffer_L, recEQ_LevelScale[7], rec_EQ8_float_buffer_L, 256);
  arm_scale_f32(rec_EQ9_float_buffer_L, -recEQ_LevelScale[8], rec_EQ9_float_buffer_L, 256);
  arm_scale_f32(rec_EQ10_float_buffer_L, recEQ_LevelScale[9], rec_EQ10_float_buffer_L, 256);
  arm_scale_f32(rec_EQ11_float_buffer_L, -recEQ_LevelScale[10], rec_EQ11_float_buffer_L, 256);
  arm_scale_f32(rec_EQ12_float_buffer_L, recEQ_LevelScale[11], rec_EQ12_float_buffer_L, 256);
  arm_scale_f32(rec_EQ13_float_buffer_L, -recEQ_LevelScale[12], rec_EQ13_float_buffer_L, 256);
  arm_scale_f32(rec_EQ14_float_buffer_L, recEQ_LevelScale[13], rec_EQ14_float_buffer_L, 256);

  arm_add_f32(rec_EQ1_float_buffer_L , rec_EQ2_float_buffer_L, float_buffer_L , 256 ) ;

  arm_add_f32(float_buffer_L , rec_EQ3_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ4_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ5_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ6_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ7_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ8_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ9_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ10_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ11_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ12_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ13_float_buffer_L, float_buffer_L , 256 ) ;
  arm_add_f32(float_buffer_L , rec_EQ14_float_buffer_L, float_buffer_L , 256 ) ;
}

/*****
  Purpose: void DoExciterEQ

  Parameter list:
    void
  Return value;
    void
*****/
void DoExciterEQ() //AFP 10-02-22
{
  for (int i = 0; i < 14; i++) {
    xmtEQ_Level[i] = (float)EEPROMData.equalizerXmt[i] / 100.0;
  }
  arm_biquad_cascade_df2T_f32(&S1_Xmt,  float_buffer_L_EX, xmt_EQ1_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S2_Xmt,  float_buffer_L_EX, xmt_EQ2_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S3_Xmt,  float_buffer_L_EX, xmt_EQ3_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S4_Xmt,  float_buffer_L_EX, xmt_EQ4_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S5_Xmt,  float_buffer_L_EX, xmt_EQ5_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S6_Xmt,  float_buffer_L_EX, xmt_EQ6_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S7_Xmt,  float_buffer_L_EX, xmt_EQ7_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S8_Xmt,  float_buffer_L_EX, xmt_EQ8_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S9_Xmt,  float_buffer_L_EX, xmt_EQ9_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S10_Xmt, float_buffer_L_EX, xmt_EQ10_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S11_Xmt, float_buffer_L_EX, xmt_EQ11_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S12_Xmt, float_buffer_L_EX, xmt_EQ12_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S13_Xmt, float_buffer_L_EX, xmt_EQ13_float_buffer_L, 256);
  arm_biquad_cascade_df2T_f32(&S14_Xmt, float_buffer_L_EX, xmt_EQ14_float_buffer_L, 256);

  arm_scale_f32(xmt_EQ1_float_buffer_L,  -xmtEQ_Level[0],  xmt_EQ1_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ2_float_buffer_L,   xmtEQ_Level[1],  xmt_EQ2_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ3_float_buffer_L,  -xmtEQ_Level[2],  xmt_EQ3_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ4_float_buffer_L,   xmtEQ_Level[3],  xmt_EQ4_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ5_float_buffer_L,  -xmtEQ_Level[4],  xmt_EQ5_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ6_float_buffer_L,   xmtEQ_Level[5],  xmt_EQ6_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ7_float_buffer_L,  -xmtEQ_Level[6],  xmt_EQ7_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ8_float_buffer_L,   xmtEQ_Level[7],  xmt_EQ8_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ9_float_buffer_L,  -xmtEQ_Level[8],  xmt_EQ9_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ10_float_buffer_L,  xmtEQ_Level[9],  xmt_EQ10_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ11_float_buffer_L, -xmtEQ_Level[10], xmt_EQ11_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ12_float_buffer_L,  xmtEQ_Level[11], xmt_EQ12_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ13_float_buffer_L, -xmtEQ_Level[12], xmt_EQ13_float_buffer_L, 256);
  arm_scale_f32(xmt_EQ14_float_buffer_L,  xmtEQ_Level[13], xmt_EQ14_float_buffer_L, 256);

  arm_add_f32(xmt_EQ1_float_buffer_L , xmt_EQ2_float_buffer_L, float_buffer_L_EX , 256 ) ;

  arm_add_f32(float_buffer_L_EX , xmt_EQ3_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ4_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ5_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ6_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ7_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ8_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ9_float_buffer_L,  float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ10_float_buffer_L, float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ11_float_buffer_L, float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ12_float_buffer_L, float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ13_float_buffer_L, float_buffer_L_EX , 256 ) ;
  arm_add_f32(float_buffer_L_EX , xmt_EQ14_float_buffer_L, float_buffer_L_EX , 256 ) ;
}

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
  //int filter_BW_highest = bands[currentBand].FHiCut;
  //if (filter_BW_highest < -bands[currentBand].FLoCut) {
  // filter_BW_highest = -bands[currentBand].FLoCut;
  // }
  //SetIIRCoeffs((float32_t)filter_BW_highest, 1.3, (float32_t)SR[SampleRate].rate / DF, 0); // 1st stage

  for (int i = 0; i < 5; i++) {
    biquad_lowpass1_coeffs[i] = coefficient_set[i];
  }

  // and adjust decimation and interpolation filters
  SetDecIntFilters();
  ShowBandwidth();
//BandInformation();
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

  for (unsigned i = 0; i < m_NumTaps; i++) {
    // try out a window function to eliminate ringing of the filter at the stop frequency
    //             sd.FFT_Samples[i] = (float32_t)((0.53836 - (0.46164 * arm_cos_f32(PI*2 * (float32_t)i / (float32_t)(FFT_IQ_BUFF_LEN-1)))) * sd.FFT_Samples[i]);
    FIR_filter_mask[i * 2] = FIR_Coef_I [i];
    FIR_filter_mask[i * 2 + 1] = FIR_Coef_Q [i];
  }

  for (unsigned i = FFT_length + 1; i < FFT_length * 2; i++) {
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

  //== AFP 10-27-22
  switch (bands[currentBand].mode) {
    case DEMOD_IQ:
      bands[currentBand].FLoCut = - bands[currentBand].FHiCut;
      break;
    case DEMOD_LSB:
      if (bands[currentBand].FHiCut > 0) bands[currentBand].FHiCut = -100;

      break;
    case DEMOD_USB:
      if (bands[currentBand].FLoCut < 0) bands[currentBand].FLoCut = 100;  // AFP 10-27-22
      break;
    case DEMOD_AM:
      bands[currentBand].FLoCut = - bands[currentBand].FHiCut;
      //bands[currentBand].FHiCut= 4000;
      break;
    case DEMOD_SAM:               //== AFP 11-04-22
      bands[currentBand].FLoCut = - bands[currentBand].FHiCut;
      break;
  }   //== AFP 10-27-22
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
