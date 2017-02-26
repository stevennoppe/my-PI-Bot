#include "L298HN.h"

void L298HN::forward(int motor, int speed)
{
	// Check parameter 'speed'
	if (speed < 0)
	{
		printf("The value of the speed of the motors must be higher ") ;
		printf("then 0! Setting it to 0.\n") ;
		speed = 0 ;
	}
	
	if (speed > 255)
	{
		printf("The value of the  speed of the motors must be lower ") ;
		printf("then 255! Setting it to 255.\n") ;
		speed = 255 ;
	}
	
	switch(motor)
	{
		case 1:
			printf("Forward motor 1 (%d)...\n", speed) ;
			// set pwm of ENB to given speed
			softPwmWrite(ENA, speed) ;	
			// set EN1 to high and EN2 to low (forward motor 1)
			digitalWrite(EN1, HIGH) ;
			digitalWrite(EN2, LOW) ;
			
			break ;
		
		case 2:
			printf("Forward motor 2 (%d)...\n", speed) ;
			// set pwm of ENB to given speed
			softPwmWrite(ENB, speed) ;	
			// set EN3 to high and EN4 to low (forward motor 2)
			digitalWrite(EN3, HIGH) ;
			digitalWrite(EN4, LOW) ;
			break ;
		default:
			printf("Only motor 1 and 2 are available!\n") ;
	}
}

void L298HN::reverse(int motor, int speed)
{
	// check speed
	if (speed < 0)
	{
		printf("The value of the speed of the motors must be higher ") ;
		printf("then 0! Setting it to 0.\n") ;
		speed = 0 ;
	}
	
	if (speed > 255)
	{
		printf("The value of the speed of the motors must be lower ") ;
		printf("then 255! Setting it to 255.\n") ;
		speed = 255 ;
	}
	
	switch(motor)
	{
		case 1:
			printf("Reverse motor 1 (%d)...\n", speed) ;
			// set pwm of ENB to given percentage
			softPwmWrite(ENA, speed) ;	
			// set EN1 to low and EN2 to high (reverse motor 1)
			digitalWrite(EN1, LOW) ;
			digitalWrite(EN2, HIGH) ;
			break ;
		case 2:
			printf("Reverse motor 2 (%d)...\n", speed) ;
			// set pwm of ENB to given speed
			softPwmWrite(ENB, speed) ;	
			// set EN3 to low and EN4 to high (reverse motor 2)
			digitalWrite(EN3, LOW) ;
			digitalWrite(EN4, HIGH) ;
			break ;
		default:
			printf("Only motor 1 and 2 are available!\n") ;
	}
}

void L298HN::stop(int motor)
{
	switch(motor)
	{
		case 1:
			printf("Stopping motor 1...\n") ;
			digitalWrite(EN1, LOW) ;
			digitalWrite(EN2, LOW) ;
			break ;
		case 2:
			printf("Stopping motor 2...\n") ;
			digitalWrite(EN3, LOW) ;
			digitalWrite(EN4, LOW) ;
			break ;
		default:
			printf("Only motor 1 and 2 are available!") ;
	}
}

void L298HN::setupMotors()
{
	// Initialise the pins as PWM with an initial value of 100 % from 100 %
	softPwmCreate(ENA, 255, 255) ;	
	softPwmCreate(ENB, 255, 255) ;	
		
	// Initialise the pins as output
	pinMode(EN1, OUTPUT) ;			
	pinMode(EN2, OUTPUT) ;			
	pinMode(EN3, OUTPUT) ;			
	pinMode(EN4, OUTPUT) ;			
	
	// Reset all outputs to zero, because by default we 
	// may not just think that the raspberry will do this for us!
	this->stop(1) ;
	this->stop(2) ;
}