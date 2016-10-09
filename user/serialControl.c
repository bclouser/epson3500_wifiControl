#include "osapi.h"

#include "serialControl.h"
#include "softuart.h"

static const int timeout_msecs = 400;
static Softuart softuart;

void ICACHE_FLASH_ATTR SerialControl_init(void){
	//init software uart
	Softuart_SetPinRx(&softuart, 14);    
	Softuart_SetPinTx(&softuart, 12);

	//startup
	Softuart_Init(&softuart, 9600);
}

void SerialControl_flushRx(void){
	while(Softuart_Available(&softuart))
	{
		// read all available bytes out, throw em away
	    Softuart_Read(&softuart);
	}
}

void ICACHE_FLASH_ATTR SerialControl_write(const char* buf){
	Softuart_Puts(&softuart, buf);
}

int ICACHE_FLASH_ATTR SerialControl_read(char* buf, int len){
	int i = 0;
	int timeSpentWaiting = 0;
	for(i = 0; i<len; ++i){
		// wait for bytes to show up or timeout
		while(!Softuart_Available(&softuart)){
			// 10 ms at a time
			os_delay_us(10000);
			timeSpentWaiting += 10;
			// Check against timeout
			if(timeSpentWaiting > timeout_msecs){
				return i;
			}
		}
		buf[i] = Softuart_Read(&softuart);
	}
	return len;
}




