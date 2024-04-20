// ---------------------------------------------------------------- /
// BPFTest
// Written by Oliver King - KI3P for the K9HZ LPF Checkout
// Modified by Dr. WJ Schmidt - K9HZ for the BPF Checkout
// April 12, 2024
// Using Arduino IDE 2.3.2
// 
// This sketch tests Switching functions of the T41 V12 BPF filters 
// so that the filters can be tested individually.
// 
// THINGS TO CHANGE:
//    BPF #1 is on Wire3 at address 0x24 (main receiver)
//    BPF #2 is on Wire3 at address 0x26 (second receiver)
//    Uncomment the appropriate address for the BPF board below.
//
// 
// ---------------------------------------------------------------- /

#include <Wire.h>
#include <Adafruit_MCP23X17.h>

//
// The BOARD ADDRESS is the only adjustable parameter in this program
//
#define BPF_BOARD_MCP23017_ADDR 0x24   // For BPF #1 Address
//#define BPF_BOARD_MCP23017_ADDR 0x26   // For BPF #2 Address

<<<<<<< Updated upstream
// Define BPF Band words [GPB7,...,GPB0,GPA7,...,GPA0]

#define BAND_BYPASS 0b0000000000001000  //GPA3 = 1
#define BAND_6M     0b0000000000000100  //GPA2 = 1
#define BAND_10M    0b0100000000000010  //GPA1 = 1
#define BAND_12M    0b0000000000000001  //GPA0 = 1
#define BAND_15M    0b1000000000000000  //GPB7 = 1
#define BAND_17M    0b0100000000000000  //GPB6 = 1
#define BAND_20M    0b0010000000000000  //GPB5 = 1
#define BAND_30M    0b0001000000000000  //GPB4 = 1
#define BAND_40M    0b0000100000000000  //GPB3 = 1
#define BAND_80M    0b0000010000000000  //GPB2 = 1
#define BAND_160M   0b0000001000000000  //GPB1 = 1
#define BAND_60M    0b0000000100000000  //GPB0 = 1
#define BAND_UDEF   0b0000000000000000  //GPBA = 0
=======
#define ESP32

// Define BPF Band words
// When the pin of the MCP23017 is HIGH, then pin6/V1/control A of 
// MASWSS0179 is HIGH and RFC is connected to RF1.
// To select any of the bands we want to connect RFC to RF1, so drive
// the corresponding pin on MCP23017 HIGH. To select BYPASS we want 
// instead to connect RFC to RF2, which means driving it LOW.
// Word definition: GPB7 GPB6 ... GPB0 GPA7 GPA6 ... GPA0
#define BAND_BYPASS 0x0000
#define BAND_6M     0x0004+0x0008
#define BAND_10M    0x0002+0x0008
#define BAND_12M    0x0001+0x0008
#define BAND_15M    0x8000+0x0008
#define BAND_17M    0x4000+0x0008
#define BAND_20M    0x2000+0x0008

#define BAND_30M    0x1000+0x0008
#define BAND_40M    0x0800+0x0008
#define BAND_60M    0x0100+0x0008
#define BAND_80M    0x0400+0x0008
#define BAND_160M   0x0200+0x0008
//                0000 0010 0000 1000

#define PINRXTX 0
>>>>>>> Stashed changes

static Adafruit_MCP23X17 mcpBPF;

uint16_t GPAB_state;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(5000);

  #ifdef ESP32
  Wire.begin(21,22,100000UL);
  #else
  // Set Wire2 I2C bus to 100KHz and start
  Wire2.setClock(100000UL);
<<<<<<< Updated upstream
  //Wire.setSDA(0);
  //Wire.setSCL(1);
  Wire2.begin();
  
=======
  Wire2.begin();
  #endif

>>>>>>> Stashed changes
  // Set the I2C Address
  #ifdef ESP32
  while (!mcpBPF.begin_I2C(BPF_BOARD_MCP23017_ADDR,&Wire)){
  #else
  while (!mcpBPF.begin_I2C(BPF_BOARD_MCP23017_ADDR,&Wire2)){
  #endif
    Serial.println("BPF MCP23017 not found at 0x"+String(BPF_BOARD_MCP23017_ADDR,HEX));
    delay(5000);
  }

  // Enable the address pins A0, A1, and A2.  
  mcpBPF.enableAddrPins();
  // Set all chip pins to be outputs
  for (int i=0;i<16;i++){
    mcpBPF.pinMode(i, OUTPUT);
  }
  // Set to BYPASS for startup.
  GPAB_state = BAND_BYPASS;
  mcpBPF.writeGPIOAB(GPAB_state); 

  // Set up RXTX
  pinMode(PINRXTX, OUTPUT);
  digitalWrite(PINRXTX, 0);  // RX mode
}

void print_state(void){
uint16_t band = GPAB_state;
  Serial.print("BPF Band: ");
  switch(band) {
    case BAND_BYPASS:
      Serial.print("BYPASS [0b");
      Serial.print(BAND_BYPASS,BIN);
      Serial.println("]");
      break;
    case BAND_6M:
      Serial.print("6m [0b");
      Serial.print(BAND_6M,BIN);
      Serial.println("]");
      break;
    case BAND_10M:
      Serial.print("10m [0b");
      Serial.print(BAND_10M,BIN);
      Serial.println("]");
      break;
    case BAND_12M:
      Serial.print("12m [0b");
      Serial.print(BAND_12M,BIN);
      Serial.println("]");
      break;
    case BAND_15M:
      Serial.print("15m [0b");
      Serial.print(BAND_15M,BIN);
      Serial.println("]");
      break;
    case BAND_17M:
      Serial.print("17m [0b");
      Serial.print(BAND_17M,BIN);
      Serial.println("]");
      break;
    case BAND_20M:
      Serial.print("20m [0b");
      Serial.print(BAND_20M,BIN);
      Serial.println("]");
      break;
    case BAND_30M:
      Serial.print("30m [0b");
      Serial.print(BAND_30M,BIN);
      Serial.println("]");
      break;
    case BAND_40M:
      Serial.print("40m [0b");
      Serial.print(BAND_40M,BIN);
      Serial.println("]");
      break;
    case BAND_60M:
      Serial.print("60m [0b");
      Serial.print(BAND_60M,BIN);
      Serial.println("]");
      break;
    case BAND_80M:
      Serial.print("80m [0b");
      Serial.print(BAND_80M,BIN);
      Serial.println("]");
      break;
    case BAND_160M:
      Serial.print("160m [0b");
      Serial.print(BAND_160M,BIN);
      Serial.println("]");
      break;
    default:
      Serial.print("Unknown band: [0b");
      Serial.print(band, BIN);
      Serial.print("]");
      break;
  }
}

void loop() {
  // print the state and selection menu
  Serial.println("Current state:");
  print_state();
  Serial.println("");
  Serial.println("Select option and hit enter:");
  Serial.println("N   - Select BYPASS");
  Serial.println("6   - Select 6M band");
  Serial.println("10  - Select 10M band");
  Serial.println("12  - Select 12M band");
  Serial.println("15  - Select 15M band");
  Serial.println("17  - Select 17M band");
  Serial.println("20  - Select 20M band");
  Serial.println("30  - Select 30M band");
  Serial.println("40  - Select 40M band");
  Serial.println("60  - Select 60M band");
  Serial.println("80  - Select 80M band");
  Serial.println("160 - Select 160M band");
  Serial.println("T   - Select TX mode");
  Serial.println("R   - Select RX mode");

  while (Serial.available() == 0) {}       //wait for data available
  String selection = Serial.readString();  //read until timeout
  selection.trim();                        // remove any \r \n whitespace at the end of the String
  
  Serial.print("Selection was-> ");
  Serial.println(selection);
  if (selection == "N"){
      Serial.println("Selecting bypass");
      GPAB_state = BAND_BYPASS;
  } 
  if (selection == "6"){
      Serial.println("Selecting 6m band");
      GPAB_state = BAND_6M;
  } 
  if (selection == "10"){
      Serial.println("Selecting 10m band");
      GPAB_state = BAND_10M;
  } 
  if (selection == "12"){
      Serial.println("Selecting 12m band");
      GPAB_state = BAND_12M;
  } 
  if (selection == "15"){
      Serial.println("Selecting 15m band");
      GPAB_state = BAND_15M;
  } 
  if (selection == "17"){
    Serial.println("Selecting 17m band");
      GPAB_state = BAND_17M;
  } 
  if (selection == "20"){
    Serial.println("Selecting 20m band");
      GPAB_state = BAND_20M;
  } 
  if (selection == "30"){
    Serial.println("Selecting 30m band");
      GPAB_state = BAND_30M;
  } 
  if (selection == "40"){
    Serial.println("Selecting 40m band");
      GPAB_state = BAND_40M;
  } 
  if (selection == "60"){
    Serial.println("Selecting 60m band");
      GPAB_state = BAND_60M;
  } 
  if (selection == "80"){
    Serial.println("Selecting 80m band");
      GPAB_state = BAND_80M;
  } 
  if (selection == "160"){
    Serial.println("Selecting 160m band");
      GPAB_state = BAND_160M;
  }
  if (selection == "T"){
    Serial.println("Selecting TX mode");
    digitalWrite(PINRXTX, 1);  // TX mode
  }
  if (selection == "R"){
    Serial.println("Selecting RX mode");
    digitalWrite(PINRXTX, 0);  // RX mode
  }

//  Update to new band
  mcpBPF.writeGPIOAB(GPAB_state);
  Serial.print("Written to GPAB: [");
  Serial.print(GPAB_state, BIN);
  Serial.println("]");
  Serial.println("-------------------------\n");

}

