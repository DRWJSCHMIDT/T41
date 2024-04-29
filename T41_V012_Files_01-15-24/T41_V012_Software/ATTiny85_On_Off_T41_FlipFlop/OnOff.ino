// Code for the T41 Easy radio ON and OFF Flip-Flop routines.  
//
//       Written by Dr. William J. Schmidt, II, Ph.D. - K9HZ
//
// I'm an excellent hardware engineer but a very shitty software engineer.
// Therefore, plagerize and modify away!  No warranty.  Just give me a little
// credit.
#define PANEL_SWITCH      3       // Front panel switch to turn radio ON/OFF
#define FET_SWITCH        4       // HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
//
// Variables:
//
bool CurrentStateOn; 
//
void setup() {
  // Set up the Input and Output pins as follows://
  // PIN 2 - PB3 - INPUT.   Push Button INPUT for turn radio ON and OFF.
  // PIN 3 - PB4 - OUTPUT.  HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
  pinMode(PANEL_SWITCH,   INPUT);
  pinMode(FET_SWITCH,     OUTPUT);
  // Set initial state to "radio off" initially
  digitalWrite(FET_SWITCH,     LOW);        //RADIO OFF - Shut off power FET
  CurrentStateOn = false;                   // No switchPush to start
}

void loop() 
{  // Read pushbutton switch
  if (digitalRead(PANEL_SWITCH) == HIGH) {  // Test for a push button for pushed state - change status
    if (CurrentStateOn == false) {          // IF we are "OFF" and the button was pushed, start 
      digitalWrite(FET_SWITCH, HIGH);       // Start by turning the FET switch "ON" to boot the Teensy and start the radio.
      CurrentStateOn = true;                // And make a note that we turned on the radio.
      delay(2000L);                         // And wait 2 seconds so that we don't accidently turn the radio back off.
      }                                     
    else {                                  // Switch was pressed and radio is "ON"...
      digitalWrite(FET_SWITCH, LOW);        // Turn the FET switch "OFF" to shut the power off to the radio.
      CurrentStateOn = false;               // Note that we turned off the radio.
      delay(2000L);                         // Anw wait 2 seconds to that we don't accidently turn the radio back on. 
      }
    }
} // LOOP

