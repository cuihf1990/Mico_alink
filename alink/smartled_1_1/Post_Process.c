
#include "alink_config.h"
#include "mico_app_define.h"
#include "MICO.h"
#include "platform.h"
#include "alink_export.h"
#include "Post_Process.h"
#include "system_internal.h"


#define client_log(M, ...) custom_log("CLIENT API", M, ##__VA_ARGS__)

uint8_t uart_recv_buf[UartRecvBufferLen]    =  {0};

uint8_t wifi_status_return[12] = {0xAA,0x0B,0xA5,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0xff,0xff};

extern void *alink_sample_mutex;
extern char device_status_change;
uart_cmd_t uartcmd;

int cloud_connect_status = -1;

/* uart data transfer and post */

int device_status_get(void)
{
	client_log("Alink callback, get device status\r\n");
	device_status_change = 1;
	return 1;
}


void PraseUartData(uint8_t * buf)
{
}

static uint8_t factory_test[13] = {0xAA,0x0C,0xA5,0,0,0,0,0,0,0x43,0x00,0x01,0xff};
void DeviceNotify_ScanCompleteHandler(ScanResult *pApList, mico_Context_t * const inContext)
{
  int i;

  factory_test[11] = 0x03;

  for (i=0;i<pApList->ApNum;i++)
  {
    if(strcmp(pApList->ApList[i].ssid,"MA1705 App Test") == 0)
    {
      if(pApList->ApList[i].rssi > 50)
          factory_test[11] = 0x02;
      break;
    }
  }
  factory_test[12] = (0x100-(sum(factory_test,factory_test[1])&0xff));
  MicoUartSend(UART_FOR_APP, factory_test, 13);   /// ɨ����
}


void TestMode()
{
    OSStatus err = kUnknownErr;

    system_context_t* in_context = system_context();
    require( in_context, exit );

    factory_test[10] = 0x01;
    factory_test[12] = (0x100-(sum(factory_test,factory_test[1])&0xff));
    MicoUartSend(UART_FOR_APP, factory_test, 13);   ////����ȷ��

    if(in_context->flashContentInRam.micoSystemConfig.configured  == wLanUnConfigured ||
            in_context->flashContentInRam.micoSystemConfig.configured == wLanUnConfigured == unConfigured){
        micoWlanStopEasyLink();
        CloseEasylink();
        }else{
          micoWlanSuspendStation();
        }

    err = mico_system_notify_register( mico_notify_WIFI_SCAN_COMPLETED, (void *)DeviceNotify_ScanCompleteHandler, in_context );
    require_noerr( err, exit );

    mxchip_active_scan("MA1705 App Test", 0);

 exit:
     return;
}

void device_cmd_process (uint8_t *buf, int inLen)
{

	protocol_type_t *protocol_data = (protocol_type_t *)buf;

    client_log("device_flag = %02x",protocol_data->device_type);

    if(protocol_data->device_type == 0xA5){     ////air  robot

    client_log(" message_type = %02x",protocol_data->message_type);

    switch(protocol_data->message_type){
    case device_contro_report:
  	 	  uartcmd.ChildLock = buf[26];
  	  	  uartcmd.Countdown = buf[25];
  	  	  uartcmd.CueTone = buf[16];
  	  	  uartcmd.ErrorCode  = buf[24];
  	  	  uartcmd.FilterLife1 = buf[21];
  	  	  uartcmd.Hepa =  buf[11];
  	  	  uartcmd.PM25 = (buf[22]*256 +buf[23]);
  	  	  uartcmd.Switch = buf[10];
      	  uartcmd.UILight = buf[15];
    //    uartcmd.WindSpeed = (buf[13]*256+buf[14]);
      	  uartcmd.WorkMode = buf[12];
          break;
      case device_status_report :     ///0x04
  	 	  uartcmd.ChildLock = buf[26];
  	  	  uartcmd.Countdown = buf[25];
  	  	  uartcmd.CueTone = buf[16];
  	  	  uartcmd.ErrorCode  = buf[24];
  	  	  uartcmd.FilterLife1 = buf[21];
  	  	  uartcmd.Hepa =  buf[11];
          //                                          (170 - (-20) + 1)
//          uartcmd.PM25 = (buf[22]*256 +buf[23]) + rand() % 191 - 20;
          uartcmd.PM25 = (buf[22]*256 +buf[23]);
  	  	  uartcmd.Switch = buf[10];
      	  uartcmd.UILight = buf[15];
    //    uartcmd.WindSpeed = (buf[13]*256+buf[14]);
      	  uartcmd.WorkMode = buf[12];
          client_log("uartcmd.Switch = %d",uartcmd.Switch);
    	  break;
      case device_notif_report:     ///0x06
    	  break;
      case device_error_report:      //0x0a
    	  break;
      case erase_wifi_config:     //0x83

    	  PlatformEasyLinkButtonLongPressedCallback();
    	  break;
      case get_wifi_status:           //0x63
    	  wifi_status_return[10] = cloud_connect_status;
    	  wifi_status_return[11] = (0x100-(sum(wifi_status_return)&0xff));
		//MicoUartSend(UART_FOR_APP, wifi_status_return, 12);
    	  break;
      case wifi_workmode_change:       //0x64
    	  break;
      case wifi_factory_test:  ///0x43
          TestMode();
          break;
      }


/*
    client_log("uartcmd.Switch = %d",uartcmd.Switch);
    client_log("uartcmd.CueTone = %d",uartcmd.CueTone);
    client_log("uartcmd.Countdown = %d",uartcmd.Countdown);
    client_log("uartcmd.FilterLife1 = %d",uartcmd.FilterLife1);
    client_log("uartcmd.Hepa = %d",uartcmd.Hepa);
    client_log("uartcmd.UILight = %d",uartcmd.UILight);
    client_log("uartcmd.WorkMode = %d",uartcmd.WorkMode);
    client_log("uartcmd.PM25 = %d",uartcmd.PM25);
*/
    device_status_change = 1;

    }else if(protocol_data->device_type == 0xff)    {    ////UDP

    }else{
    	client_log("unknow commond");
    }

    client_log("have receive data");


}

/********************************** post function**********************/

void fillDeviceStatus (char *str)
{

}

