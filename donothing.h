/*
 * This class will make use of the nanosleep() function.
 * Just to make my original code a little bit smaller...
 *
 * Usage :
 *
 * #include "donothing.h"
 *
 * doNothing dn ;
 * dn.waitSec(0.1) // wait 0.1 sec
 * dn.waitNanoSec(100000000) ; // wait 0.1 sec
 *
 * The function returns a 0 if it failed, 1 if succeeded
 */

/* 
 * File:   doNothing.h
 * Author: Steven Noppe
 *
 * Created on 5 februari 2017, 10:03
 */

#ifndef DONOTHING_H
#define DONOTHING_H

#include <time.h>

class doNothing
{
	private:
		timespec waitTime ;
				
		float hours ;
		float minutes ;
		float seconds ;
		float milliseconds ;
		float microseconds ;
		long int nanoseconds ;
		
		void resetAll() ;
		
	public:
		doNothing() ;
		int waitSec(float s) ;
		int waitNanoSec(long int nanoS) ;
		int waitMicroSec(float microS) ;
		int waitMilliSec(float milliS) ;
};

#endif /* WAIT_H */

