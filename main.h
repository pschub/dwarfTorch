/*
dwarftorch
Patrick Schubert
March 2012

*/

#ifndef MAINH
#define MAINH

#include <avr/io.h>
#include <avr/interrupt.h>
#include "boolean.h"

//with clk/256, there are 15.3 ticks per sec. 
//do the math here so avr doesn't have to

/*//15 seconds on
#define ONTIME 228
#define DECAMT 38
*/
 //10 seconds on
#define ONTIME 153
#define DECAMT 26


//pin definitions (index in port b reg)
#define RELAY 4
#define SRCLK 0
#define SRDAT 1
#define PIR 3

//active low!
#define PIR_ON ((~PINB) & (1<<PIR))  

//misc
#define DEBOUNCE_TIME 3000


#endif
