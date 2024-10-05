// Code for the T41 Easy radio ON and OFF Flip-Flop routines.  
//
//       Written by Dr. William J. Schmidt, II, Ph.D. - K9HZ
//
// I'm an excellent hardware engineer but a very shitty software engineer.
// Therefore, plagerize and modify away!  No warranty.  Just give me a little
// credit.
#define PANEL_SWITCH_PIN      3       // Front panel switch to turn radio ON/OFF
#define FET_SWITCH_PIN        4       // HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
#define OUTPUT_TO_TEENSY_PIN  1
#define INPUT_FROM_TEENSY_PIN 2
#define SWITCH_PUSHED         HIGH
#define SWITCH_NOT_PUSHED     LOW
#define FET_ON                HIGH
#define FET_OFF               LOW
#define ASKING_FOR_SHUTDOWN      HIGH            // to the Teensy!
#define NOT_ASKING_FOR_SHUTDOWN LOW
#define SHUTDOWN_APPROVED     HIGH
#define SHUTDOWN_NOT_APPROVED LOW

//
// Variables:
//
bool radio_power_on = false;           //when we boot up.
int state = 0;

//
// Note:  for some reason millis() is WAY slower than 1 mS per tick.
void wait_millis( uint32_t howlong )
  {
  uint32_t beginning = millis();
  uint32_t end = beginning + howlong;
  while( millis() < end );
  }

void setup()
  {
  // Set up the Input and Output pins as follows://
  // PIN 2 - PB3 - INPUT.   Push Button INPUT for turn radio ON and OFF.
  // PIN 3 - PB4 - OUTPUT.  HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
  pinMode( PANEL_SWITCH_PIN, INPUT  );
  pinMode( FET_SWITCH_PIN,   OUTPUT );
  digitalWrite( OUTPUT_TO_TEENSY_PIN, NOT_ASKING_FOR_SHUTDOWN );  //before we make it an output!
  pinMode( OUTPUT_TO_TEENSY_PIN, OUTPUT );
  pinMode( INPUT_FROM_TEENSY_PIN, INPUT );
  // Set initial state to "radio off" initially
  digitalWrite( FET_SWITCH_PIN, LOW );        //RADIO OFF - Shut off power FET
  radio_power_on = false;                   // No switchPush to start
  }

void loop() 
  {
  switch( state )
    {
    case 0:       //power off
    if( digitalRead( PANEL_SWITCH_PIN ) == SWITCH_PUSHED ) state = 1;
    break;

    case 1:        //power off, switch has been pushed
    radio_power_on = true;
    digitalWrite( FET_SWITCH_PIN, FET_ON );
    state = 2;
    break;

    case 2:   //initial power on.  We still need to hiccup
    wait_millis( 100 );               // trying two seconds
    state = 3;
    break;

    case 3:
    digitalWrite( FET_SWITCH_PIN, FET_OFF );          //hiccup!
    radio_power_on = false;
    state = 4;
    break;

    case 4:               // radio power is off for the initial hiccup
    wait_millis( 100 );      //keeping it off for two seconds
    state = 5;
    break;

    case 5:                // turn it back on to stay
    digitalWrite( FET_SWITCH_PIN, FET_ON );
    radio_power_on = true;
    wait_millis( 100 );                 //ignore the switch for a while
    state = 6;
    break;

    case 6:
    if( digitalRead( PANEL_SWITCH_PIN) == SWITCH_PUSHED )
      {
      digitalWrite( OUTPUT_TO_TEENSY_PIN, ASKING_FOR_SHUTDOWN );
      state = 7;
      }
    break;

    case 7: // We have asked the Teensy for a shutdown.  We'll wait here until it gives us permission
    while( digitalRead( INPUT_FROM_TEENSY_PIN ) != SHUTDOWN_APPROVED );
    digitalWrite( OUTPUT_TO_TEENSY_PIN, NOT_ASKING_FOR_SHUTDOWN );
    digitalWrite( FET_SWITCH_PIN, FET_OFF );
    radio_power_on = false;
    state = 8;
    break;

    case 8:
    // We just turned the radio off.  Let's wait a while before we accept input to turn it off again.
    wait_millis( 100 );
    state = 0;                 //back to the beginning
    break;
    }
  } // endLOOP

