dwarfTorch - a motion-sensing light for ATTiny85
Patrick Schubert
Started March 2012

Version 2.0 - July 2013

Controls a light bulb in the basement. Reads input from PIR sensors and turns
on lights if motion is detected. The time the light bulb is on starts at a low
number and increases if more movement is detected before the timer runs out.
When the timer finally does run out, the light is turned off.

Displays time remaining on an led bar-graph and has a 1Hz blinking LED when
timer is running.

Compiles to 638 bytes with avr-gcc.
