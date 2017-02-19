#include <stdlib.h>
#include "osapi.h"

#include "Epson.h"
#include "serialControl.h"

static char readBuf[64] = {0};


int Epson_init(void){
	SerialControl_flushRx();
	// Try to talk to projector and see if it responds as we expect
	SerialControl_write("\r");
	os_delay_us(1000000); // 1 second
	// we anticipate timeout
	int numBytes = SerialControl_read(readBuf, 16);

	os_printf("Read back %d bytes\n", numBytes);
	os_printf("readBuf[0] = %c and readBuf[1] = %c\n", readBuf[0], readBuf[1]);

	if(numBytes != 2){
		return -1;
	}
	// Expecting to get a single colon response
	if( ((int)readBuf[0] != (int)'\r') && ((int)readBuf[1] != (int)':') ){
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
	if (bytes <= 0){
		os_printf("We didn't read any data\n");
		return -1;
	}

	readBuf[bytes] = '\0';
	os_printf("Read back %d bytes\n", bytes);
	os_printf("Buffer looks like: %s\n", readBuf);
	
	if(strncmp("PWR", readBuf, 3) != 0){
		os_printf("Bad or invalid response\n");
		return -1;
	}

	rawPwrState = atoi(&readBuf[4]);
	os_printf("Converting power code to int = %d\n", rawPwrState);
	*pwrState = (PowerStateEnum)rawPwrState;
	return 0;
}

int Epson_PowerOn(void){
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();
	SerialControl_write("PWR ON\r");
	SerialControl_flushRx();
}

int Epson_PowerOff(void){
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();
	SerialControl_write("PWR OFF\r");
	SerialControl_flushRx();
}

int Epson_GetFilterUseTime(int* useTime){
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();

	SerialControl_write("FILTER?\r");
	int bytes = SerialControl_read(readBuf, 12);
	os_printf("Read back %d bytes\n", bytes);
	readBuf[bytes] = '\0';
	os_printf("Buffer looks like: %s\n", readBuf);

	if(strncmp("ERR", readBuf, 3) != 0){
		os_printf("Bad or invalid response\n");
		return -1;
	}

	*useTime = atoi(&readBuf[4]);
	os_printf("Converting power code to int = %d\n", *useTime);
	SerialControl_flushRx();
	return 0;
}

int Epson_GetErrors(ProjectorErrorEnum* error){
	int rawError = 0;
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();

	SerialControl_write("ERR?\r");
	int bytes = SerialControl_read(readBuf, 12);
	os_printf("Read back %d bytes\n", bytes);
	readBuf[bytes] = '\0';
	os_printf("Buffer looks like: %s\n", readBuf);

	if(strncmp("ERR", readBuf, 3) != 0){
		os_printf("Bad or invalid response\n");
		return -1;
	}

	rawError = atoi(&readBuf[4]);
	os_printf("Converting power code to int = %d\n", rawError);
	*error = (ProjectorErrorEnum)rawError;
	SerialControl_flushRx();
	return 0;
}

int Epson_VolumeChange(VolumeDirection direction){
	// Get rid of any leftover junk in our rx buffer
	SerialControl_flushRx();
	if(direction == e_volumeUp){
		SerialControl_write("VOL INC\r");
	}
	else if(direction == e_volumeDown){
		SerialControl_write("VOL DEC\r");
	}
	else{
		os_printf("value volume specified\n");
	}
	SerialControl_flushRx();
	return 0;
}

void Epson_ProcessCommand(int commandId){
	PowerStateEnum pwrState;
	ProjectorErrorEnum projectorError;
	int filterUseTime = 0;

	switch(commandId){
		case 0:
			os_printf("Powering off projector...\n");
			Epson_PowerOff();
			break;
		case 1:
			os_printf("Powering on projector...\n");
			Epson_PowerOn();
			break;
		case 2:
			os_printf("Getting power state of projector...\n");
			if( Epson_GetPowerState(&pwrState) != 0) {
				os_printf("Failed to get power state of projector\n");
				//publish_message(Shit went wrong);
			}
			break;
		case 3: 
			os_printf("Getting projector filter use time...\n");
			if( Epson_GetFilterUseTime(&filterUseTime) != 0) {
				os_printf("Failed to get filter time of projector\n");
				//publish_message(Shit went wrong);
			}
			break;
		case 4:
			os_printf("Getting projector error status...\n");
			if( Epson_GetErrors(&projectorError) != 0) {
				os_printf("Failed to get error status of projector\n");
				//publish_message(Shit went wrong);
			}
		case 5: // Volume Up
			os_printf("Turning the projector volume up one...\n");
			Epson_VolumeChange(e_volumeUp);
			break;
		case 6: // Volume Down
			os_printf("Turning the projector volume down one...\n");
			Epson_VolumeChange(e_volumeDown);
			break;
		default:
			break;
	}
}


