/*
 * Rotary encoder library for Arduino.
 */

#if defined(G0ORX_FRONTPANEL)

#ifndef G0ORX_ROTARY_H
#define G0ORX_ROTARY_H

/*
 * Note: BOURN encoders have their A/B pins reversed compared to cheaper encoders.
 */

#define BOURN_ENCODERS

// Enable weak pullups
#define ENABLE_PULLUPS


// Enable this to emit codes twice per step.
//#define HALF_STEP

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0
// Clockwise step.
#define DIR_CW 1
// Counter-clockwise step.
#define DIR_CCW 2


class G0ORX_Rotary
{
  public:
    G0ORX_Rotary();
    void updateA(unsigned char aState);
    void updateB(unsigned char bState);
    int process();

  private:
    int aLastState;
    int bLastState;
    int value;
};

#endif
#endif
