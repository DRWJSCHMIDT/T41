/*********************************************************************************************

  This comment block must appear in the load page (e.g., main() or setup()) in any source code
  that uses code presented as whole or part of the T41-EP source code.

  (c) Frank Dziock, DD4WH, 2020_05_8
  "TEENSY CONVOLUTION SDR" substantially modified by Jack Purdum, W8TEE, and Al Peter, AC8GY

  This software is made available under the GNU GPLv3 license agreement. If commercial use of this
  software is planned, we would appreciate it if the interested parties contact Jack Purdum, W8TEE, 
  and Al Peter, AC8GY.

  Any and all other uses, written or implied, by the GPLv3 license are forbidden without written 
  permission from from Jack Purdum, W8TEE, and Al Peter, AC8GY.

V049.2 October 16, 2023 Greg Raven (KF5N)
  1. Changed folder to SDT and file to SDT.ino.
  2. Mic compression indicator fixed.
  3. Updates to fix remaining paddle flip issues.
  4. Mic compressor fixes.
  5. CW sidetone adjustment via encoder.
  6. Changed volume control to logarithmic.
  7. Changed instances of currentBandA to currentBand in CW Excite.
  8. Removed Serial print statement.
  9. Removed Serial print statements from volume control.
 10. Increased audio memory to 450.
 11. Working sidetone adjust with encoder.
 12. Perfect log responding volume control for sidetone.
 13. Inserted G0ORX main loop efficiency improvement.  Si5351 CLK2 is now set to PLLB.
 14. John G0ORX AGC enhancements.
 15. AGC updates.
 16. Fixed minor problem with RX/TX indicator showing TX coming out of setup().
 17. Added defines for ITU_REGION and TIME24H for internationalization. (DB2OO)
 18. S-Meter bar aligned to dBm value (DB2OO)
 19. Fixed override of EEPROM defaults in EEPROMRead(). (DB2OO)
 20. Jack changed encoder pins for 4SQRP.
 21. Changed ENCODER3_ENCODER_A/B to FINETUNE_ENCODER_A/B.
 22. Updated SetBandRelay() function to toggle only necessary GPOs.
 23. Fixed receiver outage after sidetone adjust.  Sidetone default = 50.0
 24. S-Meter: added debug code and rfGainAllBands (DB2OO)
 25. Add additional debug info in DisplaydbM() (DB2OO)
 26. Removed attribute DMAMEM from audioSpectBuffer because it breaks the S-meter.
 27. Added Jack's code to change color of version when running 4SQRP.
 28. Deleted increment variable in Encoders.cpp.  Was causing a compile warning.
 29. ITU_REGION set to 2, TCXO_25MHZ undefined, DEBUG_SMETER undefined.
 30. Added comments to ITU_REGION.
 31. Updated changes list in SDT.ino.  Added macro to increment variable in Encoders.cpp to eliminate warning.

V049.1 Aug 21, 2023 Greg Raven (KF5N)
  1. Changed 3 instances of VFO_A to activeVFO in function ButtonFrequencyEntry().
  2. Added TxRxFreq = centerFreq to EEPROMRead() to fix frequency related bugs.
  3. Added clean-up code to CopySDtoEEPROM() in function EEPROMOptions().
  4. Fixes for calibration graphics.
  5. Changed 2 instances of currentFreqA to currentFreq in ResetTuning() function.
  6. Previous fix to CopySDToEEPROM() accidentally overwritten.  This puts it back.
  7. Reactivated text in Direct Frequency Entry.
  8. Replaced keyer symbolic constants with appropriate global variables.
  9. Removed call to CopyEEPROMToSD() from EEPROMStartup().
 10. Bypass paddle flip in the case of a straight key.
 11. Frequency overwrite, Bearing, exit, and Direct Entry updates.
 12. Fixed receive cal not saving, cal graphics, SSB power cal.
 13. Revised RX cal again because it was saving 1X zoom.
 14. Restored option for KD0RC switch matrix calibration code.
 15. A significant refactoring of calibration.
 16. Fix for keyer click-clack.
 17. Keyer lower limit 5 WPM.
 18. Decoder was modifying keyer WPM.

V049a Aug 06, 2023, 2023 Jack Purdum (W8TEE)
  1. Corrected paddle flip issue
  2. Made several changes to set sdCardPresent to the proper value when tested.
  3. Changed frequency display to account for new font.
  4. Raised the frequencies by 2 pixels as the new font made then display too low and affect band info
  5. If only 1 map file found, no longer asks you to pick. It selects automatically the one file.
  FLASH: code:244340, data:123488, headers:8996   free for files:7749640
   RAM1: variables:206304, code:240792, padding:21352   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V049 Aug 03, 2023, 2023 Jack Purdum (W8TEE)
  1. Numerous UI and formating fixes
  FLASH: code:243636, data:123488, headers:8676   free for files:7750664
   RAM1: variables:206304, code:240088, padding:22056   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V048g July 31, 2023 Jack Purdum (W8TEE)
  1. If you have multiple QTH files for the bearing feature, it did not clear screen properly. Fixed.
  2. CW filter graphics managed between mode, band, and VFO changes.
  3. Several tuning fixes by Greg (KF5N)
  FLASH: code:243572, data:123488, headers:8740   free for files:7750664
   RAM1: variables:206304, code:240024, padding:22120   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V048e July 30, 2023 Jack Purdum (W8TEE)
  1. Really made EEPROMSaveDefaults2() the function to initialize the EEPROM struct
  2. Noise filters were not completely erased in all cases after a change. Fixed.
  3. Fixed overrun of spectrum floor if a negative number entered.
  4. If you have multiple QTH files for the bearing feature, it did not clear screen properly. Fixed.
  FLASH: code:246388, data:123488, headers:8996   free for files:7747592
   RAM1: variables:206304, code:242840, padding:19304   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V048d July 29, 2023 Jack Purdum (W8TEE)
  1. Made EEPROMSaveDefaults2() the function to initialize the EEPROM struct
  FLASH: code:246260, data:123488, headers:9124   free for files:7747592
   RAM1: variables:206304, code:242712, padding:19432   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V048c July 28, 2023 Jack Purdum (W8TEE)
  1. Removed string literals for EEPROM switch values from EEPROMToSD() function call.
  2. Reformatted the EEPROMShow() output.
  FLASH: code:246388, data:123488, headers:8996   free for files:7747592
   RAM1: variables:206304, code:242840, padding:19304   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V048b July 28, 2023 Jack Purdum (W8TEE)
  1. More bug fixes by Greg, mainly in the EEPROM code.
  FLASH: code:242804, data:123488, headers:8484   free for files:7751688
   RAM1: variables:206304, code:239256, padding:22888   free for local variables:55840
   RAM2: variables:342944  free for malloc/new:181344

V048a July 27, 2023 Jack Purdum (W8TEE)
  1. FIxed EEPROMWrite() bug.
  FLASH: code:249780, data:131680, headers:8676   free for files:7736328
   RAM1: variables:214496, code:246232, padding:15912   free for local variables:47648
   RAM2: variables:342944  free for malloc/new:181344

V048 July 26, 2023 Jack Purdum (W8TEE)
  1. Integrated Greg's display and encoder changes
  2. Fixed the EEPROM to SD copy routine
  FLASH: code:249780, data:131680, headers:8676   free for files:7736328
   RAM1: variables:214496, code:246232, padding:15912   free for local variables:47648
   RAM2: variables:342944  free for malloc/new:181344
  
V047k July 22, 2023 Greg KF5N
  1. Update to calibration for improved button response.
  2. Added additional improvements for audio quality.
  3. Added user settable noise floor by band feature.
  4. Implemented fix for VFO relay switching bug.
  5. Fixed EEPROM so separator character displays correctly on EEPROM/SD dump
  FLASH: code:239700, data:131680, headers:8516   free for files:7746568
   RAM1: variables:214496, code:236152, padding:25992   free for local variables:47648
   RAM2: variables:342944  free for malloc/new:181344  
   
V047i July 18, 2023 Jack Purdum (W8TEE)
  1. Removed SD_CARD_PRESENT and set global sdCardPresent by call to SDPresentCheck() 
  FLASH: code:237972, data:134128, headers:8820   free for files:7745544
   RAM1: variables:214496, code:234424, padding:27720   free for local variables:47648
   RAM2: variables:342944  free for malloc/new:181344

V047h July 17, 2023 Jack Purdum (W8TEE)
  1. EEPROM Cancel option was not shown on secondary menu.
  2. currentFrreqA and currentFreqB now properly stored in EEPROM when Saved.
  FLASH: code:233556, data:132040, headers:9180   free for files:7751688
   RAM1: variables:212384, code:230008, padding:32136   free for local variables:49760
   RAM2: variables:342944  free for malloc/new:181344
  
V047g July 17, 2023 Jack Purdum (W8TEE)
  1. Move several constants to MyConfigurationFile.h and rearranged them
  2. Increased Splash delay to 4 seconds
  FLASH: code:233556, data:132040, headers:9180   free for files:7751688
   RAM1: variables:212384, code:230008, padding:32136   free for local variables:49760
   RAM2: variables:342944  free for malloc/new:181344

V047f July 15, 2023 Jack Purdum (W8TEE)
  1. Change ShowFrequency() and FormatFrequency() to properly align digits
  2. Fixed EEPROM sync indicator for main tune changes
  3. Commented clearScreen function call in DrawBandWidthIndicatorBar() to clean up audio. KF5N
  4. DMAMEM attribute added to TX equalizer buffers to free up stack space. KF5N
  5. Consolidated Si5351 start-up code into setup() function. KF5N
  6. Revised waterfall.  User needs to adjust noise level (pixels above axis) to just above bottom of spectrum display. KF5N
  FLASH: code:237076, data:134064, headers:8756   free for files:7746568
   RAM1: variables:228832, code:233528, padding:28616   free for local variables:33312
   RAM2: variables:328544  free for malloc/new:195744

Vo47e July 15, 2023 Jack Purdum (W8TEE)
  1. Changed the font to use the onboard font for frequency display
  2. Change SHowFrequency() and FormatFrequency() to accommodate new font.
  FLASH: code:237012, data:134064, headers:8820   free for files:7746568
   RAM1: variables:228832, code:233464, padding:28680   free for local variables:33312
   RAM2: variables:328544  free for malloc/new:195744
 
V047c July 14, 2023 Jack Purdum (W8TEE)
  1. Merged Greg's latest code with Jack's. The main difference is the calibration and bearing code, plus the new header file.
  2. Main tuning fixed
  3. Missing dB scale added back and prints correctly now
  FLASH: code:236820, data:134064, headers:9012   free for files:7746568
   RAM1: variables:228832, code:233272, padding:28872   free for local variables:33312
   RAM2: variables:328544  free for malloc/new:195744

V047b July 13, 2023 Jack Purdum (W8TEE)
  1. FIxed the "waterfall stripe" per Greg email.
  FLASH: code:232980, data:144160, headers:8900   free for files:7740424
   RAM1: variables:227808, code:229432, padding:32712   free for local variables:34336
   RAM2: variables:328544  free for malloc/new:195744

V047a July 3, 2023  Jack Purdum (W8TEE)
  1. Started work on the use of a user configuration file. Added new struct to hold map names, longitude, latitude.
  2. Added code to allow for up to 10 map files for different QTHs. They appear below starting at approximately line 300.
  3. Added new menu, Bearing, to the front of the menu list. It allows you to select the map file to use in bearing calculations
  4. Push button 18 activates the bearing calculation. Assumes the file has already been selected. Press 18 again when done. 17 backspaces.
  5. Removed several debugging Serial.print() calls.
  6. Refactored code in numerous places.
  7. FIxed the wonky color problem if you entered a call prefix that doesn't exist.
  8. Moved several symbolic constants from SDT.h to MyCOnfigurationFile.h.
  FLASH: code:232980, data:144160, headers:8900   free for files:7740424
   RAM1: variables:227808, code:229432, padding:32712   free for local variables:34336
   RAM2: variables:328544  free for malloc/new:195744

V046f July 1, 2023  Jack Purdum (W8TEE)
  1. Changed FREQ_SEP_CHARACTER in SDT.h to comma as use in FormatFrequency() in Display.cpp, near line 648
  2. Removed unused and duplicated symbolic constants.
  FLASH: code:233428, data:144160, headers:8452   free for files:7740424
   RAM1: variables:227808, code:229880, padding:32264   free for local variables:34336
   RAM2: variables:328544  free for malloc/new:195744

V046e June 30, 2023  Jack Purdum (W8TEE) and includes changes by Greg:
  1. Changed the way EEPROM works at startup. It reads the EEPROM and then checks to see if the version numbers match. If so, control
     returns to setup() and continues. IF EEPROM is virgin or has a value in the switch matrix that doesn't make sense. The code is
     the last function in EEPROM.cpp and is heavily commented.
  FLASH: code:229300, data:143144, headers:8476   free for files:7745544
   RAM1: variables:226592, code:225752, padding:3624   free for local variables:68320
   RAM2: variables:328544  free for malloc/new:195744

V046d June 27, 2023  Jack Purdum (W8TEE) and includes changes by Greg:
  1. Added Greg's changes to fix the SSB flip
  2. Fixed sync indicators for EEPROM and SD. Those variables stored in EEPROM may differ from their 
     current values (e.g. WPM) and this causes the EEPROM sync indicator to turn on. SD is RED for no
     SD card, green otherwise.
  3. During calibrate, automatically switch to CW mode and set spectrum scale to 10 dB
  4. Changed menu index on startup to 0. This allows for Menu Down to get to Calibrate option quickly.
  5. Formattted power out to use only 1 decimal place
  FLASH: code:231284, data:144320, headers:8388   free for files:7742472
   RAM1: variables:227616, code:227736, padding:1640   free for local variables:67296
   RAM2: variables:328544  free for malloc/new:195744

 V046c June 25, 2023  Jack Purdum (W8TEE) and includes changes by Greg:
  1. Added symbolic constant UPDATE_SWITCH_MATRIX that is set to 1 by default, which means run the switch calibration code. If set to 0,
     the call to update the switch values is bypassed and the values stored in EEPROM are used. 
  2. Removed the call to EEPROMSaveDefaults() from EEPROMStartup() because the defaults would be overwritten if just the version number 
    was changed. If the user wants to save the defaults, they can uncomment the call in setup().
  FLASH: code:231284, data:144320, headers:8388   free for files:7742472
   RAM1: variables:227616, code:227736, padding:1640   free for local variables:67296
   RAM2: variables:328544  free for malloc/new:195744
  
  

 V046b June 16, 2023  Jack Purdum (W8TEE) and includes changes by Greg:
  1.  Add PROGMEM to line 97 in Bearing.cpp (John's change).
  2.  In EEPROM.cpp, change the strcpy function to strncpy in 2 places.  Add the parameter 10 as the third parameter.  Do this on lines 18 and 160.
  3.  In SDTVer045.ino, change line 1032 to:
      int audioVolume = 30;
  4.  In SDTVer045.ino, comment line 698, thereby removing the global variable audioTemp.
      Also remove it from line 673 of SDT.h (the extern).
      Next, add this line
      int audioTemp;
      to the function DoCWReceiveProcessing in CWProcessing.cpp.
      This is line 40 (remove the commented line).
  5.  in SDTVer045.ino, remove two calls to the function SetFreq() at lines 2493 and 2617.
  6.  Add new buffer play code in the file Process2.cpp at line 360.
  7.  Remove redundant call to ShowSpectrum() at line 246.
  8. Fixed warning of uninitialized use of R[]. It actually was initialized via a pointer, 
     but the compiler doesn't recognize that. Used memset() call in DSP_Fn.cpp, line 103.
     Unneeded, but it bugged me (JJP).
  9. Change Fine Tune increment so it may have values 10, 50, 250, and 500. Press the Fine Tune inrement button
     and it advances to the next value and wraps at the end. EEPROM sync indicator displayed to show this value
     is different than what's in EEPROM. Use meu EEPROM-->Save Current to persist it.
  10. Fixed the "ghosting" image after CW xmit is finished.

 V045 May 12, 2023  Jack Purdum (W8TEE):
    fixed: Al reviewed, fixed, and test all of the Calibrate routines 
    fixed: The IDEs compiler was replaced with the latest incarnation, leading to an increase in the number of warnings. We have fixed those where possible. (Some can't be because of the way the parser works.)
    fixed: The fine tune increment is now stored in EEPROM
  FLASH: code:230964, data:144400, headers:8628   free for files:7742472
   RAM1: variables:256288, code:227416, padding:1960   free for local variables:38624
   RAM2: variables:328544  free for malloc/new:195744
 
  V040a Feb 01, 2023  Jack Purdum (W8TEE):
    Fixed: used wrong character array for first member of the CopyEEPROMToSD() routine. 
    Added: to main increment 100,000 and 1,000,000 to allow faster moves in frequency (i.e., Calibrate on WWV)
    Fixed: Calibration called the wrong function. Now calls the correct function.
  FLASH: code:227096, data:138388, headers:8272   free for files:7752708
   RAM1: variables:246592, code:223432, padding:5944   free for local variables:48320
   RAM2: variables:340864  free for malloc/new:183424
     
  V040 Feb 01, 2023  Jack Purdum (W8TEE):
    Expanded EEPROM code
    Added code to use SD card for EEPROM back up and to calculate bearing heading.
    Added code to process SD card, including EEPROM dump
  FLASH: code:225912, data:137400, headers:8396   free for files:7754756
     RAM1: variables:245536, code:222600, padding:6776   free for local variables:49376
   RAM2: variables:340864  free for malloc/new:183424

  V039d Jan 11, 2023  Jack Purdum (W8TEE):
    Fixed: Requesting switch matrix values when already in place
    Fixed: VFOb problem of not changing to the correct frequecy
    Fixed: Sync problem with SD card and EEPROM
  FLASH: code:226872, data:137576, headers:8284   free for files:7753732
   RAM1: variables:245632, code:223560, padding:5816   free for local variables:49280
   RAM2: variables:340864  free for malloc/new:183424

  V039 Jan 11, 2023  Jack Purdum (W8TEE):
    Activated SD card for EEPROM data and BMP file for bearing mapping. Bearing mapping requires
    creating a BMP file which is copied onto the SD card. The name of the file appears at line
    226 in the Button.cpp file and as a symbolic constant at line 48 in the header file.
  FLASH: code:245832, data:135512, headers:8796   free for files:7736324
   RAM1: variables:243584, code:242520, padding:19624   free for local variables:18560
   RAM2: variables:340864  free for malloc/new:183424

  V034a Nov. 24, 2022  Jack Purdum (W8TEE):
    Fixed the code that generated warnings. Also refactored the code and several math expressions that used constants only.
   FLASH: code:202851, data:99675, headers:8668   free for files:7815176
   RAM1: variables:207104, code:199624, padding:29752   free for local variables:87808
   RAM2: variables:328512  free for malloc/new:195776

  V033_DE  Nov, 11, 2022  Al Peter (AC8GY)
    Added Direct Frequency input using Switch Matrix Base code courtesy of Harry  GM3RVL
  V033  Nov, 06, 2022  Al Peter (AC8GY)
    Revised Receive Cal and Transmit Cal to utilize T41 Excite to provide reference signal - no Sig Gen or Freq Analyzer required.
    Fixed problem with startup parameter setting of bands and last frequencies
    Reorganized EEPROM and formatted printout of EEPROM data
    Added Synchronous AM detection  - SAM
    Set Demod mode and set Receive filters for Favorite frequencies.
    Extended favorite frequecies to add WWV and set proper Receive filter
    Fixed CW mode display of sidetone & audio out
    Changed High Pass filter around DC for 1x spectrum display
    Revised Xmit Cal routine - added real time on-screen spectrum display of cal process
    Added new Mic Compressor with full parameter selection.
    Added Mic Gain Menu option
    Fixed : Print center freq correctly in zoom = 1X
  V031d Oct, 30, 2022  Al Peter (AC8GY)
    Recalibrated Main Spectrum display
    Recalibrated Audio Spectrum display
    Fixed: a couple of zoom anomolies
    Fixed: AM demod window width issue
    Added additional Cal parameter for xmit Power calibration
    Fixed: vol value over-writes AGC display
    Fixed: favorite frequency sets frequency and freq display
    Added Auto band selection when new favorite frequency is in a different band
  V031b Oct, 27, 2022  Al Peter (AC8GY)
    Added PA power output by band for CW and SSB separately.  Added to EEPROM
    Added IQ calibration variables by band.  Each band is separately calibrated.
    Fixed: Filter/demod window tracks Zoom
    Fixed: Set Spectrum Scale display
    Changed AGC indicator
    Added: Fine Tune demod window resets to center at band ends
    Fixed Zoom Fine tune action
  V031a Oct, 27, 2022  Al Peter (AC8GY)
    Fixed: Tuning "jumps" when using Fine Tune
    Reset CW Decode Detection Level
    Revised CW Decode Narrow band filter
    Implimented Power-on reset with button hold per KD0RC
    Fixed AM demodulation
    Adjusted Audio spectrum display height
    Fixed Zoom Display
    Adjusted NR levels
    Moved Clock update to Process
    Refactored several if else to switch case
    Removed unused AM demod code

  V029 Oct, 19, 2022  Al Peter (AC8GY), and Jack Purdum (W8TEE) .
    Added: 5 New CW filters and save choice in EEPROM
    Fixed: VFO jumps
    Fixed: power output
    Fixed: Spectrum scale disappeared after band change
    Fixed: Touch-up on Zoom feature - fixed scale markings
    Fixed: Notch filter
    Added: Power output calibration for both CW and SSB
    Added: Power variables for CW and SSB and calibration variables
  FLASH: code:188404, data:98128, headers:8372   free for files:7831560
   RAM1: variables:200864, code:185128, padding:11480   free for local variables:126816
   RAM2: variables:325920  free for malloc/new:198368

  V027e Oct, 17, 2022  Jack Purdum (W8TEE):
    Fixed: height of filter plots for USB/AM too high by 3 pixels
    Fixed: removed all dead variables from the EEPROM code which affect read(), write(), and show().
    Fixed: only shows red for out of band conditions for both VFOs

  V026f-7 Oct, 10, 2022  Jack Purdum (W8TEE) and Al Peter (AC8GY):
    Added: If the VFO moves outside of a US band, the active VFO frequency is displayed in RED. The band
      limits are found in the bands[] structure array defined in this file.
    Activated: Receiver EQ
    Activated; Mic compression, but not yet tested
    Simplified: Refactored the band up-/down code
    Added: Menu CW option to set sidetone volume
    Fixed: Activated transmit qualizer
    Fixed: Reset fine tune indicator to center line when main tuning changed
    Fixed: AGC attach speed
    Fixed: Band up/down bugs plus several menu prompts and formatting
    Fixed: 80M upper limit was wrong.
    Fixed: Switching between bands now preserves the last frequency of the VFO before the switch so you
    Added: Last used frequency by band and by VFO saved in EEPROM if an EEPROM --> Save is done before poer off
    Fixed: Could lock up after a mode push button change
    Added: Display noise filter currently in use
    Added: EEPROM option to save up to 10 "favorite frequencies
    Fixed: spectrum display scale disappeared after band change
    Added: Sidetone Volume and Transmit Delay variables added to CWOptions choices
    Fixed: When calibrating the switch matrix, some of the prompts overwrote data on the display
    Fixed: Spectrum scale disappeared after the mode was changed
    Added: When changing bands, saves the current frequency before band change. Returning to that band recalls this last frequency
    Fixed: Changing Noise Filter erased part of Zoom message in Information Window when display updated.
    Fixed: Changing VFO's did not change the x axis for frequency
    Fixed: Press Select with no menu selected followed by a menu decrease locked up system
    Added: Small rectangle below the Seconds field to show the Xmt/Rec state of the T41
    Removed: Menu options: Display, Audio post processor, Sidetone. They are 0no longer used.
    Fixed: CW-->Paddle Flip prompt was overwritten
    Moved "CW Fine Tune" message from Audio plot to bottom of Info Window to minimize refresh jitter
    Fixed: Some Noise Reduction filter info was not written to EEPROM and display was awkward and didn't erase correctly
    Changed: Noise floor changed to the number of pixels above the spectrum X axis to plot the noise floor.
    Fixed: The CalibrateFrequency() is now functional. I'll be writing up how to use it with and without a signal generator.
    Fixed: Overwrote Zoom field on setting the Noise filter. Same for Compress field when writing Zoom.
    Fixed: Changing Noise Filter erased part of Zoom message in Information Window when display updated.
    Fixed: Changing VFO's did not change the x axis for frequency
    Fixed: Press Select with no menu selected followed by a menu decrease locked up system
    Added: Small rectangle below the Seconds field to show the Xmt/Rec state of the T41
    Removed: Menu options: Display, Audio post processor, Sidetone. They are 0no longer used.
    Fixed: CW-->Paddle Flip prompt was overwritten
    Moved "CW Fine Tune" message from Audio plot to bottom of Info Window to minimize refresh jitter
    Fixed: Some Noise Reduction filter info was not written to EEPROM and display was awkward and didn't erase correctly
    Changed: Noise floor changed to the number of pixels above the spectrum X axis to plot the noise floor.
    Fixed: The CalibrateFrequency() is now functional. I'll be writing up how to use it with and without a signal generator.
    Fixed: Overwrote Zoom field on setting the Noise filter. Same for Compress field when writing Zoom.
  Memory Usage on Teensy 4.1:
  FLASH: code:188276, data:98016, headers:8612   free for files:7831560
   RAM1: variables:200832, code:185000, padding:11608   free for local variables:126848
   RAM2: variables:323872  free for malloc/new:200416

  V015 Aug, 06, 2022  Jack Purdum (W8TEE): Made the following changes (most "runaway" problems caused by moving to ISR's):
    Fixed: Changed encoders from polling to interrupt-driven
    Fixed: Spectrum set now saved to EEPROM
    Fixed: frequency filter overlay (i.e., the "blue box") so it doesn't "jump" when Fine Tune used
    Fixed: Opaque overlay after band changed
    Fixed: Kim noise reduction has "runaway" (where the code auto-incremented the value)
    Fixed: "AGC" removed from Info Window when turned off: WHITE = Slow, ORANGE = Medium, GREEN = FAST, RED = AGC in action
    Fixed: Part of "T41-EP" in upper-right corner partially erased when doing certain screen updates.
    Fixed: Text under VFO A partially erased on certain screen updates.
    Fixed: Switching modes caused filter overlay to disappear.
    Fixed: Receiver equalization caused "runaway"
    Fixed: transmit equalization caused "runaway"
    Fixed: switching to VFO B and tuning messed up VFO A frequency readout.
    Fixed: Audio post-processing caused "runaway"
    Fixed: RF Gain caused "runaway"
    Fixed: Power setting caused "runaway"
    Fixed: Mic compression caused "runaway"
    Fixed: Mode switch
    Fixed: Pushing Select when no menu item selected; issue error message
    Fixed: EEPROM SetFavoriteFrequency() and GetFavoriteFrequency()
    Fixed: No longer need to remove spectrum or waterfall displays; removed.
    Not fixed yet: Unresponsive S Meter
    Not fixed yet: IQ Manual setting
    Not fixed yet: Other undiscovered bugs...and there will be some!
    Removed dead code, unused variables, etc. Also added Split VFO code
    Removed non-macro Serial.print() statements
  FLASH: code:182384, data:95848, headers:8484   free for files:7839748
   RAM1: variables:165280, code:179112, padding:17496   free for local variables:162400
   RAM2: variables:425824  free for malloc/new:98464

  V010 June, 03, 2022  Jack Purdum (W8TEE): CW sending section still incomplete
    FLASH: code:193424, data:82860, headers:8384   free for files:7841796
      RAM1: variables:162528, code:190152, padding:6456   free for local variables:165152
      RAM2: variables:429920  free for malloc/new:94368
  V025 9-30-22 Al Peter AC8GY : 1. RFGain is hooked up – set gain from -60dB to +10dB
      2.  Optimized the CW straight key lags
      3.  Fixed the Decoder on/off so it is is only on when the front button is pushed to turn it on
      4.  Set up the filter band display so it works correctly at each Zoom level
      5.  Re-organized the T41 States for SSB Receive, SSB Transmit, CW Receive, and CW transmit
      6.  Fixed the interaction between Center tune and Fine tune so hey work properly
      7.  Fixed up fine Tune to work with the spectrum Zoom
      8.  Got rid of a bunch of dead code – display spectrum and waterfall that we don’t use.
      9.  Power Setting calibration in Watts
     10. IQ for Receive and Xmit and frequency cal all combined into one submenu.
     11. Exciter IQ calibration Partly done
     12. Receive EQ connected with menu options

*********************************************************************************************/

// setup() and loop() at the bottom of this file

#ifndef BEENHERE
#include "SDT.h"
#endif
/*                                  Presented here so you can see how the members allign
struct maps {
  char mapNames[50];
  float lat;
  float lon;
}; 
*/
struct maps myMapFiles[10] = {
  { "Cincinnati.bmp", 39.07466, -84.42677 },  // Map name and coordinates for QTH
  { "Denver.bmp", 39.61331, -105.01664 },
  { "Honolulu.bmp", 21.31165, -157.89291 },
  { "SiestaKey.bmp", 27.26657, -82.54197 },
  { "", 0.0, 0.0 },
  { "", 0.0, 0.0 },
  { "", 0.0, 0.0 },
  { "", 0.0, 0.0 },
  { "", 0.0, 0.0 }
};

bool save_last_frequency = false;
struct band bands[NUMBER_OF_BANDS] = {  //AFP Changed 1-30-21 // G0ORX Changed AGC to 20
//freq    band low   band hi   name    mode      Low    Hi  Gain  type    gain  AGC   pixel
//                                             filter filter             correct     offset
//DB2OO, 29-AUG-23: take ITU_REGION into account for band limits
// and changed "gainCorrection" to see the correct dBm value on all bands.
// Calibration done with TinySA as signal generator with -73dBm levels (S9) at the FT8 frequencies
// with V010 QSD with the 12V mod of the pre-amp
#if defined(ITU_REGION) && ITU_REGION == 1
  3700000, 3500000, 3800000, "80M", DEMOD_LSB, -200, -3000, 1, HAM_BAND, -2.0, 20, 20,
  7150000, 7000000, 7200000, "40M", DEMOD_LSB, -200, -3000, 1, HAM_BAND, -2.0, 20, 20,
#else
  3700000, 3500000, 4000000, "80M", DEMOD_LSB, -200, -3000, 1, HAM_BAND, -2.0, 20, 20,
  7150000, 7000000, 7300000, "40M", DEMOD_LSB, -200, -3000, 1, HAM_BAND, -2.0, 20, 20,
#endif
  14200000, 14000000, 14350000, "20M", DEMOD_USB, 3000, 200, 1, HAM_BAND, 2.0, 20, 20,
  18100000, 18068000, 18168000, "17M", DEMOD_USB, 3000, 200, 1, HAM_BAND, 2.0, 20, 20,
  21200000, 21000000, 21450000, "15M", DEMOD_USB, 3000, 200, 1, HAM_BAND, 5.0, 20, 20,
  24920000, 24890000, 24990000, "12M", DEMOD_USB, 3000, 200, 1, HAM_BAND, 6.0, 20, 20,
  28350000, 28000000, 29700000, "10M", DEMOD_USB, 3000, 200, 1, HAM_BAND, 8.5, 20, 20
};

const char *topMenus[] = { "CW Options", "RF Set", "VFO Select",
                           "EEPROM", "AGC", "Spectrum Options",
                           "Noise Floor", "Mic Gain", "Mic Comp",
                           "EQ Rec Set", "EQ Xmt Set", "Calibrate", "Bearing" };
const char *CWFilter[] = { "0.8kHz", "1.0kHz", "1.3kHz", "1.8kHz", "2.0kHz", " Off " };
int (*functionPtr[])() = { &CWOptions, &RFOptions, &VFOSelect,
                           &EEPROMOptions, &AGCOptions, &SpectrumOptions,
                           &ButtonSetNoiseFloor, &MicGainSet, &MicOptions,
                           &EqualizerRecOptions, &EqualizerXmtOptions, &IQOptions, &BearingMaps };
const char *labels[] = { "Select", "Menu Up", "Band Up",
                         "Zoom", "Menu Dn", "Band Dn",
                         "Filter", "DeMod", "Mode",
                         "NR", "Notch", "Noise Floor",
                         "Fine Tune", "Decoder", "Tune Increment",
                         "Reset Tuning", "Frequ Entry", "User 2" };

int switchThreshholds[] = { 921, 867, 818,
                            767, 715, 665,
                            614, 563, 511,
                            460, 406, 352,
                            299, 242, 184,
                            124, 64, 5 };

uint32_t FFT_length = FFT_LENGTH;

extern "C" uint32_t set_arm_clock(uint32_t frequency);

// lowering this from 600MHz to 200MHz makes power consumption less
uint32_t T4_CPU_FREQUENCY = 500000000UL;  //AFP 2-10-21
//uint32_t T4_CPU_FREQUENCY  =  400000000UL;

//======================================== Global object definitions ==================================================
// ===========================  AFP 08-22-22

AudioControlSGTL5000_Extended sgtl5000_1;      //controller for the Teensy Audio Board
AudioConvert_I16toF32 int2Float1, int2Float2;  //Converts Int16 to Float.  See class in AudioStream_F32.h
AudioEffectGain_F32 gain1, gain2;              //Applies digital gain to audio data.  Expected Float data.
AudioEffectCompressor_F32 comp1, comp2;
AudioConvert_F32toI16 float2Int1, float2Int2;  //Converts Float to Int16.  See class in AudioStream_F32.h

AudioInputI2SQuad i2s_quadIn;
AudioOutputI2SQuad i2s_quadOut;

AudioMixer4 recMix_3;  // JJP

AudioMixer4 modeSelectInR;    // AFP 09-01-22
AudioMixer4 modeSelectInL;    // AFP 09-01-22
AudioMixer4 modeSelectInExR;  // AFP 09-01-22
AudioMixer4 modeSelectInExL;  // AFP 09-01-22

AudioMixer4 modeSelectOutL;    // AFP 09-01-22
AudioMixer4 modeSelectOutR;    // AFP 09-01-22
AudioMixer4 modeSelectOutExL;  // AFP 09-01-22
AudioMixer4 modeSelectOutExR;  // AFP 09-01-22

//=============== // AFP 09-01-22
//AudioMixer4           CW_AudioOutR; //AFP 09-01-22
//AudioMixer4           CW_AudioOutL; //AFP 09-01-22

AudioMixer4 CW_AudioOut;

AudioRecordQueue Q_in_L;
AudioRecordQueue Q_in_R;
AudioRecordQueue Q_in_L_Ex;
AudioRecordQueue Q_in_R_Ex;

AudioPlayQueue Q_out_L;
AudioPlayQueue Q_out_R;
AudioPlayQueue Q_out_L_Ex;
AudioPlayQueue Q_out_R_Ex;

// ===============
AudioConnection patchCord1(i2s_quadIn, 0, int2Float1, 0);  //connect the Left input to the Left Int->Float converter
AudioConnection patchCord2(i2s_quadIn, 1, int2Float2, 0);  //connect the Right input to the Right Int->Float converter

AudioConnection_F32 patchCord3(int2Float1, 0, comp1, 0);  //Left.  makes Float connections between objects
AudioConnection_F32 patchCord4(int2Float2, 0, comp2, 0);  //Right.  makes Float connections between objects
AudioConnection_F32 patchCord5(comp1, 0, float2Int1, 0);  //Left.  makes Float connections between objects
AudioConnection_F32 patchCord6(comp2, 0, float2Int2, 0);  //Right.  makes Float connections between objects
//AudioConnection_F32     patchCord3(int2Float1, 0, float2Int1, 0); //Left.  makes Float connections between objects
//AudioConnection_F32     patchCord4(int2Float2, 0, float2Int2, 0); //Right.  makes Float connections between objects

// ===============

AudioConnection patchCord7(float2Int1, 0, modeSelectInExL, 0);  //Input Ex
AudioConnection patchCord8(float2Int2, 0, modeSelectInExR, 0);

AudioConnection patchCord9(i2s_quadIn, 2, modeSelectInL, 0);  //Input Rec
AudioConnection patchCord10(i2s_quadIn, 3, modeSelectInR, 0);

AudioConnection patchCord11(modeSelectInExR, 0, Q_in_R_Ex, 0);  //Ex in Queue
AudioConnection patchCord12(modeSelectInExL, 0, Q_in_L_Ex, 0);

AudioConnection patchCord13(modeSelectInR, 0, Q_in_R, 0);  //Rec in Queue
AudioConnection patchCord14(modeSelectInL, 0, Q_in_L, 0);

AudioConnection patchCord15(Q_out_L_Ex, 0, modeSelectOutExL, 0);  //Ex out Queue
AudioConnection patchCord16(Q_out_R_Ex, 0, modeSelectOutExR, 0);

AudioConnection patchCord17(Q_out_L, 0, modeSelectOutL, 0);  //Rec out Queue
AudioConnection patchCord18(Q_out_R, 0, modeSelectOutR, 0);

AudioConnection patchCord19(modeSelectOutExL, 0, i2s_quadOut, 0);  //Ex out
AudioConnection patchCord20(modeSelectOutExR, 0, i2s_quadOut, 1);
AudioConnection patchCord21(modeSelectOutL, 0, i2s_quadOut, 2);  //Rec out
AudioConnection patchCord22(modeSelectOutR, 0, i2s_quadOut, 3);

AudioConnection patchCord23(Q_out_L_Ex, 0, modeSelectOutL, 1);  //Rec out Queue for sidetone
AudioConnection patchCord24(Q_out_R_Ex, 0, modeSelectOutR, 1);

// ================================== AFP 11-01-22

// ===================

//AudioControlSGTL5000  sgtl5000_1;
AudioControlSGTL5000 sgtl5000_2;
// ===========================  AFP 08-22-22 end

Bounce decreaseBand = Bounce(BAND_MENUS, 50);
Bounce increaseBand = Bounce(BAND_PLUS, 50);
Bounce modeSwitch = Bounce(CHANGE_MODE, 50);
Bounce decreaseMenu = Bounce(MENU_MINUS, 50);
Bounce frequencyIncrement = Bounce(CHANGE_INCREMENT, 50);
Bounce filterSwitch = Bounce(CHANGE_FILTER, 50);
Bounce increaseMenu = Bounce(MENU_PLUS, 50);
Bounce selectExitMenues = Bounce(CHANGE_MENU2, 50);
Bounce changeNR = Bounce(CHANGE_NOISE, 50);
Bounce demodSwitch = Bounce(CHANGE_DEMOD, 50);
Bounce zoomSwitch = Bounce(CHANGE_ZOOM, 50);
Bounce cursorSwitch = Bounce(SET_FREQ_CURSOR, 50);
Bounce KeyPin2 = Bounce(KEYER_DAH_INPUT_RING, 5);
Bounce KeyPin1 = Bounce(KEYER_DIT_INPUT_TIP, 5);

Rotary volumeEncoder = Rotary(VOLUME_ENCODER_A, VOLUME_ENCODER_B);        //( 2,  3)
Rotary tuneEncoder = Rotary(TUNE_ENCODER_A, TUNE_ENCODER_B);              //(16, 17)
Rotary filterEncoder = Rotary(FILTER_ENCODER_A, FILTER_ENCODER_B);        //(15, 14)
Rotary fineTuneEncoder = Rotary(FINETUNE_ENCODER_A, FINETUNE_ENCODER_B);  //( 4,  5)

Metro ms_500 = Metro(500);  // Set up a Metro
Metro ms_300000 = Metro(300000);
Metro encoder_check = Metro(100);  // Set up a Metro

Si5351 si5351;

int radioState, lastState;  // KF5N
int resetTuningFlag = 0;
#ifndef RA8875_DISPLAY
ILI9488_t3 tft = ILI9488_t3(&SPI, TFT_CS, TFT_DC, TFT_RST);
#else
#define RA8875_CS TFT_CS
#define RA8875_RESET TFT_DC  // any pin or nothing!
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
#endif

SPISettings settingsA(70000000UL, MSBFIRST, SPI_MODE1);

const uint32_t N_B_EX = 16;
//================== Receive EQ Variables================= AFP 08-08-22
float32_t recEQ_Level[14];
float32_t recEQ_LevelScale[14];
//Setup for EQ filters
float32_t rec_EQ1_float_buffer_L[256];
float32_t rec_EQ2_float_buffer_L[256];
float32_t rec_EQ3_float_buffer_L[256];
float32_t rec_EQ4_float_buffer_L[256];
float32_t rec_EQ5_float_buffer_L[256];
float32_t rec_EQ6_float_buffer_L[256];
float32_t rec_EQ7_float_buffer_L[256];
float32_t rec_EQ8_float_buffer_L[256];
float32_t rec_EQ9_float_buffer_L[256];
float32_t rec_EQ10_float_buffer_L[256];
float32_t rec_EQ11_float_buffer_L[256];
float32_t rec_EQ12_float_buffer_L[256];
float32_t rec_EQ13_float_buffer_L[256];
float32_t rec_EQ14_float_buffer_L[256];

float32_t rec_EQ_Band1_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };  //declare and zero biquad state variables
float32_t rec_EQ_Band2_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band3_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band4_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band5_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band6_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band7_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band8_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };  //declare and zero biquad state variables
float32_t rec_EQ_Band9_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band10_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band11_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band12_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band13_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t rec_EQ_Band14_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };

//EQ filter instances
arm_biquad_cascade_df2T_instance_f32 S1_Rec = { IIR_NUMSTAGES, rec_EQ_Band1_state, EQ_Band1Coeffs };
arm_biquad_cascade_df2T_instance_f32 S2_Rec = { IIR_NUMSTAGES, rec_EQ_Band2_state, EQ_Band2Coeffs };
arm_biquad_cascade_df2T_instance_f32 S3_Rec = { IIR_NUMSTAGES, rec_EQ_Band3_state, EQ_Band3Coeffs };
arm_biquad_cascade_df2T_instance_f32 S4_Rec = { IIR_NUMSTAGES, rec_EQ_Band4_state, EQ_Band4Coeffs };
arm_biquad_cascade_df2T_instance_f32 S5_Rec = { IIR_NUMSTAGES, rec_EQ_Band5_state, EQ_Band5Coeffs };
arm_biquad_cascade_df2T_instance_f32 S6_Rec = { IIR_NUMSTAGES, rec_EQ_Band6_state, EQ_Band6Coeffs };
arm_biquad_cascade_df2T_instance_f32 S7_Rec = { IIR_NUMSTAGES, rec_EQ_Band7_state, EQ_Band7Coeffs };
arm_biquad_cascade_df2T_instance_f32 S8_Rec = { IIR_NUMSTAGES, rec_EQ_Band8_state, EQ_Band8Coeffs };
arm_biquad_cascade_df2T_instance_f32 S9_Rec = { IIR_NUMSTAGES, rec_EQ_Band9_state, EQ_Band9Coeffs };
arm_biquad_cascade_df2T_instance_f32 S10_Rec = { IIR_NUMSTAGES, rec_EQ_Band10_state, EQ_Band10Coeffs };
arm_biquad_cascade_df2T_instance_f32 S11_Rec = { IIR_NUMSTAGES, rec_EQ_Band11_state, EQ_Band11Coeffs };
arm_biquad_cascade_df2T_instance_f32 S12_Rec = { IIR_NUMSTAGES, rec_EQ_Band12_state, EQ_Band12Coeffs };
arm_biquad_cascade_df2T_instance_f32 S13_Rec = { IIR_NUMSTAGES, rec_EQ_Band13_state, EQ_Band13Coeffs };
arm_biquad_cascade_df2T_instance_f32 S14_Rec = { IIR_NUMSTAGES, rec_EQ_Band14_state, EQ_Band14Coeffs };

// ===============================  AFP 10-02-22 ================
//Setup for Xmit EQ filters
float32_t DMAMEM xmtEQ_Level[14];
//EQBuffers
float32_t DMAMEM xmt_EQ1_float_buffer_L[256];
float32_t DMAMEM xmt_EQ2_float_buffer_L[256];
float32_t DMAMEM xmt_EQ3_float_buffer_L[256];
float32_t DMAMEM xmt_EQ4_float_buffer_L[256];
float32_t DMAMEM xmt_EQ5_float_buffer_L[256];
float32_t DMAMEM xmt_EQ6_float_buffer_L[256];
float32_t DMAMEM xmt_EQ7_float_buffer_L[256];
float32_t DMAMEM xmt_EQ8_float_buffer_L[256];
float32_t DMAMEM xmt_EQ9_float_buffer_L[256];
float32_t DMAMEM xmt_EQ10_float_buffer_L[256];
float32_t DMAMEM xmt_EQ11_float_buffer_L[256];
float32_t DMAMEM xmt_EQ12_float_buffer_L[256];
float32_t DMAMEM xmt_EQ13_float_buffer_L[256];
float32_t DMAMEM xmt_EQ14_float_buffer_L[256];

float32_t xmt_EQ_Band1_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };  //declare and zero biquad state variables
float32_t xmt_EQ_Band2_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band3_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band4_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band5_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band6_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band7_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band8_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band9_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band10_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band11_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band12_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band13_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t xmt_EQ_Band14_state[IIR_NUMSTAGES * 2] = { 0, 0, 0, 0, 0, 0, 0, 0 };

//EQ filter instances
arm_biquad_cascade_df2T_instance_f32 S1_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band1_state, EQ_Band1Coeffs };
arm_biquad_cascade_df2T_instance_f32 S2_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band2_state, EQ_Band2Coeffs };
arm_biquad_cascade_df2T_instance_f32 S3_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band3_state, EQ_Band3Coeffs };
arm_biquad_cascade_df2T_instance_f32 S4_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band4_state, EQ_Band4Coeffs };
arm_biquad_cascade_df2T_instance_f32 S5_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band5_state, EQ_Band5Coeffs };
arm_biquad_cascade_df2T_instance_f32 S6_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band6_state, EQ_Band6Coeffs };
arm_biquad_cascade_df2T_instance_f32 S7_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band7_state, EQ_Band7Coeffs };
arm_biquad_cascade_df2T_instance_f32 S8_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band8_state, EQ_Band8Coeffs };
arm_biquad_cascade_df2T_instance_f32 S9_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band9_state, EQ_Band9Coeffs };
arm_biquad_cascade_df2T_instance_f32 S10_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band10_state, EQ_Band10Coeffs };
arm_biquad_cascade_df2T_instance_f32 S11_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band11_state, EQ_Band11Coeffs };
arm_biquad_cascade_df2T_instance_f32 S12_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band12_state, EQ_Band12Coeffs };
arm_biquad_cascade_df2T_instance_f32 S13_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band13_state, EQ_Band13Coeffs };
arm_biquad_cascade_df2T_instance_f32 S14_Xmt = { IIR_NUMSTAGES, xmt_EQ_Band14_state, EQ_Band14Coeffs };
// ===============================End xmit EQ filter setup  AFP 10-02-22 ================

// HP BiQuad IIR DC filter
float32_t HP_DC_Butter_state[6] = { 0, 0, 0, 0, 0, 0 };
float32_t HP_DC_Butter_state2[2] = { 0, 0 };                                                          // AFP 11-04-11
arm_biquad_cascade_df2T_instance_f32 s1_Receive = { 3, HP_DC_Butter_state, HP_DC_Filter_Coeffs };     //AFP 09-23-22
arm_biquad_cascade_df2T_instance_f32 s1_Receive2 = { 1, HP_DC_Butter_state2, HP_DC_Filter_Coeffs2 };  //AFP 11-04-22
//Hilbert FIR Filters
float32_t FIR_Hilbert_state_L[100 + 256 - 1];
float32_t FIR_Hilbert_state_R[100 + 256 - 1];
arm_fir_instance_f32 FIR_Hilbert_L;
arm_fir_instance_f32 FIR_Hilbert_R;

// CW decode Filters
arm_fir_instance_f32 FIR_CW_DecodeL;  //AFP 10-25-22
arm_fir_instance_f32 FIR_CW_DecodeR;  //AFP 10-25-22
float32_t FIR_CW_DecodeL_state[64 + 256 - 1];
float32_t FIR_CW_DecodeR_state[64 + 256 - 1];

//Decimation and Interpolation Filters
arm_fir_decimate_instance_f32 FIR_dec1_EX_I;
arm_fir_decimate_instance_f32 FIR_dec1_EX_Q;
arm_fir_decimate_instance_f32 FIR_dec2_EX_I;
arm_fir_decimate_instance_f32 FIR_dec2_EX_Q;

arm_fir_interpolate_instance_f32 FIR_int1_EX_I;
arm_fir_interpolate_instance_f32 FIR_int1_EX_Q;
arm_fir_interpolate_instance_f32 FIR_int2_EX_I;
arm_fir_interpolate_instance_f32 FIR_int2_EX_Q;

float32_t DMAMEM FIR_dec1_EX_I_state[2095];
float32_t DMAMEM FIR_dec1_EX_Q_state[2095];

float32_t audioMaxSquaredAve;

float32_t DMAMEM FIR_dec2_EX_I_state[535];
float32_t DMAMEM FIR_dec2_EX_Q_state[535];

float32_t DMAMEM FIR_int2_EX_I_state[519];
float32_t DMAMEM FIR_int2_EX_Q_state[519];
float32_t DMAMEM FIR_int1_EX_coeffs[48];
float32_t DMAMEM FIR_int2_EX_coeffs[48];

float32_t DMAMEM FIR_int1_EX_I_state[279];
float32_t DMAMEM FIR_int1_EX_Q_state[279];

float32_t DMAMEM float_buffer_L_EX[2048];
float32_t DMAMEM float_buffer_R_EX[2048];
float32_t DMAMEM float_buffer_LTemp[2048];
float32_t DMAMEM float_buffer_RTemp[2048];
//==================== End Excite Variables================================

//======================================== Global structure declarations ===============================================
//=== CW Filter ===
float32_t CW_Filter_state[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float32_t CW_AudioFilter1_state[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // AFP 10-18-22
float32_t CW_AudioFilter2_state[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // AFP 10-18-22
float32_t CW_AudioFilter3_state[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // AFP 10-18-22
float32_t CW_AudioFilter4_state[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // AFP 10-18-22
float32_t CW_AudioFilter5_state[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // AFP 10-18-22
//---------  Code Filter instance -----
arm_biquad_cascade_df2T_instance_f32 S1_CW_Filter = { IIR_CW_NUMSTAGES, CW_Filter_state, CW_Filter_Coeffs };
arm_biquad_cascade_df2T_instance_f32 S1_CW_AudioFilter1 = { 6, CW_AudioFilter1_state, CW_AudioFilterCoeffs1 };  // AFP 10-18-22
arm_biquad_cascade_df2T_instance_f32 S1_CW_AudioFilter2 = { 6, CW_AudioFilter2_state, CW_AudioFilterCoeffs2 };  // AFP 10-18-22
arm_biquad_cascade_df2T_instance_f32 S1_CW_AudioFilter3 = { 6, CW_AudioFilter3_state, CW_AudioFilterCoeffs3 };  // AFP 10-18-22
arm_biquad_cascade_df2T_instance_f32 S1_CW_AudioFilter4 = { 6, CW_AudioFilter4_state, CW_AudioFilterCoeffs4 };  // AFP 10-18-22
arm_biquad_cascade_df2T_instance_f32 S1_CW_AudioFilter5 = { 6, CW_AudioFilter5_state, CW_AudioFilterCoeffs5 };  // AFP 10-18-22
//=== end CW Filter ===

struct config_t EEPROMData;

const struct SR_Descriptor SR[18] = {
  // x_factor, x_offset and f1 to f4 are NOT USED ANYMORE !!!
  //   SR_n ,            rate,  text,   f1,   f2,   f3,   f4, x_factor = pixels per f1 kHz in spectrum display, x_offset
  { SAMPLE_RATE_8K, 8000, "  8k", " 1", " 2", " 3", " 4", 64.00, 11 },       // not OK
  { SAMPLE_RATE_11K, 11025, " 11k", " 1", " 2", " 3", " 4", 43.10, 17 },     // not OK
  { SAMPLE_RATE_16K, 16000, " 16k", " 4", " 4", " 8", "12", 64.00, 1 },      // OK
  { SAMPLE_RATE_22K, 22050, " 22k", " 5", " 5", "10", "15", 58.05, 6 },      // OK
  { SAMPLE_RATE_32K, 32000, " 32k", " 5", " 5", "10", "15", 40.00, 24 },     // OK, one more indicator?
  { SAMPLE_RATE_44K, 44100, " 44k", "10", "10", "20", "30", 58.05, 6 },      // OK
  { SAMPLE_RATE_48K, 48000, " 48k", "10", "10", "20", "30", 53.33, 11 },     // OK
  { SAMPLE_RATE_50K, 50223, " 50k", "10", "10", "20", "30", 53.33, 11 },     // NOT OK
  { SAMPLE_RATE_88K, 88200, " 88k", "20", "20", "40", "60", 58.05, 6 },      // OK
  { SAMPLE_RATE_96K, 96000, " 96k", "20", "20", "40", "60", 53.33, 12 },     // OK
  { SAMPLE_RATE_100K, 100000, "100k", "20", "20", "40", "60", 53.33, 12 },   // NOT OK
  { SAMPLE_RATE_101K, 100466, "101k", "20", "20", "40", "60", 53.33, 12 },   // NOT OK
  { SAMPLE_RATE_176K, 176400, "176k", "40", "40", "80", "120", 58.05, 6 },   // OK
  { SAMPLE_RATE_192K, 192000, "192k", "40", "40", "80", "120", 53.33, 12 },  // OK
  { SAMPLE_RATE_234K, 234375, "234k", "40", "40", "80", "120", 53.33, 12 },  // NOT OK
  { SAMPLE_RATE_256K, 256000, "256k", "40", "40", "80", "120", 53.33, 12 },  // NOT OK
  { SAMPLE_RATE_281K, 281000, "281k", "40", "40", "80", "120", 53.33, 12 },  // NOT OK
  { SAMPLE_RATE_353K, 352800, "353k", "40", "40", "80", "120", 53.33, 12 }   // NOT OK
};

const arm_cfft_instance_f32 *S;
const arm_cfft_instance_f32 *iS;
const arm_cfft_instance_f32 *maskS;
const arm_cfft_instance_f32 *NR_FFT;
const arm_cfft_instance_f32 *NR_iFFT;
const arm_cfft_instance_f32 *spec_FFT;

arm_biquad_casd_df1_inst_f32 biquad_lowpass1;
arm_biquad_casd_df1_inst_f32 IIR_biquad_Zoom_FFT_I;
arm_biquad_casd_df1_inst_f32 IIR_biquad_Zoom_FFT_Q;

arm_fir_decimate_instance_f32 FIR_dec1_I;
arm_fir_decimate_instance_f32 FIR_dec1_Q;
arm_fir_decimate_instance_f32 FIR_dec2_I;
arm_fir_decimate_instance_f32 FIR_dec2_Q;
arm_fir_decimate_instance_f32 Fir_Zoom_FFT_Decimate_I;
arm_fir_decimate_instance_f32 Fir_Zoom_FFT_Decimate_Q;
arm_fir_interpolate_instance_f32 FIR_int1_I;
arm_fir_interpolate_instance_f32 FIR_int1_Q;
arm_fir_interpolate_instance_f32 FIR_int2_I;
arm_fir_interpolate_instance_f32 FIR_int2_Q;
arm_lms_norm_instance_f32 LMS_Norm_instance;
arm_lms_instance_f32 LMS_instance;



const DEMOD_Descriptor DEMOD[3] = {
  //   DEMOD_n, name
  { DEMOD_USB, "(USB)" },
  { DEMOD_LSB, "(LSB)" },
  { DEMOD_AM, "(AM)" },  //AFP09-22-22

};
/*
struct dispSc
{
  const char *dbText;
  float32_t   dBScale;
  uint16_t    pixelsPerDB;
  uint16_t    baseOffset;
  float32_t   offsetIncrement;
};
*/
dispSc displayScale[] =  //r *dbText,dBScale, pixelsPerDB, baseOffset, offsetIncrement
  {
    { "20 dB/", 10.0, 2, 24, 1.00 },
    { "10 dB/", 20.0, 4, 10, 0.50 },  //  JJP 7/14/23
    { "5 dB/", 40.0, 8, 58, 0.25 },
    { "2 dB/", 100.0, 20, 120, 0.10 },
    { "1 dB/", 200.0, 40, 200, 0.05 }
  };

//======================================== Global variables declarations for Quad Oscillator 2 ===============================================
long NCOFreq;

int stepFineTuneOld = 0;
long stepFineTune = 50UL;
long stepFineTune2 = 50UL;

float32_t NCO_INC;

double OSC_COS;
double OSC_SIN;
double Osc_Vect_Q = 1.0;
double Osc_Vect_I = 0.0;
double Osc_Gain = 0.0;
double Osc_Q = 0.0;
double Osc_I = 0.0;
float32_t i_temp = 0.0;
float32_t q_temp = 0.0;

//======================================== Global variables declarations ===============================================
//=============================== Any variable initialized to zero is done for documentation ===========================
//=============================== purposes since the compiler does that for globals by default =========================
//================== Global CW Correlation and FFT Variables =================
float32_t corrResult;
uint32_t corrResultIndex;
float32_t cosBuffer2[256];
float32_t cosBuffer3[256];
float32_t cosBuffer4[256];
float32_t sinBuffer[256];
float32_t sinBuffer2[256];
float32_t sinBuffer3[256];
float32_t sinBuffer4[256];
float32_t aveCorrResult;
float32_t aveCorrResultR;
float32_t aveCorrResultL;
float32_t magFFTResults[256];
float32_t float_Corr_Buffer[511];
float32_t corrResultR;
uint32_t corrResultIndexR;
float32_t corrResultL;
uint32_t corrResultIndexL;
float32_t combinedCoeff;
float32_t combinedCoeff2;
float32_t combinedCoeff2Old;
int CWCoeffLevelOld = 0.0;
float CWLevelTimer = 0.0;
float CWLevelTimerOld = 0.0;
float ticMarkTimer = 0.0;
float ticMarkTimerOld = 0.0;
// === Compressor patameters AFP 11-01-22
float min_gain_dB = -20.0, max_gain_dB = 40.0;  //set desired gain range
float gain_dB = 0.0;                            //computed desired gain value in dB
boolean use_HP_filter = true;                   //enable the software HP filter to get rid of DC?
float knee_dBFS, comp_ratio, attack_sec, release_sec;
// ===========
float32_t float_Corr_BufferR[511];
float32_t float_Corr_BufferL[511];
long tempSigTime = 0;

//int audioTemp           = 0;  KF5N
int audioTempPrevious = 0;
float sigStart = 0.0;
float sigDuration = 0.0;
float gapStartData = 0.0;
float gapDurationData = 0.0;
float goertzelMagnitude;

int audioValuePrevious = 0;
int CWOnState;

bool gEEPROM_current = false;  //mdrhere does the data in EEPROM match the current structure contents
bool NR_gain_smooth_enable = false;
bool NR_long_tone_reset = true;
bool NR_long_tone_enable = false;
//bool omitOutputFlag                       = false;
bool timeflag = 0;
bool volumeChangeFlag = false;

char letterTable[] = {
  // Morse coding: dit = 0, dah = 1
  0b101,    // A                first 1 is the sentinel marker
  0b11000,  // B
  0b11010,  // C
  0b1100,   // D
  0b10,     // E
  0b10010,  // F
  0b1110,   // G
  0b10000,  // H
  0b100,    // I
  0b10111,  // J
  0b1101,   // K
  0b10100,  // L
  0b111,    // M
  0b110,    // N
  0b1111,   // O
  0b10110,  // P
  0b11101,  // Q
  0b1010,   // R
  0b1000,   // S
  0b11,     // T
  0b1001,   // U
  0b10001,  // V
  0b1011,   // W
  0b11001,  // X
  0b11011,  // Y
  0b11100   // Z
};

char numberTable[] = {
  0b111111,  // 0
  0b101111,  // 1
  0b100111,  // 2
  0b100011,  // 3
  0b100001,  // 4
  0b100000,  // 5
  0b110000,  // 6
  0b111000,  // 7
  0b111100,  // 8
  0b111110   // 9
};

char punctuationTable[] = {
  0b01101011,  // exclamation mark 33
  0b01010010,  // double quote 34
  0b10001001,  // dollar sign 36
  0b00101000,  // ampersand 38
  0b01011110,  // apostrophe 39
  0b01011110,  // parentheses (L) 40, 41
  0b01110011,  // comma 44
  0b00100001,  // hyphen 45
  0b01010101,  // period  46
  0b00110010,  // slash 47
  0b01111000,  // colon 58
  0b01101010,  // semi-colon 59
  0b01001100,  // question mark 63
  0b01001101,  // underline 95
  0b01101000,  // paragraph
  0b00010001   // break
};
int ASCIIForPunctuation[] = { 33, 34, 36, 39, 41, 44, 45, 46, 47, 58, 59, 63, 95 };  // Indexes into code

long startTime = 0;

char theversion[10];
char decodeBuffer[30];    // The buffer for holding the decoded characters
char keyboardBuffer[10];  // Set for call prefixes. May be increased later
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

char *bigMorseCodeTree = (char *)"-EISH5--4--V---3--UF--------?-2--ARL---------.--.WP------J---1--TNDB6--.--X/-----KC------Y------MGZ7----,Q------O-8------9--0----";
//012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678
//         10        20        30        40        50        60        70        80        90       100       110       120

char mapFileName[50];
char myCall[10];
char myTimeZone[10];
const char *tune_text = "Fast Tune";
const char *zoomOptions[] = { "1x ", "2x ", "4x ", "8x ", "16x" };
char versionSettings[10];

byte currentDashJump = DECODER_BUFFER_SIZE;
byte currentDecoderIndex = 0;

int8_t AGCMode = 2;
int8_t auto_IQ_correction;
int filterWidthX;  // The current filter X.
int filterWidthY;  // The current filter Y.
float32_t pixel_per_khz = ((1 << spectrum_zoom) * SPECTRUM_RES * 1000.0 / SR[SampleRate].rate);
int pos_left = centerLine - (int)(bands[currentBand].FLoCut / 1000.0 * pixel_per_khz);
int centerLine = (MAX_WATERFALL_WIDTH + SPECTRUM_LEFT_X) / 2;
int fLoCutOld;
int fHiCutOld;
int filterWidth = (int)((bands[currentBand].FHiCut - bands[currentBand].FLoCut) / 1000.0 * pixel_per_khz);
int h = SPECTRUM_HEIGHT + 3;
int8_t first_block = 1;

int8_t Menu2 = MENU_F_LO_CUT;
int8_t mesz = -1;
int8_t menuStatus = NO_MENUS_ACTIVE;
int8_t mesz_old = 0;
int8_t NB_taps = 10;
int8_t NB_impulse_samples = 7;
int8_t NR_first_block = 1;
int8_t pos_x_date = 14;
int8_t pos_y_date = 68;
int8_t xmtMode = SSB_MODE;  // 0 = SSB, 1 = CW

uint8_t agc_action = 0;
uint8_t agc_switch_mode = 0;
uint8_t ANR_on = 0;
uint8_t ANR_notch = 0;
uint8_t ANR_notchOn = 0;
uint8_t atan2_approx = 1;
uint8_t auto_codec_gain = 1;
uint8_t audio_flag = 1;
uint8_t bitnumber = 16;  // test, how restriction to twelve bit alters sound quality
uint8_t codec_restarts = 0;
uint8_t dbm_state = 0;
uint8_t dcfParityBit;
uint8_t decay_type = 0;
uint8_t digimode = 0;
uint8_t digits_old[2][10] = { { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 },
                              { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 } };
uint8_t display_dbm = DISPLAY_S_METER_DBM;
uint8_t display_S_meter_or_spectrum_state = 0;
uint8_t eeprom_saved = 0;
uint8_t eeprom_loaded = 0;
uint8_t erase_flag = 0;
uint8_t FIR_filter_window = 1;
uint8_t flagg = 0;
uint8_t half_clip = 0;
uint8_t hang_enable;
uint8_t hour10_old;
uint8_t hour1_old;
uint8_t IQCalFlag = 0;
uint8_t iFFT_flip = 0;
uint8_t IQ_state = 1;
uint8_t IQ_RecCalFlag = 0;
uint8_t keyPressedOn = 0;
uint8_t relayLatch = 0;
uint8_t LastSampleRate = SAMPLE_RATE_192K;
uint8_t minute10_old;
uint8_t minute1_old;
uint8_t NB_on = 0;
uint8_t NB_test = 0;
uint8_t notchButtonState = 0;
uint8_t notchIndex = 0;
uint8_t notches_on[2] = { 0, 0 };
uint8_t NR_first_time = 1;
uint8_t NR_Kim;
uint8_t NR_LMS = 0;
uint8_t NR_Spect;
uint8_t NR_use_X = 0;
uint8_t NR_VAD_enable = 1;
uint8_t precision_flag = 0;
uint8_t quarter_clip = 0;
uint8_t SampleRate = SAMPLE_RATE_192K;
uint8_t save_energy;
uint8_t sch = 0;
uint8_t second10_old;
uint8_t second1_old;
uint8_t show_spectrum_flag = 1;
uint8_t spectrum_mov_average = 0;
uint8_t state = 0;
uint8_t twinpeaks_tested = 2;  // initial value --> 2 !!
uint8_t T41State = 1;
uint8_t wait_flag;
uint8_t which_menu = 1;
uint8_t write_analog_gain = 0;
uint8_t zoom_display = 1;

const uint8_t NR_L_frames = 3;
const uint8_t NR_N_frames = 15;

uint16_t base_y = SPECTRUM_BOTTOM;

int16_t activeVFO;
int16_t currentMode;
int16_t pixelCurrent[SPECTRUM_RES];
int16_t pixelnew[SPECTRUM_RES];
int16_t pixelold[SPECTRUM_RES];
int16_t pixelnew2[MAX_WATERFALL_WIDTH + 1];  //AFP
int16_t pixelold2[MAX_WATERFALL_WIDTH];
int16_t notch_L[2] = { 156, 180 };
int16_t fineEncoderRead;
int16_t notch_R[2] = { 166, 190 };
int16_t notch_pixel_L[2] = { 1, 2 };
int16_t notch_pixel_R[2] = { 2, 3 };
int16_t offsetPixels;
int16_t pos_x_dbm = pos_x_smeter + 170;
int16_t pos_y_dbm = pos_y_smeter - 10;
int16_t pos_y_db;
int16_t pos_y_frequency = 48;
int16_t pos_x_time = 390;  // 14;
int16_t pos_y_time = 5;    //114;
int16_t s_w = 10;
int16_t *sp_L1;
int16_t *sp_R1;
int16_t *sp_L2;
int16_t *sp_R2;
int16_t spectrum_brightness = 255;
int16_t spectrum_height = 96;
int16_t spectrum_pos_centre_f = 64 * xExpand;
int16_t spectrum_WF_height = 96;
int16_t spectrum_x = SPECTRUM_LEFT_X;
int16_t spectrum_y = SPECTRUM_TOP_Y;

uint16_t adcMaxLevel, dacMaxLevel;
uint16_t barGraphUpdate = 0;
uint16_t currentScale = 1;  // 20 dB/division

//===== New histogram stuff ===
int endDitFlag = 0;
volatile int filterEncoderMove = 0;
volatile long fineTuneEncoderMove = 0L;

int currentNoiseFloor[NUMBER_OF_BANDS];
int endGapFlag = 0;
int freqSeparationChar;
int selectedMapIndex;
int topDitIndex;
int topDitIndexOld;
int topGapIndex;
int topGapIndexOld;

int32_t gapHistogram[HISTOGRAM_ELEMENTS];
int32_t signalHistogram[HISTOGRAM_ELEMENTS];

uint32_t histMaxIndexDitOld = 80;  // Defaults for 15wpm
uint32_t histMaxIndexDahOld = 200;

uint32_t histMaxDit;
uint32_t histMaxDah;
uint32_t histMaxIndexDit;
uint32_t histMaxIndexDah;

int atomGapLength;
int atomGapLength2;
int charGapLength;
int charGapLength2;
int receiveEQFlag;
int xmitEQFlag;
int centerTuneFlag = 0;
int x1AdjMax = 0;  //AFP 2-6-23
unsigned long cwTimer;
long signalTime;
unsigned long ditTimerOn;
long DahTimer;
long cwTime0;
long cwTime5;
long cwTime6;
long valRef1;
long valRef2;
long gapRef1;
int valFlag = 0;
long signalStartOld = 0;
int valCounter;
long aveDitLength = 80;
long aveDahLength = 200;
float thresholdGeometricMean = 140.0;  // This changes as decoder runs
float thresholdArithmeticMean;
float aveAtomGapLength = 40;
float thresholdGapGeometricMean;
float thresholdGapArithmeticMean;
long CWFreqShift;
long calFreqShift;
long filter_pos = 0;
long last_filter_pos = 0;
// ============ end new stuff =======


uint16_t notches_BW[2] = { 4, 4 };  // no. of bins --> notch BW = no. of bins * bin_BW
uint16_t temp_check_frequency;
uint16_t uAfter;
uint16_t uB4;
uint16_t xx;

int16_t y_old, y_new, y1_new, y1_old, y_old2;  //A
int16_t y1_old_minus = 0;
int16_t y1_new_minus = 0;

const float32_t DF1 = 4.0;             // decimation factor
const float32_t DF2 = 2.0;             // decimation factor
const float32_t DF = DF1 * DF2;        // decimation factor
const float32_t n_samplerate = 176.0;  // samplerate before decimation

const uint32_t N_B = FFT_LENGTH / 2 / BUFFER_SIZE * (uint32_t)DF;
const uint32_t N_DEC_B = N_B / (uint32_t)DF;
const uint32_t NR_add_counter = 128;

const float32_t n_att = 90.0;        // need here for later def's
const float32_t n_desired_BW = 9.0;  // desired max BW of the filters
const float32_t n_fpass1 = n_desired_BW / n_samplerate;
const float32_t n_fpass2 = n_desired_BW / (n_samplerate / DF1);
const float32_t n_fstop1 = ((n_samplerate / DF1) - n_desired_BW) / n_samplerate;
const float32_t n_fstop2 = ((n_samplerate / (DF1 * DF2)) - n_desired_BW) / (n_samplerate / DF1);

const uint32_t IIR_biquad_Zoom_FFT_N_stages = 4;
const uint32_t N_stages_biquad_lowpass1 = 1;
const uint16_t n_dec1_taps = (1 + (uint16_t)(n_att / (22.0 * (n_fstop1 - n_fpass1))));
const uint16_t n_dec2_taps = (1 + (uint16_t)(n_att / (22.0 * (n_fstop2 - n_fpass2))));

int resultOldFactor;
float incrFactor;
int mute = 0;

float adjustVolEncoder;
int adjustIQ;
int exciteOn = 0;
int agc_decay = 100;
int agc_slope = 100;
int agc_thresh = 30;
int ANR_buff_size = FFT_length / 2.0;
int ANR_delay = 16;
int ANR_dline_size = ANR_DLINE_SIZE;
int ANR_in_idx = 0;
int ANR_mask = ANR_dline_size - 1;
int ANR_position = 0;
int ANR_taps = 64;
int attenuator = 0;
int attack_buffsize;
int audioVolume = 30;  // KF5N JJP 7/14/23
int audioVolumeOld2 = 30;
int audioYPixel[1024];
int audioPostProcessorCells[AUDIO_POST_PROCESSOR_BANDS];

int bandswitchPins[] = {
  30,  // 80M
  31,  // 40M
  28,  // 20M
  29,  // 17M
  29,  // 15M
  0,   // 12M  Note that 12M and 10M both use the 10M filter, which is always in (no relay).  KF5N September 27, 2023.
  0    // 10M
};
int button9State;
int buttonRead = 0;
int calibrateFlag = 0;
int calTypeFlag = 0;
int calOnFlag = 0;
int chipSelect = BUILTIN_SDCARD;
int countryIndex = -1;
int currentBand = BAND_40M;
int currentBandA = BAND_40M;
int currentBandB = BAND_40M;
int CWFilterIndex = 5;  //AFP10-18-22
int dahLength;
int dcfCount;
int dcfLevel;
int dcfSilenceTimer;
int dcfTheSecond;
int dcfPulseTime;
int decoderFlag = DECODER_STATE;  // Startup state for decoder
int demodIndex = 0;               //AFP 2-10-21
int directFreqFlag = 0;
int EEPROMChoice;
int encoderStepOld;
int equalizerRecChoice;
int equalizerXmtChoice;
int fastTuneActive;
int filterLoPositionMarkerOld;
int filterHiPositionMarkerOld;
int FLoCutOld;
int FHiCutOld;
int freqCalibration = -1000;
int freqIncrement = DEFAULTFREQINCREMENT;
int gapAtom;  // Space between atoms
int gapChar;  // Space between characters
int hang_counter = 0;
int helpmin;
int helphour;
int helpday;
int helpmonth;
int helpyear;
int helpsec;
int idx, idpk;
int lidx, hidx;
int IQChoice;
int IQCalType;
int IQEXChoice;
int kDisplay = 0;  //AFP
int keyType;
int LMS_nr_strength;
int LP_F_help = 3500;
int mainTuneEncoder;
int micChoice;
int micGainChoice;
int minPinRead = 1024;
int NR_Index = 0;
int n_L;
int n_R;
int n_tau;
int NBChoice;
int nrOptionSelect;
int newCursorPosition = 0;
int NR_Choice = 0;
int NR_Filter_Value = -1;
int NR_KIM_K_int = 1000;
int NR_VAD_delay = 0;
int NR_VAD_duration = 0;
int oldCursorPosition = 256;
int operatingMode;
int old_demod_mode = -99;
int oldnotchF = 10000;
int out_index = -1;
int paddleDah = KEYER_DAH_INPUT_RING;
int paddleDit = KEYER_DIT_INPUT_TIP;
int paddleFlip = PADDLE_FLIP;
int pmode = 1;
int pos_centre_f = 64;
int pos_x_frequency = 12;
int pos_y_smeter = (spectrum_y - 12);
int rfGainAllBands = 1;

int sdCardPresent = 0;  // Do they have an micro SD card installed?
int secondaryMenuChoiceMade;
int smeterLength;
int spectrumNoiseFloor = SPECTRUM_NOISE_FLOOR;
int splitOn;
int switchFilterSideband = 0;

int syncEEPROM;

int termCursorXpos = 0;
float transmitPowerLevel;
int x2 = 0;  //AFP

int zoom_sample_ptr = 0;
int zoomIndex = 1;                 //AFP 9-26-22
int tuneIndex = DEFAULTFREQINDEX;  //AFP 2-10-21
int wtf;
int updateDisplayFlag = 1;
int xrState;  // Is the T41 in xmit or rec state? 1 = rec, 0 = xmt

const int BW_indicator_y = SPECTRUM_TOP_Y + SPECTRUM_HEIGHT + 2;
const int DEC2STATESIZE = n_dec2_taps + (BUFFER_SIZE * N_B / (uint32_t)DF1) - 1;
const int INT1_STATE_SIZE = 24 + BUFFER_SIZE * N_B / (uint32_t)DF - 1;
const int INT2_STATE_SIZE = 8 + BUFFER_SIZE * N_B / (uint32_t)DF1 - 1;
const int myInput = AUDIO_INPUT_LINEIN;
const int pos_x_smeter = 11;
const int waterfallBottom = spectrum_y + spectrum_height + 4;
const int waterfallTop = spectrum_y + spectrum_height + 4;

unsigned framesize;
unsigned nbits;
unsigned ring_buffsize = RB_SIZE;
unsigned tcr5;
unsigned tcr2div;

int32_t FFT_shift = 2048;
long long freqCorrectionFactor = 68000LL;
long long freqCorrectionFactorOld = 68000LL;
int32_t IFFreq = SR[SampleRate].rate / 4;  // IF (intermediate) frequency
int32_t IF_FREQ1 = 0;
int32_t mainMenuIndex = START_MENU;  // Done so we show menu[0] at startup
int32_t secondaryMenuIndex = -1;     // -1 means haven't determined secondary menu
int32_t subMenuMaxOptions;           // holds the number of submenu options
int32_t subMenuIndex = currentBand;
int32_t O_iiSum19;
int32_t O_integrateCount19;
int32_t spectrum_zoom = SPECTRUM_ZOOM_2;

uint32_t N_BLOCKS = N_B;
uint32_t BUF_N_DF = BUFFER_SIZE * N_BLOCKS / (uint32_t)DF;
uint32_t highAlarmTemp;
uint32_t in_index;
uint32_t lowAlarmTemp;
uint32_t MDR;
uint32_t n_para = 512;
uint32_t NR_X_pointer = 0;
uint32_t NR_E_pointer = 0;
uint32_t IQ_counter = 0;
uint32_t m_NumTaps = (FFT_LENGTH / 2) + 1;
uint32_t panicAlarmTemp; /*!< The panic alarm temperature.*/
uint32_t roomCount;      /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at the hot temperature.*/
uint32_t s_roomC_hotC;   /*!< The value of s_roomCount minus s_hotCount.*/
uint32_t s_hotTemp;      /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at room temperature .*/
uint32_t s_hotCount;     /*!< The value of TEMPMON_TEMPSENSE0[TEMP_VALUE] at the hot temperature.*/
uint32_t twinpeaks_counter = 0;

unsigned long cwTransmitDelay = 2000L;
long averageDit;
long averageDah;

long currentFreq;
long centerFreq = 0L;
long CWRecFreq;                //  = TxRxFreq +/- 750Hz
long currentFreqA = 7150000L;  //Initial VFOA center freq
long currentFreqAOld2 = 0;
long currentFreqB = 7030000;  //Initial VFOB center freq
long currentFreqBOld2 = 0;
long currentWPM = 15L;
long favoriteFrequencies[13];

//long frequencyCorrection;
long incrementValues[] = { 10, 50, 100, 250, 1000, 10000, 100000, 1000000 };
long int n_clear;
long lastFrequencies[NUMBER_OF_BANDS][2];
long notchPosOld;
long notchFreq = 1000;
long notchCenterBin;
long recClockFreq;  //  = TxRxFreq+IFFreq  IFFreq from FreqShift1()=48KHz
long signalElapsedTime;
long spaceSpan;
long signalStart;
long signalEnd;  // Start-end of dit or dah
long spaceStart;
long spaceEnd;
long spaceElapsedTime;
long TxRxFreq;  // = centerFreq+NCOFreq  NCOFreq from FreqShift2()
long TxRxFreqOld;
long TxRxFreqDE;
long gapEnd, gapLength, gapStart;    // Time for noise measures
long ditTime = 80L, dahTime = 240L;  // Assume 15wpm to start

ulong samp_ptr;

uint64_t output12khz;

unsigned long long Clk2SetFreq;                  // AFP 09-27-22
unsigned long long Clk1SetFreq = 1000000000ULL;  // AFP 09-27-22
unsigned long ditLength;
unsigned long transmitDitLength;  // JJP 8/19/23
float dcfRefLevel;
float CPU_temperature = 0.0;

float DD4WH_RF_gain = 6.0;
float help;
float s_hotT_ROOM; /*!< The value of s_hotTemp minus room temperature(25¡æ).*/
float lastII = 0;
float lastQQ = 0;
float myLat = MY_LAT;
float myLong = MY_LON;

float RXbit = 0;
float bitSampleTimer = 0;
float Tsample = 1.0 / 12000.0;
//====== SAM stuff AFP 11-02-22
float32_t a[3 * SAM_PLL_HILBERT_STAGES + 3];
float32_t b[3 * SAM_PLL_HILBERT_STAGES + 3];
float32_t c[3 * SAM_PLL_HILBERT_STAGES + 3];  // Filter c variables
float32_t c0[SAM_PLL_HILBERT_STAGES];
float32_t c1[SAM_PLL_HILBERT_STAGES];
float32_t d[3 * SAM_PLL_HILBERT_STAGES + 3];

float32_t abs_ring[RB_SIZE];
float32_t abs_out_sample;
float32_t ai, bi, aq, bq;
float32_t ai_ps, bi_ps, aq_ps, bq_ps;
float32_t ANR_d[ANR_DLINE_SIZE];
float32_t ANR_den_mult = 6.25e-10;
float32_t ANR_gamma = 0.1;
float32_t ANR_lidx = 120.0;
float32_t ANR_lidx_min = 120.0;
float32_t ANR_lidx_max = 200.0;
float32_t ANR_lincr = 1.0;
float32_t ANR_ldecr = 3.0;
float32_t ANR_ngamma = 0.001;
float32_t ANR_two_mu = 0.0001;
float32_t ANR_w[ANR_DLINE_SIZE];
float32_t attack_mult;
float32_t audio;
float32_t audiotmp = 0.0f;
float32_t audiou;
float32_t audioSpectBuffer[1024];  // This can't be DMAMEM.  It will break the S-Meter.  KF5N October 10, 2023
float32_t bass = 0.0;
float32_t farnsworthValue;
int currentMicThreshold;  // Don't need to define here, will happen with EEPROMRead().  KF5N August 27, 2023
float currentMicCompRatio = 5.0;
float currentMicAttack = 0.1;
float currentMicRelease = 2.0;
int currentMicGain = -10;
int compressorFlag = 0;
float32_t midbass = 0.0;
float32_t mid = 0.0;
float32_t midtreble = 0.0;
float32_t treble = 0.0;
float32_t bin_BW = 1.0 / (DF * FFT_length) * SR[SampleRate].rate;
float32_t bin = 2000.0 / bin_BW;
float32_t biquad_lowpass1_state[N_stages_biquad_lowpass1 * 4];
float32_t biquad_lowpass1_coeffs[5 * N_stages_biquad_lowpass1] = { 0, 0, 0, 0, 0 };
float32_t DMAMEM buffer_spec_FFT[1024] __attribute__((aligned(4)));
float32_t coefficient_set[5] = { 0, 0, 0, 0, 0 };
float32_t corr[2];
float32_t Cos = 0.0;
float32_t cursorIncrementFraction;
//float32_t CPU_temperature               = 0.0;

float32_t dbm = -145.0;
;
float32_t dbm_calibration = 22.0;
float32_t dbm_old = -145.0;
;
float32_t dbmhz = -145.0;
float32_t decay_mult;
float32_t display_offset;
float32_t DMAMEM FFT_buffer[FFT_LENGTH * 2] __attribute__((aligned(4)));
float32_t DMAMEM FFT_spec[1024];
float32_t DMAMEM FFT_spec_old[1024];
float32_t dsI;
float32_t dsQ;
float32_t fast_backaverage;
float32_t fast_backmult;
float32_t fast_decay_mult;


float32_t DMAMEM FIR_Coef_I[(FFT_LENGTH / 2) + 1];
float32_t DMAMEM FIR_Coef_Q[(FFT_LENGTH / 2) + 1];
float32_t DMAMEM FIR_dec1_I_state[n_dec1_taps + (uint16_t)BUFFER_SIZE * (uint32_t)N_B - 1];
float32_t DMAMEM FIR_dec2_I_state[DEC2STATESIZE];
float32_t DMAMEM FIR_dec2_coeffs[n_dec2_taps];
float32_t DMAMEM FIR_dec2_Q_state[DEC2STATESIZE];
float32_t DMAMEM FIR_int2_I_state[INT2_STATE_SIZE];
float32_t DMAMEM FIR_int2_Q_state[INT2_STATE_SIZE];
float32_t DMAMEM FIR_int1_coeffs[48];
float32_t DMAMEM FIR_int2_coeffs[32];
float32_t DMAMEM FIR_dec1_Q_state[n_dec1_taps + (uint16_t)BUFFER_SIZE * (uint16_t)N_B - 1];
float32_t DMAMEM FIR_dec1_coeffs[n_dec1_taps];
float32_t DMAMEM FIR_filter_mask[FFT_LENGTH * 2] __attribute__((aligned(4)));
float32_t DMAMEM FIR_int1_I_state[INT1_STATE_SIZE];
float32_t DMAMEM FIR_int1_Q_state[INT1_STATE_SIZE];
float32_t DMAMEM Fir_Zoom_FFT_Decimate_I_state[4 + BUFFER_SIZE * N_B - 1];
float32_t DMAMEM Fir_Zoom_FFT_Decimate_Q_state[4 + BUFFER_SIZE * N_B - 1];
float32_t DMAMEM Fir_Zoom_FFT_Decimate_coeffs[4];
float32_t fixed_gain = 1.0;
float32_t DMAMEM float_buffer_L[BUFFER_SIZE * N_B];
float32_t DMAMEM float_buffer_R[BUFFER_SIZE * N_B];
float32_t DMAMEM float_buffer_L2[BUFFER_SIZE * N_B];
float32_t DMAMEM float_buffer_R2[BUFFER_SIZE * N_B];
float32_t float_buffer_L_3[BUFFER_SIZE * N_B];
float32_t float_buffer_R_3[BUFFER_SIZE * N_B];

float32_t DMAMEM float_buffer_L_CW[256];       //AFP 09-01-22
float32_t DMAMEM float_buffer_R_CW[256];       //AFP 09-01-22
float32_t DMAMEM float_buffer_R_AudioCW[256];  //AFP 10-18-22
float32_t DMAMEM float_buffer_L_AudioCW[256];  //AFP 10-18-22
float32_t hang_backaverage;
float32_t hang_backmult;
float32_t hang_decay_mult;
float32_t hang_thresh;
float32_t hang_level;
float32_t hangtime;
float32_t hh1 = 0.0;
float32_t hh2 = 0.0;
float32_t DMAMEM iFFT_buffer[FFT_LENGTH * 2 + 1];
float32_t I_old = 0.2;
float32_t I_sum;
float32_t IIR_biquad_Zoom_FFT_I_state[IIR_biquad_Zoom_FFT_N_stages * 4];
float32_t IIR_biquad_Zoom_FFT_Q_state[IIR_biquad_Zoom_FFT_N_stages * 4];
float32_t inv_max_input;
float32_t inv_out_target;

float32_t IQAmpCorrectionFactor[7] = { 1, 1.024, 1, 1, 1, 1, 1 };
float32_t IQPhaseCorrectionFactor[7] = { 0, 0.007, 0, 0, 0, 0, 0 };
float32_t IQXAmpCorrectionFactor[7] = { 1, 1.097, 1, 1, 1, 1, 1 };
float32_t IQXPhaseCorrectionFactor[7] = { 0, 0.193, 0, 0, 0, 0, 0 };

/*float32_t IQAmpCorrectionFactorUSB[7]        = {1,1.057,1,1,1,1,1};
  float32_t IQPhaseCorrectionFactorUSB[7]      = {0,-0.02,0,0,0,0,0};
  float32_t IQXAmpCorrectionFactorUSB[7]       = {1,1.097,1,1,1,1,1};
  float32_t IQXPhaseCorrectionFactor[7]     = {0,0.193,0,0,0,0,0};*/

float32_t IQ_sum = 0.0;
float32_t K_dirty = 0.868;
float32_t K_est = 1.0;
float32_t K_est_old = 0.0;
float32_t K_est_mult = 1.0 / K_est;
float32_t last_dc_level = 0.0f;
float32_t DMAMEM last_sample_buffer_L[BUFFER_SIZE * N_DEC_B];
float32_t DMAMEM last_sample_buffer_R[BUFFER_SIZE * N_DEC_B];
float32_t DMAMEM L_BufferOffset[BUFFER_SIZE * N_B];
float32_t LMS_errsig1[256 + 10];
float32_t LMS_NormCoeff_f32[MAX_LMS_TAPS + MAX_LMS_DELAY];
float32_t LMS_nr_delay[512 + MAX_LMS_DELAY];
float32_t LMS_StateF32[MAX_LMS_TAPS + MAX_LMS_DELAY];
float32_t LPFcoeff;
float32_t LP_Astop = 90.0;
float32_t LP_Fpass = 3500.0;
float32_t LP_Fstop = 3600.0;
float32_t LPF_spectrum = 0.82;
float32_t M_c1 = 0.0;
float32_t M_c2 = 0.0;
float32_t m_AttackAlpha = 0.03;
float32_t m_AttackAvedbm = -73.0;
float32_t m_DecayAvedbm = -73.0;
;
float32_t m_DecayAlpha = 0.01;
float32_t m_AverageMagdbm = -73.0;
;
float32_t m_AttackAvedbmhz = -103.0;
float32_t m_DecayAvedbmhz = -103.0;
float32_t m_AverageMagdbmhz = -103.0;
float32_t max_gain;
float32_t max_input = -0.1;
float32_t min_volts;

float32_t noiseThreshhold;
float32_t notches[10] = { 500.0, 1000.0, 1500.0, 2000.0, 2500.0, 3000.0, 3500.0, 4000.0, 4500.0, 5000.0 };
float32_t DMAMEM NR_FFT_buffer[512] __attribute__((aligned(4)));
float32_t NR_sum = 0;
float32_t NR_PSI = 3.0;
float32_t NR_KIM_K = 1.0;
float32_t NR_alpha = 0.95;
float32_t NR_onemalpha = (1.0 - NR_alpha);
float32_t NR_beta = 0.85;
float32_t NR_onemtwobeta = (1.0 - (2.0 * NR_beta));
float32_t NR_onembeta = 1.0 - NR_beta;
float32_t NR_G_bin_m_1;
float32_t NR_G_bin_p_1;
float32_t NR_T;
float32_t DMAMEM NR_output_audio_buffer[NR_FFT_L];
float32_t DMAMEM NR_last_iFFT_result[NR_FFT_L / 2];
float32_t DMAMEM NR_last_sample_buffer_L[NR_FFT_L / 2];
float32_t DMAMEM NR_last_sample_buffer_R[NR_FFT_L / 2];
float32_t DMAMEM NR_X[NR_FFT_L / 2][3];
float32_t DMAMEM NR_E[NR_FFT_L / 2][15];
float32_t DMAMEM NR_M[NR_FFT_L / 2];
float32_t DMAMEM NR_Nest[NR_FFT_L / 2][2];  //
float32_t NR_vk;
float32_t DMAMEM NR_lambda[NR_FFT_L / 2];
float32_t DMAMEM NR_Gts[NR_FFT_L / 2][2];
float32_t DMAMEM NR_G[NR_FFT_L / 2];
float32_t DMAMEM NR_SNR_prio[NR_FFT_L / 2];
float32_t DMAMEM NR_SNR_post[NR_FFT_L / 2];
float32_t NR_SNR_post_pos;
float32_t DMAMEM NR_Hk_old[NR_FFT_L / 2];
float32_t NR_VAD = 0.0;
float32_t NR_VAD_thresh = 6.0;
float32_t DMAMEM NR_long_tone[NR_FFT_L / 2][2];
float32_t DMAMEM NR_long_tone_gain[NR_FFT_L / 2];
float32_t NR_long_tone_alpha = 0.9999;
float32_t NR_long_tone_thresh = 12000;
float32_t NR_gain_smooth_alpha = 0.25;
float32_t NR_temp_sum = 0.0;
float32_t NB_thresh = 2.5;
float32_t offsetDisplayDB = 10.0;


// new synchronous AM PLL & PHASE detector
// wdsp Warren Pratt, 2016
//float32_t Sin = 0.0;
//float32_t Cos = 0.0;
float32_t pll_fmax = +4000.0;
int zeta_help = 65;
float32_t zeta = (float32_t)zeta_help / 100.0;  // PLL step response: smaller, slower response 1.0 - 0.1
float32_t omegaN = 200.0;                       // PLL bandwidth 50.0 - 1000.0

//pll  AFP 11-03-22
float32_t omega_min = TPI * -pll_fmax * 1 / 24000;
float32_t omega_max = TPI * pll_fmax * 1 / 24000;
float32_t g1 = 1.0 - exp(-2.0 * omegaN * zeta * 1 / 24000);
float32_t g2 = -g1 + 2.0 * (1 - exp(-omegaN * zeta * 1 / 24000) * cosf(omegaN * 1 / 24000 * sqrtf(1.0 - zeta * zeta)));
float32_t phzerror = 0.0;
float32_t det = 0.0;
float32_t fil_out = 0.0;
float32_t del_out = 0.0;
float32_t omega2 = 0.0;

//fade leveler  // AFP 11-03-22
float32_t tauR = 0.02;  // original 0.02;
float32_t tauI = 1.4;   // original 1.4;
float32_t dc = 0.0;
float32_t dc_insert = 0.0;
float32_t dcu = 0.0;
float32_t dc_insertu = 0.0;
float32_t mtauR = exp(-1 / 24000 * tauR);
float32_t onem_mtauR = 1.0 - mtauR;
float32_t mtauI = exp(-1 / 24000 * tauI);
float32_t onem_mtauI = 1.0 - mtauI;
uint8_t fade_leveler = 1;

float32_t onemfast_backmult;
float32_t onemhang_backmult;
float32_t out_sample[2];
float32_t out_targ;
float32_t out_target;
float32_t P_dirty = 1.0;
float32_t P_est;
float32_t P_est_old;
float32_t P_est_mult = 1.0 / (sqrtf(1.0 - P_est * P_est));

float32_t phaseLO = 0.0;
float32_t pop_ratio;
float32_t powerOutSSB[7] = { 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03 };                       // AFP 10-28-22
float32_t powerOutCW[7] = { 0.017, 0.02, 0.025, 0.03, 0.03, 0.039, 0.02 };                     // AFP 10-28-22
float32_t CWPowerCalibrationFactor[7] = { 0.019, 0.0190, .0190, .0190, .0190, .0190, .019 };   //AFP 10-29-22
float32_t SSBPowerCalibrationFactor[7] = { 0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.008 };  //AFP 10-29-22       = 0.008;  //AFP 10-21-22
float32_t Q_old = 0.2;
float32_t Q_sum;
float32_t DMAMEM R_BufferOffset[BUFFER_SIZE * N_B];
float32_t ring[RB_SIZE * 2];
float32_t ring_max = 0.0;
float32_t sidetoneVolume;
float32_t Sin = 0.0;
float32_t sample_meanL = 0.0;
float32_t sample_meanR = 0.0;
float32_t sample_meanLNew = 0.0;  //AFP 10-11-22
float32_t sample_meanRNew = 0.0;
float32_t save_volts = 0.0;
float32_t slope_constant;
float32_t SAM_carrier = 0.0;                 //AFP 11-02-22
float32_t SAM_lowpass = 2700.0;              //AFP 11-02-22
float32_t SAM_carrier_freq_offset = 0.0;     //AFP 11-02-22
float32_t SAM_carrier_freq_offsetOld = 0.0;  //AFP 11-02-22
float32_t spectrum_display_scale = 20.0;     // 30.0
float32_t stereo_factor = 100.0;
float32_t tau_attack;
float32_t tau_decay;
float32_t tau_fast_backaverage = 0.0;
float32_t tau_fast_decay;
float32_t tau_hang_backmult;
float32_t tau_hang_decay;

float32_t teta1 = 0.0;
float32_t teta2 = 0.0;
float32_t teta3 = 0.0;
float32_t teta1_old = 0.0;
float32_t teta2_old = 0.0;
float32_t teta3_old = 0.0;
float32_t tmp;
float32_t var_gain;
float32_t volts = 0.0;
float32_t w;
float32_t wold = 0.0f;


float angl;
float bitSamplePeriod = 1.0 / 500.0;
float bsq, usq;
float cf1, cf2;
float dcfMean;
float dcfSum;
float lolim, hilim;
float partr, parti;
float pi = 3.14159265358979;
float tau;
float temp;
float xExpand = 1.5;  //
float x;

const float32_t sqrtHann[256] = {
  0, 0.01231966, 0.024637449, 0.036951499, 0.049259941, 0.061560906,
  0.073852527, 0.086132939, 0.098400278, 0.110652682, 0.122888291, 0.135105247, 0.147301698,
  0.159475791, 0.171625679, 0.183749518, 0.195845467, 0.207911691, 0.219946358, 0.231947641, 0.24391372,
  0.255842778, 0.267733003, 0.279582593, 0.291389747, 0.303152674, 0.314869589, 0.326538713, 0.338158275,
  0.349726511, 0.361241666, 0.372701992, 0.384105749, 0.395451207, 0.406736643, 0.417960345, 0.429120609,
  0.440215741, 0.451244057, 0.462203884, 0.473093557, 0.483911424, 0.494655843, 0.505325184, 0.515917826,
  0.526432163, 0.536866598, 0.547219547, 0.557489439, 0.567674716, 0.577773831, 0.587785252, 0.597707459,
  0.607538946, 0.617278221, 0.626923806, 0.636474236, 0.645928062, 0.65528385, 0.664540179, 0.673695644,
  0.682748855, 0.691698439, 0.700543038, 0.709281308, 0.717911923, 0.726433574, 0.734844967, 0.743144825,
  0.75133189, 0.759404917, 0.767362681, 0.775203976, 0.78292761, 0.790532412, 0.798017227, 0.805380919,
  0.812622371, 0.819740483, 0.826734175, 0.833602385, 0.840344072, 0.846958211, 0.853443799, 0.859799851,
  0.866025404, 0.872119511, 0.878081248, 0.88390971, 0.889604013, 0.895163291, 0.900586702, 0.905873422,
  0.911022649, 0.916033601, 0.920905518, 0.92563766, 0.930229309, 0.934679767, 0.938988361, 0.943154434,
  0.947177357, 0.951056516, 0.954791325, 0.958381215, 0.961825643, 0.965124085, 0.968276041, 0.971281032,
  0.974138602, 0.976848318, 0.979409768, 0.981822563, 0.984086337, 0.986200747, 0.988165472, 0.989980213,
  0.991644696, 0.993158666, 0.994521895, 0.995734176, 0.996795325, 0.99770518, 0.998463604, 0.999070481,
  0.99952572, 0.99982925, 0.999981027, 0.999981027, 0.99982925, 0.99952572, 0.999070481, 0.998463604,
  0.99770518, 0.996795325, 0.995734176, 0.994521895, 0.993158666, 0.991644696, 0.989980213, 0.988165472,
  0.986200747, 0.984086337, 0.981822563, 0.979409768, 0.976848318, 0.974138602, 0.971281032, 0.968276041,
  0.965124085, 0.961825643, 0.958381215, 0.954791325, 0.951056516, 0.947177357, 0.943154434, 0.938988361,
  0.934679767, 0.930229309, 0.92563766, 0.920905518, 0.916033601, 0.911022649, 0.905873422, 0.900586702,
  0.895163291, 0.889604013, 0.88390971, 0.878081248, 0.872119511, 0.866025404, 0.859799851, 0.853443799,
  0.846958211, 0.840344072, 0.833602385, 0.826734175, 0.819740483, 0.812622371, 0.805380919, 0.798017227,
  0.790532412, 0.78292761, 0.775203976, 0.767362681, 0.759404917, 0.75133189, 0.743144825, 0.734844967,
  0.726433574, 0.717911923, 0.709281308, 0.700543038, 0.691698439, 0.682748855, 0.673695644, 0.664540179,
  0.65528385, 0.645928062, 0.636474236, 0.626923806, 0.617278221, 0.607538946, 0.597707459, 0.587785252,
  0.577773831, 0.567674716, 0.557489439, 0.547219547, 0.536866598, 0.526432163, 0.515917826, 0.505325184,
  0.494655843, 0.483911424, 0.473093557, 0.462203884, 0.451244057, 0.440215741, 0.429120609, 0.417960345,
  0.406736643, 0.395451207, 0.384105749, 0.372701992, 0.361241666, 0.349726511, 0.338158275, 0.326538713,
  0.314869589, 0.303152674, 0.291389747, 0.279582593, 0.267733003, 0.255842778, 0.24391372, 0.231947641,
  0.219946358, 0.207911691, 0.195845467, 0.183749518, 0.171625679, 0.159475791, 0.147301698, 0.135105247,
  0.122888291, 0.110652682, 0.098400278, 0.086132939, 0.073852527, 0.061560906, 0.049259941, 0.036951499,
  0.024637449, 0.01231966, 0
};

// Voltage in one-hundred 1 dB steps for volume control.
const float32_t volumeLog[] = { 0.000010, 0.000011, 0.000013, 0.000014, 0.000016, 0.000018, 0.000020, 0.000022, 0.000025, 0.000028,
                                0.000032, 0.000035, 0.000040, 0.000045, 0.000050, 0.000056, 0.000063, 0.000071, 0.000079, 0.000089,
                                0.000100, 0.000112, 0.000126, 0.000141, 0.000158, 0.000178, 0.000200, 0.000224, 0.000251, 0.000282,
                                0.000316, 0.000355, 0.000398, 0.000447, 0.000501, 0.000562, 0.000631, 0.000708, 0.000794, 0.000891,
                                0.001000, 0.001122, 0.001259, 0.001413, 0.001585, 0.001778, 0.001995, 0.002239, 0.002512, 0.002818,
                                0.003162, 0.003548, 0.003981, 0.004467, 0.005012, 0.005623, 0.006310, 0.007079, 0.007943, 0.008913,
                                0.010000, 0.011220, 0.012589, 0.014125, 0.015849, 0.017783, 0.019953, 0.022387, 0.025119, 0.028184,
                                0.031623, 0.035481, 0.039811, 0.044668, 0.050119, 0.056234, 0.063096, 0.070795, 0.079433, 0.089125,
                                0.100000, 0.112202, 0.125893, 0.141254, 0.158489, 0.177828, 0.199526, 0.223872, 0.251189, 0.281838,
                                0.316228, 0.354813, 0.398107, 0.446684, 0.501187, 0.562341, 0.630957, 0.707946, 0.794328, 0.891251, 1.000000 };

double elapsed_micros_idx_t = 0;
double elapsed_micros_mean;
double elapsed_micros_sum;

/*****
  Purpose: To read the local time

  Parameter list:
    void

  Return value:
    time_t                a time data point
*****/
time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

//#pragma GCC diagnostic ignored "-Wunused-variable"

PROGMEM
//==================================================================================

/*****
  Purpose: To set the codec gain

  Parameter list:
    void

  Return value:
    void

*****/
void Codec_gain() {
  static uint32_t timer = 0;
  timer++;
  if (timer > 10000) timer = 10000;
  if (half_clip == 1)  // did clipping almost occur?
  {
    if (timer >= 20)  // 100  // has enough time passed since the last gain decrease?
    {
      if (bands[currentBand].RFgain != 0)  // yes - is this NOT zero?
      {
        bands[currentBand].RFgain -= 1;  // decrease gain one step, 1.5dB
        if (bands[currentBand].RFgain < 0) {
          bands[currentBand].RFgain = 0;
        }
        timer = 0;  // reset the adjustment timer
        AudioNoInterrupts();
        AudioInterrupts();
        if (Menu2 == MENU_RF_GAIN) {
          //         ShowMenu(1);
        }
      }
    }
  } else if (quarter_clip == 0)  // no clipping occurred
  {
    if (timer >= 50)  // 500   // has it been long enough since the last increase?
    {
      bands[currentBand].RFgain += 1;  // increase gain by one step, 1.5dB
      timer = 0;                       // reset the timer to prevent this from executing too often
      if (bands[currentBand].RFgain > 15) {
        bands[currentBand].RFgain = 15;
      }
      AudioNoInterrupts();
      AudioInterrupts();
    }
  }
  half_clip = 0;     // clear "half clip" indicator that tells us that we should decrease gain
  quarter_clip = 0;  // clear indicator that, if not triggered, indicates that we can increase gain
}

// is added in Teensyduino 1.52 beta-4, so this can be deleted !?

/*****
  Purpose: To set the real time clock

  Parameter list:
    void

  Return value:
    void
*****/
void T4_rtc_set(unsigned long t) {
  //#if defined (T4)
#if 0
  // stop the RTC
  SNVS_HPCR &= ~(SNVS_HPCR_RTC_EN | SNVS_HPCR_HP_TS);
  while (SNVS_HPCR & SNVS_HPCR_RTC_EN); // wait
  // stop the SRTC
  SNVS_LPCR &= ~SNVS_LPCR_SRTC_ENV;
  while (SNVS_LPCR & SNVS_LPCR_SRTC_ENV); // wait
  // set the SRTC
  SNVS_LPSRTCLR = t << 15;
  SNVS_LPSRTCMR = t >> 17;
  // start the SRTC
  SNVS_LPCR |= SNVS_LPCR_SRTC_ENV;
  while (!(SNVS_LPCR & SNVS_LPCR_SRTC_ENV)); // wait
  // start the RTC and sync it to the SRTC
  SNVS_HPCR |= SNVS_HPCR_RTC_EN | SNVS_HPCR_HP_TS;
#endif
}


/*****
  Purpose: void initTempMon

  Parameter list:
    void
  Return value;
    void
*****/
void initTempMon(uint16_t freq, uint32_t lowAlarmTemp, uint32_t highAlarmTemp, uint32_t panicAlarmTemp) {

  uint32_t calibrationData;
  uint32_t roomCount;
  //first power on the temperature sensor - no register change
  TEMPMON_TEMPSENSE0 &= ~TMS0_POWER_DOWN_MASK;
  TEMPMON_TEMPSENSE1 = TMS1_MEASURE_FREQ(freq);

  calibrationData = HW_OCOTP_ANA1;
  s_hotTemp = (uint32_t)(calibrationData & 0xFFU) >> 0x00U;
  s_hotCount = (uint32_t)(calibrationData & 0xFFF00U) >> 0X08U;
  roomCount = (uint32_t)(calibrationData & 0xFFF00000U) >> 0x14U;
  s_hotT_ROOM = s_hotTemp - TEMPMON_ROOMTEMP;
  s_roomC_hotC = roomCount - s_hotCount;
}

/*****
  Purpose: Read the Teensy's temperature. Get worried over 50C

  Parameter list:
    void

  Return value:
    float           temperature Centigrade
*****/
float TGetTemp() {
  uint32_t nmeas;
  float tmeas;
  while (!(TEMPMON_TEMPSENSE0 & 0x4U)) {
    ;
  }
  /* ready to read temperature code value */
  nmeas = (TEMPMON_TEMPSENSE0 & 0xFFF00U) >> 8U;
  tmeas = s_hotTemp - (float)((nmeas - s_hotCount) * s_hotT_ROOM / s_roomC_hotC);  // Calculate temperature
  return tmeas;
}

/*****
  Purpose: scale volume from 0 to 100

  Parameter list:
    int volume        the current reading

  Return value;
    void
*****/
float VolumeToAmplification(int volume) {
  float x = volume / 100.0f;  //"volume" Range 0..100
                              //#if 0
                              //  float a = 3.1623e-4;
                              //  float b = 8.059f;
                              //  float ampl = a * expf( b * x );
                              //  if (x < 0.1f) ampl *= x * 10.0f;
                              //#else
  //Approximation:
  float ampl = 5 * x * x * x * x * x;  //70dB
                                       //#endif
  return ampl;
}


// Teensy 4.0, 4.1
/*****
  Purpose: To set the I2S frequency

  Parameter list:
    int freq        the frequency to set

  Return value:
    int             the frequency or 0 if too large

*****/
int SetI2SFreq(int freq) {
  int n1;
  int n2;
  int c0;
  int c2;
  int c1;
  double C;

  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  // Fudge to handle 8kHz - El Supremo
  if (freq > 8000) {
    n1 = 4;  //SAI prescaler 4 => (n1*n2) = multiple of 4
  } else {
    n1 = 8;
  }
  n2 = 1 + (24000000 * 27) / (freq * 256 * n1);
  if (n2 > 63) {
    // n2 must fit into a 6-bit field
#ifdef DEBUG
    Serial.printf("ERROR: n2 exceeds 63 - %d\n", n2);
#endif
    return 0;
  }
  C = ((double)freq * 256 * n1 * n2) / 24000000;
  c0 = C;
  c2 = 10000;
  c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
               | CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1)   // &0x07
               | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1);  // &0x3f

  CCM_CS2CDR = (CCM_CS2CDR & ~(CCM_CS2CDR_SAI2_CLK_PRED_MASK | CCM_CS2CDR_SAI2_CLK_PODF_MASK))
               | CCM_CS2CDR_SAI2_CLK_PRED(n1 - 1)   // &0x07
               | CCM_CS2CDR_SAI2_CLK_PODF(n2 - 1);  // &0x3f)
  return freq;
}


/*****
  Purpose: to cause a delay in program execution

  Parameter list:
    unsigned long millisWait    // the number of millseconds to wait

  Return value:
    void
*****/
void MyDelay(unsigned long millisWait) {
  unsigned long now = millis();

  while (millis() - now < millisWait)
    ;  // Twiddle thumbs until delay ends...
}
/*****
  Purpose: to collect array inits in one place

  Parameter list:
    void

  Return value:
    void
*****/
void InitializeDataArrays() {
  //DB2OO, 11-SEP-23: don't use the fixed sizes, but use the caculated ones, otherwise a code change will create very difficult to find problems
#define CLEAR_VAR(x) memset(x, 0, sizeof(x))
  memset(FFT_spec_old, 0, sizeof(FFT_spec_old));
#ifdef DEBUG
  Serial.printf("InitializeDataArrays(): sizeof(FFT_spec_old) %d", sizeof(FFT_spec_old));
  Serial.printf("\tsizeof(NR_output_audio_buffer) %d", sizeof(NR_output_audio_buffer));
  Serial.printf("\tsizeof(LMS_StateF32) %d", sizeof(LMS_StateF32));
  Serial.println();
#endif
  CLEAR_VAR(FFT_spec_old);             //memset(FFT_spec_old, 0, 4096);            // SPECTRUM_RES = 512 * 4 = 2048
  CLEAR_VAR(pixelnew);                 //memset(pixelnew, 0, 1024);                // 512 * 2
  CLEAR_VAR(pixelold);                 //memset(pixelold, 0, 1024);                // 512 * 2
  CLEAR_VAR(pixelCurrent);             //memset(pixelCurrent, 0, 1024);            // 512 * 2  KF5N JJP  7/14/23
  CLEAR_VAR(buffer_spec_FFT);          //memset(buffer_spec_FFT, 0, 4096);         // SPECTRUM_RES = 512 * 2 = 1024
  CLEAR_VAR(FFT_spec);                 //memset(FFT_spec, 0, 4096);                // 512 * 2 * 4
  CLEAR_VAR(NR_FFT_buffer);            //memset(NR_FFT_buffer, 0, 2048);           // NR_FFT_L * sizeof(NR_FFT_buffer[0]));
  CLEAR_VAR(NR_output_audio_buffer);   //memset(NR_output_audio_buffer, 0, 1024);  // 256 * sizeof(NR_output_audio_buffer[0]));
  CLEAR_VAR(NR_last_iFFT_result);      //memset(NR_last_iFFT_result, 0, 512);
  CLEAR_VAR(NR_last_sample_buffer_L);  //memset(NR_last_sample_buffer_L, 0, 512);
  CLEAR_VAR(NR_last_sample_buffer_R);  //memset(NR_last_sample_buffer_R, 0, 512);
  CLEAR_VAR(NR_M);                     //memset(NR_M, 0, 512);
  CLEAR_VAR(NR_lambda);                //memset(NR_lambda, 0, 512);
  CLEAR_VAR(NR_G);                     //memset(NR_G, 0, 512);
  CLEAR_VAR(NR_SNR_prio);              //memset(NR_SNR_prio, 0, 512);
  CLEAR_VAR(NR_SNR_post);              //memset(NR_SNR_post, 0, 512);
  CLEAR_VAR(NR_Hk_old);                //memset(NR_Hk_old, 0, 512);
  CLEAR_VAR(NR_X);                     //memset(NR_X, 0, 1536);
  CLEAR_VAR(NR_Nest);                  //memset(NR_Nest, 0, 1024);
  CLEAR_VAR(NR_Gts);                   //memset(NR_Gts, 0, 1024);
  CLEAR_VAR(NR_E);                     //memset(NR_E, 0, 7680);
  CLEAR_VAR(ANR_d);                    //memset(ANR_d, 0, 2048);
  CLEAR_VAR(ANR_w);                    //memset(ANR_w, 0, 2048);
  CLEAR_VAR(LMS_StateF32);             //memset(LMS_StateF32, 0, 1408);  // 96 + 256 * 4
  CLEAR_VAR(LMS_NormCoeff_f32);        //memset(LMS_NormCoeff_f32, 0, 1408);
  CLEAR_VAR(LMS_nr_delay);             //memset(LMS_nr_delay, 0, 2312);

  CalcCplxFIRCoeffs(FIR_Coef_I, FIR_Coef_Q, m_NumTaps, (float32_t)bands[currentBand].FLoCut, (float32_t)bands[currentBand].FHiCut, (float)SR[SampleRate].rate / DF);

  /****************************************************************************************
     init complex FFTs
  ****************************************************************************************/
  switch (FFT_length) {
    case 2048:
      S = &arm_cfft_sR_f32_len2048;
      iS = &arm_cfft_sR_f32_len2048;
      maskS = &arm_cfft_sR_f32_len2048;
      break;
    case 1024:
      S = &arm_cfft_sR_f32_len1024;
      iS = &arm_cfft_sR_f32_len1024;
      maskS = &arm_cfft_sR_f32_len1024;
      break;
    case 512:
      S = &arm_cfft_sR_f32_len512;
      iS = &arm_cfft_sR_f32_len512;
      maskS = &arm_cfft_sR_f32_len512;
      break;
  }

  spec_FFT = &arm_cfft_sR_f32_len512;  //Changed specification to 512 instance
  NR_FFT = &arm_cfft_sR_f32_len256;
  NR_iFFT = &arm_cfft_sR_f32_len256;

  /****************************************************************************************
     Calculate the FFT of the FIR filter coefficients once to produce the FIR filter mask
  ****************************************************************************************/
  InitFilterMask();

  /****************************************************************************************
     Set sample rate
  ****************************************************************************************/
  SetI2SFreq(SR[SampleRate].rate);
  // essential ?
  IFFreq = SR[SampleRate].rate / 4;

  biquad_lowpass1.numStages = N_stages_biquad_lowpass1;  // set number of stages
  biquad_lowpass1.pCoeffs = biquad_lowpass1_coeffs;      // set pointer to coefficients file

  for (unsigned i = 0; i < 4 * N_stages_biquad_lowpass1; i++) {
    biquad_lowpass1_state[i] = 0.0;  // set state variables to zero
  }
  biquad_lowpass1.pState = biquad_lowpass1_state;  // set pointer to the state variables

  /****************************************************************************************
     set filter bandwidth of IIR filter
  ****************************************************************************************/
  // also adjust IIR AM filter
  // calculate IIR coeffs
  LP_F_help = bands[currentBand].FHiCut;
  if (LP_F_help < -bands[currentBand].FLoCut)
    LP_F_help = -bands[currentBand].FLoCut;
  SetIIRCoeffs((float32_t)LP_F_help, 1.3, (float32_t)SR[SampleRate].rate / DF, 0);  // 1st stage
  for (int i = 0; i < 5; i++) {                                                     // fill coefficients into the right file
    biquad_lowpass1_coeffs[i] = coefficient_set[i];
  }

  ShowBandwidth();

  /****************************************************************************************
     Initiate decimation and interpolation FIR filters
  ****************************************************************************************/
  // Decimation filter 1, M1 = DF1
  //    CalcFIRCoeffs(FIR_dec1_coeffs, 25, (float32_t)5100.0, 80, 0, 0.0, (float32_t)SR[SampleRate].rate);
  CalcFIRCoeffs(FIR_dec1_coeffs, n_dec1_taps, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);

  if (arm_fir_decimate_init_f32(&FIR_dec1_I, n_dec1_taps, (uint32_t)DF1, FIR_dec1_coeffs, FIR_dec1_I_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1)
      ;
  }

  if (arm_fir_decimate_init_f32(&FIR_dec1_Q, n_dec1_taps, (uint32_t)DF1, FIR_dec1_coeffs, FIR_dec1_Q_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1)
      ;
  }

  // Decimation filter 2, M2 = DF2
  CalcFIRCoeffs(FIR_dec2_coeffs, n_dec2_taps, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)(SR[SampleRate].rate / DF1));
  if (arm_fir_decimate_init_f32(&FIR_dec2_I, n_dec2_taps, (uint32_t)DF2, FIR_dec2_coeffs, FIR_dec2_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1)
      ;
  }

  if (arm_fir_decimate_init_f32(&FIR_dec2_Q, n_dec2_taps, (uint32_t)DF2, FIR_dec2_coeffs, FIR_dec2_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1)
      ;
  }

  // Interpolation filter 1, L1 = 2
  // not sure whether I should design with the final sample rate ??
  // yes, because the interpolation filter is AFTER the upsampling, so it has to be in the target sample rate!
  //    CalcFIRCoeffs(FIR_int1_coeffs, 8, (float32_t)5000.0, 80, 0, 0.0, 12000);
  //    CalcFIRCoeffs(FIR_int1_coeffs, 16, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, SR[SampleRate].rate / 4.0);
  CalcFIRCoeffs(FIR_int1_coeffs, 48, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, SR[SampleRate].rate / 4.0);
  //    if(arm_fir_interpolate_init_f32(&FIR_int1_I, (uint32_t)DF2, 16, FIR_int1_coeffs, FIR_int1_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
  if (arm_fir_interpolate_init_f32(&FIR_int1_I, (uint8_t)DF2, 48, FIR_int1_coeffs, FIR_int1_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
    while (1)
      ;
  }
  //    if(arm_fir_interpolate_init_f32(&FIR_int1_Q, (uint32_t)DF2, 16, FIR_int1_coeffs, FIR_int1_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
  if (arm_fir_interpolate_init_f32(&FIR_int1_Q, (uint8_t)DF2, 48, FIR_int1_coeffs, FIR_int1_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF)) {
    while (1)
      ;
  }
  // Interpolation filter 2, L2 = 4
  // not sure whether I should design with the final sample rate ??
  // yes, because the interpolation filter is AFTER the upsampling, so it has to be in the target sample rate!
  //    CalcFIRCoeffs(FIR_int2_coeffs, 4, (float32_t)5000.0, 80, 0, 0.0, 24000);
  //    CalcFIRCoeffs(FIR_int2_coeffs, 16, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);
  CalcFIRCoeffs(FIR_int2_coeffs, 32, (float32_t)(n_desired_BW * 1000.0), n_att, 0, 0.0, (float32_t)SR[SampleRate].rate);

  if (arm_fir_interpolate_init_f32(&FIR_int2_I, (uint8_t)DF1, 32, FIR_int2_coeffs, FIR_int2_I_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1)
      ;
  }
  //    if(arm_fir_interpolate_init_f32(&FIR_int2_Q, (uint32_t)DF1, 16, FIR_int2_coeffs, FIR_int2_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
  if (arm_fir_interpolate_init_f32(&FIR_int2_Q, (uint8_t)DF1, 32, FIR_int2_coeffs, FIR_int2_Q_state, BUFFER_SIZE * N_BLOCKS / (uint32_t)DF1)) {
    while (1)
      ;
  }

  SetDecIntFilters();  // here, the correct bandwidths are calculated and set accordingly

  /****************************************************************************************
     Zoom FFT: Initiate decimation and interpolation FIR filters AND IIR filters
  ****************************************************************************************/
  float32_t Fstop_Zoom = 0.5 * (float32_t)SR[SampleRate].rate / (1 << spectrum_zoom);

  CalcFIRCoeffs(Fir_Zoom_FFT_Decimate_coeffs, 4, Fstop_Zoom, 60, 0, 0.0, (float32_t)SR[SampleRate].rate);

  // Attention: max decimation rate is 128 !
  //  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_I, 4, 1 << spectrum_zoom, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_I_state, BUFFER_SIZE * N_BLOCKS)) {
  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_I, 4, 128, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_I_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1)
      ;
  }
  // same coefficients, but specific state variables
  //  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_Q, 4, 1 << spectrum_zoom, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_Q_state, BUFFER_SIZE * N_BLOCKS)) {
  if (arm_fir_decimate_init_f32(&Fir_Zoom_FFT_Decimate_Q, 4, 128, Fir_Zoom_FFT_Decimate_coeffs, Fir_Zoom_FFT_Decimate_Q_state, BUFFER_SIZE * N_BLOCKS)) {
    while (1)
      ;
  }

  IIR_biquad_Zoom_FFT_I.numStages = IIR_biquad_Zoom_FFT_N_stages;  // set number of stages
  IIR_biquad_Zoom_FFT_Q.numStages = IIR_biquad_Zoom_FFT_N_stages;  // set number of stages
  for (unsigned i = 0; i < 4 * IIR_biquad_Zoom_FFT_N_stages; i++) {
    IIR_biquad_Zoom_FFT_I_state[i] = 0.0;  // set state variables to zero
    IIR_biquad_Zoom_FFT_Q_state[i] = 0.0;  // set state variables to zero
  }
  IIR_biquad_Zoom_FFT_I.pState = IIR_biquad_Zoom_FFT_I_state;  // set pointer to the state variables
  IIR_biquad_Zoom_FFT_Q.pState = IIR_biquad_Zoom_FFT_Q_state;  // set pointer to the state variables

  // this sets the coefficients for the ZoomFFT decimation filter
  // according to the desired magnification mode
  // for 0 the mag_coeffs will a NULL  ptr, since the filter is not going to be used in this  mode!
  IIR_biquad_Zoom_FFT_I.pCoeffs = mag_coeffs[spectrum_zoom];
  IIR_biquad_Zoom_FFT_Q.pCoeffs = mag_coeffs[spectrum_zoom];

  ZoomFFTPrep();

  SpectralNoiseReductionInit();
  InitLMSNoiseReduction();

  temp_check_frequency = 0x03U;  //updates the temp value at a RTC/3 clock rate
  //0xFFFF determines a 2 second sample rate period
  highAlarmTemp = 85U;  //42 degrees C
  lowAlarmTemp = 25U;
  panicAlarmTemp = 90U;

  initTempMon(temp_check_frequency, lowAlarmTemp, highAlarmTemp, panicAlarmTemp);
  // this starts the measurements
  TEMPMON_TEMPSENSE0 |= 0x2U;
}
/*****
  Purpose: The initial screen display on startup. Expect this to be customized

  Parameter list:
    void

  Return value:
    void
*****/
void Splash() {
  int centerCall;
  tft.fillWindow(RA8875_BLACK);
  tft.setFontScale(3);
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(XPIXELS / 3 - 120, YPIXELS / 10);
  tft.print("T41-EP SDR Radio");
  tft.setTextColor(RA8875_MAGENTA);
  tft.setCursor(XPIXELS / 2 - 60, YPIXELS / 10 + 55);
  tft.setFontScale(2);
  tft.print(VERSION);
  tft.setFontScale(1);
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(XPIXELS / 2 - (2 * tft.getFontWidth() / 2), YPIXELS / 3);
  tft.print("By");
  tft.setFontScale(1);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor((XPIXELS / 2) - (38 * tft.getFontWidth() / 2) + 15, YPIXELS / 4 + 80);  // 38 = letters in string
  tft.print("Al Peter, AC8GY     Jack Purdum, W8TEE");
  tft.setCursor((XPIXELS / 2) - (12 * tft.getFontWidth()) / 2, YPIXELS / 2 + 110);  // 12 = letters in "Property of:"
  tft.print("Property of:");
  tft.setFontScale(2);
  tft.setTextColor(RA8875_GREEN);
  centerCall = (XPIXELS - strlen(MY_CALL) * tft.getFontWidth()) / 2;
  tft.setCursor(centerCall, YPIXELS / 2 + 160);
  tft.print(MY_CALL);
  MyDelay(SPLASH_DELAY);
  tft.fillWindow(RA8875_BLACK);
}

//===============================================================================================================================
//==========================  Setup ================================
/*****
  Purpose: program entry point that sets the environment for program

  Parameter list:
    void

  Return value:
    void
*****/
void setup() {
  Serial.begin(9600);
  setSyncProvider(getTeensy3Time);  // get TIME from real time clock with 3V backup battery
  setTime(now());
  Teensy3Clock.set(now());  // set the RTC
  T4_rtc_set(Teensy3Clock.get());

  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  AudioMemory(500);  //  Increased to 450 from 400.  Memory was hitting max.  KF5N August 31, 2023
  AudioMemory_F32(10);
  sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000_1.micGain(20);
  sgtl5000_1.lineInLevel(0);
  sgtl5000_1.lineOutLevel(20);
  sgtl5000_1.adcHighPassFilterDisable();  //reduces noise.  https://forum.pjrc.com/threads/27215-24-bit-audio-boards?p=78831&viewfull=1#post78831
  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_2.volume(0.5);

  pinMode(FILTERPIN15M, OUTPUT);
  pinMode(FILTERPIN20M, OUTPUT);
  pinMode(FILTERPIN40M, OUTPUT);
  pinMode(FILTERPIN80M, OUTPUT);
  pinMode(RXTX, OUTPUT);
  pinMode(MUTE, OUTPUT);
  digitalWrite(MUTE, LOW);
  pinMode(PTT, INPUT_PULLUP);
  pinMode(BUSY_ANALOG_PIN, INPUT);
  pinMode(FILTER_ENCODER_A, INPUT);
  pinMode(FILTER_ENCODER_B, INPUT);
  pinMode(OPTO_OUTPUT, OUTPUT);
  pinMode(KEYER_DIT_INPUT_TIP, INPUT_PULLUP);
  pinMode(KEYER_DAH_INPUT_RING, INPUT_PULLUP);
  pinMode(TFT_MOSI, OUTPUT);
  digitalWrite(TFT_MOSI, HIGH);
  pinMode(TFT_SCLK, OUTPUT);
  digitalWrite(TFT_SCLK, HIGH);
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);

  arm_fir_init_f32(&FIR_Hilbert_L, 100, FIR_Hilbert_coeffs_45, FIR_Hilbert_state_L, 256);  //AFP01-16-22
  arm_fir_init_f32(&FIR_Hilbert_R, 100, FIR_Hilbert_coeffs_neg45, FIR_Hilbert_state_R, 256);
  arm_fir_init_f32(&FIR_CW_DecodeL, 64, CW_Filter_Coeffs2, FIR_CW_DecodeL_state, 256);  //AFP 10-25-22
  arm_fir_init_f32(&FIR_CW_DecodeR, 64, CW_Filter_Coeffs2, FIR_CW_DecodeR_state, 256);
  arm_fir_decimate_init_f32(&FIR_dec1_EX_I, 48, 4, coeffs192K_10K_LPF_FIR, FIR_dec1_EX_I_state, 2048);
  arm_fir_decimate_init_f32(&FIR_dec1_EX_Q, 48, 4, coeffs192K_10K_LPF_FIR, FIR_dec1_EX_Q_state, 2048);
  arm_fir_decimate_init_f32(&FIR_dec2_EX_I, 24, 2, coeffs48K_8K_LPF_FIR, FIR_dec2_EX_I_state, 512);
  arm_fir_decimate_init_f32(&FIR_dec2_EX_Q, 24, 2, coeffs48K_8K_LPF_FIR, FIR_dec2_EX_Q_state, 512);
  arm_fir_interpolate_init_f32(&FIR_int1_EX_I, 2, 48, coeffs48K_8K_LPF_FIR, FIR_int1_EX_I_state, 256);
  arm_fir_interpolate_init_f32(&FIR_int1_EX_Q, 2, 48, coeffs48K_8K_LPF_FIR, FIR_int1_EX_Q_state, 256);
  arm_fir_interpolate_init_f32(&FIR_int2_EX_I, 4, 32, coeffs192K_10K_LPF_FIR, FIR_int2_EX_I_state, 512);
  arm_fir_interpolate_init_f32(&FIR_int2_EX_Q, 4, 32, coeffs192K_10K_LPF_FIR, FIR_int2_EX_Q_state, 512);

  //***********************  EQ Gain Settings ************
  uint32_t iospeed_display = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(1);
  *(digital_pin_to_info_PGM + 13)->pad = iospeed_display;  //clk
  *(digital_pin_to_info_PGM + 11)->pad = iospeed_display;  //MOSI
  *(digital_pin_to_info_PGM + TFT_CS)->pad = iospeed_display;

  tuneEncoder.begin(true);
  volumeEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(VOLUME_ENCODER_A), EncoderVolume, CHANGE);
  attachInterrupt(digitalPinToInterrupt(VOLUME_ENCODER_B), EncoderVolume, CHANGE);
  filterEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(FILTER_ENCODER_A), EncoderFilter, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FILTER_ENCODER_B), EncoderFilter, CHANGE);
  fineTuneEncoder.begin(true);
  attachInterrupt(digitalPinToInterrupt(FINETUNE_ENCODER_A), EncoderFineTune, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FINETUNE_ENCODER_B), EncoderFineTune, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEYER_DIT_INPUT_TIP), KeyTipOn, CHANGE);  // Changed to keyTipOn from KeyOn everywhere JJP 8/31/22
  attachInterrupt(digitalPinToInterrupt(KEYER_DAH_INPUT_RING), KeyRingOn, CHANGE);

  tft.begin(RA8875_800x480, 8, 20000000UL, 4000000UL);  // parameter list from library code
  tft.setRotation(0);

  // Setup for scrolling attributes. Part of initSpectrum_RA8875() call written by Mike Lewis
  tft.useLayers(true);  //mainly used to turn on layers! //AFP 03-27-22 Layers
  tft.layerEffect(OR);
  tft.clearMemory();
  tft.writeTo(L2);
  tft.clearMemory();
  tft.writeTo(L1);

  Splash();

  sdCardPresent = InitializeSDCard();  // Is there an SD card that can be initialized?

  // =============== Into EEPROM section =================
  EEPROMStartup();

  syncEEPROM = 1;  // We've read current EEPROM values
#ifdef DEBUG
  EEPROMShow();
#endif

  // Push and hold a button at power up to activate switch matrix calibration.
  // Uncomment this code block to enable this feature.  Len KD0RC
  /* Remove this line and the matching block comment line below to activate.
  minPinRead = analogRead(BUSY_ANALOG_PIN);
  if (minPinRead < NOTHING_TO_SEE_HERE) {
    tft.fillWindow(RA8875_BLACK);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_GREEN);
    tft.setCursor(10, 10);
    tft.print("Release button to start calibration.");
    MyDelay(2000);
    SaveAnalogSwitchValues();
    EEPROMRead();  // Call to reset switch matrix values
  }                // KD0RC end
  Remove this line and the matching block comment line above to activate. */

  spectrum_x = 10;
  spectrum_y = 150;
  xExpand = 1.4;
  h = 135;
  nrOptionSelect = 0;

  Q_in_L.begin();  //Initialize receive input buffers
  Q_in_R.begin();
  MyDelay(100L);

  freqIncrement = incrementValues[tuneIndex];
  NR_Index = nrOptionSelect;
  NCOFreq = 0L;
  activeVFO = EEPROMData.activeVFO;        // 2 bytes
  audioVolume = EEPROMData.audioVolume;    // 4 bytes
  currentBand = EEPROMData.currentBand;    // 4 bytes
  currentBandA = EEPROMData.currentBandA;  // 4 bytes
  currentBandB = EEPROMData.currentBandB;

  // ========================  End set up of Parameters from EEPROM data ===============
  NCOFreq = 0;

  /****************************************************************************************
     start local oscillator Si5351
  ****************************************************************************************/
  si5351.reset();                                                                // KF5N.  Moved Si5351 start-up to setup. JJP  7/14/23
  si5351.init(SI5351_CRYSTAL_LOAD_10PF, Si_5351_crystal, freqCorrectionFactor);  //JJP  7/14/23
  si5351.set_ms_source(SI5351_CLK2, SI5351_PLLB);                                //  Allows CLK1 and CLK2 to exceed 100 MHz simultaneously.
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);                          //AFP 10-13-22
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);                          //CWP AFP 10-13-22

  if (xmtMode == CW_MODE && decoderFlag == DECODE_OFF) {
    decoderFlag = DECODE_OFF;  // JJP 7/1/23
  } else {
    decoderFlag = DECODE_ON;  // Turns decoder on JJP 7/1/23
  }

  TxRxFreq = centerFreq + NCOFreq;

  InitializeDataArrays();
  splitOn = 0;  // Split VFO not active
  SetupMode(bands[currentBand].mode);

  //ditLength = STARTING_DITLENGTH;  // 80 = 1200 / 15 wpm
  //averageDit = ditLength;
  //averageDah = ditLength * 3L;

  float32_t theta = 0.0;              //AFP 10-25-22
  for (int kf = 0; kf < 255; kf++) {  //Calc sine wave
    theta = kf * 0.19634950849362;    // Simplify terms: theta = kf * 2 * PI * freqSideTone / 24000  JJP 6/28/23
    sinBuffer[kf] = sin(theta);
  }
  //currentWPM = EEPROMData.currentWPM;  // Not required.  Retrieved by EEPROMRead().  KF5N August 27, 2023
  SetKeyPowerUp();  // Use keyType and paddleFlip to configure key GPIs.  KF5N August 27, 2023
  SetDitLength(currentWPM);
  SetTransmitDitLength(currentWPM);
  CWFreqShift = 750;
  calFreqShift = 0;
  //AFP 10-25-22
  sineTone(BUFFER_SINE_COUNT);  // Set to 8
  filterEncoderMove = 0;
  fineTuneEncoderMove = 0L;
  xrState = RECEIVE_STATE;  // Enter loop() in receive state.  KF5N July 22, 2023
  UpdateInfoWindow();
  DrawSpectrumDisplayContainer();
  RedrawDisplayScreen();

  mainMenuIndex = 0;             // Changed from middle to first. Do Menu Down to get to Calibrate quickly
  menuStatus = NO_MENUS_ACTIVE;  // Blank menu field
  ShowName();

  ShowBandwidth();
  FilterBandwidth();
  ShowFrequency();
  SetFreq();
  zoomIndex = spectrum_zoom - 1;  // ButtonZoom() increments zoomIndex, so this cancels it so the read from EEPROM is accurately restored.  KF5N August 3, 2023
  ButtonZoom();                   // Restore zoom settings.  KF5N August 3, 2023
  knee_dBFS = -15.0;
  comp_ratio = 5.0;
  attack_sec = .1;
  release_sec = 2.0;
  comp1.setPreGain_dB(-10);  //set the gain of the Left-channel gain processor
  comp2.setPreGain_dB(-10);  //set the gain of the Right-channel gain processor

  sdCardPresent = SDPresentCheck();  // JJP 7/18/23
  lastState = 1111;                  // To make sure the receiver will be configured on the first pass through.  KF5N September 3, 2023
}
//============================================================== END setup() =================================================================
//===============================================================================================================================

elapsedMicros usec = 0;  // Automatically increases as time passes; no ++ necessary.

/*****
  Purpose: Code here executes forever, or until: 1) power is removed, 2) user does a reset, 3) a component
           fails, or 4) the cows come home.

  Parameter list:
    void

  Return value:
    void
*****/
FASTRUN void loop()  // Replaced entire loop() with Greg's code  JJP  7/14/23
{
  int pushButtonSwitchIndex = -1;
  int valPin;
  long ditTimerOff;  //AFP 09-22-22
  long dahTimerOn;

  valPin = ReadSelectedPushButton();                     // Poll UI push buttons
  if (valPin != BOGUS_PIN_READ) {                        // If a button was pushed...
    pushButtonSwitchIndex = ProcessButtonPress(valPin);  // Winner, winner...chicken dinner!
    ExecuteButtonPress(pushButtonSwitchIndex);
  }
  //  State detection
  if (xmtMode == SSB_MODE && digitalRead(PTT) == HIGH) radioState = SSB_RECEIVE_STATE;
  if (xmtMode == SSB_MODE && digitalRead(PTT) == LOW) radioState = SSB_TRANSMIT_STATE;
  if (xmtMode == CW_MODE && (digitalRead(paddleDit) == HIGH && digitalRead(paddleDah) == HIGH)) radioState = CW_RECEIVE_STATE;  // Was using symbolic constants. Also changed in code below.  KF5N August 8, 2023
  if (xmtMode == CW_MODE && (digitalRead(paddleDit) == LOW && xmtMode == CW_MODE && keyType == 0)) radioState = CW_TRANSMIT_STRAIGHT_STATE;
  if (xmtMode == CW_MODE && (keyPressedOn == 1 && xmtMode == CW_MODE && keyType == 1)) radioState = CW_TRANSMIT_KEYER_STATE;
  if (lastState != radioState) {
    SetFreq();  // Update frequencies if the radio state has changed.
  }
  //lastState = radioState;  // G0ORX 01092023

  //  Begin radio state machines

  //  Begin SSB Mode state machine

  switch (radioState) {
    case (SSB_RECEIVE_STATE):
      if (lastState != radioState) {  // G0ORX 01092023
        digitalWrite(MUTE, LOW);      // Audio Mute off
        modeSelectInR.gain(0, 1);
        modeSelectInL.gain(0, 1);
        digitalWrite(RXTX, LOW);  //xmit off
        T41State = SSB_RECEIVE;
        xrState = RECEIVE_STATE;
        modeSelectInR.gain(0, 1);
        modeSelectInL.gain(0, 1);
        modeSelectInExR.gain(0, 0);
        modeSelectInExL.gain(0, 0);
        modeSelectOutL.gain(0, 1);
        modeSelectOutR.gain(0, 1);
        modeSelectOutL.gain(1, 0);
        modeSelectOutR.gain(1, 0);
        modeSelectOutExL.gain(0, 0);
        modeSelectOutExR.gain(0, 0);
        phaseLO = 0.0;
        barGraphUpdate = 0;
        if (keyPressedOn == 1) {
          return;
        }
        ShowTransmitReceiveStatus();
      }
      ShowSpectrum();
      break;
    case SSB_TRANSMIT_STATE:
      Q_in_L.end();  //Set up input Queues for transmit
      Q_in_R.end();
      Q_in_L_Ex.begin();
      Q_in_R_Ex.begin();
      comp1.setPreGain_dB(currentMicGain);
      comp2.setPreGain_dB(currentMicGain);
      if (compressorFlag == 1) {
        SetupMyCompressors(use_HP_filter, currentMicThreshold, comp_ratio, attack_sec, release_sec);
      } else {
        if (compressorFlag == 0) {
          SetupMyCompressors(use_HP_filter, 0.0, comp_ratio, 0.01, 0.01);
        }
      }
      xrState = TRANSMIT_STATE;
      // centerTuneFlag = 1;  Not required with revised tuning scheme.  KF5N July 22, 2023
      digitalWrite(MUTE, HIGH);  //  Mute Audio  (HIGH=Mute)
      digitalWrite(RXTX, HIGH);  //xmit on
      xrState = TRANSMIT_STATE;
      modeSelectInR.gain(0, 0);
      modeSelectInL.gain(0, 0);
      modeSelectInExR.gain(0, 1);
      modeSelectInExL.gain(0, 1);
      modeSelectOutL.gain(0, 0);
      modeSelectOutR.gain(0, 0);
      modeSelectOutExL.gain(0, powerOutSSB[currentBand]);  //AFP 10-21-22
      modeSelectOutExR.gain(0, powerOutSSB[currentBand]);  //AFP 10-21-22
      ShowTransmitReceiveStatus();

      while (digitalRead(PTT) == LOW) {
        ExciterIQData();
      }
      Q_in_L_Ex.end();  // End Transmit Queue
      Q_in_R_Ex.end();
      Q_in_L.begin();  // Start Receive Queue
      Q_in_R.begin();
      xrState = RECEIVE_STATE;
      break;
    default:
      break;
  }
  //======================  End SSB Mode =================

  // Begin CW Mode state machine

  switch (radioState) {
    case CW_RECEIVE_STATE:
      if (lastState != radioState) {  // G0ORX 01092023
        digitalWrite(MUTE, LOW);      //turn off mute
        T41State = CW_RECEIVE;
        ShowTransmitReceiveStatus();
        xrState = RECEIVE_STATE;
        //SetFreq();   // KF5N
        modeSelectInR.gain(0, 1);
        modeSelectInL.gain(0, 1);
        modeSelectInExR.gain(0, 0);
        modeSelectInExL.gain(0, 0);
        modeSelectOutL.gain(0, 1);
        modeSelectOutR.gain(0, 1);
        modeSelectOutL.gain(1, 0);
        modeSelectOutR.gain(1, 0);
        modeSelectOutExL.gain(0, 0);
        modeSelectOutExR.gain(0, 0);
        phaseLO = 0.0;
        barGraphUpdate = 0;
        keyPressedOn = 0;
      }
      ShowSpectrum();  // if removed CW signal on is 2 mS
      break;
    case CW_TRANSMIT_STRAIGHT_STATE:
      powerOutCW[currentBand] = (-.0133 * transmitPowerLevel * transmitPowerLevel + .7884 * transmitPowerLevel + 4.5146) * CWPowerCalibrationFactor[currentBand];
      CW_ExciterIQData();
      xrState = TRANSMIT_STATE;
      ShowTransmitReceiveStatus();
      digitalWrite(MUTE, HIGH);  //   Mute Audio  (HIGH=Mute)
      modeSelectInR.gain(0, 0);
      modeSelectInL.gain(0, 0);
      modeSelectInExR.gain(0, 0);
      modeSelectOutL.gain(0, 0);
      modeSelectOutR.gain(0, 0);
      modeSelectOutExL.gain(0, 0);
      modeSelectOutExR.gain(0, 0);
      cwTimer = millis();
      while (millis() - cwTimer <= cwTransmitDelay) {  //Start CW transmit timer on
        digitalWrite(RXTX, HIGH);
        if (digitalRead(paddleDit) == LOW && keyType == 0) {       // AFP 09-25-22  Turn on CW signal
          cwTimer = millis();                                      //Reset timer
          modeSelectOutExL.gain(0, powerOutCW[currentBand]);       //AFP 10-21-22
          modeSelectOutExR.gain(0, powerOutCW[currentBand]);       //AFP 10-21-22
          digitalWrite(MUTE, LOW);                                 // unmutes audio
          modeSelectOutL.gain(1, volumeLog[(int)sidetoneVolume]);  // Sidetone  AFP 10-01-22
          //  modeSelectOutR.gain(1, volumeLog[(int)sidetoneVolume]);           // Right side not used.  KF5N September 1, 2023
        } else {
          if (digitalRead(paddleDit) == HIGH && keyType == 0) {  //Turn off CW signal
            keyPressedOn = 0;
            digitalWrite(MUTE, HIGH);     // mutes audio
            modeSelectOutExL.gain(0, 0);  //Power = 0
            modeSelectOutExR.gain(0, 0);
            modeSelectOutL.gain(1, 0);  // Sidetone off
            modeSelectOutR.gain(1, 0);
          }
        }
        CW_ExciterIQData();
      }
      modeSelectOutExL.gain(0, 0);  //Power = 0 //AFP 10-11-22
      modeSelectOutExR.gain(0, 0);  //AFP 10-11-22
      digitalWrite(RXTX, LOW);      // End Straight Key Mode
      break;
    case CW_TRANSMIT_KEYER_STATE:
      CW_ExciterIQData();
      xrState = TRANSMIT_STATE;
      ShowTransmitReceiveStatus();
      digitalWrite(MUTE, HIGH);  //   Mute Audio  (HIGH=Mute)
      modeSelectInR.gain(0, 0);
      modeSelectInL.gain(0, 0);
      modeSelectInExR.gain(0, 0);
      modeSelectInExL.gain(0, 0);
      modeSelectOutL.gain(0, 0);
      modeSelectOutR.gain(0, 0);
      modeSelectOutExL.gain(0, 0);
      modeSelectOutExR.gain(0, 0);
      cwTimer = millis();
      while (millis() - cwTimer <= cwTransmitDelay) {
        digitalWrite(RXTX, HIGH);  //Turns on relay
        CW_ExciterIQData();
        modeSelectInR.gain(0, 0);
        modeSelectInL.gain(0, 0);
        modeSelectInExR.gain(0, 0);
        modeSelectInExL.gain(0, 0);
        modeSelectOutL.gain(0, 0);
        modeSelectOutR.gain(0, 0);

        if (digitalRead(paddleDit) == LOW) {  // Keyer Dit
          cwTimer = millis();
          ditTimerOn = millis();
          //          while (millis() - ditTimerOn <= ditLength) {
          while (millis() - ditTimerOn <= transmitDitLength) {       // JJP 8/19/23
            modeSelectOutExL.gain(0, powerOutCW[currentBand]);       //AFP 10-21-22
            modeSelectOutExR.gain(0, powerOutCW[currentBand]);       //AFP 10-21-22
            digitalWrite(MUTE, LOW);                                 // unmutes audio
            modeSelectOutL.gain(1, volumeLog[(int)sidetoneVolume]);  // Sidetone
                                                                     //  modeSelectOutR.gain(1, volumeLog[(int)sidetoneVolume]);           // Right side not used.  KF5N September 1, 2023
            CW_ExciterIQData();                                      // Creates CW output signal
            keyPressedOn = 0;
          }
          ditTimerOff = millis();
          //          while (millis() - ditTimerOff <= ditLength - 10) {  //Time between
          while (millis() - ditTimerOff <= transmitDitLength - 10L) {  // JJP 8/19/23
            modeSelectOutExL.gain(0, 0);                               //Power =0
            modeSelectOutExR.gain(0, 0);
            modeSelectOutL.gain(1, 0);  // Sidetone off
            modeSelectOutR.gain(1, 0);
            CW_ExciterIQData();
            keyPressedOn = 0;
          }
        } else {
          if (digitalRead(paddleDah) == LOW) {  //Keyer DAH
            cwTimer = millis();
            dahTimerOn = millis();
            //            while (millis() - dahTimerOn <= 3UL * ditLength) {
            while (millis() - dahTimerOn <= 3UL * transmitDitLength) {  // JJP 8/19/23
              modeSelectOutExL.gain(0, powerOutCW[currentBand]);        //AFP 10-21-22
              modeSelectOutExR.gain(0, powerOutCW[currentBand]);        //AFP 10-21-22
              digitalWrite(MUTE, LOW);                                  // unmutes audio
              modeSelectOutL.gain(1, volumeLog[(int)sidetoneVolume]);   // Dah sidetone was using constants.  KD0RC
                                                                        //   modeSelectOutR.gain(1, volumeLog[(int)sidetoneVolume]);           // Right side not used.  KF5N September 1, 2023
              CW_ExciterIQData();                                       // Creates CW output signal
              keyPressedOn = 0;
            }
            ditTimerOff = millis();
            //            while (millis() - ditTimerOff <= ditLength - 10UL) {  //Time between characters                         // mutes audio
            while (millis() - ditTimerOff <= transmitDitLength - 10UL) {  // JJP 8/19/23
              modeSelectOutExL.gain(0, 0);                                //Power =0
              modeSelectOutExR.gain(0, 0);
              modeSelectOutL.gain(1, 0);  // Sidetone off
              modeSelectOutR.gain(1, 0);
              CW_ExciterIQData();
            }
          }
        }
        CW_ExciterIQData();
        keyPressedOn = 0;  // Fix for keyer click-clack.  KF5N August 16, 2023
      }                    //End Relay timer

      modeSelectOutExL.gain(0, 0);  //Power = 0 //AFP 10-11-22
      modeSelectOutExR.gain(0, 0);  //AFP 10-11-22
      digitalWrite(RXTX, LOW);
      xmtMode = CW_MODE;
      //   RedrawDisplayScreen();
      //   DrawFrequencyBarValue();
      break;
    default:
      break;
  }

  //  End radio state machine
  if (lastState != radioState) {  // G0ORX 09012023
    lastState = radioState;
    ShowTransmitReceiveStatus();
  }
  //  ShowTransmitReceiveStatus();

#ifdef DEBUG1
  if (elapsed_micros_idx_t > (SR[SampleRate].rate / 960)) {
    ShowTempAndLoad();
    // Used to monitor CPU temp and load factors
  }
#endif

  if (volumeChangeFlag == true) {
    volumeChangeFlag = false;
    UpdateVolumeField();
  }
  /* if (ms_500.check() == 1)                                  // For clock updates AFP 10-26-22
    {
     //wait_flag = 0;
     DisplayClock();
    }*/
}
