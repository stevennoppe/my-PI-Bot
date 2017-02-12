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
#include <string.h>
#include <string>

using namespace std;

/* 
 baudrate settings are defined in <asm/termbits.h>, which is
 included by <termios.h> 
*/
#define BAUDRATE B38400            

#define FALSE 0
#define TRUE 1

//volatile int STOP=FALSE; 

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


#endif /* SERIALCOMM_H */

