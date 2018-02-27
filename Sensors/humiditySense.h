/*
 * humiditySense.h
 *
 *  Created on: Feb 25, 2018
 *      Author: SteliosSoton
 */

#ifndef HUMIDITYSENSE_H
#define HUMIDITYSENSE_H

#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"

#define HUMIDITY_SENSORS 1

void humiditySense(void);
void initHumiditySense(void);

volatile uint16_t humidityResults[HUMIDITY_SENSORS +1]; /*array for holding humidity sensor outputs.
														First bit used as a counter so interupt
														knows which bit to place result in*/

void humiditySense(void){



	TCNT1 = 0x0000; /* Clear Timer 1 */


	int i;/* Isn't INT obvious? I'll see myself out...*/


	for( i=1; i<(HUMIDITY_SENSORS+1); i++){/* Go through all the soil humidity sensors*/

	if(i==1) PINC |= _BV(0); /*Activate first Sensor*/
	else if (i==2)PINC |= _BV(1);/*Activate second Sensor*/



	TCCR1B |= _BV(CS12)| _BV(CS11) | _BV(CS10); /* Start Timer 1 :External Clock on T1 Pin, rising edge */

	_delay_ms(10); /*Wait a fixed amount of time for general reference*/



	TCCR1B &= ~_BV(CS12) & ~_BV(CS11) & ~_BV(CS10);/* Stop Timer 1*/



	humidityResults[0] = i; /*Increment sensor count bit*/
	humidityResults[i] = TCNT1; /* Read Timer 1*/
	TCNT1L = 0x0000; /* Clear Timer 1 */
	TCNT1H = 0x0000; /* Clear Timer 1 */

	_delay_ms(1000);

	if(i==1) PINC |= _BV(0); /*Deactivate first Sensor*/
	else if (i==2) PINC |= _BV(1);/*Deactivate second Sensor*/
	}

}

void initHumiditySense(void){

	DDRB &= ~_BV(1);/*Set Pin B1 as input*/
	PORTB &= ~_BV(1);/*Pull down the resistor*/

	TCCR1A= 0x00;
	TCCR1B= 0x00;
	TCCR1C= 0x00;
	TCNT1 = 0x0000;

	DDRC |= _BV(0) | _BV(1);/*Set Pins C0, C1 as outputs (for activating the humidity sensors)*/
	PORTC &= ~_BV(0) & ~_BV(1);/*Pull down the resistors*/

}
#endif /* HUMIDITY_SENSE_H */
