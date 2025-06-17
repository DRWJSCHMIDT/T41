#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <math.h>
#include "si5351.h"

#define RF_BOARD_MCP23017_ADDR 0x27
#define RF_BOARD_SI5351A_ADDR 0x60

#define TXSTATE 1
#define RXSTATE 0
#define XIQSTATE 0
#define XCWSTATE 1
#define CWONSTATE 1
#define CWOFFSTATE 0
#define CALONSTATE 1
#define CALOFFSTATE 0

// Names and pins should match SDT.h
#define RXTX 22
#define CW_ON_OFF 33
#define XMIT_MODE 34
#define CAL 38

Si5351 si5351;

// ===========  Quad Si5351 stuff //AFP 09-24-23 V12

unsigned long long Clk2SetFreq;  //AFP 09-24-23 V12
unsigned long long Clk0SetFreq;  // AFP 09-27-22
unsigned long long Clk1SetFreq = 1000000000ULL;
unsigned long long pll_min = 60000000000ULL;
unsigned long long pll_max = 90000000000ULL;
unsigned long long f_pll_freq;
unsigned long long pll_freq;
unsigned long long freq;
int multiple = 126;
int oldMultiple = 126;
unsigned long long oldfreq;
unsigned long long freq1;
long TxRxFreq;  // = centerFreq+NCOFreq  NCOFreq from FreqShift2()
long CWFreq = 7500000;
int32_t IFFreq;

const uint32_t N_B_EX = 16;
//AFP 09-24-23 V12 end Quad stuff
#define Si_5351_crystal             25000000L
long freqCorrectionFactor = 5000; //AFP 09-24-23 V12

static Adafruit_MCP23X17 mcpRF;
uint8_t GPB_state;
uint8_t GPA_state;
uint8_t TXRX_state;
uint8_t transmit_mode_state;
uint8_t cw_state;
uint8_t cal_state;
uint8_t *bank_ptr;

// Pin mapping:
// GPA0: RX att 0.5
// GPA1: RX att 1
// GPA2: RX att 2
// GPA3: RX att 4
// GPA4: RX att 8
// GPA5: RX att 16
// GPA6: unused
// GPA7: MF/HF (0 means HF)
// GPB0: TX att 0.5
// GPB1: TX att 1
// GPB2: TX att 2
// GPB3: TX att 4
// GPB4: TX att 8
// GPB5: TX att 16
// GPB6: unused
// GPB7: unused

void set_bank_bit(uint8_t* GPIO_bank, uint8_t bit){
  GPIO_bank[0] = GPIO_bank[0] | (1 << bit);
}
void clear_bank_bit(uint8_t* GPIO_bank, uint8_t bit){
  GPIO_bank[0] = GPIO_bank[0] & (0xFF ^ (1 << bit) );
}
void toggle_bit(uint8_t *BANK, uint8_t bit){
    if ((BANK[0] & (1<<bit)) >> bit){
      clear_bank_bit(BANK, bit);
    } else {
      set_bank_bit(BANK, bit);
    }
}

void print_attenuator_state(uint8_t* GPIO_bank){
  Serial.print("16 to 0.5 dB: ");
  for (int i=5; i>=0; i--){
    Serial.print((GPIO_bank[0] & 1<<i) >> i, BIN);
  }
  Serial.println("");
}

void scanner(TwoWire *I2C) {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    I2C->beginTransmission(address);
    error = I2C->endTransmission();

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

void setup_si5351(void){
  si5351.init(SI5351_CRYSTAL_LOAD_10PF, Si_5351_crystal, freqCorrectionFactor);
  pll_freq = freq * multiple;
  
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);  //AFP 09-24-23 V12
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);  //AFP 09-24-23 V12
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_2MA);  //AFP 09-24-23 V12
  
  si5351.pll_reset(SI5351_PLLA);  //AFP 09-24-23 V12
  si5351.pll_reset(SI5351_PLLB);  //AFP 09-24-23 V12
}

int EvenDivisor(long freq2) {
  //freq2=freq2/100;
 // Serial.print("freq2= ");
  //Serial.println(freq2);
  if (freq2 < 6850000)
    multiple = 126;

  if ((freq2 >= 6850000) && (freq2 < 9500000))
    multiple = 88;

  if ((freq2 >= 9500000) && (freq2 < 13600000))
    multiple = 64;

  if ((freq2 >= 13600000) && (freq2 < 17500000))
    multiple = 44;

  if ((freq2 >= 17500000) && (freq2 < 25000000))
    multiple = 34;

  if ((freq2 >= 25000000) && (freq2 < 36000000))
    multiple = 24;

  if ((freq2 >= 36000000) && (freq2 < 45000000))
    multiple = 18;

  if ((freq2 >= 45000000) && (freq2 < 60000000))
    multiple = 14;

  if ((freq2 >= 60000000) && (freq2 < 80000000))
    multiple = 10;

  if ((freq2 >= 80000000) && (freq2 < 100000000))
    multiple = 8;

  if ((freq2 >= 100000000) && (freq2 < 146600000))
    multiple = 6;

  if ((freq2 >= 150000000) && (freq2 < 220000000))
    multiple = 4;
  return multiple;
}

void SetFreq() {  //AFP 09-24-23 V12
    Clk1SetFreq = ((TxRxFreq * SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT);
    multiple = EvenDivisor(Clk1SetFreq / SI5351_FREQ_MULT);

    pll_freq = Clk1SetFreq * multiple;
    freq = pll_freq / multiple;
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.output_enable(SI5351_CLK1, 1);
    si5351.output_enable(SI5351_CLK2, 0);
    si5351.set_freq_manual(freq, pll_freq, SI5351_CLK0);
    si5351.set_freq_manual(freq, pll_freq, SI5351_CLK1);

    si5351.set_phase(SI5351_CLK0, 0);
    si5351.set_phase(SI5351_CLK1, multiple);

}

void SetCW() {  //AFP 09-24-23 V12
    Clk1SetFreq = (CWFreq * SI5351_FREQ_MULT);
    multiple = EvenDivisor(Clk1SetFreq / SI5351_FREQ_MULT);

    pll_freq = Clk1SetFreq * multiple;
    freq = pll_freq / multiple;
    si5351.output_enable(SI5351_CLK0, 0);
    si5351.output_enable(SI5351_CLK1, 0);
    si5351.output_enable(SI5351_CLK2, 1);
    si5351.set_freq_manual(freq, pll_freq, SI5351_CLK2);
    si5351.set_phase(SI5351_CLK2, 0);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);

  Wire.begin();
  while (!mcpRF.begin_I2C(RF_BOARD_MCP23017_ADDR,&Wire)){
    Serial.println("RF MCP23017 not found at 0x"+String(RF_BOARD_MCP23017_ADDR,HEX));
    scanner(&Wire);
    delay(1000);
  }
  mcpRF.enableAddrPins();
  // Set all pins to be outputs
  for (int i=0;i<16;i++){
    mcpRF.pinMode(i, OUTPUT);
  }

  // Set all pins to zero. This means no attenuation and in HF mode
  GPA_state = 0x00;
  GPB_state = 0x00;
  mcpRF.writeGPIOA(GPA_state); 
  mcpRF.writeGPIOB(GPB_state); 

  // GPIO2 is mapped to TX pin:
  // HIGH means TX mode
  // LOW means RX mode
  pinMode(RXTX, OUTPUT);
  TXRX_state = RXSTATE;
  digitalWrite(RXTX, TXRX_state);  // RX mode

  pinMode(XMIT_MODE, OUTPUT);
  transmit_mode_state = XIQSTATE;
  digitalWrite(XMIT_MODE, transmit_mode_state);  // IQ mode

  pinMode(CW_ON_OFF, OUTPUT);
  cw_state = CWOFFSTATE;
  digitalWrite(CW_ON_OFF, cw_state);  // CW off

  pinMode(CAL, OUTPUT);
  cal_state = CALOFFSTATE;
  digitalWrite(CAL, cal_state);  // Calibration off

  /* Initialise the LO */
  setup_si5351();
  si5351.set_freq_manual(1ULL, 10000000, SI5351_CLK2);  //AFP 09-24-23 V12
  si5351.output_enable(SI5351_CLK2, 0);                  //AFP 09-24-23 V12

  TxRxFreq = 10000000;
  IFFreq = 0;
  SetFreq();
}

void print_state(void){
  Serial.println("RX attenuator: ");
  print_attenuator_state(&GPA_state);
  Serial.println("TX attenuator: ");
  print_attenuator_state(&GPB_state);
  
  if ((GPA_state & 0b10000000) >> 7){
    Serial.println("HF/MF selection: MF");
  } else {
    Serial.println("HF/MF selection: HF");
  }

  if (TXRX_state == TXSTATE) {
    Serial.println("TX/RX state: Transmit");
  } else {
    Serial.println("TX/RX state: Receive");
  }

  if (transmit_mode_state == XIQSTATE) {
    Serial.println("TX mode state: IQ");
  } else {
    Serial.println("TX mode state: CW");
  }

  if (cw_state == CWOFFSTATE) {
    Serial.println("CW state: OFF");
  } else {
    Serial.println("CW state: ON");
  }

  if (cal_state == CALOFFSTATE) {
    Serial.println("CAL state: OFF");
  } else {
    Serial.println("CAL state: ON");
  }
}

void loop() {
  // print the state and selection menu
  Serial.println("Current state:");
  print_state();
  Serial.println("------------------------------");
  Serial.println("Select option and hit enter:");
  Serial.println("Change to TX/RX mode:      [TX,RX]");
  Serial.println("Change to TX IQ/CW mode:   [IQ,CW]");
  Serial.println("Turn CW on/off:            [CWON,CWOFF]");
  Serial.println("Toggle CW rapidly:         CWTOG");
  Serial.println("Turn CAL on/off:           [CALON,CALOFF]");
  Serial.println("Change to MF/HF mode:      [MF,HF]");
  Serial.println("Toggle RX att bit 0-5:     RX0 to RX5");
  Serial.println("Toggle TX att bit 0-5:     TX0 to TX5");
  Serial.println("------------------------------");

  while (Serial.available() == 0) {}     //wait for data available
  String selection = Serial.readString();  //read until timeout
  selection.trim(); // remove any \r \n whitespace at the end of the String
  if (selection == "TX"){
      Serial.println("Change to TX mode");
      TXRX_state = TXSTATE;
      digitalWrite(RXTX, TXRX_state);
  } 
  if (selection == "RX"){
    Serial.println("Change to RX mode");
    TXRX_state = RXSTATE;
    digitalWrite(RXTX, TXRX_state);
  }
  if (selection == "IQ"){
    Serial.println("Change to IQ mode");
    transmit_mode_state = XIQSTATE;
    SetFreq();
    digitalWrite(XMIT_MODE, transmit_mode_state);
  }
  if (selection == "CW"){
    Serial.println("Change to CW mode");
    transmit_mode_state = XCWSTATE;
    SetCW();
    digitalWrite(XMIT_MODE, transmit_mode_state);
  }
  if (selection == "CWON"){
    Serial.println("Turn CW on");
    cw_state = CWONSTATE;
    digitalWrite(CW_ON_OFF, cw_state);
  }
  if (selection == "CWOFF"){
    Serial.println("Turn CW off");
    cw_state = CWOFFSTATE;
    digitalWrite(CW_ON_OFF, cw_state);
  }
  if (selection == "CWTOG"){
    Serial.println("Toggling CW");
    // 100 ms on, 100ms off.
    for (int i=0; i<5*60; i++){
      cw_state = CWONSTATE;
      digitalWrite(CW_ON_OFF, cw_state);
      delay(100);
      cw_state = CWOFFSTATE;
      digitalWrite(CW_ON_OFF, cw_state);
      delay(100);
    }
  }
  
  if (selection == "CALON"){
    cal_state = CALONSTATE;
    digitalWrite(CAL, cal_state);
  }
  if (selection == "CALOFF"){
    cal_state = CALOFFSTATE;
    digitalWrite(CAL, cal_state);
  }
  if (selection == "MF"){
    Serial.println("Change to MF mode");
    set_bank_bit(&GPA_state, 7);
  }
  if (selection == "HF"){
    Serial.println("Change to HF mode");
    clear_bank_bit(&GPA_state, 7);
  }

  if (selection == "RX0"){
    toggle_bit(&GPA_state, 0);
  }
  if (selection == "RX1"){
    toggle_bit(&GPA_state, 1);
  }
  if (selection == "RX2"){
    toggle_bit(&GPA_state, 2);
  }
  if (selection == "RX3"){
    toggle_bit(&GPA_state, 3);
  }
  if (selection == "RX4"){
    toggle_bit(&GPA_state, 4);
  }
  if (selection == "RX5"){
    toggle_bit(&GPA_state, 5);
  }
  
  if (selection == "TX0"){
    toggle_bit(&GPB_state, 0);
  }
  if (selection == "TX1"){
    toggle_bit(&GPB_state, 1);
  }
  if (selection == "TX2"){
    toggle_bit(&GPB_state, 2);
  }
  if (selection == "TX3"){
    toggle_bit(&GPB_state, 3);
  }
  if (selection == "TX4"){
    toggle_bit(&GPB_state, 4);
  }
  if (selection == "TX5"){
    toggle_bit(&GPB_state, 5);
  }

  mcpRF.writeGPIOB(GPB_state); 
  mcpRF.writeGPIOA(GPA_state); 

  Serial.print("Written to GPA: ");
  Serial.println(GPA_state, BIN);
  Serial.print("Written to GPB: ");
  Serial.println(GPB_state, BIN);

  Serial.println("------------------------------");

}
