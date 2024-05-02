#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <math.h>
#include "AD7991.h"

#define LPF_BOARD_MCP23017_ADDR 0x25
#define TXSTATE 1
#define RXSTATE 0
#define TXRX_PIN 2

// The attenuation of the binocular toroid coupler and the attenuation of the pad
#define COUPLER_ATTENUATION_DB 20
#define PAD_ATTENUATION_DB 26
#define SWR_REPETITIONS 100 // 100 repetitions takes roughly 100 ms
#define VREF_MV 5000.0 // the reference voltage on your board

// Uncomment if you are KI3P and you modified your prototype board
//#define KI3PMODS 1

#ifndef KI3PMODS
#define BAND_NF 0b1111
#define BAND_6M 0b1010
#define BAND_10M 0b1001
#define BAND_12M 0b1000
#define BAND_15M 0b0111
#define BAND_17M 0b0110
#define BAND_20M 0b0101
#define BAND_30M 0b0100
#define BAND_40M 0b0011
#define BAND_60M 0b0000
#define BAND_80M 0b0010
#define BAND_160M 0b0001
#else
#define BAND_NF 0b0111
#define BAND_6M 0b0110
#define BAND_10M 0b1111
#define BAND_12M 0b1010
#define BAND_15M 0b1001
#define BAND_17M 0b1000
#define BAND_20M 0b0010
#define BAND_30M 0b0001
#define BAND_40M 0b0101
#define BAND_60M 0b0100
#define BAND_80M 0b0011
#define BAND_160M 0b0000
#endif


static Adafruit_MCP23X17 mcpLPF;
static AD7991 swrADC;

uint8_t GPB_state;
uint8_t GPA_state;
uint8_t TXRX_state;

void scanner() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(5000);

  // Set Wire2 I2C bus to 100KHz and start
  Wire2.setClock(100000UL);
  Wire2.begin();
  
  while (!mcpLPF.begin_I2C(LPF_BOARD_MCP23017_ADDR, &Wire2)){
    Serial.println("LPF MCP23017 not found at 0x"+String(LPF_BOARD_MCP23017_ADDR,HEX));
    scanner();
    delay(5000);
  }
  mcpLPF.enableAddrPins();
  // Set all pins to be outputs
  for (int i=0;i<16;i++){
    mcpLPF.pinMode(i, OUTPUT);
  }

  // Set all pins to zero. This puts BPF in the path for both RX and TX
  GPA_state = 0x00;
  mcpLPF.writeGPIOA(GPA_state); 

  // Pin mapping:
  // GPB0: Band BIT0
  // GPB1: Band BIT1
  // GPB2: Band BIT2
  // GPB3: Band BIT3
  // GPB4: Antenna BIT0
  // GPB5: Antenna BIT1
  // GPB6: XVTR_SEL (1 means no XVTR)
  // GPB7: 100W_PA_SEL (0 means no 100W)

  // Default state: NOFILT Ant0  No XVTR  No 100W PA
  GPB_state =       0b01001111;
  mcpLPF.writeGPIOB(GPB_state); 

  // GPIO2 is mapped to TX pin:
  // HIGH means TX mode
  // LOW means RX mode
  pinMode(TXRX_PIN, OUTPUT);
  TXRX_state = RXSTATE;
  digitalWrite(TXRX_PIN, TXRX_state);  // RX mode

  while (!swrADC.begin(AD7991_I2C_ADDR1,&Wire)){
    Serial.println("AD7991 not found at 0x"+String(AD7991_I2C_ADDR1,HEX));

    if (swrADC.begin(AD7991_I2C_ADDR2,&Wire)){
      Serial.println("AD7991 found at alternative 0x"+String(AD7991_I2C_ADDR2,HEX));
      break;
    }
    scanner();
    delay(5000);
  }
}

void read_swr(void){

  // Need 32 bit uints to keep from overflowing 
  uint16_t f = 0;
  uint16_t pkf = 0;
  uint32_t adcF_s = 0;
  uint32_t adcR_s = 0;

  // Measure the peak forward voltage and the RMS of the forward
  // and reverse voltages
  for(uint8_t i=0; i<SWR_REPETITIONS; i++) {
    // Forward voltage
    // If Vref was actually 4.096 V, adcF and adcR would be in units of mV
    f = (float)(swrADC.readADCsingle(0))*VREF_MV/4096;
    adcR_s += pow((float)(swrADC.readADCsingle(1))*VREF_MV/4096,2);
    adcF_s += pow(f,2);
    if (f > pkf){
      pkf = f;
    }
  }

  float Vr_RMS_mV = pow(adcR_s/SWR_REPETITIONS,0.5);
  float Vf_RMS_mV = pow(adcF_s/SWR_REPETITIONS,0.5);
  float Vf_pk_mV  = pkf;
  Serial.print("Forward voltage peak [mV]  = ");
  Serial.println(Vf_pk_mV,1);
  Serial.print("Forward voltage RMS [mV]   = ");
  Serial.println(Vf_RMS_mV,1);
  Serial.print("Reflected voltage RMS [mV] = ");
  Serial.println(Vr_RMS_mV,1);

  // Convert the measured voltage into powers
  // Vmeas [V] = 0.025 * (Pin [dBm] + 84)
  // Pin [dBm] = Pf [dBm] - PAD_ATTENUATION_DB - COUPLER_ATTENUATION_DB
  // After some math:
  // A = sqrt(Pr/Pf) = 10**(2*(Vr [V] - Vf [V]))
  // SWR = (1+A)/(1-A)
  // Pf [W] = 10^( 4*Vf [V]  - (84 - PAD_ATTENUATION_DB - COUPLER_ATTENUATION_DB)/10 - 3 )

  float Pf_dBm = (Vf_RMS_mV/1000)/0.025 - (84 - PAD_ATTENUATION_DB - COUPLER_ATTENUATION_DB); 
  float Pf_W = pow(10, 4*Vf_RMS_mV/1000 - (84 - PAD_ATTENUATION_DB - COUPLER_ATTENUATION_DB)/10 - 3 );
  float Pf_pk_W = pow(10, 4*Vf_pk_mV/1000 - (84 - PAD_ATTENUATION_DB - COUPLER_ATTENUATION_DB)/10 - 3 );
  float A = pow(10, 2*(Vr_RMS_mV - Vf_RMS_mV)/1000 );
  float swr = (1+A)/(1-A);

  Serial.print("Pf RMS [W] = ");
  Serial.println(Pf_W, 2);
  Serial.print("Pf pk [W] = ");
  Serial.println(Pf_pk_W, 2);
  //Serial.print("A = ");
  //Serial.println(A, 4);
  Serial.print("SWR = ");
  Serial.println(swr, 2);

}

void print_state(void){
  uint8_t band = GPB_state & 0b00001111;
  Serial.print("LPF Band: ");
  switch(band) {
    case BAND_NF:
      Serial.println("NO FILT [1111]");
      break;
    case BAND_6M:
      Serial.println("6m [1010]");
      break;
    case BAND_10M:
      Serial.println("10m [1001]");
      break;
    case BAND_12M:
      Serial.println("12m [1000]");
      break;
    case BAND_15M:
      Serial.println("15m [0111]");
      break;
    case BAND_17M:
      Serial.println("17m [0110]");
      break;
    case BAND_20M:
      Serial.println("20m [0101]");
      break;
    case BAND_30M:
      Serial.println("30m [0100]");
      break;
    case BAND_40M:
      Serial.println("40m [0011]");
      break;
    case BAND_60M:
      Serial.println("60m [0000]");
      break;
    case BAND_80M:
      Serial.println("80m [0010]");
      break;
    case BAND_160M:
      Serial.println("160m [0001]");
      break;
    default:
      Serial.print("Unknown band: ");
      Serial.println(band, BIN);
      break;
  }

  // GPB4: Antenna BIT0
  // GPB5: Antenna BIT1
  uint8_t antenna = (GPB_state & 0b00110000)>>4;
  Serial.print("Antenna: ");
  Serial.println(antenna, DEC);
  // GPB6: XVTR_SEL (0 means no XVTR)
  Serial.print("XVTR: ");
  Serial.println(((GPB_state & 0b01000000)>>6) ^ 1, DEC);
  // GPB7: 100W_PA_SEL (0 means no 100W)
  Serial.print("100W_PA: ");
  Serial.println((GPB_state & 0b10000000)>>7, DEC);

  // GPA0: TX_BPF_SEL (0 means default (in path))
  Serial.print("TX_BPF: ");
  Serial.println((GPA_state & 0b00000001), DEC);
  // GPA1: RX_BPF_SEL (0 means default (in path))
  Serial.print("RX_BPF: ");
  Serial.println((GPA_state & 0b00000010)>>1, DEC);

  if (TXRX_state == TXSTATE) {
    Serial.println("In transmit state");
  } 
  if (TXRX_state == RXSTATE) {
    Serial.println("In receive state");  
  }

}

void loop() {
  // print the state and selection menu
  Serial.println("Current state:");
  print_state();
  Serial.println("");
  Serial.println("Select option and hit enter:");
  Serial.println("N   - Select NO FILT band");
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
  Serial.println("1-4 - Select antenna 1 to 4");
  Serial.println("XV  - Select TXVR state");
  Serial.println("HF  - Select none-TXVR state");
  Serial.println("PA  - Toggle 100W PA state");
  Serial.println("TXB - Toggle TX BPF state");
  Serial.println("RXB - Toggle RX BPF state");
  Serial.println("TX  - Select TX state");
  Serial.println("RX  - Select RX state");
  Serial.println("SWR - Read the SWR");


  while (Serial.available() == 0) {}     //wait for data available
  String selection = Serial.readString();  //read until timeout
  selection.trim();                        // remove any \r \n whitespace at the end of the String

  if (selection == "N"){
      Serial.println("Selecting NF band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_NF;
  } 
  if (selection == "6"){
      Serial.println("Selecting 6m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_6M;
  } 
  if (selection == "10"){
      Serial.println("Selecting 10m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_10M;
  } 
  if (selection == "12"){
      Serial.println("Selecting 12m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_12M;
  } 
  if (selection == "15"){
      Serial.println("Selecting 15m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_15M;
  } 
  if (selection == "17"){
    Serial.println("Selecting 17m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_17M;
  } 
  if (selection == "20"){
    Serial.println("Selecting 20m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_20M;
  } 
  if (selection == "30"){
    Serial.println("Selecting 30m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_30M;
  } 
  if (selection == "40"){
    Serial.println("Selecting 40m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_40M;
  } 
  if (selection == "60"){
    Serial.println("Selecting 60m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_60M;
  } 
  if (selection == "80"){
    Serial.println("Selecting 80m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_80M;
  } 
  if (selection == "160"){
    Serial.println("Selecting 160m band");
      GPB_state = GPB_state & 0b11110000;
      GPB_state = GPB_state | BAND_160M;
  } 
  if (selection == "1"){
    Serial.println("Selecting antenna 1");
      GPB_state = GPB_state & 0b11001111;
      #ifndef KI3PMODS
      GPB_state = GPB_state | (0 << 4);
      #else
      GPB_state = GPB_state | (2 << 4);
      #endif
  } 
  if (selection == "2"){
    Serial.println("Selecting antenna 2");
      GPB_state = GPB_state & 0b11001111;
      #ifndef KI3PMODS
      GPB_state = GPB_state | (1 << 4);
      #else
      GPB_state = GPB_state | (3 << 4);
      #endif
  } 
  if (selection == "3"){
    Serial.println("Selecting antenna 3");
      GPB_state = GPB_state & 0b11001111;
      #ifndef KI3PMODS
      GPB_state = GPB_state | (2 << 4);
      #else
      GPB_state = GPB_state | (0 << 4);
      #endif
  } 
  if (selection == "4"){
    Serial.println("Selecting antenna 4");
      GPB_state = GPB_state & 0b11001111;
      #ifndef KI3PMODS
      GPB_state = GPB_state | (3 << 4);
      #else
      GPB_state = GPB_state | (1 << 4);
      #endif
  } 
  if (selection == "XV"){
    Serial.println("Selecting XVTR path");
    GPB_state = GPB_state & 0b10111111;
  } 
  if (selection == "HF"){
    Serial.println("Selecting HF path");
    GPB_state = GPB_state & 0b10111111;
    GPB_state = GPB_state | 0b01000000;
  } 
  if (selection == "PA"){
    Serial.println("Toggling 100W PA selection");
      GPB_state = GPB_state ^ 0b10000000;
  } 
  if (selection == "TXB"){
    Serial.println("Toggling TX BPF selection");
      GPA_state = GPA_state ^ 0b00000001;
  } if (selection == "RXB"){
    Serial.println("Toggling RX BPF selection");
      GPA_state = GPA_state ^ 0b00000010;
  } 
  if (selection == "TX"){
    Serial.println("Selecting TX state");
    TXRX_state = TXSTATE;
    digitalWrite(TXRX_PIN, TXRX_state);  // TX mode
  } 
  if (selection == "RX"){
    Serial.println("Selecting RX state");
    TXRX_state = RXSTATE;
    digitalWrite(TXRX_PIN, TXRX_state);  // RX mode
  }
  if (selection == "SWR"){

    for (uint8_t i=0; i<60; i++){
    read_swr();
    delay(1000);
    }
  }

  mcpLPF.writeGPIOB(GPB_state); 
  mcpLPF.writeGPIOA(GPA_state); 

  Serial.print("Written to GPA: ");
  Serial.println(GPA_state, BIN);
  Serial.print("Written to GPB: ");
  Serial.println(GPB_state, BIN);


  Serial.println("-------------------------\n");

}
