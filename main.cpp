/*
 * creating the PI robot
 *
 * Connection on a Raspberry Pi 3 model B v1.2 :
 * - +12V	-> '+' of battery 7,2 VDC
 * - GND	->	GND (6) on the Raspberry and '-' of battery 7,2 VDC
 * - ENA	->	GPIO17 (11) - wiringPi 0
 * - EN1	->	GPIO18 (12) - wiringPi 1
 * - EN2	->	GPIO27 (13) - wiringPi 2
 * - ENB	->	GPIO22 (15) - wiringPi 3
 * - EN3	->	GPIO23 (16) - wiringPi 4
 * - EN4	->	GPIO24 (18) - wiringPi 5
 *
 * - OUT1	->	motor 1 - connection 1
 * - OUT2	->	motor 1 - connection 2
 * - OUT3	->	motor 2 - connection 1
 * - OUT4	->	motor 2 - connection 2
 *
 * Connection of the HMC5883L :
 * - VCC	->	3V3 (1)
 * - GND	->	GND (6)
 * - SCL	->	GPIO3 - SCL1 I2C (5)
 * - SDA	->	GPIO2 - SDA1 I2C (3)
 *
 * Note :	The raspberry has his own 5V - 2A tension from
 *			a powerbank 10000 mAh
 *
 * Compiled in Netbeans IDE 8.1
 * set the following option under project properties ->
 * build -> linker -> libraries :
 * -I/usr/local/include -L/usr/local/lib -lwiringPi -lpthread
 *
 * Also set console to 'standard output' under 
 * project properties -> Run -> console type
 
 * Compile under Debian :
 *  g++ main.cpp -o robot -lm -I/usr/local/include -L/usr/local/lib -lwiringPi
 */

/* 
 * File:   main.cpp
 * Author: Steven Noppe
 *
 * Created on 06 november 2016, 12:52
 */

#include <stdio.h>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>

#include <math.h>


// #include "HMC588L_mit.h"
#include "HMC5883L.h"
#include "L298HN.h"

int main(int argc, char** argv)
{
	// Disable buffering on stdout
	// If we do not do this the printf() statement does not 
	// show our text in the terminal. Our text will go directly to the stdout.
	setbuf(stdout, NULL);
	
	// Setup WiringPi
	wiringPiSetup() ;
	
	// do some initialisations for the motors
	L298HN motors ;
	motors.setupMotors() ;
	
	// initialize the compass
	HMC5883L compass(280, 285, 0, 35) ;
	compass.updateData() ;
	
	float setpointAngle = compass.getAngle() ;
	float currentAngle = 0.0f ;
	float previousAngle = 0.0f ;
	
	float errorAngle = 0.0f ;
	float previousErrorAngle = 0.0f ;
	
	float P = 0.0f ;
	float I = 0.0f ;
	float D = 0.0f ;
	
	float kP = 20.0f ;
	float kI = 10.0f ;
	float kD = 0.0f ;
	
	float drive = 0.0f ;
	float scaleFactor = 1 ;
	
	float speed = 10.0f ;
	
	/*
	while (1)
	{
		compass.updateData() ;
		
		currentAngle = compass.getAngle() ;
		printf("Current angle = %0.2f.\n", currentAngle) ;
		printf("Setpoint angle = %0.2f.\n", setpointAngle) ;
		
		errorAngle = setpointAngle - currentAngle ;
		printf("Error angle = %0.2f.\n", errorAngle) ;
		
		P = errorAngle * kP ;
		printf("P = %0.2f.\n", P) ;
		D = ((previousAngle - currentAngle) / 0.1 )* kD ;
		printf("D = %0.2f.\n", D) ;
		
		drive = P + D * 25 ;
		printf("Drive = %0.2f.\n", drive) ;
		
		previousErrorAngle = errorAngle ;
		
		/*
		if (drive > 0)
		{
			motors.forward(1, speed) ;
			motors.forward(2, speed+abs(drive)) ;
		}
		else
		{
			motors.forward(1, speed+abs(drive)) ;
			motors.forward(2, speed) ;
		}
		 
		
		//motors.stop(1) ;
		//motors.stop(2) ;
		
		printf("\n\n") ;
		delay(100) ;
		//fflush(stdout) ;
		//getchar() ;
	}
	 */

	compass.calibrateHMC5883L() ;
	
	return 0;
}

