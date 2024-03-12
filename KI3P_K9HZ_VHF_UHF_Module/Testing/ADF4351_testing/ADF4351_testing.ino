#include "adf435x.h"
#include <Wire.h>
#include "SC18IS602B.h"

// Uncomment below if using the I2C bridge. Comment if using the SPI pins
#define I2CBRIDGE

// Wire1 I2C bus: pins 17,16 for SDA,SCL
// SPI bus: pin 25 to LD:   T41 Pin for Lock Detect
// SPI bus: pin 10 to EN:   T41 Pin for SPI Slave Select
// SPI bus: pin 11 to MOSI: T41 Pin for SPI MOSI
// SPI bus: pin 13 to CLK:  T41 Pin for SPI CLK

#ifdef I2CBRIDGE
// a0, a1, a2 are connected to GND. No reset pin.
SC18IS602B LOspiBridge(-1,0,0,0);

uint8_t LO_spi_transfer(uint32_t *regs) {
  uint8_t ret;
  uint8_t txData[4];
  for (int i = 5 ; i >= 0 ; i--) { // sequence according to the ADF4351 datasheet
    // Pin LE, which is connected to ~(SSO), needs to be pulled low before SPI write
    // I2C chip does this automatically
    for (int j=3; j>=0; j--){
      txData[3-j] = (regs[i] >> 8*j) & 0xFF;
    }
    ret = LOspiBridge.spiTransfer(0, txData, 4);
    delayMicroseconds(1); 
  }
	return ret;
}
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  #ifdef I2CBRIDGE  
  Wire1.setClock(100000UL);
  Wire1.setSDA(17);
  Wire1.setSCL(16);
  Wire1.begin();
  
  LOspiBridge.begin(&Wire1);
  // lsbFirst, spiMode, clockSpeed
	LOspiBridge.configureSPI(false, SC18IS601B_SPIMODE_0, SC18IS601B_SPICLK_1843_kHz);

  #else
  // Configure the SPI control of the ADF4351
  pinMode(PIN_SS, OUTPUT);
  pinMode(PIN_CE, OUTPUT);
  pinMode(PIN_LD, INPUT);
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("Enter the desired frequency in kHz: ");
  while (Serial.available() == 0) {}     //wait for data available
  String selection = Serial.readString();  //read until timeout
  selection.trim();                        // remove any \r \n whitespace at the end of the String
  
  int freq_kHz = selection.toInt();
  double freq_MHz = (float) freq_kHz / 1000.0;
	Serial.print("Output frequency [MHz]: ");
  Serial.println(freq_MHz,DEC);

  if ((freq_MHz > 33.0) & (freq_MHz < 4500.0)){
    uint32_t regs[6];
    #ifdef I2CBRIDGE
      if (!freq_make_regs(freq_MHz, 25.0, regs, -1)){
    	  // write these over I2C interface
        Serial.println("Writing to LO via I2C");
	      if (!LO_spi_transfer(regs)) Serial.println("I2C/SPI transfer failed");
      }
    #else
      if (!freq_make_regs(freq_MHz, 25.0, regs, -1)){
        Serial.println("Writing to LO via SPI");
        WriteRegs(regs);
        digitalWrite(PIN_CE,HIGH);
        delay(200);
        //if (digitalRead(PIN_LD)) Serial.println("OK");
        //else Serial.println("No lock!");
      }
    #endif
  } else {
    Serial.println("Invalid frequency! Must be between 33 and 4500 MHz.");
  }
}
