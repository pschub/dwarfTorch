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

/* dwarfTorch
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

#include "main.h"


void setupUC(void);
void updateLights(unsigned char t_elapsed, char t_limit, bool blink);

volatile unsigned char timeElapsed;
volatile bool motionDetected;
volatile bool secondTick;

int main(void){
    bool lightsOn = false;
    bool blinkState = false;
    unsigned char timeLimit = INIT_TIME;
    timeElapsed = 0;
    motionDetected = false;
    secondTick = false;

    setupUC();

    while(1){
        if (lightsOn && secondTick){                     // lights ON
            secondTick = false;
            blinkState = ~blinkState;
            if (motionDetected){                         // motion detected
                if (timeLimit < MAX_TIME)                // increase time limit
                    timeLimit = timeLimit*2;
                else
                    timeElapsed = 0;
                motionDetected = false;
            }
            else{
                timeElapsed++;                           // inc timer
            }
            updateLights(timeElapsed, timeLimit, blinkState);
            if (timeElapsed > timeLimit){
                lightsOn = false;
                PORTB &= ~(1<<RELAY);
            }
        }
        else{                                            // lights OFF
            if (!lightsOn && motionDetected){            // motion detected
                lightsOn = true;                         // initialize and turn on
                timeLimit = INIT_TIME;
                timeElapsed = 0;
                updateLights(timeElapsed, timeLimit, blinkState);
                PORTB |= (1<<RELAY);
                if (secondTick)
                    secondTick = false;
                motionDetected = false;
            }
        }
     }

    return 0;
}


/* updateLights
 * Takes char of time elapsed and max time limit and updates led display showing
 * time left (as a bar graph). Bool blink is a single led at the bottom of graph.
 * This func will set that led to the state of blink.
 */
void updateLights(unsigned char t_elapsed, char t_limit, bool blink){
    char num_lightsOn;
    char i;

    if (t_elapsed > t_limit){ //time's up
        num_lightsOn = 0;
        blink = false;
    }
    else{
        num_lightsOn = NUM_LIGHTS - (NUM_LIGHTS * t_elapsed) / t_limit;
    }

    if (blink){                                    // clock in seconds blinker
        PORTB |= (1<<SRDAT);
    }
    else{
        PORTB &= ~(1<<SRDAT);
    }
    PORTB ^= (1<<SRCLK);
    PORTB ^= (1<<SRCLK);

    PORTB |= (1<<SRDAT);
    for (i=0; i<num_lightsOn; i++){                // clock in ON lights
        PORTB ^= (1<<SRCLK);
        PORTB ^= (1<<SRCLK);
    }
    PORTB &= ~(1<<SRDAT);
    for (i=0; i<(NUM_LIGHTS - num_lightsOn); i++){ //clock in OFF lights
        PORTB ^= (1<<SRCLK);
        PORTB ^= (1<<SRCLK);
    }

    return;
}


//initializes uC.
void setupUC(void){
    //Setup the rtc
    cli();
    TCCR0B |= (1<<2);//|(1<<0); //Divide by 1024
    OCR0A = 217;        //Count 36x217 ticks for 1 second interrupt @ 8MHz
    TCCR0B |= 1<<WGM02;        //Put Timer/Counter1 in CTC mode
    TIMSK |= 1<<OCIE0A;        //enable timer compare interrupt
    sei();

    //set SR data and clock pins to output
    DDRB |= (1<<RELAY)|(1<<SRCLK)|(1<<SRDAT);    //set as output
    PORTB &= ~(1<<RELAY);

    //input pin
    DDRB &= ~(1<<PIR);   // set PIR pin as input
    PORTB &= ~(1<<PIR);
    GIMSK |= (1<<PCIE);  // turn on pinchange interrupts
    PCMSK |= (1<<PIR);   // set PIR pin as interrupt
    return;
}


/* PIR sensor isr. Sets motionDetected flag if any of the PIR sensors
 * reported motion.
 */
ISR(PCINT0_vect)
{
    if (PIR_ON){
        volatile unsigned int i = 0;
        for (i = 0; i<DEBOUNCE_TIME; i++); // debounce
        if (PIR_ON){
            motionDetected = true;
        }
    }
}


/* lights-off countdown isr. Sets secondTick flag about every 1 second.
 * Currently there is a large round-off error, but it's not important for this
 * application
 */
ISR(TIMER0_COMPA_vect)
{
    static char secFraction = 0;
    secFraction++;
    if (TICKS_PER_SEC == secFraction){
        secFraction = 0;
        timeElapsed++;
        secondTick = true;
    }
}



