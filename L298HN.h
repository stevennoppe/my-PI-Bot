/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   L298HN.h
 * Author: s_nop
 *
 * Created on 15 november 2016, 18:37
 */

#ifndef L298HN_H
#define L298HN_H

/* 
 * Enable A and Enable B are used for setting the speed on a L298N
 * Here we use wiringPi output 0 and 3
 */
#define ENA 0
#define ENB 3

/* 
 * Enable 1 and Enable 2 are used for setting the direction of motor 1
 * Here we use wiringPi output 1 and 2
 *
 * forward = EN1->high and EN2->low
 * reverse = EN1->low and EN2->high
 */
#define EN1 1
#define EN2 2

/* 
 * Enable 3 and Enable 4 are used for setting the direction of motor 2
 * Here we use wiringPi output 4 and 5
 *
 * forward = EN3->high and EN4->low
 * reverse = EN3->low and EN4->high
 *
 * Note : I reversed the wires on the OUT3 and OUT4
 */
#define EN3 4
#define EN4 5


#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

class L298HN
{
	private:
		
	public:
		void forward(int motor, int speed) ;
		void reverse(int motor, int speed) ;
		void stop(int motor) ;
		void setupMotors() ;
} ;
#endif /* L298HN_H */

