#include "Get_Process.h"

#include "json_object.h"
#include "alink_config.h"
#include "MICO.h"
#include "platform.h"
#include "alink_export.h"
#include "mico_app_define.h"
#include "alink_export.h"
#include "Post_Process.h"

#define client_set_log(M, ...) custom_log("CLIENT API", M, ##__VA_ARGS__)
#define FILL(a)       {a, #a}

static int hour, min;
extern uart_cmd_t uartcmd;
uint8_t wifi2device[25] = {0xAA,0x18,0xA5,0,0,0,0x00,0,0,0x02};

uint8_t sum(uint8_t * inputdata )
{
	uint8_t check_sum = 0;
	for(int i =1;i<*(inputdata +1);i++)
	{

		check_sum +=*(inputdata +i);
	}

	return check_sum;
}

device_func_index hiFuncStr[15] =
{
		{Switch, "Switch"},
		{ChildLock, "ChildLock"},
		{CueTone, "CueTone"},
		{UILight, "UILight"},
		{Hepa, "Hepa"},
		{Countdown, "Countdown"},
		{WindSpeed, "WindSpeed"},
		{0, NULL}
};

static void parseUTCtime (device_func_t command, const char *quartz)
{
  if (NULL == quartz)
    return;

  sscanf (quartz, "UTC+08:00 0 %d %d", &min, &hour);
}

int commandToDevice (device_func_t command)
{
	client_set_log(" get current %d , value = %d",command.func , command.value);
	wifi2device[10] = uartcmd.Switch;
	wifi2device[11] = uartcmd.Hepa;
	wifi2device[12] = uartcmd.WorkMode;
	wifi2device[15] = uartcmd.UILight;
	wifi2device[16] = uartcmd.CueTone;
	wifi2device[22] = uartcmd.Countdown;
	wifi2device[23] = uartcmd.ChildLock;
	 switch(command.func){
	 case 1:
		 wifi2device[10] = uartcmd.Switch = command.value;
		 break;
	 case 2:
		 wifi2device[23] = uartcmd.ChildLock = command.value;
	 		 break;
	 case 3:
		 wifi2device[16] = uartcmd.CueTone = command.value;
	 		 break;
	 case 4:
		 wifi2device[15] = uartcmd.UILight = command.value;
	 		 break;
	 case 5:
		 wifi2device[11] = uartcmd.Hepa = command.value;
	 		 break;
	 case 6:

		 wifi2device[22] = uartcmd.Countdown = command.value;
	 		 break;
	 case 7:
		 wifi2device[12] = uartcmd.WorkMode = command.value;
	 		 break;
	 default:
		 break;
	  }

	 wifi2device[24] = (0x100-(sum(wifi2device)&0xff));

/*	 client_set_log("uartcmd.Switch = %d",uartcmd.Switch);
	 client_set_log("uartcmd.CueTone = %d",uartcmd.CueTone);
	 client_set_log("uartcmd.Countdown = %d",uartcmd.Countdown);
	 client_set_log("uartcmd.FilterLife1 = %d",uartcmd.FilterLife1);
	 client_set_log("uartcmd.Hepa = %d",uartcmd.Hepa);
	 client_set_log("uartcmd.UILight = %d",uartcmd.UILight);
	 client_set_log("uartcmd.WorkMode = %d",uartcmd.WorkMode);
	 client_set_log("uartcmd.PM25 = %d",uartcmd.PM25);*/

	 MicoUartSend(UART_FOR_APP, wifi2device, 25);
}


int device_command_execute(alink_down_cmd_ptr down_cmd)
{
  static json_object* jsonObj , *j_param ,*j_val ;
  char *valueStr;
  device_func_t cmd;

  client_set_log("get cloud data %s", down_cmd->param);
  jsonObj = json_tokener_parse(down_cmd->param);
  for (int i = 0; hiFuncStr[i].name != NULL; i++) {
		j_param = json_object_object_get(jsonObj, (char *) hiFuncStr[i].name);
		if (j_param != NULL) {
		  j_val = json_object_object_get(j_param, "value");
		  valueStr = json_object_get_string(j_val);
		  client_set_log("get the value = %s",valueStr);
		  cmd.func = hiFuncStr[i].idx;
		  cmd.value = atoi (valueStr);
		  commandToDevice (cmd);
		}
	  }
   json_object_put (jsonObj);
   return 0;
}


