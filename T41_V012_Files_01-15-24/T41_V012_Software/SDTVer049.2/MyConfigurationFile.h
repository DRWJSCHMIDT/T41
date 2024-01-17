//====================== User Specific Preferences =============

#define DEBUG 		                                                        // Uncommented for debugging, comment out for normal use
#define DECODER_STATE							0						                              // 0 = off, 1 = on
#define DEFAULT_KEYER_WPM   			15                                        // Startup value for keyer wpm
#define FREQ_SEP_CHARACTER  			'.'					                              // Some may prefer period, space, or combo
#define MAP_FILE_NAME   					"Cincinnati.bmp"                          // Name you gave to BMP map file. Max is 50 chars
#define MY_LAT										39.07466                                  // Coordinates for QTH
#define MY_LON										-84.42677
#define MY_CALL										"Your Call"                                   // Default max is 10 chars
#define MY_TIMEZONE          			"EST: "                                   // Default max is 10 chars
//DB2OO, 29-AUG-23: TIME_24H to display 24 hour times (Utility.cpp). If this is not defined 12h display will be used
#define TIME_24H                  1
//DB2OO, 29-AUG-23: ITU_REGION to determine band borders: Upper band limits on 80m (3.8MHz vs 4.0MhHz) and 40m (7.2MHz vs. 7.3MHz)
//#define ITU_REGION                1 //for Europe
#define ITU_REGION    2   // for USA
// DB2OO, 29.823:. Analog Signal on this pin will be used for an analog S-Meter (250uA full scale) connected via 10kOhm to this output. 1uF capacitor paralle to the S-Meter. --> Display.cpp. 
// This might conflict with other hardware modifications, that might use Pin 33 for a different purpose --> please check, before defining this
//#define HW_SMETER              33
// DB2OO, 30-AUG-23: with TCVSDR_SMETER (TCVSDR=Teensy Convolution SDR) defined the S-Meter bar will be consistent with the dBm value and it will not go over the box for S9+40+
#define TCVSDR_SMETER             1
//DB2OO, 10-SEP-23: TCXO_25MHZ defined sets the default EEPROMData.freqCorrectionFactor = 0, as the TCXO is supposed to deliver 25.00000MHz
//#define TCXO_25MHZ                1

#define PADDLE_FLIP								0						                              // 0 = right paddle = DAH, 1 = DIT
#define STRAIGHT_KEY_OR_PADDLES		0						                              // 0 = straight, 1 = paddles
#define SDCARD_MESSAGE_LENGTH     3000L                                     // The number of milliseconds to leave error message on screen
//#define SD_CARD_PRESENT         0                                         //   JJP  7/18/23

//====================== System specific ===============
#define CURRENT_FREQ_A            7200000                                   // VFO_A
#define CURRENT_FREQ_B            7030000                                   // VFO_B
#define DEFAULTFREQINCREMENT 		  4                                         // Default: (10, 50, 100, 250, 1000, 10000Hz)
#define DEFAULT_POWER_LEVEL       10                                        // Startup power level. Probably 20 for most people
#define FAST_TUNE_INCREMENT    	  1			 		                                // Default from above for fine tune
#define SPLASH_DELAY              4000L                                     // How long to show Splash screen. Use 1000 for testing, 4000 normally
#define STARTUP_BAND        			1                                         // This is the 40M band. see around line 575 in SDT.h

#define CENTER_SCREEN_X           400
#define CENTER_SCREEN_Y           245
#define IMAGE_CORNER_X            190                                       // ImageWidth = 378 Therefore 800 - 378 = 422 / 2 = 211
#define IMAGE_CORNER_Y            40                                        // ImageHeight = 302 Therefore 480 - 302 = 178 / 2 = 89
#define RAY_LENGTH                190
