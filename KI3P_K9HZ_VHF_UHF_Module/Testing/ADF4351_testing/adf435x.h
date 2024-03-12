#ifndef ADF4351_H
#define ADF4351_H

#include <math.h>
#include <stdio.h>
#include <SPI.h>
#include <Arduino.h>

#define TEENSY41 1
//#define ESP32 1

#ifdef TEENSY41
#define PIN_CE    24  //CE: T41 Pin for Chip Enable
#define PIN_LD    25  //LD: T41 Pin for Lock Detect
#define PIN_SS    10  //LE:  T41 Pin for SPI Slave Select
#define PIN_MOSI  11  //DAT: T41 Pin for SPI MOSI
#define PIN_MISO  12  //N/C: T41 Pin for SPI MISO
#define PIN_SCK   13  //CLK: T41 Pin for SPI CLK
#endif

#ifdef ESP32
#define PIN_CE   4    ///< Ard Pin for Chip Enable
#define PIN_LD   2    ///< Ard Pin for Lock Detect
#define PIN_SS   5    ///< Ard Pin for SPI Slave Select
#define PIN_MOSI  23  ///< Ard Pin for SPI MOSI
#define PIN_MISO  19  ///< Ard Pin for SPI MISO
#define PIN_SCK  18   ///< Ard Pin for SPI CLK
#endif

typedef enum {
	ADF4350 = 0,
	ADF4351 = 1
} DeviceType;

typedef enum {
	LowNoiseMode = 0,
	LowSpurMode = 1
} LowNoiseSpurMode;

typedef enum {
    ThreeState = 0,
    DVdd = 1,
    DGND = 2,
    RCounterOuput = 3,
    NDividerOutput = 4,
    AnalogLockDetect = 5,
    DigitalLockDetect = 6
} MuxOut;

typedef enum {
    Positive = 1,
    Negative = 0
} PDPolarity;

typedef enum {
    BandSelectClockLow = 0,
    BandSelectClockHigh = 1
} BandSelectClockMode;

typedef enum {
    ClockDividerOff = 0,
    FastLockEnable = 1,
    ResyncEnable = 2
} ClkDivMode;

typedef enum {
    Fundamental = 1,
    Divider = 0
} FeedbackSelect;

typedef enum {
    DividedOutput = 0,
    FundamentalOutput = 1
} AuxOutputSelect;

typedef enum {
    LDPinModeLow=0,
    LDPDigitalLockDetect=1,
    LDPinModeHigh=3
} LDPinMode;


int check_uint_val(char* name, int val, int max_val, int min_val);
int check_lookup_val(char* name, int val, int* lookup_table);
int get_index(int* lookup_table, int val);
int gcd(int a, int b);
void calculate_regs(
        DeviceType device_type,
        double freq,
        double ref_freq,
        int r_counter,
        bool ref_doubler,
        bool ref_div2,
        FeedbackSelect feedback_select,
        BandSelectClockMode band_select_clock_mode,
        // The output parameters
        int *INT, 
        int *MOD, 
        int *FRAC, 
        int *output_divider,
        int *band_select_clock_divider);
int make_regs(
        DeviceType device_type,
        int INT,
        int FRAC,
        int MOD,
        int phase_value,
        int band_select_clock_divider,
        BandSelectClockMode band_select_clock_mode,
        bool prescaler_4_5,
        LowNoiseSpurMode low_noise_spur_mode,
        MuxOut mux_out,
        bool ref_doubler,
        bool ref_div2,
        int r_counter,
        bool double_buff_r4,
        int charge_pump_current_x100,
        int ldp,
        PDPolarity pd_polarity,
        bool powerdown,
        bool cp_three_state,
        bool counter_reset,
        int abp,
        bool charge_cancel,
        bool csr,
        ClkDivMode clk_div_mode,
        int clock_divider_value,
        FeedbackSelect feedback_select,
        int output_divider,
        bool vco_powerdown,
        bool mute_till_lock_detect,
        AuxOutputSelect aux_output_select,
        bool aux_output_enable,
        int aux_output_power,
        bool output_disable,
        int output_power,
        LDPinMode ld_pin_mode,
        // The output registers
        uint32_t *regs);
int freq_make_regs(double freq_MHz, double ref_freq_MHz, uint32_t *regs, int output_power);
void WriteRegs(uint32_t *regs);

#endif //ADF4351_H
