// ---------------------------------------------------------------- /
// BPFTest
// Written Oliver - KI3P
// Modified by Dr. WJ Schmidt - K9HZ
// April 12, 2024
// Using Arduino IDE 2.3.2
// 
// This sketch tests Switching functions of the T41 V12 BPF filters 
// so that the filters can be tested individually.
// 
// DATA:
//    BPF #1 is on Wire3 at address 0x24 (main receiver)
//    BPF #2 is on Wire3 at address 0x26 (second receiver)
//    Uncomment the appropriate address for the BPF board below.
//
// 
// ---------------------------------------------------------------- /

#include <Wire.h>
#include <Adafruit_MCP23X17.h>
//#include <math.h>

#define BPF_BOARD_MCP23017_ADDR 0x24   // For BPF #1 Address
//#define BPF_BOARD_MCP23017_ADDR 0x26   // For BPF #2 Address

// Define BPF Band words

#define BAND_BYPASS 0x0008
#define BAND_6M     0x0004
#define BAND_10M    0x0002
#define BAND_12M    0x0001
#define BAND_15M    0x8000
#define BAND_17M    0x4000
#define BAND_20M    0x2000
#define BAND_30M    0x1000
#define BAND_40M    0x0800
#define BAND_60M    0x0400
#define BAND_80M    0x0200
#define BAND_160M   0x0100

static Adafruit_MCP23X17 mcpBPF;

uint16_t GPAB_state;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(5000);

  // Set Wire2 I2C bus to 100KHz and start
  Wire.setClock(100000UL);
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();
  
  // Set the I2C Address
  while (!mcpBPF.begin_I2C(BPF_BOARD_MCP23017_ADDR,&Wire2)){
    Serial.println("BPF MCP23017 not found at 0x"+String(BPF_BOARD_MCP23017_ADDR,HEX));
    delay(5000);
  }
  mcpBPF.enableAddrPins();

  // Set all pins to be outputs
  for (int i=0;i<16;i++){
    mcpBPF.pinMode(i, OUTPUT);
  }

  // Set to BYPASS for startup.
  GPAB_state = 0x0008;
  mcpBPF.writeGPIOAB(GPAB_state); 
}

void print_state(void){
  uint16_t band = GPAB_state;
  Serial.print("LPF Band: ");
  switch(band) {
    case BAND_BYPASS:
      Serial.println("BYPASS [0008]");
      break;
    case BAND_6M:
      Serial.println("6m [0004]");
      break;
    case BAND_10M:
      Serial.println("10m [0002]");
      break;
    case BAND_12M:
      Serial.println("12m [0001]");
      break;
    case BAND_15M:
      Serial.println("15m [8000]");
      break;
    case BAND_17M:
      Serial.println("17m [4000]");
      break;
    case BAND_20M:
      Serial.println("20m [2000]");
      break;
    case BAND_30M:
      Serial.println("30m [1000]");
      break;
    case BAND_40M:
      Serial.println("40m [0800]");
      break;
    case BAND_60M:
      Serial.println("60m [0400]");
      break;
    case BAND_80M:
      Serial.println("80m [0200]");
      break;
    case BAND_160M:
      Serial.println("160m [0100]");
      break;
    default:
      Serial.print("Unknown band: ");
      Serial.println(band, BIN);
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

  while (Serial.available() == 0) {}       //wait for data available
  String selection = Serial.readString();  //read until timeout
  selection.trim();                        // remove any \r \n whitespace at the end of the String

  if (selection == "N"){
      Serial.println("Selecting NF band");
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
//  Update to new band
  mcpBPF.writeGPIOAB(GPAB_state); 
//  Write new band filter selection to the console
  Serial.print("Written to GPAB: ");
  Serial.println(GPAB_state, BIN);
  Serial.println("-------------------------\n");

}

