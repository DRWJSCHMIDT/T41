#ifndef BEENHERE
#include "SDT.h"
#endif

/*****
  Purpose: void FreqShift1()
          AFP 12-31-20
        Frequency translation by Fs/4 without multiplication from Lyons (2011): chapter 13.1.2 page 646
        together with the savings of not having to shift/rotate the FFT_buffer, this saves
        about 1% of processor use

        This is for +Fs/4 [moves receive frequency to the left in the spectrum display]
           float_buffer_L contains I = real values
           float_buffer_R contains Q = imaginary values
           xnew(0) =  xreal(0) + jximag(0)
               leave first value (DC component) as it is!
           xnew(1) =  - ximag(1) + jxreal(1)
  Parameter list:
    void

  Return value:
    void
*****/
void FreqShift1()
{
  for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS; i += 4) {
    hh1 = - float_buffer_R[i + 1];  // xnew(1) =  - ximag(1) + jxreal(1)
    hh2 =   float_buffer_L[i + 1];
    float_buffer_L[i + 1] = hh1;
    float_buffer_R[i + 1] = hh2;
    hh1 = - float_buffer_L[i + 2];
    hh2 = - float_buffer_R[i + 2];
    float_buffer_L[i + 2] = hh1;
    float_buffer_R[i + 2] = hh2;
    hh1 =   float_buffer_R[i + 3];
    hh2 = - float_buffer_L[i + 3];
    float_buffer_L[i + 3] = hh1;
    float_buffer_R[i + 3] = hh2;
  }
  for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS; i ++) {
    float_buffer_L_3[i] = float_buffer_L[i];
    float_buffer_R_3[i] = float_buffer_R[i];
  }
  // this is for -Fs/4 [moves receive frequency to the right in the spectrumdisplay]
}

/*****
  Purpose: Shift Receive frequency by an arbitray amount
  
  Parameter list:
    void
    
  Return value;
    void
    Notes:  Routine includes checks to ensure the frequency selection stays within the bounds of the
    displayed spectrum
    Also included a variable frequency step, depending on how fast the encoder id turned.  Step varies from 50Hz/step to 10KHz/step
*****/
void FreqShift2() 
{
 
/*************************************************************************************************  AFP 12-13-21
    freq_conv2()

    FREQUENCY CONVERSION USING A SOFTWARE QUADRATURE OSCILLATOR (NCO)

    THIS VERSION calculates the COS AND SIN WAVE on the fly AND IS SLOW

    MAJOR ADVANTAGE: frequency conversion can be done for any frequency !

    large parts of the code taken from the mcHF code by Clint, KA7OEI, thank you!
      see here for more info on quadrature oscillators:
    Wheatley, M. (2011): CuteSDR Technical Manual Ver. 1.01. - http://sourceforge.net/projects/cutesdr/
    Lyons, R.G. (2011): Understanding Digital Processing. – Pearson, 3rd edition.
    Requires 4 complex multiplies and two adds per data point within the time domain buffer.  Applied after the data
    stream is sent to the Zoom FFT , but befor decimation.
 *************************************************************************************************/
 
  
//  static long currentFreqAOld;
  static long oldEncoderTime;
  static long oldFineEncoderRead;
  long encoderTime;
  int encoderDelta;
  long currentSpectrumWidth = SR[SampleRate].rate / (1 << spectrum_zoom); // Zoom level spectrum width in KHz AFP 04-17-22

  fineEncoderRead = fastTuneEncoder.read(); 
  encoderDelta = abs(fineEncoderRead - oldFineEncoderRead);
  if (encoderDelta > 0) {
    encoderTime = millis();
  }

  if (fineEncoderRead >= 1190) {
    fineEncoderRead = 1190;
    fastTuneEncoder.write(1190);
  }
  if (fineEncoderRead < -650) {
    fineEncoderRead = -650;
    fastTuneEncoder.write(-650);
  }

  if ((oldEncoderTime - encoderTime) < 200 and encoderDelta>2) { // Check to see how long an encoder change lasts.  If <200ms increase frequency step
    if (encoderDelta > 10) 
      stepFT2 = 10000;
    if (encoderDelta > 5 and encoderDelta <= 10)  
      stepFT2 = 5000;
    if (encoderDelta > 3 and encoderDelta <= 5) 
      stepFT2 = 1000;
    if (encoderDelta > 2 and encoderDelta <= 3) 
      stepFT2 = 500;
  } else {
    if (encoderDelta == 1 || encoderDelta == 2 ) {  //Encoderchange time >200ms
      stepFT2 = 50;
    }
  }
  int currentStep = fineEncoderRead - 256;

  NCO_FREQ = stepFT2 * currentStep;   //AFP 12-24-21
  encoderStepOld = currentStep;

  TxRxFreq = centerFreq + NCO_FREQ;   //AFP 12-24-21
  currentFreqA = TxRxFreq; // AFP 04-16-22

  if (encoderDelta > 0 and abs(NCO_FREQ) < currentSpectrumWidth / 2 ) { 
    ShowFrequency();// AFP 04-17-22
  }
  NCO_INC = 2.0 * PI * NCO_FREQ / 192000.0; //192000 SPS is the actual sample rate used in the Receive ADC

  OSC_COS = cos (NCO_INC);
  OSC_SIN = sin (NCO_INC);
  uint     i;
  for (i = 0; i < BUFFER_SIZE * N_BLOCKS; i++) {
                                  // generate local oscillator on-the-fly:  This takes a lot of processor time!
    Osc_Q = (Osc_Vect_Q * OSC_COS) - (Osc_Vect_I * OSC_SIN);  // Q channel of oscillator
    Osc_I = (Osc_Vect_I * OSC_COS) + (Osc_Vect_Q * OSC_SIN);  // I channel of oscillator
    Osc_Gain = 1.95 - ((Osc_Vect_Q * Osc_Vect_Q) + (Osc_Vect_I * Osc_Vect_I));  // Amplitude control of oscillator

                                  // rotate vectors while maintaining constant oscillator amplitude
    Osc_Vect_Q = Osc_Gain * Osc_Q;
    Osc_Vect_I = Osc_Gain * Osc_I;

                                  // do actual frequency conversion
    float freqAdjFactor = 1.1;
    float_buffer_L[i] = (float_buffer_L_3[i] * freqAdjFactor * Osc_Q) + (float_buffer_R_3[i] * freqAdjFactor * Osc_I); // multiply I/Q data by sine/cosine data to do translation
    float_buffer_R[i] = (float_buffer_R_3[i] * freqAdjFactor * Osc_Q) - (float_buffer_L_3[i] * freqAdjFactor * Osc_I);
  }
//  currentFreqAOld    = currentFreq;
  oldEncoderTime     = encoderTime;
  oldFineEncoderRead = fineEncoderRead;
  stepFT2            = stepFT;
}
