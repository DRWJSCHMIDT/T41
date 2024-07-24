/* Rotary encoder handler for arduino.
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 * Modified by John Melton (G0ORX) to allow feeding A/B pin states
 * to allow working with MCP23017.
 *
 */

#ifndef BEENHERE
#include "SDT.h"
#endif

#if defined(G0ORX_FRONTPANEL)

static bool ccw_fall = 0;
static bool cw_fall = 0;
#ifdef BOURN_ENCODERS
static bool fwd = true;
#else
static bool fwd = false;
#endif

/*
unsigned char A;
unsigned char B;
unsigned char prev_state;
*/

/*
 * Constructor. Each arg is the pin number for each encoder contact.
 */
G0ORX_Rotary::G0ORX_Rotary () {
  cw_fall = false;
  ccw_fall = false;
}

FASTRUN
void G0ORX_Rotary::updateA(unsigned char state) {
  if ((!cw_fall) && (state==0b10))  // cw_fall is set to TRUE when phase A interrupt is triggered
    cw_fall = true;

  if (ccw_fall && (state == 0b00)) { // if ccw_fall is already set to true from a previous B phase trigger, the ccw event will be triggered
    cw_fall = false;
    ccw_fall = false;
    if (fwd) {
      value++;
    } else {
      value--;
    }
  }
}

FASTRUN
void G0ORX_Rotary::updateB(unsigned char state) {
  if ((!ccw_fall) && (state == 0b01))  //ccw leading edge is true
    ccw_fall = true;

  if (cw_fall && (state == 0b00)) { //cw trigger
    cw_fall = false;
    ccw_fall = false;
    if (fwd) {
      value--;
    } else {
      value++;
    }
  }
}

FASTRUN
int G0ORX_Rotary::process() {
  __disable_irq();
  int result=value;
  value=0;
  __enable_irq();
  return result;
}
#endif

