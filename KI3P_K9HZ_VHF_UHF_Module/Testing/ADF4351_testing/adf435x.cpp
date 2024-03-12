#include "adf435x.h"

//#define DEBUG

int check_uint_val(char* name, int val, int max_val, int min_val) {
    if (val < min_val || val > max_val) {
        printf("%s must be between %d and %d\n", name, min_val, max_val);
        return 1;
    }
    return 0;
}

int check_lookup_val(char* name, int val, int* lookup_table, int N) {
    int i;
    for (i = 0; i < N; i++) {
        if (lookup_table[i] == val) {
            return 0;
        }
    }
    printf("%s is not a valid value. Selected %d\n", name, val);
    return 1;
}

int get_index(int* lookup_table, int val, int N){
    int i;
    for (i = 0; i < N; i++) {
        if (lookup_table[i] == val) {
            return i;
        }
    }
    return -1;
}

int gcd(int a, int b) {
    while (1) {
        if (a == 0) {
            return b;
        } else if (b == 0) {
            return a;
        } else if (a > b) {
            a = a % b;
        } else {
            b = b % a;
        }
    }
}

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
        int *band_select_clock_divider) 
        {
    double fMAX = 4500.0; // 4400
    double fMIN;
    double pfd_freq;
    double N;
    int div;
    double pfd_scale;
    double band_select_clock_freq;

    if (device_type == ADF4351) {
        fMIN = 33.0; // 34.375
    } else {
        fMIN = 137.5;
    }

    if (freq < fMIN || freq > fMAX) {
        printf("freq = %.2f MHz outside of valid range [%.2f...%.2f] MHz)\n", freq, fMIN, fMAX);
        return;
    }

    int m;
    int n;
    if (ref_doubler) { m = 2;} else {m = 1;}
    if (ref_div2) { n = 2;} else {n = 1;}
    pfd_freq = ((ref_freq * m) / (n * r_counter)); // 0.1

    for (int log2_output_divider = 0; log2_output_divider < 7; log2_output_divider++) {
        *output_divider = pow(2, log2_output_divider);
        if (2200.0 / *output_divider <= freq) {
            break;
        }
    } // output_divider = 32

    if (feedback_select == Fundamental) {
        N = freq * *output_divider / pfd_freq;
    } else {
        N = freq / pfd_freq;
    } // N = 35200

    *INT = (int)floor(N); // 35200
    *MOD = (int)round(1000.0 * pfd_freq); // 100
    *FRAC = (int)round((N - *INT) * *MOD); // 0

    div = gcd(*MOD, *FRAC); // 100
    *MOD = *MOD / div; // 1
    *FRAC = *FRAC / div; // 0

    if (*MOD == 1) {
        *MOD = 2;
    }

    if (pfd_freq > 32.0) {
        if (*FRAC != 0) {
            printf("Maximum PFD frequency in Frac-N mode (FRAC != 0) is 32 MHz.\n");
            return;
        }

        if (*FRAC == 0 && device_type == ADF4351) {
            if (pfd_freq > 90.0) {
                printf("Maximum PFD frequency in Int-N mode (FRAC = 0) is 90 MHz.\n");
                return;
            }

            if (band_select_clock_mode == BandSelectClockLow) {
                printf("Band Select Clock Mode must be set to High when PFD is >32 MHz in Int-N mode (FRAC = 0).\n");
                return;
            }
        }
    }

    if (*band_select_clock_divider == 0) {
        pfd_scale = (band_select_clock_mode == BandSelectClockLow) ? 8 : 2;

        if (band_select_clock_mode == BandSelectClockLow) {
            *band_select_clock_divider = ceil(8 * pfd_freq);
            *band_select_clock_divider = *band_select_clock_divider > 255 ? 255 : *band_select_clock_divider;
        }
    } // band_select_clock_divider = 255 at most

    band_select_clock_freq = 1000.0 * pfd_freq / *band_select_clock_divider; // max 90000

    if (band_select_clock_freq > 500.0) {
        printf("Band Select Clock Frequency is too High. It must be 500 kHz or less.\n");
        return;
    } else if (band_select_clock_freq > 125.0) {
        if (device_type == ADF4351) {
            if (band_select_clock_mode == BandSelectClockLow) {
                printf("Band Select Clock Frequency is too high. Reduce to 125 kHz or less, or set Band Select Clock Mode to High.\n");
                return;
            }
        } else {
            printf("Band Select Clock Frequency is too high. Reduce to 125 kHz or less.\n");
            return;
        }
    }

    #ifdef DEBUG
    printf("(%d, %d, %d, %d, %d)\n", *INT, *MOD, *FRAC, *output_divider, *band_select_clock_divider);
    #endif
}

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
        uint32_t *regs) {
    int ChargePumpCurrent_x100[] = {31, 63, 94, 125,
            156, 188, 219, 250,
            281, 313, 344, 375,
            406, 438, 449, 500}; // x100 to make it an int
    int ABP[] = {10, 6};
    int OutputPower[] = {-4, -1, 2, 5};
    if (check_uint_val("INT", INT, 65535, (prescaler_4_5 ? 23 : 75))) return 1;
    if (check_uint_val("MOD", MOD, 4095, 2)) return 1;
    if (check_uint_val("FRAC", FRAC, MOD-1, 0)) return 1;
    if (check_lookup_val("charge_pump_current", charge_pump_current_x100,
            ChargePumpCurrent_x100, sizeof(ChargePumpCurrent_x100))) return 1;
    if (check_lookup_val("abp", abp, ABP, sizeof(ABP))) return 1;
    if (check_lookup_val("aux_output_power", aux_output_power, OutputPower,sizeof(OutputPower))) return 1;
    if (check_lookup_val("output_power", output_power, OutputPower,sizeof(OutputPower))) return 1;
    int output_divider_select = log(output_divider) / log(2);
    if (output_divider_select < 0 || output_divider_select > 64 ||
            floor(output_divider_select) != output_divider_select) {
        printf("Output Divider must be a positive integer power of 2, not greater than 64.\n");
        exit(1);
    }
    
    regs[0] = (INT << 15 |
        FRAC << 3 |
        0x0);
    
    regs[1] = ((phase_value >= 0 ? 1 : 0) << 28 |
        (prescaler_4_5 ? 0 : 1) << 27 |
        (phase_value < 0 ? 1 : phase_value) << 15 |
        MOD << 3 |
        0x1);
    if (device_type == ADF4351) {
        regs[1] |= (phase_value >= 0 ? 1 : 0) << 28;
    }
    
    regs[2] = (low_noise_spur_mode << 29 |
        mux_out << 26 |
        (ref_doubler ? 1 : 0) << 25 |
        (ref_div2 ? 1 : 0) << 24 |
        r_counter << 14 |
        (double_buff_r4 ? 1 : 0) << 13 |
        get_index(ChargePumpCurrent_x100,charge_pump_current_x100,sizeof(ChargePumpCurrent_x100)) << 9 |
        (FRAC == 0 ? 0 : 1)  << 8 |
        (ldp == 10 ? 0 : 1) << 7 |
        pd_polarity << 6 |
        (powerdown ? 1 : 0) << 5 |
        (cp_three_state ? 1 : 0) << 4 |
        (counter_reset ? 1 : 0) << 3 |
        0x2);
    
    regs[3] = (((csr ? 1 : 0) << 18) |
        (clk_div_mode << 15) |
        (clock_divider_value << 3) |
        0x3);
    if (device_type == ADF4351) {
        regs[3] |= ((band_select_clock_mode << 23 |
            get_index(ABP,abp,sizeof(ABP)) << 22 |
            (charge_cancel ? 1 : 0) << 21));
    }
    
    regs[4] = (feedback_select << 23 |
        output_divider_select << 20 |
        band_select_clock_divider << 12 |
        (vco_powerdown ? 1 : 0) << 11 |
        (mute_till_lock_detect ? 1 : 0) << 10 |
        aux_output_select << 9 |
        (aux_output_enable ? 1 : 0) << 8 |
        get_index(OutputPower,aux_output_power,sizeof(OutputPower)) << 6 |
        (output_disable ? 0 : 1) << 5 |
        get_index(OutputPower,output_power,sizeof(OutputPower)) << 3 |
        0x4);
    
    regs[5] = (ld_pin_mode << 22 |
        3 << 19 |
        0x5);
    return 0;
}

int freq_make_regs(double freq_MHz, double ref_freq_MHz, uint32_t *regs, int output_power){
	int INT = 0; 
  int MOD = 0;
  int FRAC = 0; 
  int output_divider = 0;
  int band_select_clock_divider = 0; 
	
	calculate_regs(
        	ADF4351, // DeviceType
        	freq_MHz, // Frequency (MHz)
        	ref_freq_MHz, // ref_freq
        	250, // r_counter
        	false, // ref_doubler=False
        	false, // ref_div2=False
        	Fundamental, // FeedbackSelect
        	BandSelectClockLow, // BandSelectClockMode
        	// The output parameters
	        &INT,
        	&MOD,
	        &FRAC, 
	        &output_divider,
	        &band_select_clock_divider); 
  //Serial.println("Calculated values:");
  //Serial.print("")

  return make_regs(
        	ADF4351, // device_type
        	INT,
        	FRAC,
        	MOD,
        	-1, // phase_value (-1 means NULL),
        	band_select_clock_divider, // band_select_clock_divider,
        	BandSelectClockLow, // BandSelectClockMode
        	false, // prescaler_4_5 = False
        	LowNoiseMode, // low_noise_spur_mode,
	        DigitalLockDetect, // MuxOut 
        	false, // ref_doubler=False,
        	false, // ref_div2=False,
        	250, // r_counter,
        	false, // double_buff_r4=False,
        	250, // charge_pump_current x100,
        	10, // ldp,
        	Positive, // PDPolarity
        	false, // powerdown=False,
        	false, // cp_three_state=False,
        	false, // counter_reset=False,
        	10, // abp,
        	false, // charge_cancel=False,
        	false, // csr=False,
        	ClockDividerOff, // ClkDivMode
	        150, // clock_divider_value,
        	Fundamental, // FeedbackSelect,
        	output_divider,
        	false, // vco_powerdown=False,
        	false, // mute_till_lock_detect=False,
        	DividedOutput, // AuxOutputSelect
        	false, // aux_output_enable=False,
        	-4, // aux_output_power,
        	false, // output_disable=False,
        	output_power, // output_power,
        	LDPDigitalLockDetect, // LDPinMode 
        	regs);

}

void WriteRegs(uint32_t *regs) {
  SPI.begin();
  for (int i = 5 ; i >= 0 ; i--) { // sequence according to the ADF4351 datasheet

    //SPI.beginTransaction(ADF4351_SPI);
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    digitalWrite(PIN_SS, LOW);
    delayMicroseconds(1);
    for (int j=3; j>=0; j--){
      SPI.transfer((regs[i] >> 8*j) & 0xFF);
    }
    //SPI.transfer(regs[i]);
    //BeyondByte.writeDword(0, ADF4351_R[i], 4, BeyondByte_SPI, MSBFIRST);
    delayMicroseconds(1);
    digitalWrite(PIN_SS, HIGH);
    SPI.endTransaction();
    delayMicroseconds(1);
  }
}
