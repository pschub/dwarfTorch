/*
dwarfTorch
Patrick Schubert
Started March 2012

Takes inputs from pir sensor and turns on lights on motion. 
Time on is set by define in main.h. Also sends out a bargraph of on-time
remaining to a si-po shift register. 
*/


#include "main.h"

void setupUC(void);

volatile unsigned int timeElapsed;
volatile bool motionDetected;
volatile bool lightsOn;

int main(void)
{
	unsigned int nextDec = 0;
	timeElapsed = 0;
	lightsOn = false;
	motionDetected = false;

	setupUC();

	while(1){
		/*if (secondFlag){     //1 second tick
			secondFlag = false;
			if (countDown > 0){
				countDown--;
			}
			//PORTB &= ~(1<<RELAY);
		}*/
		
		if (motionDetected){ //start of countdown
			PORTB |= (1<<RELAY);
			timeElapsed = 0;
			nextDec = DECAMT; 
			resetTimer();
			motionDetected = false;
			lightsOn = true;
		}
		else if (lightsOn && timeElapsed > nextDec){
			nextDec += DECAMT;
			PORTB ^= (1<<SRCLK);
			PORTB ^= (1<<SRCLK);
			
			if (ONTIME < timeElapsed){
				PORTB &= ~(1<<RELAY);
				lightsOn = false;
			}
		}
		


	}

	return 0;
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
  PORTB |= (1<<RELAY);
  
  //input pin
  DDRB &= ~(1<<PIR); //set PIR pin as input
  PORTB &= ~(1<<PIR);
  GIMSK |= (1<<PCIE); // turn on pinchange interrupts
  PCMSK |= (1<<PIR);   //set PIR pin as interrupt
}


//sets all lights in timer to 'on'
void resetTimer(){
  int i;
  PORTB |= (1<<SRDAT); 
  for (i=0; i<6; i++){   //clock in data
	  PORTB ^= (1<<SRCLK);
	  PORTB ^= (1<<SRCLK);
  }
  PORTB &= ~(1<<SRDAT);

}


//pir sensor isr
ISR(PCINT0_vect)
{
    if (PIR_ON){
        volatile unsigned int i = 0;
        for (i = 0; i<DEBOUNCE_TIME; i++); //debounce        
        if (PIR_ON){
			motionDetected = true;
        }
    }
}


//lights off countdown isr
ISR(TIMER0_COMPA_vect)  
{
    if (lightsOn){
		timeElapsed++; //increment fraction of second counter. 
    }
/*    if (TICKS_PER_SEC == secFraction){
        //secFraction = 0;
        //secondFlag = true;
    }*/
}



