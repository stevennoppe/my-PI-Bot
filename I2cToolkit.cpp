/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   I2cToolkit.cpp
 * Author: Steven
 * 
 * Created on 18 maart 2017, 18:32
 */

#include "I2cToolkit.h"

I2cToolkit::I2cToolkit() 
{
}

I2cToolkit::I2cToolkit(const I2cToolkit& orig) 
{
}

I2cToolkit::~I2cToolkit() 
{
}

int I2cToolkit::openI2c(const char *port)
{
    if ((fd = open(port, O_RDWR)) < 0) 
    // on a Raspberry Pi - model B rev 1 use : '/dev/i2c-0'
    {
	// If it returns < 0 then something was wrong
	// exit function with a -1 : failed to open de port
	return -1 ;
    }
    return 0 ;
}

void I2cToolkit::scanI2c(const char *port)
{
    for (char i=0; i < 128; i+=16)
    {
        for (char address = 0; address < 16; address++)
        {
            //printf("0x%02x (%d) - ", i+address, i+address) ;
            if (ioctl(fd, I2C_SLAVE, i+address) < 0)
            {
                // Nothing found
                //printf("Address : 0x%02x\n", i+address) ;
            }
            else
            {
                // probe the address!
                int res = i2c_smbus_write_quick(fd, I2C_SMBUS_WRITE); 

                if (res < 0)
                {
                    // nothing found
                    //printf("Nothing found at 0x%02x\n", i+address) ;
                }
                else
                {
                    // found a slave
                    printf("Found a slave at 0x%02x\n", i+address) ;
                }
            }
        }
    }
}

int I2cToolkit::writeI2c(int address, char firstByte, char secondByte)
{
    ssize_t result ;
    char buf[2] ;
    	
    buf[0]=firstByte ;          // Doesn't matter what I enter here???
    buf[1]=secondByte ;         // value = 00 -> FF (0 -> 255)

    if (ioctl(fd, I2C_SLAVE, address) < 0)
            {
                // Nothing found
                //printf("Address : 0x%02x\n", i+address) ;
            }
            else
            {
                if ((result = write(fd, buf, 2)) != 2)
                {
                    printf("Can't write to device! Only written %d byte(s)\n", result) ;
                    return 1 ;
                }
            }
    return 0 ;
}

int I2cToolkit::writeI2c(int address, char firstByte)
{
    ssize_t result ;
    char buf[1] ;
    	
    buf[0]=firstByte ;          // Doesn't matter what I enter here???
    
    if (ioctl(fd, I2C_SLAVE, address) < 0)
            {
                // Nothing found
                //printf("Address : 0x%02x\n", i+address) ;
            }
            else
            {
                if ((result = write(fd, buf, 1)) != 1)
                {
                    printf("Can't write to device! Only written %d byte(s)\n", result) ;
                    return 1 ;
                }
            }
    return 0 ;
}

char I2cToolkit::convert8BitToChar(const char* bits)
{
    int integer = 0 ;
    int index = 0 ;
    int bit = 1 ;
    
    if (strlen(bits) != 8)
    {
        printf("8 bits opgeven (te weinig characters)!") ;
        return integer ;
    }
    
    for (int n = 7; n >= 0; n--)
    {
        //printf("%d - %c\n", n, bits[n]) ;
        
        if (bits[n] >= '0' && bits[n] <= '1')
        {
            // alles ok, doe conversie
            index = 8-n ;
            if (bits[n] == '1')
                integer += bit ;
            bit = bit*2 ;            
        }
        else
        {
            printf("Enkel 1-en of 0-en!") ;
            return -1 ;
        }
    }
    return integer ;
}

char I2cToolkit::readI2c(int address)
{
    ssize_t result ;
     
    char buf[1] ;
    
    buf[0] = 255 ;
            
    if (ioctl(fd, I2C_SLAVE, address) < 0)
    {
        // Nothing found
        //printf("Address : 0x%02x\n", i+address) ;
    }
    else
    {
        if ((result = write(fd, buf, 1)) != 1)
        {
            printf("Can't write to device! Only written %d byte(s)\n", result) ;
        }
        
        if ((result = read(fd, buf, 1)) != 1)
        {
            printf("Can't read from device!\n") ;
        }
    }
    
    printf("buffer inside function : %c\n", buf) ;
    return buf[1] ;
}