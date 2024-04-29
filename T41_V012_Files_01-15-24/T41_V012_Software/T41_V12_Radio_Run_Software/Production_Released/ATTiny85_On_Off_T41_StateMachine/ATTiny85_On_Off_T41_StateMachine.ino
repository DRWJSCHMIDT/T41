/*
 Code for the T41 Easy radio ON and OFF routines.  This allows for a 
 special trigger from the ATTiny85 sent to the Teensy that causes it to
 run through special code in the teensy 4.1 before the radio shuts down.
 The special code might save parameters that have changed.

      State Machine
        (Start)
           |
           v
  --------------------
  |   OFF STATE      |
  | Power = Off      |<-----
  | Exit when button |     |
  | pressed          |     |
  --------------------     |
           |               |
           | Turn FET on   |
           v               |
  --------------------     |
  |    ON STATE      |     |
  | Power = On       |     | 
  | Exit when button |     |
  | pressed          |     |
  --------------------     |
           |               |
           | Tell Teensy   |
           | to shutdown   |
           v               |
  ---------------------    |
  | SHUTDOWN STATE    |    | Turn FET off
  | Power = On        |-----
  | Exit when Teensy  |
  | finishes shutdown |
  ---------------------
  
       Written by Dr. William J. Schmidt, II, Ph.D. - K9HZ
       Modified by Oliver KI3P
*/

#define SHUTDOWN_COMPLETE 2       // HIGH on this pin from Teensy tells ATTiny that shutdown routine is complete
#define START_SHUTDOWN    1       // HIGH on this pin from ATTiny tells Teensy to begin shut down code
#define PANEL_SWITCH      3       // Pin connected to the front panel switch to turn radio ON/OFF
#define FET_SWITCH        4       // Pin connected to the FET switch. HIGH=RADIO ON, LOW=RADIO OFF

#define OFF_STATE      1
#define ON_STATE       2
#define SHUTDOWN_STATE 3
uint8_t currentState;

void setup() {
  // Set up the Input and Output pins as follows://
  // PIN 7 - PB2 - INPUT.   Positive pulse from Teensy tells ATTiny to complete shut down.
  // PIN 6 - PB1 - OUTPUT.  Positive pulse from ATTiny tells Teensy To begin shut down code.
  // PIN 2 - PB3 - INPUT.   Push Button INPUT for turn radio ON and OFF.
  // PIN 3 - PB4 - OUTPUT.  HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
  pinMode(SHUTDOWN_COMPLETE, INPUT); // driven by Teensy
  pinMode(START_SHUTDOWN,    OUTPUT);
  pinMode(PANEL_SWITCH,      INPUT); // this has an external pulldown. Gets pulled high when button is pressed.
  pinMode(FET_SWITCH,        OUTPUT);

  // Set initial state to "radio off" initially
  digitalWrite(FET_SWITCH,     LOW);        // RADIO OFF - Shut off power FET
  digitalWrite(START_SHUTDOWN, LOW);        // Instruction to start shutdown is LOW 
  currentState = OFF_STATE;
}

void loop() 
{  
  switch(currentState){
    case OFF_STATE:
      // Exit OFF_STATE to ON_STATE if the button is pressed
      if (digitalRead(PANEL_SWITCH) == HIGH) {
        digitalWrite(FET_SWITCH, HIGH);       // Start by turning the FET switch "ON" to boot the Teensy and start the radio.
        currentState = ON_STATE;              // and change to the ON_STATE.
        delay(3000L);                         // Wait 3 seconds (poor man's debounce)
      }
      break;
    case ON_STATE:
      // Exit ON_STATE to SHUTDOWN_STATE if the button is pressed
      if (digitalRead(PANEL_SWITCH) == HIGH) {
        digitalWrite(START_SHUTDOWN, HIGH);   // Tell the Teensy to run the Teensy shut down code
        currentState = SHUTDOWN_STATE;        // change to SHUTDOWN_STATE
        delay(3000L);                         // Wait 3 seconds (poor man's debounce)
      }
      break;
    case SHUTDOWN_STATE:
      // Exit SHUTDOWN_STATE to OFF_STATE if the Teensy has notified us that it has finished shutting down
      if (digitalRead(SHUTDOWN_COMPLETE) == HIGH) {
        digitalWrite(START_SHUTDOWN, LOW);    // Shutdown cycle is complete
        digitalWrite(FET_SWITCH, LOW);        // Set the FET switch to "OFF"
        currentState = OFF_STATE;             // Change to OFF state
        delay(1000L);                         // Wait a second to give the Teensy and rest of circuit time to turn off and stabilize
      }
      break;
  }
} // LOOP

