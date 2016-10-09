#include <queue.h>
#include "messageHandler.h"
#include "json/jsonparse.h"
#include "json/jsontree.h"
#include "osapi.h"
#include "common.h"

struct QueueEntry {
	int btnIndex;
	STAILQ_ENTRY(stailq_data_s) entries;
};

STAILQ_HEAD(MessageQueue, QueueEntry) g_messageQueue;

void initMessaging() {
	// initialize our queue!
	STAILQ_INIT(&g_messageQueue); /* Initialize the queue. */
}

bool ICACHE_FLASH_ATTR handleMessage(char* messageBuf, uint32_t len)
{
	struct jsonparse_state js;
	jsonparse_setup(&js, messageBuf, len);
	//int duty = jsonparse_get_value_as_int(js);
	char buf[32] = {0};
	int type = 0;
	int buttonNum = 0;
	int cmdKeyLen = 0;
	while( (type = jsonparse_next(&js)) != JSON_TYPE_ERROR){
		switch(type){
			case JSON_TYPE_ARRAY:
				os_printf("type = JSON_TYPE_ARRAY\n");
				break;
			case JSON_TYPE_OBJECT:
				os_printf("type = JSON_TYPE_OBJECT\n");
				break;
			case JSON_TYPE_PAIR:
				os_printf("type = JSON_TYPE_PAIR\n");
				break;
			case JSON_TYPE_PAIR_NAME:
				os_printf("type = JSON_TYPE_PAIR_NAME\n");
				break;
			case JSON_TYPE_STRING:
				os_printf("type = JSON_TYPE_STRING\n");
				break;
			case JSON_TYPE_INT:
				os_printf("type = JSON_TYPE_INT\n");
				break;
			case JSON_TYPE_NUMBER:
				os_printf("type = JSON_TYPE_NUMBER\n");
				break;
			case JSON_TYPE_ERROR:
				os_printf("type = JSON_TYPE_ERROR\n");
				break;
			case JSON_TYPE_NULL:
				os_printf("type = JSON_TYPE_NULL\n");
				break;
			case JSON_TYPE_TRUE:
				os_printf("type = JSON_TYPE_TRUE\n");
				break;
			case JSON_TYPE_FALSE:
				os_printf("type = JSON_TYPE_FALSE\n");
				break;
			case JSON_TYPE_CALLBACK:
				os_printf("type = JSON_TYPE_CALLBACK\n");
				break;
			default:
			 	os_printf("unknown type\n");
		}

		// Found a key-value pair!
		if( type == JSON_TYPE_PAIR_NAME ){
			if( jsonparse_strcmp_value(&js, "command") == 0)
			{
				cmdKeyLen = jsonparse_copy_value(&js, buf, 32);

				os_printf("Command Key received: %s. Length = %d\n", buf, cmdKeyLen);

				type = jsonparse_next(&js);
				// ok, so now look at the value
				type = jsonparse_next(&js);
				if( type == JSON_TYPE_STRING){
					jsonparse_copy_value(&js, buf, 32);
					os_printf("Here is the value: %s\n", buf);
				}
				else if(type == JSON_TYPE_INT || JSON_TYPE_NUMBER){
					jsonparse_copy_value(&js, buf, 32);
					//os_printf("Here is the value: %s\n", buf);
					uint8 commandId = jsonparse_get_value_as_int(&js);
					//os_printf("Command as an int %d\n", commandId);
					Epson_ProcessCommand(commandId);
				}
				else{
					os_printf("BAD PARSE! Couldn't find value associated with key!\n");
				}
			}
			else
			{
				os_printf("Hmmm, key value was not a string\n");
				return false;
			}
		}
	}
	return true;
}

// So this is called in ISR context... the fact that we are calling malloc
// is not great. Something we should look to improve
void ICACHE_FLASH_ATTR addBtnPressToQueue(uint8_t buttonIndex){
		// no malloc, probs setup some kind of entry pool
	// struct QueueEntry* newEntry = malloc(sizeof(struct QueueEntry)); 
	// newEntry->btnIndex = buttonIndex;
	// STAILQ_INSERT_HEAD(&g_messageQueue, newEntry, entries);
}

void ICACHE_FLASH_ATTR removeIndexFromQueue(uint8_t buttonIndex){
	// Read2 (remove).
	// datap = TAILQ_FIRST(&head);
	// printf("Read2: %d\n", datap->value);
	// TAILQ_REMOVE(&head, datap, entries);
	// free(datap);
}

bool ICACHE_FLASH_ATTR publishMessage(const char* topic, const char* message, int len){
	MQTT_Publish(&mqttClient, topic, message, len, 1, 0);
}