#ifndef _CLIENT_API_H
#define _CLIENT_API_H

#include "mico.h"
#include "platform.h"
#include "alink_export.h"
enum{

	        DATA_REPORT = 0x40,
			CONFIGURE_FUNCTION = 0x10,
			UNREGISTER = 0x02,
			START_EASYLINK = 0x01,
			FACTORY_TEST = 0x03
};

enum{
	Switch = 1,
	ChildLock,
	CueTone,
	UILight,
	Hepa,
	Countdown,
	WindSpeed
};

typedef struct _uart_cmd
{
	  uint8_t ErrorCode;
	  uint8_t Switch;
	  uint8_t WindSpeed;
	  uint8_t ChildLock;
	  uint8_t CueTone;
	  uint8_t UILight;
	  uint8_t Hepa;
	  uint8_t FilterLife1;
	  uint8_t Countdown;
	  uint8_t PM25;
	  uint8_t WorkMode;
} uart_cmd_t;


enum {
	device_contro_report = 0x02,
   device_status_report = 0x04,
   device_notif_report = 0x06,
   device_error_report = 0x0a,
   erase_wifi_config = 0x83,
   get_wifi_status = 0x63,
   wifi_workmode_change = 0x64,
   wifi_factory_test = 0x43
};

#pragma pack(push)
#pragma pack(1)
typedef struct {
	uint8_t  protocol_head;
	uint8_t  protocol_length;
	uint8_t  device_type;
	uint8_t  message1;
 	uint8_t  message2;
	uint8_t  message3;
    uint8_t  message_flag;
    uint16_t protocol_version;
	uint8_t  message_type;
	uint8_t  messagedata[1];
}protocol_type_t;
#pragma pack(pop)

void device_cmd_process (uint8_t *buf, int inLen);
uint8_t get_check_sum(uint8_t *buf,int len);
#endif


