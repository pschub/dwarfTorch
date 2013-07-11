/*
    Motion-sensing light for ATTiny85
    Copyright (C) 2012-2013 Patrick Schubert

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*dwarfTorch
Patrick Schubert
Started March 2012

Version 2.0 - July 2013

Controls a light bulb in the basement. Reads input from PIR sensors and turns
on lights if motion is detected. The time the light bulb is on starts at a low
number and increases if more movement is detected before the timer runs out.
When the timer finally does run out, the light is turned off.

Displays time remaining on an led bar-graph and has a 1Hz blinking LED when
timer is running.

*/

#ifndef MAINH
#define MAINH

#include <avr/io.h>
#include <avr/interrupt.h>
#include "boolean.h"

//with clk/256, there are 15.3 ticks per sec.
#define TICKS_PER_SEC 15

// 10 20 40 80 160
#define NUM_LIGHTS 5
#define INIT_TIME 20
#define MAX_TIME 160


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
