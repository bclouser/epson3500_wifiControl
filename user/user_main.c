#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"

#define GLOBAL_DEBUG_ON

/* non sdk includes */
#include "secrets.h"
#include "messageHandler.h"
#include "mqtt.h"
#include "wifi.h"
#include "debug.h"
#include "Epson.h"

// Application global
MQTT_Client mqttClient;
static void ICACHE_FLASH_ATTR wifiConnectCb(uint8_t status)
{
    if (status == STATION_GOT_IP) {
        os_printf("Wifi Connected!\r\n");
        // led 1 means we got an ip address connected
        MQTT_Connect(&mqttClient);
    }
    else {
        MQTT_Disconnect(&mqttClient);
    }
}
static void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Connected\r\n");
    MQTT_Subscribe(client, "/projectorControl_SOMEHASH", 0);

    MQTT_Publish(client, "/device/heartbeat", "hello2", 6, 2, 0);


    if( Epson_init() != 0){
        os_printf("Hmmm, communicating with the projector failed\n");
        return;
    }
    
    os_delay_us(200000); // 200 ms

    PowerStateEnum pwrState = 0;
    if( Epson_GetPowerState(&pwrState) < 0){
        os_printf("Getting the power state totally failed\n");
        return;
    }

    // give it a second
    os_delay_us(200000); // 200 ms

    switch(pwrState){
        case e_standby:
            // turn on projector
            os_printf("Projector is off, so im gunna turn it on\n");
            Epson_PowerOn();
            break;
        case e_poweredOn:
            os_printf("Projector is on, so im gunna turn it off\n");
            Epson_PowerOff();
            break;
        case e_warmUp:
            os_printf("Projector is warmingUp, Doing Nothing\n");  
            break;
        case e_coolDown:
            os_printf("Projector is cooling down, Doing Nothing\n");
            break;
        case e_networkStandby:
        case e_abnormalStandby:
        default:
            os_printf("Current power state is %d\n", pwrState);
            break;
    }
}

static void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Disconnected\r\n");
}

static void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Published\r\n");
}

static void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len + 1), 
        *dataBuf = (char*)os_zalloc(data_len + 1);

    MQTT_Client* client = (MQTT_Client*)args;
    os_memcpy(topicBuf, topic, topic_len);
    topicBuf[topic_len] = 0;
    os_memcpy(dataBuf, data, data_len);
    dataBuf[data_len] = 0;
    os_printf("Received topic: %s, data: %s \r\n", topicBuf, dataBuf);
    handleMessage(dataBuf, data_len);
    os_free(topicBuf);
    os_free(dataBuf);
}
/*
void ICACHE_FLASH_ATTR print_info()
{
  INFO("\r\n\r\n[INFO] BOOTUP...\r\n");
  INFO("[INFO] SDK: %s\r\n", system_get_sdk_version());
  INFO("[INFO] Chip ID: %08X\r\n", system_get_chip_id());
  INFO("[INFO] Memory info:\r\n");
  system_print_meminfo();

  INFO("[INFO] -------------------------------------------\n");
  INFO("[INFO] Build time: %s\n", BUID_TIME);
  INFO("[INFO] -------------------------------------------\n");

}*/

static void ICACHE_FLASH_ATTR app_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    //print_info();
    //MQTT_InitConnection(&mqttClient, MQTT_HOST, MQTT_PORT, DEFAULT_SECURITY);
    MQTT_InitConnection(&mqttClient, "192.168.1.199", 1883, 0);

    //MQTT_InitClient(&mqttClient, MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, MQTT_KEEPALIVE, MQTT_CLEAN_SESSION);
    MQTT_InitClient(&mqttClient, "projector", "", "", 120, 1);
    MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
    MQTT_OnConnected(&mqttClient, mqttConnectedCb);
    MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
    MQTT_OnPublished(&mqttClient, mqttPublishedCb);
    MQTT_OnData(&mqttClient, mqttDataCb);

    // WIFI SETUP
    wifi_station_set_hostname( "projector" );
    wifi_set_opmode_current( STATION_MODE );

    WIFI_Connect(WIFI_SSID, WIFI_PASSWD, wifiConnectCb);

    // initialize our serial control access to epson projector
    SerialControl_init();
}

void ICACHE_FLASH_ATTR user_init(void)
{
    system_init_done_cb(app_init);
}




