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
