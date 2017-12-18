#include "mico.h"
#include "alink_device.h"
#include "Post_Process.h"

#define json_log(M, ...) custom_log("", M, ##__VA_ARGS__)

#ifndef PASS_THROUGH
#define post_data_buffer_size    (512)
static uint8_t post_data_buffer[post_data_buffer_size];

/*do your job here*/
extern uart_cmd_t uartcmd;

char* post_str_format = "{"\
" \"Switch\": {"\
" \"value\": \"%d\""\
" },"\
" \"ErrorCode\": {"\
" \"value\": \"%d\" "\
" },"\
" \"ChildLock\": {"\
" \"value\": \"%d\""\
" },"\
" \"CueTone\": {"\
" \"value\": \"%d\""\
" },"\
" \"UILight\": {"\
" \"value\": \"%d\""\
" },"\
" \"Hepa\": {"\
" \"value\": \"%d\""\
" },"\
" \"FilterLife1\": {"\
" \"value\": \"%d\""\
" },"\
" \"Countdown\": {"\
" \"value\": \"%d\""\
" },"\
" \"WindSpeed\": {"\
" \"value\": \"%d\""\
" },"\
" \"PM25\": {"\
" \"value\": \"%d\""\
" }"\
" }";

static void get_device_status( alink_up_cmd_ptr up_cmd )
{
    sprintf( (char *) post_data_buffer, post_str_format, uartcmd.Switch,uartcmd.ErrorCode,\
			uartcmd.ChildLock,uartcmd.CueTone, \
			uartcmd.UILight,uartcmd.Hepa,uartcmd.FilterLife1,uartcmd.Countdown,\
			uartcmd.WorkMode,uartcmd.PM25 );
    up_cmd->param = (const char *) post_data_buffer;
    json_log("post data = %s",post_data_buffer);
}



#if 0
static int execute_cmd( alink_down_cmd_ptr down_cmd )
{
    int attrLen = 0, valueLen = 0, value = 0, i = 0;
    char *valueStr = NULL, *attrStr = NULL;

    json_log("set json: %s", down_cmd->param);

    for ( i = 0; i < 5; i++ )
    {
        attrStr = alink_JsonGetValueByName( down_cmd->param, strlen( down_cmd->param ),
            device_attr[i], &attrLen, 0 );
        valueStr = alink_JsonGetValueByName( attrStr, attrLen, "value", &valueLen, 0 );

        if ( valueStr && valueLen > 0 )
        {
            char lastChar = *(valueStr + valueLen);
            *(valueStr + valueLen) = 0;
            value = atoi( valueStr );
            *(valueStr + valueLen) = lastChar;
            switch ( i )
            {
                case 0:
                virtual_device.power = value;
                break;
                case 1:
                virtual_device.temp_value = value;
                break;
                case 2:
                virtual_device.light_value = value;
                break;
                case 3:
                virtual_device.time_delay = value;
                break;
                case 4:
                virtual_device.work_mode = value;
                break;
                default:
                break;
            }
        }
    }

    return 0;
}
#endif


int cloud_get_device_json_data( alink_down_cmd_ptr down_cmd )
{
    int ret = 0;
    json_log("function %s, memory %d \n", __FUNCTION__, MicoGetMemoryInfo()->free_memory);
    set_device_state( 1 );
    /*do your job end!  ��ѯָ�� */
    json_log("APP refresh=========");
    return ret;
}

int cloud_set_device_json_data( alink_down_cmd_ptr down_cmd )
{
    /* TODO: */
    /*get cmd from server, do your job here! */
    int ret = 0;
    json_log("function %s, memory %d \n", __FUNCTION__, MicoGetMemoryInfo()->free_memory);
	ret = device_command_execute(down_cmd);
    /* do your job end! */
    set_device_state( 1 );
    return ret;
}

int alink_device_post_json_data( void )
{
    /* do your job here */
    int ret = 0;
    alink_up_cmd up_cmd;

    json_log("function %s, memory %d \n", __FUNCTION__, MicoGetMemoryInfo()->free_memory);
    get_device_status( &up_cmd );
    up_cmd.target = NULL;
    up_cmd.resp_id = -1;
    ret = alink_post_device_data( &up_cmd );
    if ( ret )
    {
        json_log("post failed!\n");
    } else
    {
        json_log("dev post json data success!\n");
    }

    /* do your job end */
    return ret;
}
#endif
