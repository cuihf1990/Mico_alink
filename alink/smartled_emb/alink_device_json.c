#include "mico.h"
#include "alink_device.h"

#define json_log(M, ...) custom_log("", M, ##__VA_ARGS__)

#ifndef PASS_THROUGH

#define PostDataFormat      "{\"ErrorCode\":{\"value\":\"%d\"},\"Hue\":{\"value\":\"%d\"},\"Luminance\":{\"value\":\"%d\"},\"Switch\":{\"value\":\"%d\"},\"WorkMode\":{\"value\":\"%d\"}}"
#define post_data_buffer_size    (512)
static uint8_t post_data_buffer[post_data_buffer_size];

enum {
    ATTR_ERRORCODE_INDEX,
    ATTR_HUE_INDEX,
    ATTR_LUMINANCE_INDEX,
    ATTR_SWITCH_INDEX,
    ATTR_WORKMODE_INDEX,
    ATTR_MAX_NUMS
};

#define DEVICE_ATTRS_NUM   (ATTR_MAX_NUMS)

int device_state[] = {0, 10, 50, 1, 2};/* default value */
char *device_attr[] = {
    "ErrorCode",
    "Hue",
    "Luminance",
    "Switch",
    "WorkMode",
    NULL
};

static int execute_cmd( char *json_buffer )
{
    int attr_len = 0, value_len = 0, value = 0, i = 0;
    char *value_str = NULL, *attr_str = NULL;

    json_log("set data: %s", json_buffer);

    for (i = 0; device_attr[i]; i++) {
        attr_str = json_get_value_by_name(json_buffer, strlen(json_buffer),
                device_attr[i], &attr_len, NULL);
        value_str = json_get_value_by_name(attr_str, attr_len,
                "value", &value_len, NULL);

        if (value_str && value_len > 0) {
            char last_char = *(value_str+value_len);
            *(value_str + value_len) = 0;
            value = atoi(value_str);
            *(value_str + value_len) = last_char;
            device_state[i] = value;
        }
    }

    return 0;
}

/* activate sample */
static char active_data_tx_buffer[128];
static int errorcode = 0;
#define ActivateDataFormat    "{\"ErrorCode\": { \"value\": \"%d\" }}"
int activate_button_pressed( void )
{
    if( errorcode == 0 ){
        errorcode = 1;
    }else{
        errorcode = 0;
    }

    sprintf(active_data_tx_buffer, ActivateDataFormat, errorcode);
    json_log("send:%s", active_data_tx_buffer);
    return alink_report(Method_PostData, (char *)active_data_tx_buffer);
}

int cloud_get_device_json_data( char *json_buffer )
{
    json_log( "function %s, memory %d \n", __FUNCTION__, MicoGetMemoryInfo( )->free_memory );

    set_device_state( 1 );
    return 0;
}

int cloud_set_device_json_data( char *json_buffer )
{
    int ret = 0;

    json_log( "function %s, memory %d \n", __FUNCTION__, MicoGetMemoryInfo( )->free_memory );
    ret = execute_cmd( json_buffer );
    set_device_state( 1 );
    return ret;
}

void alink_device_post_json_data( void )
{
    /* sample for json data device */
    snprintf( (char *) post_data_buffer, post_data_buffer_size, PostDataFormat,
              device_state[ATTR_ERRORCODE_INDEX],
              device_state[ATTR_HUE_INDEX],
              device_state[ATTR_LUMINANCE_INDEX],
              device_state[ATTR_SWITCH_INDEX],
              device_state[ATTR_WORKMODE_INDEX] );

    alink_report( Method_PostData, (const char *) post_data_buffer );
}
#endif
