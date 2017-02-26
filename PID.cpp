/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "PID.h"

PID::PID()
{
	
}

void PID::compute()
{
	// How long since we last calculated
	unsigned long now = clock() ;
	double timeChange = (double)(now - lastTime) ;

	// Compute all the working error variables
	double error = setpoint - input ;
	errSum += (error * timeChange) ;
	double dErr = (error - lastErr) / timeChange ;

	// Compute PID Output
	output = kp * error + ki * errSum + kd * dErr ;

	// Remember some variables for next time
	lastErr = error ;
	lastTime = now ;
}

void PID::setTunings(double Kp, double Ki, double Kd)
{
	kp = Kp;
	ki = Ki;
	kd = Kd;
}

void PID::setInput(double Input)
{ 
	input = Input ; 
} ;

void PID::setOutput(double Output)
{ 
	output = Output ; 
} ;

void PID::setSetpoint(double Setpoint)
{ 
	setpoint = Setpoint ; 
} ;