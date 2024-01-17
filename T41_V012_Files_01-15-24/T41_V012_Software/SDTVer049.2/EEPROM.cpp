#ifndef BEENHERE
#include "SDT.h"
#endif

//DB2OO, 29-AUG-23: Don't use the overall VERSION for the EEPROM structure version information, but use a combination of an EEPROM_VERSION with the size of the EEPROMData variable.
// The "EEPROM_VERSION" should only be changed, if the structure config_t EEPROMData has changed!
// For V049.1 the new version in EEPROM will be "V049_808", for V049.2 it will be "V049_812"
#define EEPROM_VERSION  "V049"
static char version_size[10];

/*****
  Purpose: void EEPROMSetVersion()

  Parameter list:
    

  Return value;
    char* pointer to EEPROM version string of the form "V049_808"
*****/
static char* EEPROMSetVersion(void)
{
    size_t  l;
  strncpy(version_size, EEPROM_VERSION, sizeof(version_size));
  l = strlen(version_size);
  //enough space to append '_' and 4 characters for size
  if ((sizeof(version_size)-l) > 5) {
    version_size[l] = '_';
    itoa(sizeof(EEPROMData), version_size+l+1, 10);
  }
  return version_size;
}

/*****
  Purpose: void EEPROMRead()

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMRead() {
  int i;
  int v049_version=0; //DB2OO, 10-SEP-23
#define MORSE_STRING_DISPLAY(s)  {size_t j; for (j=0;j<strlen(s);j++) MorseCharacterDisplay(s[j]);}  

  //DB2OO, 25-AUG-23: don't read the EEPROM before you are sure, that it is in T41-SDT format!!
  //DB2OO, 25-AUG-23: first read only the version string and compare it with the current version. The version string must also be at the beginning of the EEPROMData structure!
  for (i=0; i<10; i++) { versionSettings[i]= EEPROM.read(EEPROM_BASE_ADDRESS+i); }
#ifdef DEBUG 
    //display version in EEPROM in last line of display
    MORSE_STRING_DISPLAY("EEPROMVersion ");
    if (strlen(versionSettings) <10) {
      MORSE_STRING_DISPLAY(versionSettings);
    }else {
      MORSE_STRING_DISPLAY("<<INVALID>>");
    }
    MyDelay(1000);
#endif
  //Do we have V049.1 or V049.2 structure in EEPROM?
  if (strcmp("V049.1", versionSettings) == 0) v049_version=1;
  if (strcmp("V049.2", versionSettings) == 0) v049_version=2;

  if (v049_version > 0) {
     //DB2OO, 29-AUG-23: allow "V049.1" or "V049.2" instead of the Version with size for a migration to the new format
     strcpy(versionSettings, EEPROMSetVersion());  // set new version format
    for (i=0; i<10; i++) { EEPROM.write(EEPROM_BASE_ADDRESS+i, versionSettings[i]); }
  }
  if (strncmp(EEPROMSetVersion(), versionSettings, 10) != 0) {
    //Different version in EEPROM: set the EEPROM values for THIS version
#ifdef DEBUG  
    const char *wrong_ver = "EEPROMRead(): different version, calling EEPROMSaveDefaults2()";
    MORSE_STRING_DISPLAY(wrong_ver);
    Serial.println(wrong_ver);
    MyDelay(1000);
#endif    
    EEPROMSaveDefaults2();
    // and write it into the EEPROM
    EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);
    // after this we will read the default values for this version
  } else {
#ifdef DEBUG  
    MORSE_STRING_DISPLAY("-->Reading EEPROM content...");
    MyDelay(1000);
#endif
  }
#ifdef DEBUG
  //clear the Morse character buffer
  MorseCharacterClear();
#endif  
  
  EEPROM.get(EEPROM_BASE_ADDRESS, EEPROMData);  // Read as one large chunk

  strncpy(versionSettings, EEPROMData.versionSettings, 10);  // KF5N
  AGCMode = EEPROMData.AGCMode;
  audioVolume = EEPROMData.audioVolume;  // 4 bytes
  rfGainAllBands = EEPROMData.rfGainAllBands;
  spectrumNoiseFloor = EEPROMData.spectrumNoiseFloor;  // AFP 09-26-22
  tuneIndex = EEPROMData.tuneIndex;
  stepFineTune = EEPROMData.stepFineTune;
  transmitPowerLevel = EEPROMData.powerLevel;
  xmtMode = EEPROMData.xmtMode;                // AFP 09-26-22
  nrOptionSelect = EEPROMData.nrOptionSelect;  // 1 byte
  currentScale = EEPROMData.currentScale;
  spectrum_zoom = EEPROMData.spectrum_zoom;
  spectrum_display_scale = EEPROMData.spectrum_display_scale;  // 4 bytes

  CWFilterIndex = EEPROMData.CWFilterIndex;  // Off
  paddleDit = EEPROMData.paddleDit;
  paddleDah = EEPROMData.paddleDah;
  decoderFlag = EEPROMData.decoderFlag;
  keyType = EEPROMData.keyType;                  // straight key = 0, keyer = 1
  currentWPM = EEPROMData.currentWPM;            // 4 bytes
  sidetoneVolume = EEPROMData.sidetoneVolume;    // 4 bytes
  cwTransmitDelay = EEPROMData.cwTransmitDelay;  // 4 bytes

  activeVFO = EEPROMData.activeVFO;          // 2 bytes
  freqIncrement = EEPROMData.freqIncrement;  // 4 bytes

  currentBand = EEPROMData.currentBand;                        // 4 bytes
  currentBandA = EEPROMData.currentBandA;                      // 4 bytes
  currentBandB = EEPROMData.currentBandB;                      // 4 bytes
  currentFreqA = EEPROMData.lastFrequencies[currentBandA][0];  // JJP 7/17/23
  currentFreqB = EEPROMData.lastFrequencies[currentBandB][1];  // JJP 7/17/23
  freqCorrectionFactor = EEPROMData.freqCorrectionFactor;

  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    recEQ_Level[i] = EEPROMData.equalizerRec[i];  // 4 bytes each
  }
  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    xmtEQ_Level[i] = EEPROMData.equalizerXmt[i];
  }

  currentMicThreshold = EEPROMData.currentMicThreshold;  // AFP 09-22-22
  currentMicCompRatio = EEPROMData.currentMicCompRatio;
  currentMicAttack = EEPROMData.currentMicAttack;
  currentMicRelease = EEPROMData.currentMicRelease;
  currentMicGain = EEPROMData.currentMicGain;

  //  Note: switch values are read and written to EEPROM only
  switchThreshholds[0] = EEPROMData.switchValues[0];
  switchThreshholds[1] = EEPROMData.switchValues[1];
  switchThreshholds[2] = EEPROMData.switchValues[2];
  switchThreshholds[3] = EEPROMData.switchValues[3];
  switchThreshholds[4] = EEPROMData.switchValues[4];
  switchThreshholds[5] = EEPROMData.switchValues[5];
  switchThreshholds[6] = EEPROMData.switchValues[6];
  switchThreshholds[7] = EEPROMData.switchValues[7];
  switchThreshholds[8] = EEPROMData.switchValues[8];
  switchThreshholds[9] = EEPROMData.switchValues[9];
  switchThreshholds[10] = EEPROMData.switchValues[10];
  switchThreshholds[11] = EEPROMData.switchValues[11];
  switchThreshholds[12] = EEPROMData.switchValues[12];
  switchThreshholds[13] = EEPROMData.switchValues[13];
  switchThreshholds[14] = EEPROMData.switchValues[14];
  switchThreshholds[15] = EEPROMData.switchValues[15];
  switchThreshholds[16] = EEPROMData.switchValues[16];
  switchThreshholds[17] = EEPROMData.switchValues[17];

  LPFcoeff = EEPROMData.LPFcoeff;  // 4 bytes
  NR_PSI = EEPROMData.NR_PSI;      // 4 bytes
  NR_alpha = EEPROMData.NR_alpha;  // 4 bytes
  NR_beta = EEPROMData.NR_beta;    // 4 bytes
  omegaN = EEPROMData.omegaN;      // 4 bytes
  pll_fmax = EEPROMData.pll_fmax;  // 4 bytes

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    powerOutCW[i] = EEPROMData.powerOutCW[i];
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    powerOutSSB[i] = EEPROMData.powerOutSSB[i];
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    CWPowerCalibrationFactor[i] = EEPROMData.CWPowerCalibrationFactor[i];  // 0.019;
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    SSBPowerCalibrationFactor[i] = EEPROMData.SSBPowerCalibrationFactor[i];  // 0.008
  }

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    IQAmpCorrectionFactor[i] = EEPROMData.IQAmpCorrectionFactor[i];
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    IQPhaseCorrectionFactor[i] = EEPROMData.IQPhaseCorrectionFactor[i];
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    IQXAmpCorrectionFactor[i] = EEPROMData.IQXAmpCorrectionFactor[i];
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    IQXPhaseCorrectionFactor[i] = EEPROMData.IQXPhaseCorrectionFactor[i];
  }
  favoriteFrequencies[0] = EEPROMData.favoriteFreqs[0];
  favoriteFrequencies[1] = EEPROMData.favoriteFreqs[1];
  favoriteFrequencies[2] = EEPROMData.favoriteFreqs[2];
  favoriteFrequencies[3] = EEPROMData.favoriteFreqs[3];
  favoriteFrequencies[4] = EEPROMData.favoriteFreqs[4];
  favoriteFrequencies[5] = EEPROMData.favoriteFreqs[5];
  favoriteFrequencies[6] = EEPROMData.favoriteFreqs[6];
  favoriteFrequencies[7] = EEPROMData.favoriteFreqs[7];
  favoriteFrequencies[8] = EEPROMData.favoriteFreqs[8];
  favoriteFrequencies[9] = EEPROMData.favoriteFreqs[9];
  favoriteFrequencies[10] = EEPROMData.favoriteFreqs[10];
  favoriteFrequencies[11] = EEPROMData.favoriteFreqs[11];
  favoriteFrequencies[12] = EEPROMData.favoriteFreqs[12];

  lastFrequencies[0][0] = EEPROMData.lastFrequencies[0][0];
  lastFrequencies[1][0] = EEPROMData.lastFrequencies[1][0];
  lastFrequencies[2][0] = EEPROMData.lastFrequencies[2][0];
  lastFrequencies[3][0] = EEPROMData.lastFrequencies[3][0];
  lastFrequencies[4][0] = EEPROMData.lastFrequencies[4][0];
  lastFrequencies[5][0] = EEPROMData.lastFrequencies[5][0];
  lastFrequencies[6][0] = EEPROMData.lastFrequencies[6][0];

  lastFrequencies[0][1] = EEPROMData.lastFrequencies[0][1];
  lastFrequencies[1][1] = EEPROMData.lastFrequencies[1][1];
  lastFrequencies[2][1] = EEPROMData.lastFrequencies[2][1];
  lastFrequencies[3][1] = EEPROMData.lastFrequencies[3][1];
  lastFrequencies[4][1] = EEPROMData.lastFrequencies[4][1];
  lastFrequencies[5][1] = EEPROMData.lastFrequencies[5][1];
  lastFrequencies[6][1] = EEPROMData.lastFrequencies[6][1];

  centerFreq = EEPROMData.lastFrequencies[currentBand][activeVFO];  // 4 bytes
  TxRxFreq = centerFreq;  // Need to assign TxRxFreq here or numerous subtle frequency bugs will happen.  KF5N August 7, 2023

  //strncpy(EEPROMData.mapFileName, MAP_FILE_NAME, 50);  KF5N
  strncpy(mapFileName, EEPROMData.mapFileName, 50);     // KF5N
  strncpy(myCall, EEPROMData.myCall, 10);               // KF5N
  strncpy(myTimeZone, EEPROMData.myTimeZone, 10);       // KF5N
  freqSeparationChar = EEPROMData.separationCharacter;  //   JJP  7/25/23  KF5N

  paddleFlip = EEPROMData.paddleFlip;  //   JJP  7/27/23.  Was setting to symbolic constant PADDLE_FLIP.  KF5N August 8, 2023
  sdCardPresent = EEPROMData.sdCardPresent = 0;      //   JJP  7/27/23

  myLat = EEPROMData.myLat;
  myLong = EEPROMData.myLong;
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    currentNoiseFloor[i] = EEPROMData.currentNoiseFloor[i];
  }
  //DB2OO, 10-SEP-23: We have V049.1 or V049.2 in EEPROM --> just initialize new/changed EEPROMData structure elements
  switch (v049_version) {
    case 1: //V049.1 --> there is no "compressorFlag" yet in the structure --> initalize it, to overwrite junk from EEPROM
      EEPROMData.compressorFlag = 0;
      //fall through
    case 2: //V049.2: compressorFlag is there, but we might have to add other inits (not required yet)
      break;
  }
  compressorFlag = EEPROMData.compressorFlag;             // JJP 8/28/23
}


/*****
  Purpose: To save the configuration data (working variables) to EEPROM

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMWrite() {
  strncpy(EEPROMData.versionSettings, EEPROMSetVersion(), 9);  // KF5N

  EEPROMData.AGCMode = AGCMode;
  EEPROMData.CWFilterIndex = CWFilterIndex;
  EEPROMData.nrOptionSelect = nrOptionSelect;
  EEPROMData.rfGainAllBands = rfGainAllBands;

  EEPROMData.activeVFO = activeVFO;  // 2 bytes

  EEPROMData.audioVolume = audioVolume;  // 4 bytes
  EEPROMData.currentBand = currentBand;  // 4 bytes
  EEPROMData.currentBandA = currentBandA;
  EEPROMData.currentBandB = currentBandB;
  EEPROMData.currentFreqA = currentFreqA;  // JJP 7/17/23
  EEPROMData.currentFreqB = currentFreqB;  // JJP 7/17/23
  EEPROMData.decoderFlag = decoderFlag;

  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    EEPROMData.equalizerRec[i] = recEQ_Level[i];  // 4 bytes each
    EEPROMData.equalizerXmt[i] = xmtEQ_Level[i];
  }

  EEPROMData.freqIncrement = freqIncrement;              // 4 bytes
  EEPROMData.keyType = keyType;                          // straight key = 0, keyer = 1
  EEPROMData.currentMicThreshold = currentMicThreshold;  // 4 bytes      // AFP 09-22-22
  EEPROMData.currentMicCompRatio = currentMicCompRatio;
  EEPROMData.currentMicAttack = currentMicAttack;
  EEPROMData.currentMicRelease = currentMicRelease;
  EEPROMData.currentMicGain = currentMicGain;

  EEPROMData.paddleDit = paddleDit;
  EEPROMData.paddleDah = paddleDah;
  EEPROMData.spectrumNoiseFloor = spectrumNoiseFloor;  // AFP 09-26-22

  EEPROMData.tuneIndex = tuneIndex;
  EEPROMData.stepFineTune = stepFineTune;

  EEPROMData.powerLevel = transmitPowerLevel;
  EEPROMData.currentWPM = currentWPM;  // 4 bytes
  EEPROMData.xmtMode = xmtMode;        // AFP 09-26-22

  EEPROMData.currentScale = currentScale;
  EEPROMData.spectrum_zoom = spectrum_zoom;
  EEPROMData.spectrum_display_scale = spectrum_display_scale;  // 4 bytes
  EEPROMData.sidetoneVolume = sidetoneVolume;                  // 4 bytes
  EEPROMData.cwTransmitDelay = cwTransmitDelay;                // 4 bytes

  EEPROMData.LPFcoeff = LPFcoeff;  // 4 bytes
  EEPROMData.NR_PSI = NR_PSI;      // 4 bytes
  EEPROMData.NR_alpha = NR_alpha;  // 4 bytes
  EEPROMData.NR_beta = NR_beta;    // 4 bytes
  EEPROMData.omegaN = omegaN;      // 4 bytes
  EEPROMData.pll_fmax = pll_fmax;  // 4 bytes

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    EEPROMData.CWPowerCalibrationFactor[i] = CWPowerCalibrationFactor[i];    // 0.019;   //AFP 10-29-22
    EEPROMData.SSBPowerCalibrationFactor[i] = SSBPowerCalibrationFactor[i];  // 0.008;   //AFP 10-29-22
    EEPROMData.powerOutCW[i] = powerOutCW[i];                                // 4 bytes //AFP 10-21-22
    EEPROMData.powerOutSSB[i] = powerOutSSB[i];                              // 4 bytes AFP 10-21-22
    EEPROMData.IQAmpCorrectionFactor[i] = IQAmpCorrectionFactor[i];
    EEPROMData.IQPhaseCorrectionFactor[i] = IQPhaseCorrectionFactor[i];
    EEPROMData.IQXAmpCorrectionFactor[i] = IQXAmpCorrectionFactor[i];
    EEPROMData.IQXPhaseCorrectionFactor[i] = IQXPhaseCorrectionFactor[i];
  }
  //  Note:favoriteFreqs are written as they are saved.

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    EEPROMData.lastFrequencies[i][0] = lastFrequencies[i][0];
    EEPROMData.lastFrequencies[i][1] = lastFrequencies[i][1];
  }

  EEPROMData.lastFrequencies[currentBand][activeVFO] = currentFreq;  // 4 bytes
  EEPROMData.lastFrequencies[currentBandA][VFO_A] = currentFreqA;    // 4 bytes
  EEPROMData.lastFrequencies[currentBandB][VFO_B] = currentFreqB;    // 4 bytes
  EEPROMData.freqCorrectionFactor = freqCorrectionFactor;

  strncpy(EEPROMData.mapFileName, mapFileName, 49);  // 1 smaller to allow for null
  strncpy(EEPROMData.myCall, myCall, 9);
  strncpy(EEPROMData.myTimeZone, myTimeZone, 9);
  EEPROMData.separationCharacter = (int)freqSeparationChar;

  EEPROMData.paddleFlip = paddleFlip;
  EEPROMData.sdCardPresent = sdCardPresent;

  EEPROMData.myLat = myLat;
  EEPROMData.myLong = myLong;
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    EEPROMData.currentNoiseFloor[i] = currentNoiseFloor[i];
  }
  EEPROMData.compressorFlag = compressorFlag;                        // JJP 8/28/23

  EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);
  // CopyEEPROMToSD();                                               // JJP 7/26/23
  syncEEPROM = 0;  // SD EEPROM different that memory EEPROM
  //UpdateEEPROMSyncIndicator(0);   //  JJP 7/25/23
}  // end void eeProm SAVE

/*****
  Purpose: To show the current EEPROM values. Used for debugging

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMShow() 
{
   int i;

  Serial.println("----- EEPROM Parameters: -----");

  Serial.print("Version                         = ");
  Serial.println(EEPROMData.versionSettings);
  Serial.print("AGCMode                         = ");
  Serial.println(EEPROMData.AGCMode);
  Serial.print("audioVolume                     = ");
  Serial.println(EEPROMData.audioVolume);
  Serial.print("rfGainAllBands                  = ");
  Serial.println(EEPROMData.rfGainAllBands);
  Serial.print("spectrumNoiseFloor              = ");
  Serial.println(EEPROMData.spectrumNoiseFloor);
  Serial.print("tuneIndex                       = ");
  Serial.println(EEPROMData.tuneIndex);
  Serial.print("stepFineTne                     = ");
  Serial.println(EEPROMData.stepFineTune);
  Serial.print("powerLevel                      = ");
  Serial.println(EEPROMData.powerLevel);
  Serial.print("xmtMode                         = ");
  Serial.println(EEPROMData.xmtMode);
  Serial.print("nrOptionSelect                  = ");
  Serial.println(EEPROMData.nrOptionSelect);
  Serial.print("currentScale                    = ");
  Serial.println(EEPROMData.currentScale);
  Serial.print("spectrum_zoom                   = ");
  Serial.println(EEPROMData.spectrum_zoom);
  Serial.print("spect_display_scale             = ");
  Serial.println(EEPROMData.spectrum_display_scale);
  Serial.println("----- CW Parameters: -----");
  Serial.print("CWFilterIndex                   = ");
  Serial.println(EEPROMData.CWFilterIndex);
  Serial.print("paddleDah                       = ");
  Serial.println(EEPROMData.paddleDah);
  Serial.print("paddleDit                       = ");
  Serial.println(EEPROMData.paddleDit);
  Serial.print("decoderFlag                     = ");
  Serial.println(EEPROMData.decoderFlag);
  Serial.print("keyType                         = ");
  Serial.println(EEPROMData.keyType);
  Serial.print("wordsPerMinute                  = ");
  Serial.println(EEPROMData.currentWPM);
  Serial.print("sidetoneVolume                  = ");
  Serial.println(EEPROMData.sidetoneVolume, 5);
  Serial.print("cwTransmitDelay                 = ");
  Serial.println(EEPROMData.cwTransmitDelay);
  Serial.println("----- Current Frequencies & Bands: -----");
  Serial.print("activeVFO                       = ");
  Serial.println(EEPROMData.activeVFO);
  Serial.print("freqIncrement                   = ");
  Serial.println(EEPROMData.freqIncrement);
  Serial.print("currentBand                     = ");
  Serial.println(EEPROMData.currentBand);
  Serial.print("currentBandA                    = ");
  Serial.println(EEPROMData.currentBandA);
  Serial.print("currentBandB                    = ");
  Serial.println(EEPROMData.currentBandB);
  Serial.print("currentFreqA                    = ");
  Serial.println(EEPROMData.currentFreqA);
  Serial.print("currentFreqB                    = ");
  Serial.println(EEPROMData.currentFreqB);
  Serial.print("freqCorrectionFactor            = ");
  Serial.println(EEPROMData.freqCorrectionFactor);
  Serial.println("----- Equalizer Parameters -----");
  for (i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    if (i < 10) {
      Serial.print(" ");
    }
    Serial.print("               equalizerRec[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.equalizerRec[i]);
  }
  Serial.println(" ");
  for (i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    if (i < 10) {
      Serial.print(" ");
    }
    Serial.print("               equalizerXmt[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.equalizerXmt[i]);
  }
  Serial.println("----- Mic Compressor Parameters -----");
  Serial.print("micCompression                  = ");
  Serial.println(EEPROMData.currentMicThreshold);
  Serial.print("currentMicCompRatio             = ");
  Serial.println(EEPROMData.currentMicCompRatio);
  Serial.print("currentMicAttack                = ");
  Serial.println(EEPROMData.currentMicAttack);
  Serial.print("currentMicRelease               = ");
  Serial.println(EEPROMData.currentMicRelease);
  Serial.print("currentMicGain                  = ");
  Serial.println(EEPROMData.currentMicGain);
  Serial.println("----- Switch Matrix Values -----");
  for (i = 0; i < NUMBER_OF_SWITCHES; i++) {
    if (i < 10) {
      Serial.print(" ");
    }
    Serial.print("               switchValues[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.switchValues[i]);
  }
  Serial.println("----- Filter Parameters -----");
  Serial.print("LPF coeff                       = ");
  Serial.println(EEPROMData.LPFcoeff);
  Serial.print("NR_PSI                          = ");
  Serial.println(EEPROMData.NR_PSI);
  Serial.print("NR_alpha                        = ");
  Serial.println(EEPROMData.NR_alpha);
  Serial.print("NR_beta                         = ");
  Serial.println(EEPROMData.NR_beta);
  Serial.print("NR_omega                        = ");
  Serial.println(EEPROMData.omegaN);
  Serial.print("pll_fmax                        = ");
  Serial.println(EEPROMData.pll_fmax);
  Serial.println("----- Power Out Calibration Parameters -----");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("                  powerOutCW[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.powerOutCW[i], 5);  //AFP 10-13-22
  }
  Serial.println(" ");  
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("                 powerOutSSB[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.powerOutSSB[i], 5);
  }
  Serial.println(" ");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("            CWPowerCalFactor[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.CWPowerCalibrationFactor[i], 5);
  }
  Serial.println(" ");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("           SSBPowerCalFactor[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.SSBPowerCalibrationFactor[i], 5);
  }
  Serial.println(" ");
  Serial.println("----- I/Q Calibration Parameters -----");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print(" IQAmplitudeCorrectionFactor[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.IQAmpCorrectionFactor[i], 3);
  }
  Serial.println(" ");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("     IQPhaseCorrectionFactor[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.IQPhaseCorrectionFactor[i], 3);
  }
  Serial.println(" ");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("IQXAmplitudeCorrectionFactor[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.IQXAmpCorrectionFactor[i], 3);
  }
  Serial.println(" ");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("    IQXPhaseCorrectionFactor[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(EEPROMData.IQXPhaseCorrectionFactor[i], 3);
  }
  Serial.println(" ");
  Serial.println("----- Favorite Frequencies -----");
  for (i = 0; i < MAX_FAVORITES; i++) {
    if (i < 10) {
      Serial.print(" ");
    }    Serial.print("              favoriteFreqs[");
    Serial.print(i);
    Serial.print("] = ");
    if (i < 4) {
      Serial.print(" ");
    }
    Serial.println(EEPROMData.favoriteFreqs[i]);
  }
  Serial.println("----- Last Frequencies -----");
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("          lastFrequencies[");
    Serial.print(i);
    Serial.print("][0] = ");
    if (i < 2) {
      Serial.print(" ");
    }
    Serial.println(EEPROMData.lastFrequencies[i][0]);
  }
  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("          lastFrequencies[");
    Serial.print(i);
    Serial.print("][1] = ");
    if (i < 2) {
      Serial.print(" ");
    }  
    Serial.println(EEPROMData.lastFrequencies[i][1]);
  }
  Serial.println(" ");
  Serial.print("centerFreq                      = ");
  Serial.println((long)EEPROMData.centerFreq);

  Serial.print("mapFileName                     = ");
  Serial.println(EEPROMData.mapFileName);

  Serial.print("myCall                          = ");
  Serial.println(EEPROMData.myCall);

  Serial.print("myTimeZone                      = ");
  Serial.println(EEPROMData.myTimeZone);

  Serial.print("separationCharacter             = ");
  Serial.println((char)EEPROMData.separationCharacter);

  Serial.println(" ");  // JJP 7-3-23

  Serial.print("paddleFlip                      = ");
  Serial.println(EEPROMData.paddleFlip);
  Serial.print("sdCardPresent                   = ");
  Serial.println(EEPROMData.sdCardPresent);
  Serial.print("myLat                           = ");
  Serial.println(EEPROMData.myLat);
  Serial.print("myLong                          = ");
  Serial.println(EEPROMData.myLong);

  Serial.println(" ");  // JJP 7-3-23

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    Serial.print("        currentNoiseFloor[");
    Serial.print(i);
    Serial.print("]    = ");
    Serial.println(EEPROMData.currentNoiseFloor[i]);
  }
  Serial.print("compressorFlag                  = ");
  Serial.println(EEPROMData.compressorFlag);

  Serial.println("----- End EEPROM Parameters -----");
}

/*****
  Purpose: Read default favorite frequencies

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMStuffFavorites(unsigned long current[]) {
  int i;

  for (i = 0; i < MAX_FAVORITES; i++) {
    current[i] = EEPROMData.favoriteFreqs[i];
  }
}

/*****
  Purpose: Used to save a favortie frequency to EEPROM

  Parameter list:

  Return value;
    void

  CAUTION: This code assumes you have set the curently active VFO frequency to the new
           frequency you wish to save. You them use the menu encoder to scroll through
           the current list of stored frequencies. Stop on the one that you wish to
           replace and press Select to save in EEPROM. The currently active VFO frequency
           is then stored to EEPROM.
*****/
void SetFavoriteFrequency() {
  int index;
  int val;

  tft.setFontScale((enum RA8875tsize)1);

  index = 0;
  tft.setTextColor(RA8875_WHITE);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
  tft.print(EEPROMData.favoriteFreqs[index]);
  while (true) {
    if (filterEncoderMove != 0) {  // Changed encoder?
      index += filterEncoderMove;  // Yep
      if (index < 0) {
        index = MAX_FAVORITES - 1;  // Wrap to last one
      } else {
        if (index > MAX_FAVORITES)
          index = 0;  // Wrap to first one
      }
      tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
      tft.print(EEPROMData.favoriteFreqs[index]);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);
    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      EraseMenus();
      EEPROMData.favoriteFreqs[index] = TxRxFreq;
      syncEEPROM = 0;  // SD EEPROM different that memory EEPROM
      //UpdateEEPROMSyncIndicator(0);       //  JJP 7/25/23
      if (activeVFO == VFO_A) {
        currentFreqA = TxRxFreq;
      } else {
        currentFreqB = TxRxFreq;
      }
      //      EEPROMWrite();
      SetFreq();
      BandInformation();
      ShowBandwidth();
      FilterBandwidth();
      ShowFrequency();
      break;
    }
  }
}

/*****
  Purpose: Used to fetch a favortie frequency as stored in EEPROM. It then copies that
           frequency to the currently active VFO

  Parameter list:

  Return value;
    void
*****/
void GetFavoriteFrequency() {
  int index = 0;
  int val;
  int currentBand2 = 0;
  tft.setFontScale((enum RA8875tsize)1);

  tft.setTextColor(RA8875_WHITE);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
  tft.print(EEPROMData.favoriteFreqs[index]);
  while (true) {
    if (filterEncoderMove != 0) {  // Changed encoder?
      index += filterEncoderMove;  // Yep
      if (index < 0) {
        index = MAX_FAVORITES - 1;  // Wrap to last one
      } else {
        if (index > MAX_FAVORITES)
          index = 0;  // Wrap to first one
      }
      tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
      tft.print(EEPROMData.favoriteFreqs[index]);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);

    centerFreq = EEPROMData.favoriteFreqs[index];  // current frequency  AFP 09-27-22
    if (centerFreq >= bands[BAND_80M].fBandLow && centerFreq <= bands[BAND_80M].fBandHigh) {
      currentBand2 = BAND_80M;
    } else if (centerFreq >= bands[BAND_80M].fBandHigh && centerFreq <= 7000000L) {  // covers 5MHz WWV AFP 11-03-22
      currentBand2 = BAND_80M;
    } else if (centerFreq >= bands[BAND_40M].fBandLow && centerFreq <= bands[BAND_40M].fBandHigh) {
      currentBand2 = BAND_40M;
    } else if (centerFreq >= bands[BAND_40M].fBandHigh && centerFreq <= 14000000L) {  // covers 10MHz WWV AFP 11-03-22
      currentBand2 = BAND_40M;
    } else if (centerFreq >= bands[BAND_20M].fBandLow && centerFreq <= bands[BAND_20M].fBandHigh) {
      currentBand2 = BAND_20M;
    } else if (centerFreq >= 14000000L && centerFreq <= 18000000L) {  // covers 15MHz WWV AFP 11-03-22
      currentBand2 = BAND_20M;
    } else if (centerFreq >= bands[BAND_17M].fBandLow && centerFreq <= bands[BAND_17M].fBandHigh) {
      currentBand2 = BAND_17M;
    } else if (centerFreq >= bands[BAND_15M].fBandLow && centerFreq <= bands[BAND_15M].fBandHigh) {
      currentBand2 = BAND_15M;
    } else if (centerFreq >= bands[BAND_12M].fBandLow && centerFreq <= bands[BAND_12M].fBandHigh) {
      currentBand2 = BAND_12M;
    } else if (centerFreq >= bands[BAND_10M].fBandLow && centerFreq <= bands[BAND_10M].fBandHigh) {
      currentBand2 = BAND_10M;
    }
    currentBand = currentBand2;


    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      switch (activeVFO) {
        case VFO_A:
          if (currentBandA == NUMBER_OF_BANDS) {  // Incremented too far?
            currentBandA = 0;                     // Yep. Roll to list front.
          }
          currentBandA = currentBand2;
          TxRxFreq = centerFreq + NCOFreq;
          lastFrequencies[currentBand][VFO_A] = TxRxFreq;
          break;

        case VFO_B:
          if (currentBandB == NUMBER_OF_BANDS) {  // Incremented too far?
            currentBandB = 0;                     // Yep. Roll to list front.
          }                                       // Same for VFO B
          currentBandB = currentBand2;
          TxRxFreq = centerFreq + NCOFreq;
          lastFrequencies[currentBand][VFO_B] = TxRxFreq;
          break;
      }
    }
    if (val == MENU_OPTION_SELECT) {

      EraseSpectrumDisplayContainer();
      currentMode = bands[currentBand].mode;
      DrawSpectrumDisplayContainer();
      DrawFrequencyBarValue();
      SetBand();
      SetFreq();
      ShowFrequency();
      ShowSpectrumdBScale();
      EraseMenus();
      ResetTuning();
      FilterBandwidth();
      BandInformation();
      NCOFreq = 0L;
      DrawBandWidthIndicatorBar();  // AFP 10-20-22
      digitalWrite(bandswitchPins[currentBand], LOW);
      SetFreq();
      ShowSpectrumdBScale();
      ShowSpectrum();
      //bands[currentBand].mode = currentBand;
      return;
    }
  }
}

//===================================================================
/*****
  Purpose: Copy EEPROM data into working variables

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void CopyEEPROM() {
  //  EEPROM.get(EEPROM_BASE_ADDRESS, EEPROMData);                       // Read as one large chunk

  AGCMode = EEPROMData.AGCMode;                // 1 byte
  CWFilterIndex = EEPROMData.CWFilterIndex;    // Off
  nrOptionSelect = EEPROMData.nrOptionSelect;  // 1 byte

  activeVFO = EEPROMData.activeVFO;  // 2 bytes

  audioVolume = EEPROMData.audioVolume;    // 4 bytes
  currentBand = EEPROMData.currentBand;    // 4 bytes
  currentBandA = EEPROMData.currentBandA;  // 4 bytes
  currentBandB = EEPROMData.currentBandB;  // 4 bytes
  decoderFlag = EEPROMData.decoderFlag;

  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    recEQ_Level[i] = EEPROMData.equalizerRec[i];  // 4 bytes each
    xmtEQ_Level[i] = EEPROMData.equalizerXmt[i];
  }

  freqIncrement = EEPROMData.freqIncrement;              // 4 bytes
  keyType = EEPROMData.keyType;                          // straight key = 0, keyer = 1
  currentMicThreshold = EEPROMData.currentMicThreshold;  // 4 bytes      // AFP 09-22-22
  currentMicCompRatio = EEPROMData.currentMicCompRatio;
  currentMicAttack = EEPROMData.currentMicAttack;
  currentMicRelease = EEPROMData.currentMicRelease;
  currentMicGain = EEPROMData.currentMicGain;

  paddleDit = EEPROMData.paddleDit;
  paddleDah = EEPROMData.paddleDah;
  rfGainAllBands = EEPROMData.rfGainAllBands;
  spectrumNoiseFloor = EEPROMData.spectrumNoiseFloor;  // AFP 09-26-22

  //  Note: switch values are read and written to EEPROM only

  tuneIndex = EEPROMData.tuneIndex;
  stepFineTune = EEPROMData.stepFineTune;
  transmitPowerLevel = EEPROMData.powerLevel;
  currentWPM = EEPROMData.currentWPM;  // 4 bytes
  xmtMode = EEPROMData.xmtMode;        // AFP 09-26-22

  currentScale = EEPROMData.currentScale;
  spectrum_zoom = EEPROMData.spectrum_zoom;
  spectrum_display_scale = EEPROMData.spectrum_display_scale;  // 4 bytes
  sidetoneVolume = EEPROMData.sidetoneVolume;                  // 4 bytes
  cwTransmitDelay = EEPROMData.cwTransmitDelay;                // 4 bytes

  freqCorrectionFactor = EEPROMData.freqCorrectionFactor;

  LPFcoeff = EEPROMData.LPFcoeff;  // 4 bytes
  NR_PSI = EEPROMData.NR_PSI;      // 4 bytes
  NR_alpha = EEPROMData.NR_alpha;  // 4 bytes
  NR_beta = EEPROMData.NR_beta;    // 4 bytes
  omegaN = EEPROMData.omegaN;      // 4 bytes
  pll_fmax = EEPROMData.pll_fmax;  // 4 bytes

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    CWPowerCalibrationFactor[i] = EEPROMData.CWPowerCalibrationFactor[i];    // 0.019;   //AFP 10-29-22
    SSBPowerCalibrationFactor[i] = EEPROMData.SSBPowerCalibrationFactor[i];  // 0.008;   //AFP 10-29-22
    powerOutCW[i] = EEPROMData.powerOutCW[i];                                // 4 bytes  //AFP 10-28-22
    powerOutSSB[i] = EEPROMData.powerOutSSB[i];                              // 4 bytes AFP 10-28-22
    IQAmpCorrectionFactor[i] = EEPROMData.IQAmpCorrectionFactor[i];
    IQPhaseCorrectionFactor[i] = EEPROMData.IQPhaseCorrectionFactor[i];
    IQXAmpCorrectionFactor[i] = EEPROMData.IQXAmpCorrectionFactor[i];      //AFP 2-21-23
    IQXPhaseCorrectionFactor[i] = EEPROMData.IQXPhaseCorrectionFactor[i];  //AFP 2-21-23
  }

  if (EEPROMData.lastFrequencies[0][1] < 3560000L || EEPROMData.lastFrequencies[0][0] > 3985000L) {  // Already set?
    EEPROMData.lastFrequencies[0][0] = 3985000L;                                                     // 80 Phone
    EEPROMData.lastFrequencies[1][0] = 7200000L;                                                     // 40
    EEPROMData.lastFrequencies[2][0] = 14285000L;                                                    // 50
    EEPROMData.lastFrequencies[3][0] = 18130000L;                                                    // 17
    EEPROMData.lastFrequencies[4][0] = 21385000L;                                                    // 15
    EEPROMData.lastFrequencies[5][0] = 24950000L;                                                    // 12
    EEPROMData.lastFrequencies[6][0] = 28385800L;                                                    // 10

    EEPROMData.lastFrequencies[0][1] = 3560000L;   // 80 CW
    EEPROMData.lastFrequencies[1][1] = 7030000L;   // 40
    EEPROMData.lastFrequencies[2][1] = 14060000L;  // 20
    EEPROMData.lastFrequencies[3][1] = 18096000L;  // 17
    EEPROMData.lastFrequencies[4][1] = 21060000L;  // 15
    EEPROMData.lastFrequencies[5][1] = 24906000L;  // 12
    EEPROMData.lastFrequencies[6][1] = 28060000L;  // 10
  } else {
    for (int i = 0; i < NUMBER_OF_BANDS; i++) {
      lastFrequencies[i][0] = EEPROMData.lastFrequencies[i][0];
      lastFrequencies[i][1] = EEPROMData.lastFrequencies[i][1];
    }
  }

  //  long favoriteFreqs[MAX_FAVORITES];               // 40 bytes
  centerFreq = EEPROMData.lastFrequencies[currentBandA][activeVFO];  // 4 bytes
  currentFreqA = EEPROMData.lastFrequencies[currentBandA][VFO_A];    // 4 bytes
  currentFreqB = EEPROMData.lastFrequencies[currentBandB][VFO_B];    // 4 bytes

  strncpy(mapFileName, EEPROMData.mapFileName, 50);
  strncpy(myCall, EEPROMData.myCall, 10);
  strncpy(myTimeZone, EEPROMData.myTimeZone, 10);
  freqSeparationChar = (char)EEPROMData.separationCharacter;  //   JJP  7/25/23

  paddleFlip = EEPROMData.paddleFlip;
  sdCardPresent = EEPROMData.sdCardPresent;

  myLat = EEPROMData.myLat;
  myLong = EEPROMData.myLong;

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    currentNoiseFloor[i] = EEPROMData.currentNoiseFloor[i];
  }
  compressorFlag = EEPROMData.compressorFlag;                     // JJP 8/28/23
}

/*****
  Purpose: To save the default setting for EEPROM variables

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/

void EEPROMSaveDefaults2() {
  strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // Update version

  EEPROMData.AGCMode = 1;
  EEPROMData.audioVolume = 30;  // 4 bytes
  EEPROMData.rfGainAllBands = 1;
  EEPROMData.spectrumNoiseFloor = SPECTRUM_NOISE_FLOOR;
  EEPROMData.tuneIndex = 5;
  EEPROMData.stepFineTune = 50L;
  EEPROMData.powerLevel = 10;
  EEPROMData.xmtMode = 0;
  EEPROMData.nrOptionSelect = 0;  // 1 byte
  EEPROMData.currentScale = 1;
  EEPROMData.spectrum_zoom = 1;
  EEPROMData.spectrum_display_scale = 20.0;  // 4 bytes

  EEPROMData.CWFilterIndex = 5;  // Off
  EEPROMData.paddleDit = 36;
  EEPROMData.paddleDah = 35;
  EEPROMData.decoderFlag = 0;
  EEPROMData.keyType = 0;            // straight key = 0, keyer = 1
  EEPROMData.currentWPM = 15;        // 4 bytes
  EEPROMData.sidetoneVolume = 50.0;  // 4 bytes.  Changed to default 50.  KF5N October 7, 2023.
  EEPROMData.cwTransmitDelay = 750;  // 4 bytes

  EEPROMData.activeVFO = 0;      // 2 bytes, 0 = VFOa
  EEPROMData.freqIncrement = 5;  // 4 bytes
  EEPROMData.currentBand = 1;    // 4 bytes
  EEPROMData.currentBandA = 1;   // 4 bytes
  EEPROMData.currentBandB = 1;   // 4 bytes
  //DB2OO, 23-AUG-23 7.1MHz for Region 1
#if defined(ITU_REGION) && ITU_REGION==1
  EEPROMData.currentFreqA = 7100000;
#else  
  EEPROMData.currentFreqA = 7200000;
#endif
  EEPROMData.currentFreqB = 7030000;
  //DB2OO, 23-AUG-23: with TCXO needs to be 0
#ifdef TCXO_25MHZ  
  EEPROMData.freqCorrectionFactor = 0; //68000;
#else
  //Conventional crystal with freq offset needs a correction factor
  EEPROMData.freqCorrectionFactor = 68000;
#endif  

  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    EEPROMData.equalizerRec[i] = 100;  // 4 bytes each
    EEPROMData.equalizerXmt[i] = 100;
  }

  EEPROMData.currentMicThreshold = -10;  // 4 bytes      // AFP 09-22-22
  EEPROMData.currentMicCompRatio = 5.0;
  EEPROMData.currentMicAttack = 0.1;
  EEPROMData.currentMicRelease = 2.0;
  //DB2OO, 23-AUG-23: MicGain 20
  EEPROMData.currentMicGain = 20;

  EEPROMData.switchValues[0] = 924;
  EEPROMData.switchValues[1] = 870;
  EEPROMData.switchValues[2] = 817;
  EEPROMData.switchValues[3] = 769;
  EEPROMData.switchValues[4] = 713;
  EEPROMData.switchValues[5] = 669;
  EEPROMData.switchValues[6] = 616;
  EEPROMData.switchValues[7] = 565;
  EEPROMData.switchValues[8] = 513;
  EEPROMData.switchValues[9] = 459;
  EEPROMData.switchValues[10] = 407;
  EEPROMData.switchValues[11] = 356;
  EEPROMData.switchValues[12] = 298;
  EEPROMData.switchValues[13] = 242;
  EEPROMData.switchValues[14] = 183;
  EEPROMData.switchValues[15] = 131;
  EEPROMData.switchValues[16] = 67;
  EEPROMData.switchValues[17] = 10;

  EEPROMData.LPFcoeff = 0.0;     // 4 bytes
  EEPROMData.NR_PSI = 0.0;       // 4 bytes
  EEPROMData.NR_alpha = 0.0;     // 4 bytes
  EEPROMData.NR_beta = 0.0;      // 4 bytes
  EEPROMData.omegaN = 0.0;       // 4 bytes
  EEPROMData.pll_fmax = 4000.0;  // 4 bytes

  EEPROMData.powerOutCW[0] = 0.188;  // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[1] = 0.21;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[2] = 0.34;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[3] = 0.44;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[4] = 0.31;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[5] = 0.31;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[6] = 0.31;   // 4 bytes  AFP 10-21-22

  EEPROMData.powerOutSSB[0] = 0.188;  // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[1] = 0.11;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[2] = 0.188;  // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[3] = 0.21;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[4] = 0.23;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[5] = 0.23;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[6] = 0.24;   // 4 bytes  AFP 10-21-22

  EEPROMData.CWPowerCalibrationFactor[0] = 0.023;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[1] = 0.023;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[2] = 0.038;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[3] = 0.052;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[4] = 0.051;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[5] = 0.028;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[6] = 0.028;  //AFP 10-29-22

  EEPROMData.SSBPowerCalibrationFactor[0] = 0.017;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[1] = 0.019;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[2] = 0.017;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[3] = 0.019;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[4] = 0.021;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[5] = 0.020;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[6] = 0.022;  //AFP 10-29-22

  EEPROMData.IQAmpCorrectionFactor[0] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[1] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[2] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[3] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[4] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[5] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[6] = 1.0;

  EEPROMData.IQPhaseCorrectionFactor[0] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[1] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[2] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[3] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[4] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[5] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[6] = 0.0;

  EEPROMData.IQXAmpCorrectionFactor[0] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[1] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[2] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[3] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[4] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[5] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[6] = 1.0;

  EEPROMData.IQXPhaseCorrectionFactor[0] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[1] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[2] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[3] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[4] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[5] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[6] = 0.0;

  EEPROMData.favoriteFreqs[0] = 3560000L;  // These are CW/SSB calling frequencies for HF bands
  EEPROMData.favoriteFreqs[1] = 3690000L;
  EEPROMData.favoriteFreqs[2] = 7030000L;
  EEPROMData.favoriteFreqs[3] = 7200000L;
  EEPROMData.favoriteFreqs[4] = 14060000L;
  EEPROMData.favoriteFreqs[5] = 14200000L;
  EEPROMData.favoriteFreqs[6] = 21060000L;
  EEPROMData.favoriteFreqs[7] = 21285000L;
  EEPROMData.favoriteFreqs[8] = 28060000L;
  EEPROMData.favoriteFreqs[9] = 28365000L;
  EEPROMData.favoriteFreqs[10] = 5000000L;
  EEPROMData.favoriteFreqs[11] = 10000000L;
  EEPROMData.favoriteFreqs[12] = 15000000L;

  //DB2OO, 23-AUG-23: Region 1 freqs (from https://qrper.com/qrp-calling-frequencies/)
#if defined(ITU_REGION) && ITU_REGION==1  
  EEPROMData.lastFrequencies[0][0] = 3690000L; //3985000L;   // 80 Phone
  EEPROMData.lastFrequencies[1][0] = 7090000L; //7200000L;   // 40
  EEPROMData.lastFrequencies[2][0] = 14285000L;  // 50
  EEPROMData.lastFrequencies[3][0] = 18130000L;  // 17
  EEPROMData.lastFrequencies[4][0] = 21285000L; //21385000L;  // 15
  EEPROMData.lastFrequencies[5][0] = 24950000L;  // 12
  EEPROMData.lastFrequencies[6][0] = 28365000L; //28385800L;  // 10
#else
  EEPROMData.lastFrequencies[0][0] = 3985000L;   // 80 Phone
  EEPROMData.lastFrequencies[1][0] = 7200000L;   // 40
  EEPROMData.lastFrequencies[2][0] = 14285000L;  // 50
  EEPROMData.lastFrequencies[3][0] = 18130000L;  // 17
  EEPROMData.lastFrequencies[4][0] = 21385000L;  // 15
  EEPROMData.lastFrequencies[5][0] = 24950000L;  // 12
  EEPROMData.lastFrequencies[6][0] = 28385800L;  // 10
#endif

  EEPROMData.lastFrequencies[0][1] = 3560000L;   // 80 CW
  EEPROMData.lastFrequencies[1][1] = 7030000L;   // 40
  EEPROMData.lastFrequencies[2][1] = 14060000L;  // 20
  EEPROMData.lastFrequencies[3][1] = 18096000L;  // 17
  EEPROMData.lastFrequencies[4][1] = 21060000L;  // 15
  EEPROMData.lastFrequencies[5][1] = 24906000L;  // 12
  EEPROMData.lastFrequencies[6][1] = 28060000L;  // 10

  EEPROMData.centerFreq = 7150000;

  strncpy(EEPROMData.mapFileName, MAP_FILE_NAME, 50);
  strncpy(EEPROMData.myCall, MY_CALL, 10);
  strncpy(EEPROMData.myTimeZone, MY_TIMEZONE, 10);
  EEPROMData.separationCharacter = (char)FREQ_SEP_CHARACTER;  //   JJP  7/25/23

  EEPROMData.paddleFlip = paddleFlip;
  EEPROMData.sdCardPresent = 0;  //   JJP  7/18/23

  EEPROMData.myLat = MY_LAT;
  EEPROMData.myLong = MY_LON;

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    EEPROMData.currentNoiseFloor[i] = 0;
  }
  EEPROMData.compressorFlag = 0;                    // Off by default JJP  8/28/23
/*        //  JJP 7/25/23
  if (sdCardPresent == 1) {                         // SD card
    syncEEPROM = 0;                                 // SD EEPROM may be different that memory EEPROM
    UpdateEEPROMSyncIndicator(0);
  }
*/
  
}

/*****
  Purpose: Read the EEPROM data from the SD card and assign it to the
           proper EEPROM members.

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    int                         0 unsuccessful, 1 ok
*****/
int CopySDToEEPROM() {
  char character;
  char line[150];
  char *target;
  char temp[150];
  int index = 0;
  int lineCount = 0;

  if (!SD.begin(chipSelect)) {  // SD failed
    // don't do anything more:
    return 0;  // Go home and report it
  }

  File file = SD.open("SDEEPROMData.txt", FILE_READ);  // Try to open file...
  if (file == 0) {
    return 0;  // Could not get a file handle, so go home and report it
  }
  //  while (file.available() > 0) {
  while (true) {
/*
    Serial.printf("lineCount = %d\n", lineCount);     // Greg's debug code
    if (lineCount == 100) {
      Serial.printf("Hit end of file!\n");
      break;
    }
*/    
    while (true) {
      character = file.read();
      if (character == EOF || lineCount > MAX_SD_ITEMS) {
        file.close();
        EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);  // KF5N
        return 1;
      }
      line[index++] = character;
      if ((character == '\n')) {       // Have we read the complete line?
        line[index - 2] = '\0';        // Overwrite newline and semicolon, we have the complete line as string
        target = strstr(line, " = ");  // Look for token
        strcpy(temp, target + 3);      // Skip past the token
        break;
      }
    }
    index = 0;

    switch (lineCount) {                           // This is a hack, but it works
      case 0:                                      // Version
        strcpy(EEPROMData.versionSettings, temp);  // Real assignment from header file
        break;
      case 1:
        EEPROMData.AGCMode = atoi(temp);
        break;
      case 2:
        EEPROMData.audioVolume = atoi(temp);
        break;
      case 3:
        EEPROMData.rfGainAllBands = atoi(temp);
        break;
      case 4:
        EEPROMData.spectrumNoiseFloor = atoi(temp);
        break;
      case 5:
        EEPROMData.tuneIndex = atoi(temp);
        break;
      case 6:
        EEPROMData.stepFineTune = atol(temp);
        break;
      case 7:
        EEPROMData.powerLevel = atoi(temp);
        break;
      case 8:
        EEPROMData.xmtMode = atoi(temp);
        break;
      case 9:
        EEPROMData.nrOptionSelect = (int8_t)atoi(temp);
        break;
      case 10:
        EEPROMData.currentScale = (uint16_t)atoi(temp);
        break;
      case 11:
        EEPROMData.spectrum_zoom = (int32_t)atol(temp);
        break;
      case 12:
        EEPROMData.spectrum_display_scale = atof(temp);
        break;
      case 13:
        EEPROMData.CWFilterIndex = atoi(temp);
        break;
      case 14:
        EEPROMData.paddleDit = atoi(temp);
        break;
      case 15:
        EEPROMData.paddleDah = atoi(temp);
        break;
      case 16:
        EEPROMData.decoderFlag = atoi(temp);
        break;
      case 17:
        EEPROMData.keyType = atoi(temp);
        break;
      case 18:
        EEPROMData.currentWPM = atoi(temp);
        break;
      case 19:
        EEPROMData.sidetoneVolume = atof(temp);
        break;
      case 20:
        EEPROMData.cwTransmitDelay = atol(temp);
        break;
      case 21:
        EEPROMData.activeVFO = (int16_t)atoi(temp);
        break;
      case 22:
        EEPROMData.freqIncrement = atoi(temp);
        break;
      case 23:
        EEPROMData.currentBand = atoi(temp);
        break;
      case 24:
        EEPROMData.currentBandA = atoi(temp);
        break;
      case 25:
        EEPROMData.currentBandB = atoi(temp);
        break;
      case 26:
        EEPROMData.currentFreqA = atol(temp);
        break;
      case 27:
        EEPROMData.currentFreqB = atol(temp);
        break;
      case 28:
        EEPROMData.freqCorrectionFactor = (long)atol(temp);
        break;

      case 29:
        EEPROMData.equalizerRec[0] = atoi(temp);
        break;
      case 30:
        EEPROMData.equalizerRec[1] = atoi(temp);
        break;
      case 31:
        EEPROMData.equalizerRec[2] = atoi(temp);
        break;
      case 32:
        EEPROMData.equalizerRec[3] = atoi(temp);
        break;
      case 33:
        EEPROMData.equalizerRec[4] = atoi(temp);
        break;
      case 34:
        EEPROMData.equalizerRec[5] = atoi(temp);
        break;
      case 35:
        EEPROMData.equalizerRec[6] = atoi(temp);
        break;
      case 36:
        EEPROMData.equalizerRec[7] = atoi(temp);
        break;
      case 37:
        EEPROMData.equalizerRec[8] = atoi(temp);
        break;
      case 38:
        EEPROMData.equalizerRec[9] = atoi(temp);
        break;
      case 39:
        EEPROMData.equalizerRec[10] = atoi(temp);
        break;
      case 40:
        EEPROMData.equalizerRec[11] = atoi(temp);
        break;
      case 41:
        EEPROMData.equalizerRec[12] = atoi(temp);
        break;
      case 42:
        EEPROMData.equalizerRec[13] = atoi(temp);
        break;
      case 43:
        EEPROMData.equalizerXmt[0] = atoi(temp);
        break;
      case 44:
        EEPROMData.equalizerXmt[1] = atoi(temp);
        break;
      case 45:
        EEPROMData.equalizerXmt[2] = atoi(temp);
        break;
      case 46:
        EEPROMData.equalizerXmt[3] = atoi(temp);
        break;
      case 47:
        EEPROMData.equalizerXmt[4] = atoi(temp);
        break;
      case 48:
        EEPROMData.equalizerXmt[5] = atoi(temp);
        break;
      case 49:
        EEPROMData.equalizerXmt[6] = atoi(temp);
        break;
      case 50:
        EEPROMData.equalizerXmt[7] = atoi(temp);
        break;
      case 51:
        EEPROMData.equalizerXmt[8] = atoi(temp);
        break;
      case 52:
        EEPROMData.equalizerXmt[9] = atoi(temp);
        break;
      case 53:
        EEPROMData.equalizerXmt[10] = atoi(temp);
        break;
      case 54:
        EEPROMData.equalizerXmt[11] = atoi(temp);
        break;
      case 55:
        EEPROMData.equalizerXmt[12] = atoi(temp);
        break;
      case 56:
        EEPROMData.equalizerXmt[13] = atoi(temp);
        break;

      case 57:
        EEPROMData.currentMicThreshold = atoi(temp);
        break;
      case 58:
        EEPROMData.currentMicCompRatio = atof(temp);
        break;
      case 59:
        EEPROMData.currentMicAttack = atof(temp);
      case 60:
        EEPROMData.currentMicRelease = atof(temp);
        break;
      case 61:
        EEPROMData.currentMicGain = atoi(temp);
        break;
      case 62:
        EEPROMData.switchValues[0] = atoi(temp);
        break;
      case 63:
        EEPROMData.switchValues[1] = atoi(temp);
        break;
      case 64:
        EEPROMData.switchValues[2] = atoi(temp);
        break;
      case 65:
        EEPROMData.switchValues[3] = atoi(temp);
        break;
      case 66:
        EEPROMData.switchValues[4] = atoi(temp);
        break;
      case 67:
        EEPROMData.switchValues[5] = atoi(temp);
        break;
      case 68:
        EEPROMData.switchValues[6] = atoi(temp);
        break;
      case 69:
        EEPROMData.switchValues[7] = atoi(temp);
        break;
      case 70:
        EEPROMData.switchValues[8] = atoi(temp);
        break;
      case 71:
        EEPROMData.switchValues[9] = atoi(temp);
        break;
      case 72:
        EEPROMData.switchValues[10] = atoi(temp);
        break;
      case 73:
        EEPROMData.switchValues[11] = atoi(temp);
        break;
      case 74:
        EEPROMData.switchValues[12] = atoi(temp);
        break;
      case 75:
        EEPROMData.switchValues[13] = atoi(temp);
        break;
      case 76:
        EEPROMData.switchValues[14] = atoi(temp);
        break;
      case 77:
        EEPROMData.switchValues[15] = atoi(temp);
        break;
      case 78:
        EEPROMData.switchValues[16] = atoi(temp);
        break;
      case 79:
        EEPROMData.switchValues[17] = atoi(temp);
        break;
      case 80:
        EEPROMData.LPFcoeff = atof(temp);
        break;
      case 81:
        EEPROMData.NR_PSI = atof(temp);
        break;
      case 82:
        EEPROMData.NR_alpha = atof(temp);
        break;
      case 83:
        EEPROMData.NR_beta = atof(temp);
        break;
      case 84:
        EEPROMData.omegaN = atof(temp);
        break;
      case 85:
        EEPROMData.pll_fmax = atof(temp);
        break;
      case 86:
        EEPROMData.powerOutCW[0] = atof(temp);
        break;
      case 87:
        EEPROMData.powerOutCW[1] = atof(temp);
        break;
      case 88:
        EEPROMData.powerOutCW[2] = atof(temp);
        break;
      case 89:
        EEPROMData.powerOutCW[3] = atof(temp);
        break;
      case 90:
        EEPROMData.powerOutCW[4] = atof(temp);
        break;
      case 91:
        EEPROMData.powerOutCW[5] = atof(temp);
        break;
      case 92:
        EEPROMData.powerOutCW[6] = atof(temp);
        break;
      case 93:
        EEPROMData.powerOutSSB[0] = atof(temp);
        break;
      case 94:
        EEPROMData.powerOutSSB[1] = atof(temp);
        break;
      case 95:
        EEPROMData.powerOutSSB[2] = atof(temp);
        break;
      case 96:
        EEPROMData.powerOutSSB[3] = atof(temp);
        break;
      case 97:
        EEPROMData.powerOutSSB[4] = atof(temp);
        break;
      case 98:
        EEPROMData.powerOutSSB[5] = atof(temp);
        break;
      case 99:
        EEPROMData.powerOutSSB[6] = atof(temp);
        break;
      case 100:
        EEPROMData.CWPowerCalibrationFactor[0] = atof(temp);
        break;
      case 101:
        EEPROMData.CWPowerCalibrationFactor[1] = atof(temp);
        break;
      case 102:
        EEPROMData.CWPowerCalibrationFactor[2] = atof(temp);
        break;
      case 103:
        EEPROMData.CWPowerCalibrationFactor[3] = atof(temp);
        break;
      case 104:
        EEPROMData.CWPowerCalibrationFactor[4] = atof(temp);
        break;
      case 105:
        EEPROMData.CWPowerCalibrationFactor[5] = atof(temp);
        break;
      case 106:
        EEPROMData.CWPowerCalibrationFactor[6] = atof(temp);
        break;
      case 107:
        EEPROMData.SSBPowerCalibrationFactor[0] = atof(temp);
        break;
      case 108:
        EEPROMData.SSBPowerCalibrationFactor[1] = atof(temp);
        break;
      case 109:
        EEPROMData.SSBPowerCalibrationFactor[2] = atof(temp);
        break;
      case 110:
        EEPROMData.SSBPowerCalibrationFactor[3] = atof(temp);
        break;
      case 111:
        EEPROMData.SSBPowerCalibrationFactor[4] = atof(temp);
        break;
      case 112:
        EEPROMData.SSBPowerCalibrationFactor[5] = atof(temp);
        break;
      case 113:
        EEPROMData.SSBPowerCalibrationFactor[6] = atof(temp);
        break;
      case 114:
        EEPROMData.IQAmpCorrectionFactor[0] = atof(temp);
        break;
      case 115:
        EEPROMData.IQAmpCorrectionFactor[1] = atof(temp);
        break;
      case 116:
        EEPROMData.IQAmpCorrectionFactor[2] = atof(temp);
        break;
      case 117:
        EEPROMData.IQAmpCorrectionFactor[3] = atof(temp);
        break;
      case 118:
        EEPROMData.IQAmpCorrectionFactor[4] = atof(temp);
        break;
      case 119:
        EEPROMData.IQAmpCorrectionFactor[5] = atof(temp);
        break;
      case 120:
        EEPROMData.IQAmpCorrectionFactor[6] = atof(temp);
        break;
      case 121:
        EEPROMData.IQPhaseCorrectionFactor[0] = atof(temp);
        break;
      case 122:
        EEPROMData.IQPhaseCorrectionFactor[1] = atof(temp);
        break;
      case 123:
        EEPROMData.IQPhaseCorrectionFactor[2] = atof(temp);
        break;
      case 124:
        EEPROMData.IQPhaseCorrectionFactor[3] = atof(temp);
        break;
      case 125:
        EEPROMData.IQPhaseCorrectionFactor[4] = atof(temp);
        break;
      case 126:
        EEPROMData.IQPhaseCorrectionFactor[5] = atof(temp);
        break;
      case 127:
        EEPROMData.IQPhaseCorrectionFactor[6] = atof(temp);
        break;
      case 128:
        EEPROMData.IQXAmpCorrectionFactor[0] = atof(temp);
        break;
      case 129:
        EEPROMData.IQXAmpCorrectionFactor[1] = atof(temp);
        break;
      case 130:
        EEPROMData.IQXAmpCorrectionFactor[2] = atof(temp);
        break;
      case 131:
        EEPROMData.IQXAmpCorrectionFactor[3] = atof(temp);
        break;
      case 132:
        EEPROMData.IQXAmpCorrectionFactor[4] = atof(temp);
        break;
      case 133:
        EEPROMData.IQXAmpCorrectionFactor[5] = atof(temp);
        break;
      case 134:
        EEPROMData.IQXAmpCorrectionFactor[6] = atof(temp);
        break;
      case 135:
        EEPROMData.IQXPhaseCorrectionFactor[0] = atof(temp);
        break;
      case 136:
        EEPROMData.IQXPhaseCorrectionFactor[1] = atof(temp);
        break;
      case 137:
        EEPROMData.IQXPhaseCorrectionFactor[2] = atof(temp);
        break;
      case 138:
        EEPROMData.IQXPhaseCorrectionFactor[3] = atof(temp);
        break;
      case 139:
        EEPROMData.IQXPhaseCorrectionFactor[4] = atof(temp);
        break;
      case 140:
        EEPROMData.IQXPhaseCorrectionFactor[5] = atof(temp);
        break;
      case 141:
        EEPROMData.IQXPhaseCorrectionFactor[6] = atof(temp);
        break;
      case 142:
        EEPROMData.favoriteFreqs[0] = atol(temp);
        break;
      case 143:
        EEPROMData.favoriteFreqs[1] = atol(temp);
        break;
      case 144:
        EEPROMData.favoriteFreqs[2] = atol(temp);
        break;
      case 145:
        EEPROMData.favoriteFreqs[3] = atol(temp);
        break;
      case 146:
        EEPROMData.favoriteFreqs[4] = atol(temp);
        break;
      case 147:
        EEPROMData.favoriteFreqs[5] = atol(temp);
        break;
      case 148:
        EEPROMData.favoriteFreqs[6] = atol(temp);
        break;
      case 149:
        EEPROMData.favoriteFreqs[7] = atol(temp);
        break;
      case 150:
        EEPROMData.favoriteFreqs[8] = atol(temp);
        break;
      case 151:
        EEPROMData.favoriteFreqs[9] = atol(temp);
        break;
      case 152:
        EEPROMData.favoriteFreqs[10] = atol(temp);
        break;
      case 153:
        EEPROMData.favoriteFreqs[11] = atol(temp);
        break;
      case 154:
        EEPROMData.favoriteFreqs[12] = atol(temp);
        break;
      case 155:
        EEPROMData.lastFrequencies[0][0] = atol(temp);
        break;
      case 156:
        EEPROMData.lastFrequencies[1][0] = atol(temp);
        break;
      case 157:
        EEPROMData.lastFrequencies[2][0] = atol(temp);
        break;
      case 158:
        EEPROMData.lastFrequencies[3][0] = atol(temp);
        break;
      case 159:
        EEPROMData.lastFrequencies[4][0] = atol(temp);
        break;
      case 160:
        EEPROMData.lastFrequencies[5][0] = atol(temp);
        break;
      case 161:
        EEPROMData.lastFrequencies[6][0] = atol(temp);
        break;
      case 162:
        EEPROMData.lastFrequencies[0][1] = atol(temp);
        break;
      case 163:
        EEPROMData.lastFrequencies[1][1] = atol(temp);
        break;
      case 164:
        EEPROMData.lastFrequencies[2][1] = atol(temp);
        break;
      case 165:
        EEPROMData.lastFrequencies[3][1] = atol(temp);
        break;
      case 166:
        EEPROMData.lastFrequencies[4][1] = atol(temp);
        break;
      case 167:
        EEPROMData.lastFrequencies[5][1] = atol(temp);
        break;
      case 168:
        EEPROMData.lastFrequencies[6][1] = atol(temp);
        break;
      case 169:

        EEPROMData.centerFreq = atol(temp);
        break;
      case 170:
        strcpy(EEPROMData.mapFileName, temp);
        break;
      case 171:
        strcpy(EEPROMData.myCall, temp);
        break;
      case 172:
        strcpy(EEPROMData.myTimeZone, temp);
        break;
      case 173:
        EEPROMData.separationCharacter = atoi(temp);  //   JJP  7/25/23
        break;
      case 174:
        EEPROMData.paddleFlip = atoi(temp);
        break;
      case 175:
        EEPROMData.sdCardPresent = atoi(temp);  //   JJP  7/18/23
        break;
      case 176:
        EEPROMData.myLat = atof(temp);
        break;
      case 177:
        EEPROMData.myLong = atof(temp);
        break;
      case 178:
        EEPROMData.currentNoiseFloor[0] = atoi(temp);
        break;
      case 179:
        EEPROMData.currentNoiseFloor[1] = atoi(temp);
        break;
      case 180:
        EEPROMData.currentNoiseFloor[2] = atoi(temp);
        break;
      case 181:
        EEPROMData.currentNoiseFloor[3] = atoi(temp);
        break;
      case 182:
        EEPROMData.currentNoiseFloor[4] = atoi(temp);
        break;
      case 183:
        EEPROMData.currentNoiseFloor[5] = atoi(temp);
        break;
      case 184:
        EEPROMData.currentNoiseFloor[6] = atoi(temp);
        break;
      case 185:
        EEPROMData.compressorFlag = atoi(temp);               // JJP  8/28/23
      default:
        Serial.print("Shouldn't be here: lineCount = ");
        Serial.println(lineCount);
        break;
    }
    //   }
    lineCount++;
  }

  file.close();
  //  EEPROM.put(0, EEPROMData);  // This rewrites the entire EEPROM struct as defined in SDT.h
  EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);  // KF5N
                                                //  EEPROMShow();
                                                //  syncEEPROM = 1;  // SD EEPROM same as memory EEPROM  KF5N
  //UpdateEEPROMSyncIndicator(1);     //  JJP 7/25/23
  RedrawDisplayScreen();
  return 1;
}

/*****
  Purpose: Provides a display indicator that the memory-based EEPROM is different than SD card EEPROM

  Parameter list:
    int inSync                    0 = no, memory and SD card not necessarily the same, 1 = same

  Return value;
    void

*****/
void UpdateEEPROMSyncIndicator(int inSync) {
  return;  // Just go home   JJP  7/25/23
  /*
  if (inSync == 0) {                                                   // SD EEPROM different that memory EEPROM
    tft.setFontScale( (enum RA8875tsize) 0);
    //tft.fillRect(SD_X,      BAND_INDICATOR_Y + 70, tft.getFontWidth() * 7, tft.getFontHeight(), RA8875_RED);
    tft.setCursor(SD_X + 2, BAND_INDICATOR_Y + 72);
    tft.setTextColor(RA8875_RED, RA8875_RED);
//    tft.print("EEPROM");

    tft.print((char) ' ');          // JJP 7/25/23
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  } else {                                                                  // Erase message
    tft.fillRect(SD_X, BAND_INDICATOR_Y + 70, tft.getFontWidth() * 2, tft.getFontHeight(), RA8875_BLACK);
  }
  */
}




/*****
  Purpose: Converts EEPROMData members and value to ASCII

  Parameter list:
    File file             file handle for the SD file
    char *buffer          pointer to the EEPROMData member
    int val               the current value of the member
    int whatDataType      1 = int, 2 = long, 3 = float, 4 = string

  Return value;
    void

*****/
void ConvertForEEPROM(File file, char *buffer, int val, int whatDataType) {
  char temp[10];

  temp[0] = '\0';
  switch (whatDataType) {
    case 1:  // int
      itoa(val, temp, DEC);
      break;
    case 2:  // long
      ltoa(val, temp, DEC);
      break;
    case 3:                      // float
      dtostrf(val, 9, 4, temp);  //Field of up to 9 digits with 4 decimal places
      break;
    case 4:
      strcpy(temp, EEPROMSetVersion());
      break;
    default:
#ifdef DEBUG
      Serial.println("Error");
#endif
      break;
  }
  strcat(buffer, " = ");
  strcat(buffer, temp);
#ifdef DEBUG
  Serial.println(buffer);
#endif
  file.println(buffer);
  buffer[0] = '\0';
}


/*****
  Purpose: Writes the current values of the working variable
           to the SD card as SDEEPROMData.txt

  Parameter list:
    void

  Return value;
    int               0 = no write, 1 = write
*****/
int CopyEEPROMToSD() {
  char buffer[100];
  char temp[15];
  char digits[10];
  int i;

  File file = SD.open("SDEEPROMData.txt", O_RDWR);  // Get a file handle
  if (!file) {                                      // Can't open the file
    return 0;                                       // Go home
  }
  file.seek(0L);  // Reset to BOF so we don't append
  strcpy(buffer, "EEPROMData.versionSettings = ");
  strcat(buffer, EEPROMSetVersion());
  file.println(buffer);

  strcpy(buffer, "EEPROMData.AGCMode = ");
  itoa(AGCMode, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.audioVolume = ");
  itoa(audioVolume, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.rfGainAllBands = ");
  itoa(rfGainAllBands, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.spectrumNoiseFloor = ");
  itoa(spectrumNoiseFloor, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.tuneIndex = ");
  itoa(tuneIndex, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.stepFineTune = ");
  ltoa(stepFineTune, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.powerLevel = ");
  itoa(transmitPowerLevel, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.xmtMode = 0");
  itoa(xmtMode, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.nrOptionSelect = ");  // KF5N
  itoa(nrOptionSelect, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentScale = ");
  itoa(currentScale, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.spectrum_zoom = ");  // long data type  KF5N
  ltoa(spectrum_zoom, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.spectrum_display_scale = ");  // float data type
  dtostrf(spectrum_display_scale, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.CWFilterIndex = ");
  itoa(CWFilterIndex, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.paddleDit = ");
  itoa(paddleDit, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.paddleDah = ");
  itoa(paddleDah, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.decoderFlag = ");
  itoa(decoderFlag, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.keyType = ");
  itoa(keyType, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentWPM = ");
  itoa(currentWPM, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.sidetoneVolume = ");  // float data type
  dtostrf(sidetoneVolume, 6, 4, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.cwTransmitDelay = ");  // long data type
  ltoa(cwTransmitDelay, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.activeVFO = 0");
  itoa(activeVFO, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.freqIncrement = ");
  itoa(freqIncrement, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.currentBand = ");
  itoa(currentBand, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentBandA = ");
  itoa(currentBandA, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentBandB = ");
  itoa(currentBandB, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentFreqA = ");  // long data type
  ltoa(currentFreqA, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentFreqB = ");  // long data type
  ltoa(currentFreqB, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.freqCorrectionFactor = ");  // long data type
  ltoa(freqCorrectionFactor, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);

  for (i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.equalizerRec[");  // long data type
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    itoa(recEQ_Level[i], temp, DEC);
    strcat(buffer, temp);
    file.println(buffer);
  }
  for (i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.equalizerXmt[");  // long data type
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    itoa(xmtEQ_Level[i], temp, DEC);
    strcat(buffer, temp);
    file.println(buffer);
  }
  strcpy(buffer, "EEPROMData.currentMicThreshold = ");
  itoa(currentMicThreshold, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentMicCompRatio = ");  // float data type
  dtostrf(currentMicCompRatio, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentMicAttack = ");  // float data type
  dtostrf(currentMicAttack, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentMicRelease = ");  // float data type
  dtostrf(currentMicRelease, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.currentMicGain = ");
  itoa(currentMicGain, temp, DEC);
  strcat(buffer, temp);
  file.println(buffer);

  for (i = 0; i < NUMBER_OF_SWITCHES; i++) {        // switch values
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.switchValues[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    itoa(switchThreshholds[i], temp, DEC);
    strcat(buffer, temp);
    file.println(buffer);
  }

  strcpy(buffer, "EEPROMData.LPFcoeff = ");  // float data type
  dtostrf(LPFcoeff, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EPROMData.NR_PSI = ");  // float data type
  dtostrf(NR_PSI, 6, 1, temp);            // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.NR_alpha = ");  // float data type
  dtostrf(NR_alpha, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.NR_beta = ");  // float data type
  dtostrf(NR_beta, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.omegaN = ");  // float data type
  dtostrf(omegaN, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);
  strcpy(buffer, "EEPROMData.pll_fmax = ");  // float data type
  dtostrf(pll_fmax, 6, 1, temp);             // Field of up to 6 digits with 1 decimal place
  strcat(buffer, temp);
  file.println(buffer);

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // powerOutCW
    itoa(i, digits, DEC);
    strcpy(buffer, "powerOutCW[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.powerOutCW[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // powerOutSSB
    itoa(i, digits, DEC);
    strcpy(buffer, "powerOutSSB[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.powerOutSSB[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // CW Calibration factor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.CWPowerCalibrationFactor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.CWPowerCalibrationFactor[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // SSB Calibration factor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.SSBPowerCalibrationFactor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.SSBPowerCalibrationFactor[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // IQ Amp Correction factor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.IQAmpCorrectionFactor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.IQAmpCorrectionFactor[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // IQ Phase Correction factor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.IQPhaseCorrectionFactor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.IQPhaseCorrectionFactor[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // IQX Amp Correction factor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.IQXAmpCorrectionFactor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.IQXAmpCorrectionFactor[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // IQX Phase Correction factor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.IQXPhaseCorrectionFactor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    dtostrf(EEPROMData.IQXPhaseCorrectionFactor[i], 6, 3, temp);
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < MAX_FAVORITES; i++) {  // Last frequencies
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.favoriteFreqs[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    ltoa(EEPROMData.favoriteFreqs[i], temp, DEC);  // long
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // Last frequencies
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.lastFrequencies[");
    strcat(buffer, digits);
    strcat(buffer, "][0] = ");
    ltoa(EEPROMData.lastFrequencies[i][0], temp, DEC);  // long
    strcat(buffer, temp);
    file.println(buffer);
  }

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // Last frequencies
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.lastFrequencies[");
    strcat(buffer, digits);
    strcat(buffer, "][1] = ");
    ltoa(EEPROMData.lastFrequencies[i][1], temp, DEC);  // long
    strcat(buffer, temp);
    file.println(buffer);
  }

  strcpy(buffer, "EEPROMData.centerFreq = ");  // Center freq
  ltoa(EEPROMData.centerFreq, temp, DEC);      // long
  strcat(buffer, temp);
  file.println(buffer);

  //                                                      JJP 7/3/23
  strcpy(buffer, "EEPROMData.mapFileName = ");  // Map file name
  strncat(buffer, EEPROMData.mapFileName, 50);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.myCall = ");  // Call
  strncat(buffer, EEPROMData.myCall, 10);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.myTimeZone = ");  // Time zone
  strncat(buffer, EEPROMData.myTimeZone, 10);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.separationCharacter = ");  // Separation character in freq display
  itoa(EEPROMData.separationCharacter, digits, DEC);
  strcat(buffer, digits);
  file.println(buffer);

  itoa(paddleFlip, digits, DEC);  // Paddle flip
  strcpy(buffer, "EEPROMData.paddleFlip = ");
  strcat(buffer, digits);
  file.println(buffer);

  itoa(sdCardPresent, temp, DEC);  // SD card
  strcpy(buffer, "EEPROMData.sdCardPresent = ");
  strcat(buffer, temp);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.myLat = ");  // Latitude
  dtostrf(EEPROMData.myLat, 6, 4, temp);
  strcat(buffer, temp);
  file.println(buffer);

  strcpy(buffer, "EEPROMData.myLong = ");  // Longitude
  dtostrf(EEPROMData.myLong, 6, 4, temp);
  strcat(buffer, temp);
  file.println(buffer);

  for (i = 0; i < NUMBER_OF_BANDS; i++) {  // Noise floor
    itoa(i, digits, DEC);
    strcpy(buffer, "EEPROMData.currentNoiseFloor[");
    strcat(buffer, digits);
    strcat(buffer, "] = ");
    itoa(EEPROMData.currentNoiseFloor[i], temp, DEC);
    strcat(buffer, temp);
    file.println(buffer);
  }
  itoa(EEPROMData.compressorFlag, temp, DEC);             // JJP  8/28/23
  strcpy(buffer, "EEPROMData.compressorFlag = ");
  strcat(buffer, temp);
  file.println(buffer);

  file.write(EOF);  // EOF marker
  file.close();
  RedrawDisplayScreen();
  return 1;
}


/*****
  Purpose: See if the EEPROM has ever been set

  Parameter list:
    void

  Return value;
    int               1 = used before, 0 = nope
*****/
int ValidEEPROMData() {
  int val = EEPROMData.switchValues[0];
  if (val > 0 && val < 1023)
    return VALID_EEPROM_DATA;  // return 1
  else
    return INVALID_EEPROM_DATA;  // return 0
}

/*****
  Purpose: Update the version number only in EEPROM

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateEEPROMVersionNumber() {
  strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // Copy the latest version to EEPROM
}



/*****
  Purpose: Reads the SD EEPROM data and writes it to the Serial object

  Parameter list:
    void

  Return value;
    int               0 = SD is untouched, 1 = has data
*****/
void SDEEPROMDump() 
{
  char c;
  int lines = 0;

  if (!SD.begin(chipSelect)) {
    Serial.print("SD card cannot be initialized.");
  }
  // open the file.
  Serial.println("----- Start SD EEPROM File Dump ------");
  File dataFile = SD.open("SDEEPROMData.txt");

  // if the file is available, write to it:
  if (dataFile) {
    //     while (dataFile.available()) {
    //      Serial.write(dataFile.read());
    while (true) {
      c = dataFile.read();
      if (c == 26 || lines > MAX_SD_ITEMS) {  // EOF Marker
        break;
      }
      if (c == '\n') {
        lines++;
      }
      Serial.print(c);
    }
    dataFile.close();
  } else {
    Serial.println("error opening SDEEPROMData.txt");
  }
}

/*****
  Purpose: Clears the first 1K of emulated EEPROM to 0xff

  Parameter list:
    void

  Return value;
    void
*****/
void ClearEEPROM() {
  int i;
  for (i = 0; i < 1000; i++) {
    EEPROM.write(i, 0xFF);
  }
}

/*****
  Purpose: Read the EEPROM from: a) EEPROM memory, b) SD card memory, or c) defaults

  Parameter list:
    void

  Return value;
    void
*****/
void EEPROMStartup() {
  EEPROMRead();  // Read current stored data

  if (strcmp(EEPROMData.versionSettings, EEPROMSetVersion()) == 0) {  // Are the versions the same?
    return;                                                // Yep. Go home and don't mess with the EEPROM
  }
  strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // Nope, this is a new the version, so copy new version title to EEPROM
                                                //                                                                     Check if calibration has not been done and/or switch values are wonky, okay to use defaults
                                                //                                                                     If the Teensy is unused, these EEPROM values are 0xFF or perhaps cleared to 0.

  if (switchThreshholds[9] < 440 || switchThreshholds[9] > 480) {
    EEPROMSaveDefaults2();     // At least give them some starting values
    switchThreshholds[9] = 0;  // This will force the next code block to set the switch values.
  }
  if (switchThreshholds[9] < 440 || switchThreshholds[9] > 480) {  // If the Teensy is unused, these EEPROM values are 0xFF or perhaps cleared to 0.
    SaveAnalogSwitchValues();                                      // In that case, we need to set the switch values.
  }
  //                                                                     If we get here, the switch values have been set, either previously or by the call to
  //                                                                     SaveAnalogSwitchValues() as has the rest of the EEPROM data. This avoids recalibration.

  EEPROM.put(0, EEPROMData);  // This rewrites the entire EEPROM struct as defined in SDT.h
  EEPROMRead();               // Read the EEPROM data, including new switch values. This also resets working variables

//  if (sdCardPresent) {  // If there's an SD card present...
//    CopyEEPROMToSD();   // ...copy the EEPROM data to the SD card, otherwise we're done  // Don't do this.  It could overwrite an existing file.  KF5N August 9, 2023
//  }
#ifdef DEBUG1
  SDEEPROMDump();  // Call this to observe EEPROM struct data
#endif
}
