#!/usr/bin/python3
"""
Python Practical Template
Keegan Crankshaw
Readjust this Docstring as follows:
Names: Mbaliyethemba Shangase
Student Number: SHNMBA004
Prac: Prac1
Date: 04/08/2019
"""
import time #import sleep library

# import Relevant Librares
import RPi.GPIO as GPIO

#set mode
GPIO.setmode(GPIO.BOARD)

#set the buttons 
GPIO.setup(11,GPIO.IN, pull_up_down=GPIO.PUD_UP) #incremental button
GPIO.setup(13,GPIO.IN, pull_up_down=GPIO.PUD_DOWN) #decremental button

#set the LED
GPIO.setup(18,GPIO.OUT)  #LED3
GPIO.setup(16,GPIO.OUT)  #LED2
GPIO.setup(15,GPIO.OUT)  #LED1

#set the LED to low
GPIO.setup(18,GPIO.LOW)  #LED3
GPIO.setup(16,GPIO.LOW)  #LED2
GPIO.setup(15,GPIO.LOW)  #LED1

counter = 0;

time_stamp = time.time()

def my_callback(channel):
    global time_stamp       # put in to debounce
    time_now = time.time()
    if (time_now - time_stamp) >= 0.3:
        print "Rising edge detected on port 24 - even though, in the main thread,"
        print "we are still waiting for a falling edge - how cool?\n"
    time_stamp = time_now
    
   
GPIO.add_event_detect(11, GPIO.RISING, callback=my_callback)


def button():
	global counter
	if (GPIO.input(11) == 1 and counter != 7): #counts up
		print("1 pressed")
		counter += 1
		print (bin(counter)[2:].zfill(3))
		#time.sleep(.100)
		#GPIO.add_event_detect(11, GPIO.RISING)
		
	if (GPIO.input(11) == 1 and counter == 7): #wraps around to zero
		counter = 0
		print (bin(counter)[2:].zfill(3))
		#time.sleep(.100)
		#GPIO.add_event_detect(11, GPIO.RISING)
		
	if (GPIO.input(13) == 1 and counter != 0): #counts down
		print("2 pressed")
		counter -= 1
		print (bin(counter)[2:].zfill(3))
		#time.sleep(.100)
		#GPIO.add_event_detect(13, GPIO.RISING)
		
	if (GPIO.input(13) == 1 and counter == 0): #wraps around back to 7
		counter = 7
		print (bin(counter)[2:].zfill(3))
		#time.sleep(.100)
		#GPIO.add_event_detect(13, GPIO.RISING)
		
	return

#LED logic controller
def led(count):
	binS = bin(count)[2:].zfill(3)
	for index, value in enumerate(binS):
		if (value == '1'):
			LEDOn(index)
		else:
			LEDOff(index)
	return

#Turning LED on
def LEDOn(pin):
	if (pin == 0):
		GPIO.output(15,GPIO.HIGH)
	if (pin == 1):
		GPIO.output(16,GPIO.HIGH)
	if (pin == 2):
		GPIO.output(18,GPIO.HIGH)

#Turning LED off
def LEDOff(pin):
        if (pin == 0):
                GPIO.output(15,GPIO.LOW)
        if (pin == 1):
                GPIO.output(16,GPIO.LOW)
        if (pin == 2):
                GPIO.output(18,GPIO.LOW)

# Logic that you write
def main():
		button()
		led(counter)


# Only run the functions if 
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
            GPIO.wait_for_edge(23, GPIO.FALLING)
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    #except e:
        #GPIO.cleanup()
        #print("Some other error occurred")
        #print(e.message)
