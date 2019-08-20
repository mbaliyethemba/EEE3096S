/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <STUDNUM_1> <STUDNUM_2>
 * Date
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

int HH,MM,SS;

void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. If you want to change pinouts, be aware
	
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
	    pinMode(LEDS[i], OUTPUT);
	}
	
	//Set Up the Seconds LED for PWM
	//Write your logic here
   pinMode (SECS, PWM_OUTPUT);      // 
   pwmSetMode(PWM_MODE_MS);
	
	printf("LEDS done\n");
	
	//Set up the Buttons
	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);
	}
   
   
	
	//Attach interrupts to Buttons
	//Write your logic here
   if(BTNS[0] == 1){
      hoursInc();
      HH = getHours();
      MM = getMins();
      SS = getSec();
      lightHours(HH);
      lightMins(MM);
      secPWM(SS);
   }
   else if (BTNS[1]== 1){
	   minInc();
	   HH = getHours();
      MM = getMins();
      SS = getSec();
      lightHours(HH);
      lightMins(MM);
      secPWM(SS);
   }
	
	printf("BTNS done\n");
	printf("Setup done\n");
}


/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	initGPIO();

	//Set random time (3:04PM)
	//You can comment this file out later
	wiringPiI2CWriteReg8(RTC, HOUR, 0x13+TIMEZONE);
	wiringPiI2CWriteReg8(RTC, MIN, 0x4);
	wiringPiI2CWriteReg8(RTC, SEC, 0x00);
	
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
		//Write your logic here
      int hours = getHours();
      int mins = getMins();
      int secs = getSecs();
		//Function calls to toggle LEDs
		//Write your logic here
      toggleTime();
		
		// Print out the time we have stored on our RTC
		printf("The current time is: %x:%x:%x\n", hours, mins, secs);

		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}

/*
 * Change the hour format to 12 hours
 */
int hFormat(int hours){
	/*formats to 12h*/
	if (hours >= 24){
		hours = 0;
	}
	else if (hours > 12){
		hours -= 12;
	}
	return (int)hours;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
	// Write your logic to light up the hour LEDs here
   
   char * convertToBinary4(int hour, char *output) {
   output[4] = '\0';
   output[3] = (hour & 1) + '0';
   output[2] = ((hour >> 1) & 1) + '0';
   output[1] = ((hour >> 2) & 1) + '0';
   output[0] = ((hour >> 3) & 1) + '0';
   return output;
}

   char binary[5];
   convertToBinary4(hr, binary);
   
   for(int i = 0; i < 4; i++){
      if(binary[i] == '0'){
         digitalWrite(LEDS[i], LOW);
      }
      else{
         digitalWrite(LEDS[i], HIGH);
      }
   }  	
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
	//Write your logic to light up the minute LEDs here
   char * convertToBinary6(int hour, char *output) {
   output[6] = '\0';
   output[5] = (hour & 1) + '0';
   output[4] = ((hour >> 1) & 1) + '0';
   output[3] = ((hour >> 2) & 1) + '0';
   output[2] = ((hour >> 3) & 1) + '0';
   output[1] = ((hour >> 4) & 1) + '0';
   output[0] = ((hour >> 5) & 1) + '0';
   return output;
}

   char binary[7];
   convertToBinary6(units, binary);
   
   for(int i = 0; i < 6; i++){
      if(binary[i] == '0'){
         digitalWrite(LEDS[i+4], LOW);
      }
      else{
         digitalWrite(LEDS[i+4], HIGH);
      }
   }
}

/*
 * PWM on the Seconds LED
 * The LED should have 60 brightness levels
 * The LED should be "off" at 0 seconds, and fully bright at 59 seconds
 */
void secPWM(int units){
	// Write your logic here
   if (units == 0 || units = 60){
      pwmWrite(SECS, 0);
    }
    
   else{
      pwmWrite(SECS, (units+1));
   }
}

/*
 * hexCompensation
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}


/*
 * hourInc
 * Fetch the hour value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 23 hours in a day
 * Software Debouncing should be used
 */
void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 1 triggered, %x\n", hours);
		//Fetch RTC Time
		//Increase hours by 1, ensuring not to overflow
		//Write hours back to the RTC
      HH = getHours();
      HH = hFormat(HH);
      HH = HH + 1
      if( HH > 24){
         HH = HH - 24;
         HH = decCompensation(HH);
		   wiringPiI2CWriteReg8(RTC, HOUR, HH);
      }
      else{
		   HH = decCompensation(HH);
		   wiringPiI2CWriteReg8(RTC, HOUR, HH);
      }
	}
	lastInterruptTime = interruptTime;
}

/* 
 * minInc
 * Fetch the minute value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 60 minutes in an hour
 * Software Debouncing should be used
 */
void minInc(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 2 triggered, %x\n", mins);
		//Fetch RTC Time
		//Increase minutes by 1, ensuring not to overflow
		//Write minutes back to the RTC
      HH = getHours();
      MM = getMins();
      MM = MM + 1;
      if (MM > 60){
         MM = MM - 60;
         HH = HH + 1;
         if( HH > 24){
            HH = HH - 24;
            HH = decCompensation(HH);
		      wiringPiI2CWriteReg8(RTC, HOUR, HH);
            MM = decCompensation(MM);
		      wiringPiI2CWriteReg8(RTC, HOUR, HH);
          }
          else{
            HH = decCompensation(HH);
		      wiringPiI2CWriteReg8(RTC, HOUR, HH);
            MM = decCompensation(MM);
		      wiringPiI2CWriteReg8(RTC, HOUR, HH);
         }
      }
      else{
		   MM = decCompensation(MM);
		   wiringPiI2CWriteReg8(RTC, HOUR, HH);
      }
      
	}
	lastInterruptTime = interruptTime;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		HH = hFormat(HH);
		HH = decCompensation(HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}
