#ifndef _ST_CLIENT_SET_H
#define _ST_CLIENT_SET_H

#include "mico.h"
#include "platform.h"
#include "alink_export.h"

typedef struct
{
	int idx;
	const char *name;
} device_func_index;


typedef struct _device_func
{
	uint8_t func;
	uint32_t value;
} device_func_t;

typedef struct _device_func_json
{
	const char *value;
} device_func_json_t;

typedef struct _device_func_json_extra
{
	const char *value;
	const char *extra;
} device_func_json_extra_t;

typedef struct _device_status
{
	device_func_json_t heatermode;
	device_func_json_t ontime;
	device_func_json_t power;
	device_func_json_t currtemp;
	device_func_json_t temp;
	device_func_json_t watervol;
	device_func_json_t powersave;
	device_func_json_t heating;
	device_func_json_t switch1;
	device_func_json_t erro;
} device_status_t;

typedef struct _real_time
{
	uint16_t year;
	uint8_t month;
	uint8_t date;
	uint8_t week;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}real_time;

int device_command_execute(alink_down_cmd_ptr down_cmd);

#endif
