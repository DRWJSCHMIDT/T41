# i2cscan

Modified version for test T41-EP

Scans Wire, Wire1 and Wire2 to look for connected I2C devices. If all the boards are connected you should see the following output:

```
Scanning Wire ...
I2C device found at address 0x0A: Teensy audio adapter / hat  !
I2C device found at address 0x27: RF board MCP23017 for first receiver  !
I2C device found at address 0x60: RF board Si5351 oscillator  !
done

Scanning Wire1 ...
I2C device found at address 0x20: Front panel MCP23017 for switches 1-16  !
I2C device found at address 0x21: Front panel MCP23017 for switches 17-18, encoders  !
done

Scanning Wire2 ...
I2C device found at address 0x24: BPF MCP23017 for first receiver  !
I2C device found at address 0x25: K9HZ LPF MCP23017  !
I2C device found at address 0x28: K9HZ LPF AD7991 ADC  !
done
```
