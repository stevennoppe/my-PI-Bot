/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HMC5883L.h
 * Author: s_nop
 *
 * Created on 14 november 2016, 19:44
 */

#ifndef HMC5883L_H
#define HMC5883L_H

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

#include <iostream>
#include <fstream>

using namespace std;

class HMC5883L
{
	private:
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

		/* 
		 * Find out which address the HMC5883L is on with the command : 
		 * 'sudo i2cdetect -y 1' on a Raspberry Pi - model A, B rev 2 of B+
		 * 'sudo i2cdetect -y 0' on a Raspberry Pi - model B rev 1
		 * 1E => 0x1E 
		 */
		const int HMC5883L_I2C_ADDR = 0x1E ;

		void selectDevice(int fd, int addr, char *name) ;
	
		void writeToDevice(int fd, int reg, int val) ;
		
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
} ;

HMC5883L::HMC5883L(short xO, short yO, float declDegree, float declMinutes)
{
	xOffset = xO ;
	yOffset = yO ;
	declinationDegree = declDegree ;
	declinationMinutes = declMinutes ;
	/* 
	 * Open i2c-1 port for reading and writing with the open system call
	 * The open system call returns a number which you will need in future 
	 * functions for reading and writing
	 * see also : https://en.wikipedia.org/wiki/Open_(system_call)
	 */
	
	if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) 
	// on a Raspberry Pi - model B rev 1 use : '/dev/i2c-0'
	{
		// If it returns < 0 then something was wrong
		fprintf(stderr, "Failed to open i2c bus\n") ;
		// exit program with a 1 (failure)
		exit(1) ;
	}
	else 
	{
		// successfull !
		printf("Succesfully openend i2c bus\n") ;
	}

	/* 
	 * Initialise the HMC5883L with the integer 'fd' that was returned from the 
	 * open system call
	 */
	selectDevice(fd, HMC5883L_I2C_ADDR, "HMC5883L") ;

	/* 
	 * Now write some values to the HMC5883L to initalize it
	 */
	
	/* 
	 * First write the value 32 to the configuration register A
	 * 32 = Gain = 1090 Lsb/Gauss
	 * We send the following byte : 00100000
	 */
	writeToDevice(fd, 0x01, 32) ;	// 0x01 = configuration register A
    
	/* 
	 * Second write the value 0 to the mode register
	 * 0 = Continuous-Measurement Mode
	 * We send the following byte : 0000000
	 */
	writeToDevice(fd, 0x02, 0) ;	// 0x02 = Mode register
}
	
void HMC5883L::updateData()
{
	buf[0] = 0x03 ;

    if ((write(fd, buf, 1)) != 1)
    {
        // Send the register to read from
        fprintf(stderr, "Error writing to i2c slave\n") ;
    }
	else
	{
		//printf("Wrote to i2c slave.\n") ;
	}

	if (read(fd, buf, 6) != 6) 
	{
		fprintf(stderr, "Unable to read from HMC5883L\n") ;
	} 
	else 
	{
		// Received the values
        x = ((buf[0] << 8) | buf[1]) -xOffset ;
        z = ((buf[2] << 8) | buf[3]) ;
		y = ((buf[4] << 8) | buf[5]) -yOffset ;
			
		//printf("X : %d, Y : %d, Z : %d\n", x, y, z) ;
        float decl = (declinationDegree+declinationMinutes/60) * (M_PI/180) ;
		
		float headingRad = atan2(float(y), float(x)) ;
		//printf("Heading in RAD : %0.2f\n", headingRad) ;
		headingRad += decl ;
		//printf("Heading after adding declination in RAD : %0.2f\n", headingRad) ;
			
		if (headingRad < 0)
			headingRad += 2*M_PI ;
			
		if (headingRad > 2*M_PI)
			headingRad -= 2*M_PI ;
			
		//printf("Heading after checking < 0 or > 2xPI in RAD : %0.2f\n", headingRad) ;
			
		angle = headingRad * 180/M_PI ;
		//printf("Heading in degrees : %0.2f\n", headingDeg) ;
		float degrees = floor(angle) ;
		//printf("Heading in degrees after floor : %0.2f\n", degrees) ;
		float minutes = round((angle - degrees)*60) ;
		//printf("Heading in minutes : %0.2f\n", minutes) ;
				
		//printf("Heading : %0.2f degrees, %0.2f minutes.\n", degrees, minutes) ;
		
		// with GCC you need to declare b outside the for loop 
		// or compile with the option -std=c11
		/* 
		int b ;	
		for (b=0; b<6; ++b)
        {
            printf("%02x ",buf[b]) ;
        }
        // printf("\n") ;
            
        // printf("x=%d, y=%d, z=%d\n", x, y, z) ;
         */
	}
}		
#endif /* HMC5883L_H */

void HMC5883L::calibrateHMC5883L()
{
	// Reset the current offsets
	xOffset = 0 ;
	yOffset = 0 ;
	
	short minX = 0 ;
	short maxX = 0 ;
	short minY = 0 ;
	short maxY = 0 ;
	int i = 0 ;
	
	ofstream calibrationData ;
	// This is the directory that is a samba share on my raspberry pi
	calibrationData.open("/raspberry/calibration.dat") ;
	
	if (calibrationData.is_open())
	{
		// File is successfully created and open for writing
	}
	else
	{
		printf("Failed to open file for calibration... \n") ;
	}
	
	printf("Calibration :\n\n") ;
	printf("When you start the calibration rotate the sensor "
			"from left to right and vice versa.\n") ;
	printf("This takes about 1 minute!\n") ;
	printf("Press any key to start the calibration and "
			"start rotating the sensor.\n") ;
	fflush(stdout) ;
	getchar() ;
		
	calibrationData << "\n " ;
	
	for (i=0; i<500; i++)
	{
		this->updateData() ;
		
		calibrationData << x ;
		calibrationData << " " ;
		calibrationData << y ;
		calibrationData << "\n " ;
		
		if (x < minX)
			minX = x ;

		if (y < minY)
			minY=y ;

		if (x > maxX)
			maxX=x ;

		if (y > maxY)
			maxY=y ;
		delay(100) ;
	} 
	
	calibrationData.close() ;

	printf("minX: %d\n", minX) ;
	printf("minY: %d\n", minY) ;
	printf("maxX: %d\n", maxX) ;
	printf("maxY: %d\n\n", maxY) ;
	
	xOffset = (maxX + minX) / 2 ;
	yOffset = (maxY + minY) / 2 ;
	
	printf("x offset : %d\n", xOffset) ;
	printf("Y offset : %d\n", yOffset) ;
	printf("Write these offsets down and use them in the constructor!\n") ;
	
	fflush(stdout) ;
	getchar() ;
}

void HMC5883L::selectDevice(int fd, int addr, char* name)
{
	/*
	 * With the ioctl() function call we initialize the HMC3882L as 
	 * a i2c slave on the address that we found with 
	 * 'sudo i2cdetect -y 1' and declared in the integer HMC5883L_I2C_ADDR
	 * see also : http://man7.org/linux/man-pages/man2/ioctl.2.html	 
	 */
	if (ioctl(fd, I2C_SLAVE, addr) < 0)
	{
		// if ioctl() returns < 0 then we had a problem
		fprintf(stderr, "%s not present\n", name) ;
		exit(1) ;
	}	
}

void HMC5883L::writeToDevice(int fd, int reg, int val)
{
	//printf("reg = %d, val = %d\n", reg, val) ;
	
	char buf[2] ;
	buf[0]=reg ;
	buf[1]=val ;

	if (write(fd, buf, 2) != 2)
	{
		fprintf(stderr, "Can't write to ADXL345\n") ;
		exit(1) ;
	}
	else
	{
		//printf("Wrote : %s\n", buf) ;
	}
}