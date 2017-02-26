#include "serialcomm.h"

int serial::openSerialPort(string port)
{
	/* 
     Open serial device for reading and writing and not as controlling tty
     because we don't want to get killed if linenoise sends CTRL-C.
     */
    fd = open(port.c_str(), O_RDWR | O_NOCTTY ) ; 
		 
    if (fd < 0) 
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
     now clean the serial line and activate the settings for the port
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
	int result = read(fd, buf, 255) ;
	buf[result] = 0 ;
	
	tcflush(fd, TCIFLUSH) ;

	return result ;
}

ssize_t serial::writeSerialPort(char const *string)
{
	/*
	 write a string to the serial connection
	 */
	
	ssize_t result = write(fd, string, strlen(string)) ;
	
	tcdrain(fd);
	
	/*if (result != strlen(string))
		printf("Failed to write to serial port!\n") ;
	else
		printf("Wrote to serial port!\n") ;
	*/
	return result ;
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
