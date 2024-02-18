# i2cscan

Modified version for test T41-EP

Scans Wire, Wire1 and Wire2 and initializes Audio Board so that the boards I2C address is seen.

If just the Audio Board and si5351 are installed then you should see:

```
14:27:19.730 -> Scanning Wire ...
14:27:19.730 -> I2C device found at address 0x0A  !
14:27:19.730 -> I2C device found at address 0x60  !
14:27:19.730 -> done
14:27:19.730 -> 
14:27:19.730 -> Scanning Wire1 ...
14:27:19.839 -> No I2C devices found
14:27:19.839 -> 
14:27:19.839 -> Scanning Wire2 ...
14:27:19.839 -> No I2C devices found
```

The Audio board is at address 0x0A and the si5351 at 0x60.
