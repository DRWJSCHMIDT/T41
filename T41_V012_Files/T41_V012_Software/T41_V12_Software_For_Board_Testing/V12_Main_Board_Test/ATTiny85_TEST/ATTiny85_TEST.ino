#define TOTAL_SHUTDOWN    7       // 7 Positive pulse from Teensy tells ATTiny to complete shut down.
#define START_SHUTDOWN    6       // 6 Positive pulse from ATTiny tells Teensy To begin shut down code.
#define PANEL_SWITCH      4       // 2 Front panel switch to turn radio ON/OFF
#define FET_SWITCH        3       // 3 HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
void setup() {
  // Set up the Input and Output pins as follows://
  // PIN 7 - P2 - OUTPUT.  
  // PIN 6 - P1 - INPUT.   Positive pulse from Teensy tells ATTiny to complete shut down.
  // PIN 2 - P4 - OUTPUT.  HIGH=RADIO ON, LOW=RADIO OFF. FET switch connection.
  // PIN 3 - P3 - INPUT.   Push Button INPUT for turn radio ON and OFF.
  pinMode(TOTAL_SHUTDOWN, OUTPUT);
  pinMode(START_SHUTDOWN, OUTPUT);
  pinMode(PANEL_SWITCH,   OUTPUT);
  pinMode(FET_SWITCH,     OUTPUT);

  // Set initial state to "radio off" initially
  digitalWrite(TOTAL_SHUTDOWN, LOW);        //RADIO OFF - Shut off power FET
  digitalWrite(START_SHUTDOWN, LOW);        //Prepare for RADIO ON - Tell Teensy that 
  digitalWrite(PANEL_SWITCH,   LOW);        //RADIO OFF - Shut off power FET
  digitalWrite(FET_SWITCH,     LOW);        //Prepare for RADIO ON - Tell Teensy that 
}

void loop() 
{  
  //digitalWrite(TOTAL_SHUTDOWN, HIGH);       // by telling the Teensy to start shutting down by running the Teensy shut down code
  //digitalWrite(START_SHUTDOWN, HIGH);        //Prepare for RADIO ON - Tell Teensy that 
  digitalWrite(PANEL_SWITCH,   HIGH);        //RADIO OFF - Shut off power FET
  //digitalWrite(FET_SWITCH,     HIGH);        //Prepare for RADIO ON - Tell Teensy that 
  delay(200L);                              // and then wait a five seconds so we don't just trigger the radio to the "ON" state immediately 
  digitalWrite(TOTAL_SHUTDOWN, LOW);       // by telling the Teensy to start shutting down by running the Teensy shut down code
  digitalWrite(START_SHUTDOWN, LOW);        //Prepare for RADIO ON - Tell Teensy that 
  digitalWrite(PANEL_SWITCH,   LOW);        //RADIO OFF - Shut off power FET
  digitalWrite(FET_SWITCH,     LOW);        //Prepare for RADIO ON - Tell Teensy that 
  delay(200L);                              // and then wait a five seconds so we don't just trigger the radio to the "ON" state immediately   
} // LOOP

