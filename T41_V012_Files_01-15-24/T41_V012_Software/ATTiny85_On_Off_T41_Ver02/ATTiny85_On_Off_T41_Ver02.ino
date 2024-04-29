// Code for the T41 Easy radio ON and OFF routines.  This allows for a 
// special trigger from the ATTiny85 sent to the Tensy that causes it to
// run through special code in the teensy 4.1 before the radio shuts down.
// The special code could be code to save parameters that have changed.
//
//       Written by Dr. William J. Schmidt, II, Ph.D. - K9HZ
//
// I'm an excellent hardware engineer but a very shitty software engineer.
// Therefore, plagerize and modify away!  No warranty.  Just give me a little
// credit.

#define TOTAL_SHUTDOWN    2       // Positive pulse from Teensy tells ATTiny to complete shut down.
#define START_SHUTDOWN    1       // Positive pulse from ATTiny tells Teensy To begin shut down code.
#define PANEL_SWITCH      3       // Front panel switch to turn radio ON/OFF
#define FET_SWITCH        4       // HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
//
// Variables:
//
bool CurrentStateOn; 
//
void setup() {
  // Set up the Input and Output pins as follows://
  // PIN 7 - PB2 - INPUT.   Positive pulse from Teensy tells ATTiny to complete shut down.
  // PIN 6 - PB1 - OUTPUT.  Positive pulse from ATTiny tells Teensy To begin shut down code.
  // PIN 2 - PB3 - INPUT.   Push Button INPUT for turn radio ON and OFF.
  // PIN 3 - PB4 - OUTPUT.  HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
  pinMode(TOTAL_SHUTDOWN, INPUT);
  pinMode(START_SHUTDOWN, OUTPUT);
  pinMode(PANEL_SWITCH,   INPUT);
  pinMode(FET_SWITCH,     OUTPUT);

  // Set initial state to "radio off" initially
  digitalWrite(FET_SWITCH,     LOW);        //RADIO OFF - Shut off power FET
  digitalWrite(START_SHUTDOWN, LOW);        //Prepare for RADIO ON - Tell Teensy that 
  CurrentStateOn = false;                   // No switchPush to start
}

void loop() 
{  // Read pushbutton switch
  if (digitalRead(PANEL_SWITCH) == HIGH) {  // Test for a push button for pushed state - change status
    if (CurrentStateOn == true) {           // IF we are already "ON" and the button was pushed, start 
      digitalWrite(START_SHUTDOWN, LOW);    // the Shutdown by setting the start shutdown line to the teensy high...
      delay(5000L);                         // and then wait a five seconds so we don't just trigger the radio to the "ON" state immediately 
    } else {                                // Otherwise we are "OFF" and we should turn the radio "ON"" because the switch was pushed
      digitalWrite(FET_SWITCH, HIGH);       // Start by turning the FET switch "ON" to boot the Teensy and start the radio.
      CurrentStateOn = true;                // and make a note that we turned on the radio.
      delay(3000L);                         // Wait 3 seconds so we don't just accidntally just turn the switch off again.
    }
  }
  //
  // test to see if the radio is currently "ON" and the the teensy says shut "OFF" because it has completed
  // the execution of the shut down code and the ATTiny can complete the shut down process.
  //
  if (CurrentStateOn == true) {
    if (digitalRead(TOTAL_SHUTDOWN) == HIGH) {   // Teensy says we are through the shutdown code.  Just shut down now.
      digitalWrite(FET_SWITCH, LOW);        // Set the FET switch to "OFF".
      digitalWrite(START_SHUTDOWN, LOW);    // Prepare for RADIO ON - Tell Teensy that 
      CurrentStateOn = false;               // and remember we shut off the radio for next pass.
      delay(1000L);                         // Wait a second while so we don't just trigger an "ON" state accidentally.
    }
  }
} // LOOP

