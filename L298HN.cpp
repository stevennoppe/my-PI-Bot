#include "L298HN.h"

void L298HN::forward(int motor, int percentage)
{
	// Check parameter 'percentage'
	if (percentage < 0 | percentage > 100)
	{
		printf("The percentage of speed of the motors must be between ") ;
		printf("0 and 100 percentage! Setting it to default.\n") ;
		// set percentage to default value = 100 %
		percentage = 100 ;
	}
	
	switch(motor)
	{
		case 1:
			printf("Forward motor 1 (%d percent)...\n", percentage) ;
			// set pwm of ENB to given percentage
			softPwmWrite(ENA, percentage) ;	
			// set EN1 to high and EN2 to low (forward motor 1)
			digitalWrite(EN1, HIGH) ;
			digitalWrite(EN2, LOW) ;
			break ;
		case 2:
			printf("Forward motor 2 (%d percent)...\n", percentage) ;
			// set pwm of ENB to given percentage
			softPwmWrite(ENB, percentage) ;	
			// set EN3 to high and EN4 to low (forward motor 2)
			digitalWrite(EN3, HIGH) ;
			digitalWrite(EN4, LOW) ;
			break ;
		default:
			printf("Only motor 1 and 2 are available!\n") ;
	}
}

void L298HN::reverse(int motor, int percentage)
{
	// Check parameter 'percentage'
	if (percentage < 0 | percentage > 100)
	{
		printf("The percentage of speed of the motors must be between ") ;
		printf("0 and 100 percentage! Setting it to default.\n") ;
		// set percentage to default value = 100 %
		percentage = 100 ;
	}
	
	switch(motor)
	{
		case 1:
			printf("Reverse motor 1 (%d percent)...\n", percentage) ;
			// set pwm of ENB to given percentage
			softPwmWrite(ENA, percentage) ;	
			// set EN1 to low and EN2 to high (reverse motor 1)
			digitalWrite(EN1, LOW) ;
			digitalWrite(EN2, HIGH) ;
			break ;
		case 2:
			printf("Reverse motor 2 (%d percent)...\n", percentage) ;
			// set pwm of ENB to given percentage
			softPwmWrite(ENB, percentage) ;	
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
	softPwmCreate(ENA, 100, 100) ;	
	softPwmCreate(ENB, 100, 100) ;	
		
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