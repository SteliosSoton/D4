/*
 *      Author: Nathan
 *      Contributions: Matt
 */

#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <string.h>

#define STANDBY 4
#define IDLE 0

#define START 0x7e
#define END 0x7f

#define WATER_PLANT 0x01			// header tags for data sent in "command"
#define REQUEST_SENSOR_DATA 0x02	// actually hex data/command sent in "commadInfo"
#define AUDIO 0x03

typedef struct data { //Create new type for holding transmission data
	char version; //Version of device sending the data
	char command; //Command hex code
	char commandInfoLength; //Length of data sent over command and before end
	char commandInfo[50]; //Buffer for command info
	char feedback; //If sender wants data in return
}data; //Call this new type of variable 'data'

volatile data receive; //Create new instance of data for receive transmissions
volatile data transmit; //transmit transmissions

void init_interrupts(void) {
    sei(); 
}
void init_spi_master(void) {
	// out: MOSI, SCK, SS (in: MISO)
	DDRB = _BV(PB4) | _BV(PB5) | _BV(PB7); // Set = 1: PB4 (SS), PB5 (MOSI), PB7(SCK)

	PORTB |= _BV(PB4); //Set SS pin high as this is default setting for SPI communications

	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0) | _BV(SPR1);	// SPI Control Register
    SPCR &= ~_BV(SPI2X);
	// SPE - the SPI is enabled when this bit is 1
	// MSTR - configures SPI as master
	// SPI2X, SPR0, SPR1 - configure SPI clock frequency (0 1 1 fosc/128)
}

void sendByte(char byte) { //Regular function to send a byte over SPI
	PORTB &= ~_BV(PB4); //Set SS low
	SPDR = byte; //Put byte in SPI register
	while(!(SPSR & _BV(SPIF))); //Wait for transfer complete
	PORTB |= _BV(PB4); //set SS high
	_delay_ms(100);
}

char receiveByte(void) {
	PORTB &= ~_BV(PB4); //Set SS low
	SPDR = 0x01; //Random data to initialize salve to master transfer
	while(!(SPSR & _BV(SPIF))); //Wait for transfer complete
	char receivedByte = SPDR; //Read data transfered from slave to master
	PORTB |= _BV(PB4); //Set SS high
	_delay_ms(100);
	return receivedByte; //Return read byte
}
void receiveData(void) {
	char loopTerminator = 0x00;
	while(!(loopTerminator == END)) {
		if(receiveByte() == START)	{ //Only record data once start byte has been seen
			receive.version = receiveByte();
			receive.command = receiveByte();
			receive.commandInfoLength = receiveByte();
			for(uint8_t i = 0; i < (receive.commandInfoLength); i++) //Loop for amount of times slave said it would transfer commandinfo data
				receive.commandInfo[i] = receiveByte();
			receive.feedback = receiveByte();
			loopTerminator = END;
		}
	}
}

void sendData(data sendData) {
	sendByte(START);
	sendByte(sendData.version);
	sendByte(sendData.command);
	sendByte(sendData.commandInfoLength);
	for(uint8_t i = 0; i < sendData.commandInfoLength; i++)
		sendByte(sendData.commandInfo[i]);
	sendByte(sendData.feedback);
	sendByte(END);

	if(sendData.feedback) //If feedback was selected the master will now wait for said data
		receiveData();
}

// UART in/out control
char get_ch(void)
{
	while(!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

void put_ch(char ch)
{
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = ch;
}

void put_str(char *str)
{
	int i;
	for(i=0; str[i]; i++) put_ch(str[i]);
}

// END UART input control


int main(void)
{
	init_spi_master();	// initialise SPI as master or slave
	init_debug_uart0();	// initialise UART debug
    init_interrupts();
    
    char ch;

    for (;;)
    {
        	/* get character from UART */
        	ch = get_ch();
        	/* send message back to the host terminal */
        	put_str("You sent the character '");
            _delay_ms(10);

        	put_ch(ch);	// UART out
        	put_str("'.\n\r");
            _delay_ms(10);

            switch(ch){
            case '1':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x22;
            	transmit.commandInfo[1] = 0x1E;
            	transmit.commandInfo[2] = 0x01;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '2':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x22;
            	transmit.commandInfo[1] = 0x1E;
            	transmit.commandInfo[2] = 0x02;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '3':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x22;
            	transmit.commandInfo[1] = 0x1E;
            	transmit.commandInfo[2] = 0x03;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '4':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x22;
            	transmit.commandInfo[1] = 0x1E;
            	transmit.commandInfo[2] = 0x04;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case 'p':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x0E;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case 'r':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x0D;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '+':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x04;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '-':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x05;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '<':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x02;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case '>':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x01;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case 's':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x16;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case 'z':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x0A;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            case 'w':
            	transmit.version = 0x01;
            	transmit.command = AUDIO;
            	transmit.commandInfoLength = 0x03;
            	transmit.commandInfo[0] = 0x0B;
            	transmit.commandInfo[1] = 0x00;
            	transmit.commandInfo[2] = 0x00;
            	transmit.feedback = 0x00;
            	sendData(transmit);
            	break;
            default:
            	put_str("\nThat input does not match a command (switch defaulted)\n[1 2 3 4 p r + - < > s z w]\n\r");
            	break;
            }

		/* E.G of Send + Feedback
		transmit.version = 0x01;
		transmit.command = REQUEST_SENSOR_DATA;
		transmit.commandInfoLength = 0x03;
		transmit.commandInfo[0] = 0x01;
		transmit.commandInfo[1] = 0x01;
		transmit.commandInfo[2] = 0x01;
		transmit.feedback = 0x01;
		sendData(transmit);

		printf("\nreceive.version: %x", receive.version); //Print received data collected by sendData as feedback was set to 1 in previous transmission
		printf("\nreceive.command: %x", receive.command);
		printf("\nreceive.commandInfoLength: %x", receive.commandInfoLength);
		for(uint8_t i = 0; i < (receive.commandInfoLength); i++)
			printf("\nreceive.commandInfo: %x", receive.commandInfo[i]);
		printf("\nreceive.feedback: %x", receive.feedback);

		_delay_ms(1000);
		*/

	}
}
