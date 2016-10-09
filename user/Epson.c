#include <stdlib.h>
#include "osapi.h"

#include "Epson.h"
#include "serialControl.h"

static char readBuf[64] = {0};
static PowerStateEnum pwrState;


int Epson_init(void){
	SerialControl_flushRx();
	// Try to talk to projector and see if it responds as we expect
	SerialControl_write("\r");
	os_delay_us(100000); // 100ms
	// we anticipate timeout
	int numBytes = SerialControl_read(readBuf, 16);

	if(numBytes != 1){
		return -1;
	}
	// Expecting to get a single colon response
	if( (int)readBuf[0] != (int)':'){
		return -1;
	}
	return 0;
}


int Epson_GetPowerState(PowerStateEnum* pwrState){
	int rawPwrState = 0;
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();

	SerialControl_write("PWR?\r");
	// ReadBack PWR=XX
	int bytes = SerialControl_read(readBuf, 64);
	if(bytes>0){
		if(strncmp("PWR", readBuf, 3) == 0){
			os_printf("Read back %d bytes\n", bytes);
			os_printf("Buffer looks like: %s\n", readBuf);
			rawPwrState = atoi(&readBuf[4]);
			os_printf("Converting power code to int = %d\n", rawPwrState);
			*pwrState = (PowerStateEnum)rawPwrState;
		}
		else if(strncmp("ERR", readBuf, 3) == 0){
			os_printf("Got error\n");
			return -1;
		}
	}
	else{
		os_printf("We didn't read any data");
		return -1;
	}
	return 0;
}

int Epson_PowerOn(void){
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();

	SerialControl_write("PWR ON");
	int bytes = SerialControl_read(readBuf, 12);
	os_printf("Read back %d bytes\n", bytes);
	readBuf[bytes] = '\0';
	os_printf("Buffer looks like: %s\n", readBuf);
	SerialControl_flushRx();
}

int Epson_PowerOff(void){
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();

	SerialControl_write("PWR OFF");
	int bytes = SerialControl_read(readBuf, 12);
	os_printf("Read back %d bytes\n", bytes);
	readBuf[bytes] = '\0';
	os_printf("Buffer looks like: %s\n", readBuf);
	SerialControl_flushRx();
}

void Epson_ProcessCommand(int commandId){
	switch(commandId){
		case 0:
			os_printf("Powering on projector...\n");
			Epson_PowerOn();
			break;
		case 1:
			os_printf("Powering off projector...\n");
			Epson_PowerOff();
			break;
		case 2:
			os_printf("Getting power state of projector...\n");
			if( Epson_GetPowerState(&pwrState) != 0) {
				//publish_message(Shit went wrong);
			}
			//publish_message(here is status);
			break;
		default:
			break;
	}
}


