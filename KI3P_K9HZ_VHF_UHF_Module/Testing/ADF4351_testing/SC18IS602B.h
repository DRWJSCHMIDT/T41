/*
 * SC18IS602B.h
 *
 * Library for interfacing with the NXP SC18IS602B I2C to SPI bridge
 * 
 * Derived from:
 * https://github.com/maxgerhardt/SC18IS602B-arduino
 *
 */

#ifndef SRC_SC18IS602B_H_
#define SRC_SC18IS602B_H_

#include <stddef.h>
#include <stdint.h>
#include <Wire.h>

/* Describes the pin modes a pin of the SC18IS602B can be in */
enum SC18IS601B_GPIOPinMode {
    SC18IS601B_GPIO_MODE_QUASI_BIDIRECTIONAL    = 0B00,
    SC18IS601B_GPIO_MODE_PUSH_PULL              = 0B01,
    SC18IS601B_GPIO_MODE_INPUT_ONLY             = 0B10,
    SC18IS601B_GPIO_MODE_OPEN_DRAIN             = 0B11
};

/* Describes the possible SPI speeds */
enum SC18IS601B_SPI_Speed {
    SC18IS601B_SPICLK_1843_kHz      = 0B00,     /* 1.8 MBit/s */
    SC18IS601B_SPICLK_461_kHz       = 0B01,     /* 461 kbit/s */
    SC18IS601B_SPICLK_115_kHz       = 0B10,     /* 115 kbit/s */
    SC18IS601B_SPICLK_58_kHz        = 0B11      /* 58 kbit/s */
};

/* Describes the possible SPI modes */
enum SC18IS601B_SPI_Mode {
    SC18IS601B_SPIMODE_0 = 0B00,        /* CPOL: 0  CPHA: 0 */
    SC18IS601B_SPIMODE_1 = 0B01,        /* CPOL: 0  CPHA: 1 */
    SC18IS601B_SPIMODE_2 = 0B10,        /* CPOL: 1  CPHA: 0 */
    SC18IS601B_SPIMODE_3 = 0B11         /* CPOL: 1  CPHA: 1 */
};

/* Function IDs */
#define SC18IS601B_CONFIG_SPI_CMD           0xF0
#define SC18IS601B_CLEAR_INTERRUPT_CMD      0xF1
#define SC18IS601B_IDLE_CMD                 0xF2
#define SC18IS601B_GPIO_WRITE_CMD           0xF4
#define SC18IS601B_GPIO_READ_CMD            0xF5
#define SC18IS601B_GPIO_ENABLE_CMD          0xF6
#define SC18IS601B_GPIO_CONFIGURATION_CMD   0xF7

#define SC18IS601B_DATABUFFER_DEPTH         200

class SC18IS602B {
public:
    /* By default instantiate it with no attached RESET pin and a0 to a2 to GND */
    SC18IS602B() : SC18IS602B(-1, 0, 0, 0) {};

    /* if resetPin = -1 it will be ignored */
    SC18IS602B(int resetPin, bool a0, bool a1, bool a2);

    /* pass the initializer a TwoWire object which has already had
     * its begin() function called. Check to see if I2C object at
     * address is present. Return 0 if I2C is present. Return !=0 
     * if I2C not found or other error */
    int begin(TwoWire *wire);

    /* reset by pulsing RESET */
    void reset();

    /* Enables or disables GPIO functionality for a slave select pin */
    bool enableGPIO(int num, bool enable);

    /* Sets up a specific GPIO in some mode */
    bool setupGPIO(int num, SC18IS601B_GPIOPinMode mode);

    /* Writes a value to an output pin */
    bool writeGPIO(int num, bool value);

    /* Writes a 4-bit value to the entire GPIO bank (GPIO3 to GPIO0) */
    bool writeGPIOBank(uint8_t value);

    /* Same function as above with just all values split up */
    bool writeGPIOBank(bool gpio0, bool gpio1, bool gpio2, bool gpio3);

    /* Reads GPIO value */
    bool readGPIO(int num);

    /* Sets the chip into low power mode */
    bool setLowPowerMode();

    /* Clears the INT pin asserted HIGH after every SPI transfer */
    bool clearInterrupt();

    /* SPI functions. spiMode should be SPI_MODE0 to SPI_MODE3 */
    bool configureSPI(bool lsbFirst, SC18IS601B_SPI_Mode spiMode, SC18IS601B_SPI_Speed clockSpeed);

    /* Executes a SPI transfer.
     * Slave select number `slaveNum` will be used.
     * The txLen bytes from the txData buffer will be sent,
     * then txLen bytes will be read into readBuf.
     * Returns success (true/false).
     * */
    bool spiTransfer(int slaveNum, const uint8_t* txData, size_t txLen, uint8_t* readBuf);
    /* Multi-byte send without read */
    bool spiTransfer(int slaveNum, const uint8_t* txData, size_t txLen);

    int getAddress();

    /* Transfers a single byte to a slave. returns the read value. */
    uint8_t spiTransfer(int slaveNum, uint8_t txByte);

    virtual ~SC18IS602B();
private:

    bool i2c_write(uint8_t cmdByte, const uint8_t* data, size_t len);
    size_t i2c_read(uint8_t* readBuf, size_t len);
    TwoWire *wire;
    int resetPin = -1;      /* reset pin */
    int address = 0;        /* address of the module */
    uint8_t gpioEnable = 0; /* last value for GPIO enable */
    uint8_t gpioConfig = 0; /* last value for GPIO configuration */
    uint8_t gpioWrite = 0;  /* last value for GPIO write */
};

#endif /* SRC_SC18IS602B_H_ */