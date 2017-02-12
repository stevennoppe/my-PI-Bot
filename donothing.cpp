#include "donothing.h"

doNothing::doNothing()
{
	resetAll() ;	// Set everything to 0
}

void doNothing::resetAll()
{
	hours = 0.0f ;
	minutes = 0.0f ;
	seconds = 0.0f ;
	milliseconds = 0.0f ;
	microseconds = 0.0f ;
	nanoseconds = 0 ;
}

int doNothing::waitSec(float s)
{
	resetAll() ;
	
	if (s < 0)
	{
		nanoseconds = s * 1000000000 ;
	}
	else
	{
		seconds = s ;
	}

	// 1 000 000 000 nanoseconds = 1 seconds
	waitTime.tv_sec = seconds ;
	waitTime.tv_nsec =  long(nanoseconds) ;	
		
	if (nanosleep(&waitTime, NULL) < 0) 
		return 0 ;
	else
		return 1 ;
}

int doNothing::waitMilliSec(float milliS)
{
	resetAll() ;
	nanoseconds = milliS * 1000000 ;

	// 1 000 000 microseconds = 1 seconds
	waitTime.tv_sec = seconds ;
	waitTime.tv_nsec =  long(nanoseconds) ;	
		
	if (nanosleep(&waitTime, NULL) < 0) 
		return 0 ;
	else
		return 1 ;
}

int doNothing::waitMicroSec(float microS)
{
	resetAll() ;
	nanoseconds = microS * 1000 ;

	// 1 000 000 microseconds = 1 seconds
	waitTime.tv_sec = seconds ;
	waitTime.tv_nsec =  long(nanoseconds) ;	
		
	if (nanosleep(&waitTime, NULL) < 0) 
		return 0 ;
	else
		return 1 ;
}

int doNothing::waitNanoSec(long int nanoS)
{
	resetAll() ;
	nanoseconds = nanoS ;

	// 1 000 000 000 nanoseconds = 1 seconds
	waitTime.tv_sec = seconds ;
	waitTime.tv_nsec =  long(nanoseconds) ;	
		
	if (nanosleep(&waitTime, NULL) < 0) 
		return 0 ;
	else
		return 1 ;
}