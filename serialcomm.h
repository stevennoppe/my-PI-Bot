/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   serialcomm.h
 * Author: s_nop
 *
 * Created on 1 januari 2017, 19:02
 */

#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>		// for exit() 
#include <strings.h>	// for bzero()
#include <unistd.h>		// for read() 
#include <string>

using namespace std;

/* 
 baudrate settings are defined in <asm/termbits.h>, which is
 included by <termios.h> 
*/
#define BAUDRATE B38400            

/* 
 change this definition for the correct port 
*/
//#define MODEMDEVICE "/dev/rfcomm0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

class serial
{
public:
	int openSerialPort(string port) ;
	int readSerialPort() ;
	void printSerialPort() ;
	string getBuffer() ;
	void restoreOldParam() ;
	int checkBuffer(unsigned int microsec) ;
	void flushBuffer() ;
	
private:
	int fd, c, res;
	struct termios oldtio,newtio;
	char buf[255];	
} ;

int serial::openSerialPort(string port)
{
	/* 
     Open modem device for reading and writing and not as controlling tty
     because we don't want to get killed if linenoise sends CTRL-C.
     */
    fd = open(port.c_str(), O_RDWR | O_NOCTTY ) ; 
		 
    if (fd <0) 
	{
		printf("Error opening port : %s !", port.c_str()) ;	
		exit(-1) ; 
	}
	else
	{
		printf("Succesfully created Bluetooth port!\n") ;
	}
	/* 
     BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     CRTSCTS : output hardware flow control (only used if the cable has
               all necessary lines. See sect. 7 of Serial-HOWTO)
     CS8     : 8n1 (8bit,no parity,1 stopbit)
     CLOCAL  : local connection, no modem contol
     CREAD   : enable receiving characters
    */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
         
    /*
     IGNPAR  : ignore bytes with parity errors
     ICRNL   : map CR to NL (otherwise a CR input on the other computer
               will not terminate input)
     otherwise make device raw (no other input processing)
    */
    newtio.c_iflag = IGNPAR | ICRNL;
         
    /*
     Raw output.
    */
    newtio.c_oflag = 0;
         
    /*
     ICANON  : enable canonical input
     disable all echo functionality, and don't send signals to calling program
    */
    newtio.c_lflag = ICANON;
         
    /* 
     initialize all control characters 
     default values can be found in /usr/include/termios.h, and are given
     in the comments, but we don't need them here
    */
	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */	
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */
        
    /* 
     now clean the modem line and activate the settings for the port
    */
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
	
	flushBuffer() ;
}

int serial::readSerialPort()
{
	/* 
	 read blocks program execution until a line terminating character is 
     input, even if more than 255 chars are input. If the number
     of characters read is smaller than the number of chars available,
     subsequent reads will return the remaining chars. res will be set
     to the actual number of characters actually read 
	*/
	res = read(fd,buf,255) ;
	buf[res]=0;
	
	tcflush(fd, TCIOFLUSH);

	return res ;
}

void serial::printSerialPort()
{
	printf("%s", buf) ;
	fflush(stdout) ;
}

string serial::getBuffer()
{
	return buf ;
}

void serial::restoreOldParam()
{
	/* 
	 restore the old port settings 
	*/
	tcsetattr(fd,TCSANOW,&oldtio);
}

int serial::checkBuffer(unsigned int microsec)
{
  fd_set set;
  struct timeval timeout;

  /* Initialize the file descriptor set. */
  FD_ZERO (&set);
  FD_SET (fd, &set);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = 0;
  timeout.tv_usec = microsec;

  /* select returns 0 if timeout, 1 if input available, -1 if error. */
  return select(fd+1, &set, NULL, NULL, &timeout);
}

void serial::flushBuffer()
{
	// Clear the buffer
	memset(buf, 0, sizeof buf);
}
#endif /* SERIALCOMM_H */
