/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   I2cToolkit.h
 * Author: Steven
 *
 * Created on 18 maart 2017, 18:32
 */

#ifndef I2CTOOLKIT_H
#define I2CTOOLKIT_H

#include <linux/i2c-dev.h>              // for the ioctl() function
#include <unistd.h>			// for the read() and write() function
#include <fcntl.h>			// for the open() function
#include <stdio.h>
#include <string.h>                     // for the strlen() function

class I2cToolkit {
public:
    I2cToolkit();
    I2cToolkit(const I2cToolkit& orig);
    virtual ~I2cToolkit();
    
    int openI2c(const char *port) ;
    void scanI2c(const char *port) ;
    int writeI2c(int address, char firstByte, char secondByte) ;
    int writeI2c(int address, char firstByte) ;
    char readI2c(int address) ;
    char convert8BitToChar(const char *bits) ;
    
private:
    int fd ;
};

#endif /* I2CTOOLKIT_H */

