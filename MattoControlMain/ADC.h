/*
 * ADC.h
 *
 *  Created on: Feb 25, 2018
 *      Author: NRS1G15
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "debug.h"
#include <util/delay.h>
#include "power_saving.h"
#include "sensors.h"
#include <stdio.h>
#include <stdlib.h>

#define SENSOR_COUNT 2

void init_adc(void);
uint16_t *getSensorData(void);
uint16_t *getSensorConvertedData(void);

volatile uint16_t ADCResults[SENSOR_COUNT]; //array for holding adc conversion outputs. First bit used as a counter so interrupt knows which bit to place result in

void init_adc(void)
{
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1); //ADC rate
    ADMUX |= _BV(REFS0) | _BV(REFS1); //Reference voltages for all
    ADCSRA |= _BV(ADIE); //Enable ADC interrupt
    sei();
}


ISR(ADC_vect) //interrupt handler for completed adc conversion
{
    uint16_t result = ADC; //Read ADC register value
    ADCResults[0] = ADCResults[0] + 1; //Add one to ADC results counter bit
    ADMUX = 0xc0 + ADCResults[0]; //Move channel along by 1 each time
    ADCResults[ADCResults[0]] = result; //Save result to ADCresults array
    ADCSRA |= _BV(ADSC); //Enable ADC for another conversion
}

uint16_t *getSensorData(void)
{
    ADCResults[0] = 0; //Reset ADCResalt counter bit
    ADMUX = 0xc0; //Reset ADC channel back to 0
    ADCSRA |= _BV(ADEN); //Start conversion
    while(ADCResults[0] < SENSOR_COUNT) { //Repeat loop to keep IC in IDLE while conversion is done
        enterSleepMode(IDLE);
    }
    ADCSRA &= ~_BV(ADEN); //Ensure ADC is disabled by clearing by manually

    _delay_ms(2); //Device enters sleep too fast for UART to send data
    return getSensorConvertedData();
}

uint16_t *getSensorConvertedData(void)
{
	static uint16_t *sensorData = 0; //Static so only created once and not a new instance each time...thats a memory leak!
	if(sensorData == 0) //if sensor data is empty aka first time initialized
		sensorData = malloc(SENSOR_COUNT); //Creates memory space size of sensor count. This memory is used as an array to store the sensor data.
    for(uint8_t i = 0; i < SENSOR_COUNT; i++) {
    	switch(i) {
    	case 0:
    		sensorData[0] = getBatteryLevel(ADCResults[i + 1]); //Battery level sensor assumed to be on ADC channel 0
    		printf("\nBattery level: %d", sensorData[0]);
    		break;
    	case 1:
    		sensorData[1] = getTemperature(ADCResults[i + 2]); //Temperature sensor assumed to be on ADC channel 1
    		printf("\nBattery level: %d", sensorData[1]);
    		break;
    	case 2:
    		sensorData[2] = getLDRpercentage(ADCResults[i + 3]); //Temperature sensor assumed to be on ADC channel 1
    		printf("\nBattery level: %d", sensorData[2]);
    		break;
    	}
    	printf("\nChannel %d: %4d", i, ADCResults[i + 1]);
    }
    return sensorData;
}

#endif /* ADC_H_ */