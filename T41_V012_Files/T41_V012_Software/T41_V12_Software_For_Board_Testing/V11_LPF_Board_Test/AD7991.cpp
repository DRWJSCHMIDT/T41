/*
  Arduino library for AD7991, a 4-Channel, 12-Bit ADC with I2C Compatible Interface in 8-Lead SOT-23 
 */

#include "AD7991.h"


/**
 * @brief Construct a new AD7991 object.
 *
 * Initializes the i2c_dev member variable to nullptr.
 */
AD7991::AD7991() { i2c_dev = nullptr; }

/**
 * @brief Initialize the AD7991 chip.
 *
 * @param i2c_addr I2C address of the AD7991 chip.
 * @param theWire Pointer to the TwoWire object.
 * @return true if initialization is successful, false otherwise.
 */
bool AD7991::begin(uint8_t i2c_addr, TwoWire *theWire) {
  if (i2c_dev) {
    delete i2c_dev;
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);

  return i2c_dev->begin();
}

/**
 * @brief Read ADC value in single-ended mode.
 *
 * @param ch Channel to read from (0-2).
 * @return ADC value or 0 if read failed.
 */
uint16_t AD7991::readADCsingle(uint8_t ch) {
  if (ch > 2) {
    return 0; // Invalid channel
  }

  uint8_t commandByte = 0;

  // Command byte is:
  // D7 D6 D5 D4 D3 D2 D1 D0
  // D4=ch0, D5=ch1, D6=Ch2

  if (ch==0) {
    commandByte = REGISTER_SETUP | (0b0001 << 4);
  }
  if (ch==1) {
    commandByte = REGISTER_SETUP | (0b0010 << 4);
  }
  if (ch==2) {
    commandByte = REGISTER_SETUP | (0b0100 << 4);
  }
  
  uint8_t adcValue[2];
  if (!i2c_dev->write_then_read(&commandByte, 1, adcValue, 2)) {
    return -1; // Write-then-read failed
  }

  // Confirm that the returned channel identifier matches what we specified
  // bits 5 and 4 of the first returned byte are the channel idenfier
  uint8_t CHid = (0b00110000 & adcValue[0]) >> 4;
  if (CHid != ch){
    Serial.print("Wrong channel! Expected ");
    Serial.print(ch,DEC);
    Serial.print(". Received ");
    Serial.println(CHid,DEC);
    return -2; // wrong channel returned
  }
  return ((0b00001111 & adcValue[0]) << 8) | adcValue[1];
}
