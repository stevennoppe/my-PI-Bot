/* 
 * File:   HMC5883L.h
 * Author: Steven Noppe
 *
 * Created on 14 november 2016, 19:44
 *
 * Last modified on 5 februari 2017
 * -> used my new class "sleep"
 */

#ifndef HMC5883L_H
#define HMC5883L_H

#include <linux/i2c-dev.h>	// for the ioctl() function
#include <cmath>			// for some math functions
#include <unistd.h>			// for the read() and write() function
#include <fcntl.h>			// for the open() function
#include <cstdlib>			// for the exit() function
#include <fstream>			// for some streaming functions...
#include <sstream>			// for using string streams	
#include <string.h>			// for manipulating strings
#include <iomanip>			// for setw() and setfill() functions

#include "donothing.h"

/*
 * Compiling with GCC, gives a fault because M_PI is not defined
 * With an IDE like Netbeans, we do not have that problem
 * Thats why we define it here
 */
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

/*
 * Same problem here with GCC, it seems he doesn't know what 'true' is 
 */
#ifndef true
	#define true 1
#endif

using namespace std;

class HMC5883L
{
	private:
		doNothing dn ;
		
		short x ;
		short y ;
		short z ;
		short xOffset ;
		short yOffset ;
		
		float angle ;
		float declinationDegree ;
		float declinationMinutes ;
		
		int fd ;	// 'fd' stands for 'file descriptor'
		unsigned char buf[16] ;
		
	public:
		HMC5883L(short xO, short yO, float declDegree, float declMinutes) ;
		
		short getX() 
		{
			return x ;
		}
		
		short getY() 
		{
			return y ;
		}
		
		short getZ()
		{
			return z ;
		}
		
		void setXOffset(short xO)
		{
			xOffset = xO ;
		}
		
		void setYOffset(short yO)
		{
			yOffset = yO ;
		}
		
		float getAngle()
		{
			return angle ;
		}
		
		void updateData() ;
		
		void calibrateHMC5883L() ;
		
		void calibrateHMC5883L(int xOffs, int yOffs) ;
} ;



#endif /* HMC5883L_H */