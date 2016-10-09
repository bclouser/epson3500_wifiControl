#include "ets_sys.h"

typedef enum{
	e_standby = 0,
	e_poweredOn,
	e_warmUp,
	e_coolDown,
	e_networkStandby,
	e_abnormalStandby
}PowerStateEnum;

typedef enum{
	e_errNone=0x00, //There is no error or the error is recovered
	e_errFan=0x01, //Fan error
	e_errLampPwrON=0x03, //Lamp failure at power on
	e_errHighTemp=0x04, //High internal temperature error
	e_errLamp=0x06, //Lamp error
	e_errLampCover=0x07, //Open Lamp cover door error
	e_errCinemaFilter=0x08, //Cinema filter error
	e_errCapDisconnect=0x09, //Electric dual-layered capacitor is disconnected
	e_errAutoIris=0x0A, //Auto iris error
	e_errSubsystem=0x0B, //Subsystem Error
	e_errLowAirFlow=0x0C, //Low air flow error
	e_errAirFilter=0x0D, //Air filter air flow sensor error
	e_errPwrSupply=0x0E, //Power supply unit error (Ballast)
	e_errShutter=0x0F, //Shutter error
	e_errCoolPeltiert=0x10, //Cooling system error (peltiert element)
	e_errCoolPump=0x11 //Cooling system error (Pump)
}ProjectorErrorEnum;


int Epson_GetPowerState(PowerStateEnum* pwrState);