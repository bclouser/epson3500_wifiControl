#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER

#include "ets_sys.h"

bool handleMessage(char* messageBuf, uint32_t len);

bool publishMessage();


#endif