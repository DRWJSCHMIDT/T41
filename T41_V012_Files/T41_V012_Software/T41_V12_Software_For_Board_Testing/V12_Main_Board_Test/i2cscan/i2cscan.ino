// ---------------------------------------------------------------- /
// Arduino I2C Scanner
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using GY-87 module for the target
// Tested on 10 September 2019
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//
// This code was modified to scan all three I2C busses in the Teensy,
// ---------------------------------------------------------------- /

#include <Wire.h> //include Wire.h library

TwoWire *_wire=&Wire;

#include <Audio.h>
AudioInputI2SQuad i2s_quadIn; // just need one Audio interface to get Audio Board initialized

//#define WAKE 15  // SHUTDOWN LOW, ACTIVE HIGH
//#define IRQ 16  // HIGH When interrupt

void identify_device(int i, byte address){
  if (i == 0){
    // Wire
    switch (address){
      case 0x0A:{
        Serial.print(": Teensy audio adapter / hat");
        break;
      }
      case 0x27:{
        Serial.print(": RF board MCP23017 for first receiver");
        break;
      }
      case 0x23:{
        Serial.print(": RF board MCP23017 for first receiver");
        break;
      }
      default:{
        if ((address >= 0x60) & (address <= 0x6F)){
          Serial.print(": RF board Si5351 oscillator");
        } else {
          Serial.print(": Unknown device!");
        }
        break;
      }
    }
  }
  if (i == 1){
    // Wire1
    switch (address){
      case 0x22:{
        Serial.print(": Front panel Pico communication");
        break;
      }
      case 0x20:{
        Serial.print(": Front panel MCP23017 for switches 1-16");
        break;
      }
      case 0x21:{
        Serial.print(": Front panel MCP23017 for switches 17-18, encoders");
        break;
      }
      default:{
        Serial.print(": Unknown device!");
        break;
      }
    }
  }
  if (i == 2){
    // Wire2
    switch (address){
      case 0x40:{
        Serial.print(": Touch screen controller");
        break;
      }
      case 0x38:{
        Serial.print(": Touch screen controller");
        break;
      }
      case 0x24:{
        Serial.print(": BPF MCP23017 for first receiver");
        break;
      }
      case 0x26:{
        Serial.print(": BPF MCP23017 for second receiver");
        break;
      }
      case 0x25:{
        Serial.print(": K9HZ LPF MCP23017");
        break;
      }
      case 0x28:{
        Serial.print(": K9HZ LPF AD7991 ADC");
        break;
      }
      case 0x29:{
        Serial.print(": K9HZ LPF AD7991 ADC");
        break;
      }
      default:{
        Serial.print(": Unknown device!");
        break;
      }
    }
  }
}

void setup()
{

  
  Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
  while (!Serial); // Waiting for Serial Monitor
  Serial.println("\nI2C Scanner");

  //Wire1.setSCL(27);
  //Wire1.setSDA(26);
}

void loop()
{
  byte error, address; //variable for error and I2C address
  int nDevices;

  for(int i=0;i<3;i++) {
    switch(i) {
      case 0:
        Serial.println("Scanning Wire ...");
        _wire=&Wire;
        break;
      case 1:
        Serial.println("Scanning Wire1 ...");
        _wire=&Wire1;
        break;
      case 2:
        Serial.println("Scanning Wire2 ...");
        _wire=&Wire2;
        break;
    }
    _wire->begin();
    nDevices = 0;
    for (address = 1; address < 127; address++ )
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      _wire->beginTransmission(address);
      error = _wire->endTransmission();

      if (error == 0)
      {
        Serial.print("I2C device found at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.print(address, HEX);
        identify_device(i,address);
        Serial.println("  !");
        nDevices++;
      }
      else if (error == 4)
      {
        Serial.print("Unknown error at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.println(address, HEX);
      }
    }
    if (nDevices == 0)
      Serial.println("No I2C devices found\n");
    else
      Serial.println("done\n");
  }
  
  delay(5000); // wait 5 seconds for the next I2C scan
}
