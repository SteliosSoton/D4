/*
 * interpretSPI.h
 *
 *  Created on: 26 Feb 2018
 *      Author: mp7g16
 */

#ifndef INTERPRETSPI_H_
#define INTERPRETSPI_H_

#include "UART_audio_module.h"
#include "pump_control.h"

#define WATER_PLANT 0x01
#define REQUEST_SENSOR_DATA 0x02
#define AUDIO 0x03

void testReceived(SPIdata received){
    	// Check command to see what to do (where to send the "commandInfo")
    	switch(received.command){
    	case WATER_PLANT:	// water plant
    		printf("\nWatering Plant...");
    		togglePump(received.commandInfo[0]);
    		break;
    	case REQUEST_SENSOR_DATA:	// request data
    		printf("\nGetting ADC data...");
    		//getADCdata();
    		break;
    	case AUDIO:	// audio
    		sendCommand(received.commandInfo[0], received.commandInfo[1], received.commandInfo[2]);
    		break;
    	}
}

#endif /* INTERPRETSPI_H_ */
