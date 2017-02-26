/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PID.h
 * Author: s_noppe
 *
 * Created on 26 februari 2017, 12:44
 */

#ifndef PID_H
#define PID_H

#include <time.h>

class PID
{
	private:
		unsigned long lastTime ;
		double input, output, setpoint ;
		double errSum, lastErr ;
		double kp, ki, kd ;
		clock_t tm ;
		
	public:
		PID() ;
		void compute() ;
		void setTunings(double Kp, double Ki, double Kd) ;
		double getInput(){ return input ; } ;
		double getOutput(){ return output ; } ;
		double getSetpoint(){ return setpoint ; } ;
		void setInput(double Input) ;
		void setOutput(double Output) ;
		void setSetpoint(double Setpoint) ;
};




#endif /* PID_H */

