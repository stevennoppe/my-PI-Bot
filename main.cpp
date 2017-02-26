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

#include "serialcomm.h"
#include "HMC5883L.h"
#include "L298HN.h"
#include "PID.h"

int main(int argc, char** argv)
{
	bool bNoReceivedCommand = true ;
	
	clock_t t;
	
	// Disable buffering on stdout
	// If we do not do this the printf() statement does not 
	// show our text in the terminal. Our text will go directly to the stdout.
	setbuf(stdout, NULL);
	
	// initialize the bluetooth serial communication
	serial bluetooth ;
	bluetooth.openSerialPort("/dev/rfcomm0") ;
				
	// initialize motor driver
	L298HN motors ;
	
	// initialize the compass
	HMC5883L compass(207, 178, 0, 35) ;
	
	// initialize the PID
	
	PID pid ;
	
	//float setpointAngle = 0.0f ;
	//float currentAngle = 0.0f ;
	
	/*
	float previousAngle = 0.0f ;
	
	float errorAngle = 0.0f ;
	float previousErrorAngle = 0.0f ;
	
	float P = 0.0f ;
	float I = 0.0f ;
	float D = 0.0f ;
	
	float kP = 1.0f ;
	float kI = 0.0f ;
	float kD = 0.0f ;
	*/
	
	float drive = 0.0f ;
	float scaleFactor = 1 ;
	
	
	float speed = 0.0f ;
		
	int i = 0 ;
	
	bool firstForward = false ;
	
	// used in changing parameters and showing them through serial Bluetooth
	char *str1 ;
	char *param ; 
	char *value ;
	std::string strCom ;
	char str[100] ;
	
	enum States
	{
		STATE_INITIALIZE,
		STATE_STOP,
		STATE_NOTHING,
		STATE_START_FORWARD,
		STATE_START_REVERSE,
		STATE_START_LEFT90,
		STATE_FORWARD,
		STATE_REVERSE,
		STATE_CHANGE_PARAM,
		STATE_SHOW_VALUE,
		STATE_CALIBRATE,
		STATE_SHUTDOWN
	};
	
	States roboState ;
	
	roboState = STATE_INITIALIZE ;
	
	// clear the serial console screen	
	bluetooth.writeSerialPort("\033c") ;
	
	while (1)
	{		
		t=clock() ;
		switch (roboState)
			{
			case STATE_INITIALIZE:
				printf("State : Initializing...\n") ;
				bluetooth.writeSerialPort("Initializing...\n\r") ;				
				// Setup WiringPi
				wiringPiSetup() ;
		
				// do some initialisations for the motors
				motors.setupMotors() ;
				speed = 128.0f ;
			
				// update the compass
				compass.updateData() ;
	
				pid.setTunings(20, 0, 0) ;
				pid.setSetpoint(compass.getAngle()) ;
						
				roboState = STATE_STOP ;
											
				break ;	
		
			case STATE_STOP:
				printf("State : Stopping...\n") ;
				bluetooth.writeSerialPort("Stopping...\n\r") ;				
				motors.stop(1) ;
				motors.stop(2) ;
				
				roboState = STATE_NOTHING ;
			
				bNoReceivedCommand = false ;
				
				break ;
				
			case STATE_NOTHING:
				//printf("State : Doing nothing, please give a command...\n") ;
				break ;
				
			case STATE_START_FORWARD:
				printf("State : Starting forward...\n") ;
				bluetooth.writeSerialPort("Starting forward...\n\r") ;				
				// update the compass and set the SetpointAngle
				compass.updateData() ;
				pid.setSetpoint(compass.getAngle()) ;
				
				printf("Setpoint angle = %0.2f.\n", pid.getSetpoint()) ;
				roboState = STATE_FORWARD ;
				break ;
				
			case STATE_FORWARD:
				printf("State : Forward...\n") ;
				
				// Update compass data
				compass.updateData() ;
				
				pid.setInput(compass.getAngle()) ;
				printf("Current angle = %0.2f.\n", pid.getInput()) ;
				printf("Setpoint angle = %0.2f.\n", pid.getSetpoint()) ;
				
				pid.compute() ;
				
				printf("Output of PID = %0.2f.\n", pid.getOutput()) ;
				
				if (pid.getOutput() > 0)
				{
					motors.forward(1, speed-abs(pid.getOutput()/4)) ;
					motors.forward(2, speed+abs(pid.getOutput()/4)) ;
				}
				else
				{
					motors.forward(1, speed+abs(pid.getOutput()/2)) ;
					motors.forward(2, speed-abs(pid.getOutput()/2)) ;
				}
								
				break ;
				
			case STATE_START_REVERSE:
				printf("State : Starting reverse...\n") ;
				bluetooth.writeSerialPort("Starting to reverse...\n\r") ;				
				roboState = STATE_REVERSE ;
				
				break ;
				
			case STATE_REVERSE:
				printf("State : Reverse...\n") ;
								
				break ;
			
			case STATE_START_LEFT90:
				printf("State : Turning left 90°...\n") ;
				//setpointAngle = compass.getAngle() + 90.0f ;
				
				//if (setpointAngle > 360)
				//	setpointAngle = setpointAngle - 360 ;
				
				roboState = STATE_FORWARD ;
				
				break ;
				
			case STATE_CALIBRATE:
				printf("Calibration state...\n") ;
				bluetooth.writeSerialPort("Follow the instructions "
					"on the IDE console!\r\n") ;
				compass.calibrateHMC5883L() ;
				
				roboState = STATE_NOTHING ;
				
				break ;
				
			case STATE_CHANGE_PARAM:
			{	
				printf("Changing a parameter...\n") ;
								
				strCom = bluetooth.getBuffer() ;
				
				// The only correct command for changing a parameter is
				// examples :
				// change speed=15
				// change setPointAngle=100
				
				// no spaces around the "=" !
				
				
				// find the space after "change" and put everything
				// in frond of the space (the word "change") in str1
				str1 = strtok(&strCom[0], " ") ;
				
				// then search for the "=" and again put what is
				// in between the space and "=" in param 
				// => this is our parameter
				param = strtok(NULL, "=") ;
				
				// and last what is after the "=" is our value
				value = strtok(NULL,"") ;
				
				// remove the '\n' from the "value" string
				size_t length = strlen(value) ;
				if (length > 0 && value[length-1] == '\n') 
					value[--length] = '\0' ;

				// printf ("%s & %s & %s", str1, param, value) ;
				
				if (strcmp(param,"speed")==0)
				{
					speed = atof(value) ;
					sprintf(str,"Changed speed to %s!\n\r", value) ;
					
					bluetooth.writeSerialPort(str) ;
				}
				else if (strcmp(param,"setpointAngle")==0)
				{
					pid.setSetpoint(atof(value)) ;
					sprintf(str,"Changed setpointAngle to %s!\n\r", value) ;
					
					bluetooth.writeSerialPort(str) ;
				}
				else
				{
					printf("unknown parameter\n") ;
					sprintf(str,"I do not know this parameter!\n\r") ;
					
					bluetooth.writeSerialPort(str) ;
				}
				
				bluetooth.flushBuffer() ;
				
				roboState = STATE_NOTHING ;
				
				break ;
			}	
			case STATE_SHOW_VALUE:
				strCom = bluetooth.getBuffer() ;
				
				// The only correct command to show a value is
				// examples :
				// show speed
				// show currentAngle
												
				// find the space after "change" and put everything
				// in frond of the space (the word "change") in str1
				str1 = strtok(&strCom[0], " ") ;
				
				// and last what is after the "=" is our value
				param = strtok(NULL,"") ;
				
				//printf ("%s & %s", str1, param) ;
				
				if (strcmp(param,"speed\n")==0)
				{
					printf("speed\n") ;
					sprintf(str,"speed = %0.2f\n\r", speed) ;
					
					bluetooth.writeSerialPort(str) ;
				}
				else if (strcmp(param,"currentAngle\n")==0)
				{
					printf("currentAngle\n") ;
					sprintf(str,"currentAngle = %0.2f\n\r", compass.getAngle()) ;
					
					bluetooth.writeSerialPort(str) ;
				}
				else
				{
					printf("unknown parameter\n") ;
					sprintf(str,"I do not know this parameter!\n\r") ;
					
					bluetooth.writeSerialPort(str) ;
				}
				
				bluetooth.flushBuffer() ;
				
				roboState = STATE_NOTHING ;
				
				break;
			case STATE_SHUTDOWN:
				// Shutting down...
				printf("Shutting down...\n") ;
				sprintf(str,"Shutting down...\n\r") ;
				bluetooth.writeSerialPort(str);
				
				return 0 ;
				
				break;
			}
		
		// if something is available, read from the bluetooth port 
		if (bNoReceivedCommand == false)
		{
			bluetooth.writeSerialPort("Ready to receive commands...\n\r") ;
			bNoReceivedCommand = true ;
		}

		if (bluetooth.checkBuffer(100))
		{
			bNoReceivedCommand = false ;
			
			bluetooth.readSerialPort() ; 
			
			printf("Incoming command : ") ;
			bluetooth.printSerialPort() ;
			//printf("\n") ;
		
			std::string str = bluetooth.getBuffer() ;
			
			if (str=="stop\n")
			{
				printf("Received stop command.\n") ;
				bluetooth.writeSerialPort("Received stop command...\n\r") ;
				roboState = STATE_STOP ;
				bluetooth.flushBuffer() ;
			}
			else if (str=="forward\n")
			{
				printf("Received forward command.\n") ;
				bluetooth.writeSerialPort("Received forward command...\n\r") ;
				roboState = STATE_START_FORWARD;
				bluetooth.flushBuffer() ;
			}
			else if (str=="reverse\n")
			{
				printf("Received reverse command.\n") ;
				roboState = STATE_START_REVERSE;
				bluetooth.flushBuffer() ;
			}
			else if (str=="turnleft90\n")
			{
				printf("Received turn left 90°.\n") ;
				roboState = STATE_START_LEFT90 ;
				bluetooth.flushBuffer() ;
			}
			else if (str=="calibrate\n")
			{
				printf("Received calibrate command...\n") ;
				roboState = STATE_CALIBRATE ;
				bluetooth.flushBuffer() ;
			}
			else if (str.find("change") != -1)
			{
				// found command to change a parameter
				printf("Received command to change a parameter : ") ;
								
				roboState = STATE_CHANGE_PARAM ;
			}
			else if (str.find("show") != -1)
			{
				// found command to change a parameter
				printf("Received command to show a current value : ") ;
								
				roboState = STATE_SHOW_VALUE ;
			}
			else if (str=="shutdown\n")
			{
				printf("Received shutdown command...\n") ;
				roboState = STATE_SHUTDOWN ;
				bluetooth.flushBuffer() ;
			}
			else
			{
				printf("Nothing received or no correct command received!\n") ;
				bluetooth.writeSerialPort("Nothing received or no correct command received!\n\r") ;				
				// Do nothing, do not change state!
				bluetooth.flushBuffer() ;
			}
		}
		
		t = clock() -t ;
		
		//printf("%f seconds...\n", ((float)t)/CLOCKS_PER_SEC) ;
		delay(100) ;
	}	
	
	//fflush(stdout) ;
	//getchar() ;
	 
	 compass.calibrateHMC5883L() ;
	
	return 0; 
}

