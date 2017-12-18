/**
 ******************************************************************************
 * @file    UartRecv.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file create a UART recv thread.
 ******************************************************************************
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "alink_config.h"
#include "MICO.h"
#include "platform.h"
#include "alink_export.h"
#include "mico_app_define.h"
#include "Post_Process.h"

#define uart_recv_log(M, ...) custom_log("UART RECV", M, ##__VA_ARGS__)

#define MAX_LEN  80

static size_t _uart_get_one_packet(uint8_t* buf, int maxlen);


void uartRecv_thread(uint32_t inContext)
{
	int recvlen;
	uint8_t *inDataBuffer;

	inDataBuffer = malloc(UartRecvBufferLen);
	require(inDataBuffer, exit);

	while(1)
	{
		recvlen = _uart_get_one_packet(inDataBuffer, UartRecvBufferLen);
		if(recvlen <= 0)
			continue;
		device_cmd_process(inDataBuffer, recvlen);

	}

exit:
	if(inDataBuffer) free(inDataBuffer);
}

/* Packet format: BB 00 CMD(2B) Status(2B) datalen(2B) data(x) checksum(2B)
* copy to buf, return len = datalen+10
*/
size_t _uart_get_one_packet(uint8_t* inBuf, int inBufLen)
{
	OSStatus err = kNoErr;
	int datalen;
	uint8_t *p;

	while(1)
	{
		p = inBuf;
		err = MicoUartRecv(UART_FOR_APP, p, 1, MICO_WAIT_FOREVER);
		require_noerr(err, exit);
		require(*p == 0xAA, exit);
        p++;
		err = MicoUartRecv(UART_FOR_APP, p, 1, 500);
		require_noerr(err, exit);
        datalen = *p;
        if(datalen > MAX_LEN)
        {
        	uart_recv_log("recv datalength error");
        	return -1;
        }
        p++;
		err = MicoUartRecv(UART_FOR_APP, p, datalen-1, 500);
		require_noerr(err, exit);
        return datalen+1;
	}

exit:
	uart_recv_log("ERROR: %02x, datalen %d", *p, datalen);
	return -1;

}


volatile ring_buffer_t  st_rx_buffer;
volatile uint8_t        st_rx_data[1024*2];

OSStatus Uart_Init()
{
  OSStatus err = kNoErr;
  mico_uart_config_t uart_config;
  /*UART receive thread*/
  uart_config.baud_rate = 115200;
  uart_config.data_width = DATA_WIDTH_8BIT;
  uart_config.parity = NO_PARITY;
  uart_config.stop_bits = STOP_BITS_1;
  uart_config.flow_control = FLOW_CONTROL_DISABLED;
  uart_config.flags = UART_WAKEUP_DISABLE;
  ring_buffer_init( (ring_buffer_t *) &st_rx_buffer, (uint8_t *) st_rx_data, 2048 );
  MicoUartInitialize( UART_FOR_APP, &uart_config, (ring_buffer_t *) &st_rx_buffer );

  err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "UART Recv", uartRecv_thread,0x800,0 );
  require_noerr_action( err, exit, "ERROR: Unable to start the uart recv thread." );

exit:
  return err;
}

