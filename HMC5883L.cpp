#include "HMC5883L.h"

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
		printf("Failed to open i2c bus!\n") ;
		// exit program with a 1 (failure)
		exit(1) ;
	}
	else 
	{
		// successfull !
		printf("Succesfully openend i2c bus.\n") ;
	}

	/* 
	 * Initialise the HMC5883L with the integer 'fd' that was returned from the 
	 * open system call
	 *
	 * Find out which address the HMC5883L is on with the command : 
	 * 'sudo i2cdetect -y 1' on a Raspberry Pi - model A, B rev 2 of B+
	 * 'sudo i2cdetect -y 0' on a Raspberry Pi - model B rev 1
	 * 1E => 0x1E 
	 */
		
	/*
	 * With the ioctl() function call we initialize the HMC3882L as 
	 * a i2c slave on the address that we found with 
	 * 'sudo i2cdetect -y 1' and declared in the integer HMC5883L_I2C_ADDR
	 * see also : http://man7.org/linux/man-pages/man2/ioctl.2.html	 
	 */
	if (ioctl(fd, I2C_SLAVE, 0x1E) < 0)
	{
		// if ioctl() returns < 0 then we had a problem
		printf("HMC5883L not present!\n") ;
		exit(1) ;
	}		

	/* 
	 * Now write some values to the HMC5883L to initalize it
	 */
	
	/* 
	 * First write the value 32 to the configuration register A
	 * 32 = Gain = 1090 Lsb/Gauss
	 * We send the following byte : 00100000
	 */
	char buf[2] ;
	
	buf[0]=0x01 ;	// 0x01 = configuration register A
	buf[1]=32 ;		// value = 32

	if (write(fd, buf, 2) != 2)
	{
		printf("Can't write to device!\n") ;
		exit(1) ;
	}
	
	/* 
	 * Second write the value 0 to the mode register
	 * 0 = Continuous-Measurement Mode
	 * We send the following byte : 0000000
	 */
	buf[0]=0x02 ;	// 0x02 = Mode register
	buf[1]=0 ;		// value = 0
	
	if (write(fd, buf, 2) != 2)
	{
		printf("Can't write to device!\n") ;
		exit(1) ;
	}
}
	
void HMC5883L::updateData()
{
	buf[0] = 0x03 ;

    if ((write(fd, buf, 1)) != 1)
    {
        // Send the register to read from
        printf("Error writing to i2c slave!\n") ;
    }
	else
	{
		//printf("Wrote to i2c slave.\n") ;
	}

	if (read(fd, buf, 6) != 6) 
	{
		printf("Unable to read from HMC5883L!\n") ;
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

void HMC5883L::calibrateHMC5883L()
{
	// Get the current date and time 
	time_t result ; 
	time(&result);
	
	// create a tm structure with the time info in it
	struct tm *tmInfo ;
	tmInfo = localtime(&result) ;
	
	// Reset the current offsets
	xOffset = 0 ;
	yOffset = 0 ;
	
	int minX = 0 ;
	int maxX = 0 ;
	int minY = 0 ;
	int maxY = 0 ;
	int i = 0 ;
	
	ofstream calibrationData ;
	
	// create a filename with date and time :
	// This is the directory that is a samba share on my raspberry pi
	const char *directory = "/raspberry/" ;
	
	// name of file is calibration + date + time with an extension ".dat"
	const char *filename = "calibration" ;
	const char *extension = ".dat" ;
	stringstream fullFileName ;
	
	fullFileName << directory
			<< filename 
			<< 1900+tmInfo->tm_year 
			<< setw(2) << setfill('0') << 1+tmInfo->tm_mon 
			<< tmInfo->tm_mday << "-" 
			<< setw(2) << setfill('0') << tmInfo->tm_hour 
			<< setw(2) << setfill('0') << tmInfo->tm_min 
			<< extension 
			<< '\0' ;
			
	
	calibrationData.open(fullFileName.str().c_str());
	
	if (calibrationData.is_open())
	{
		// File is successfully created and open for writing
		printf("Opened the file %s for writing... \n", 
				fullFileName.str().c_str()) ;
		
		// Put the current date and time in the calibration file
		calibrationData << asctime(gmtime(&result)) ;
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
	
	printf("Start turning!\n") ;
	calibrationData << "\n " ;
	
	for (i=0; i<=500; i++)
	{
		printf("%d\n", i) ;
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
		
		dn.waitMilliSec(100) ;				// wait 0.1 seconds
	} 
	
	// write everything into the calibration file and close it for writing
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

void HMC5883L::calibrateHMC5883L(int xOffs, int yOffs)
{
	// Get the current date and time 
	time_t result ; 
	time(&result);
	
	// create a tm structure with the time info in it
	struct tm *tmInfo ;
	tmInfo = localtime(&result) ;
	
	// Reset the current offsets
	xOffset = xOffs ;
	yOffset = yOffs ;
	
	int minX = 0 ;
	int maxX = 0 ;
	int minY = 0 ;
	int maxY = 0 ;
	int i = 0 ;
	
	ofstream calibrationData ;
	
	// create a filename with date and time :
	// This is the directory that is a samba share on my raspberry pi
	const char *directory = "/raspberry/" ;
	
	// name of file is calibration + date + time with an extension ".dat"
	const char *filename = "calibration" ;
	const char *extension = ".dat" ;
	stringstream fullFileName ;
	
	fullFileName << directory
			<< filename 
			<< 1900+tmInfo->tm_year 
			<< setw(2) << setfill('0') << 1+tmInfo->tm_mon 
			<< tmInfo->tm_mday << "-" 
			<< setw(2) << setfill('0') << tmInfo->tm_hour 
			<< setw(2) << setfill('0') << tmInfo->tm_min 
			<< extension 
			<< '\0' ;
			
	
	calibrationData.open(fullFileName.str().c_str());
	
	if (calibrationData.is_open())
	{
		// File is successfully created and open for writing
		printf("Opened the file %s for writing... \n", 
				fullFileName.str().c_str()) ;
		
		// Put the current date and time in the calibration file
		calibrationData << asctime(gmtime(&result)) ;
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
	
	printf("Start turning!\n") ;
	calibrationData << "\n " ;
	
	for (i=0; i<=500; i++)
	{
		printf("%d\n", i) ;
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
		
		dn.waitMilliSec(100) ;				// wait 0.1 seconds
	} 
	
	// write everything into the calibration file and close it for writing
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

