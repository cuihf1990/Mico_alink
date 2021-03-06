#include "mico.h"
#include "mico_app_define.h"
#include "alink_device.h"
#include "alink_aws.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace() custom_log_trace("APP")

extern void PlatformEasyLinkButtonClickedCallback( void );
static mico_semaphore_t wait_sem = NULL;

extern int activate_button_pressed( void );

/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback( void * const user_config_data, uint32_t size )
{

}

static void aws_mode( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    PlatformEasyLinkButtonClickedCallback( );
    app_log("enter aws config mode...");
}

static void aws_activate( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    activate_button_pressed( );
    app_log("awss v3 activate...");
}

static void alink_reset( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    alink_factory_reset( );
}

static const struct cli_command user_clis[] = {
    { "aws", "enter aws config mode", aws_mode },
    { "activate", "awss v3 need activate", aws_activate},
    { "reset", "support off-line reset", alink_reset},
};

void app_alink_config_read( char *buffer, int length )
{
    app_log("alink config read");
    application_config_t *application_config = mico_system_context_get_user_data( mico_system_context_get( ) );
    memcpy( buffer, application_config->alink_config.alink_config_data, length );
}

void app_alink_config_write( char *buffer, int length )
{
    app_log("alink config write");
    application_config_t *application_config = mico_system_context_get_user_data( mico_system_context_get( ) );

    memcpy( application_config->alink_config.alink_config_data, buffer, length );
    mico_system_context_update( mico_system_context_get( ) );
}

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            if( wait_sem != NULL ){
                mico_rtos_set_semaphore( &wait_sem );
            }
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

void ssl_log( const int logLevel, const char * const logMessage )
{
    app_log("%s", logMessage);
}

int application_start( void )
{
    app_log_trace();
    OSStatus err = kNoErr;
    mico_Context_t* mico_context;
    app_context_t* app_context;
    alink_product_t product_info;
    alink_product_dev_t product_dev_info;

    /* Create application context */
    app_context = (app_context_t *) calloc( 1, sizeof(app_context_t) );
    require_action( app_context, exit, err = kNoMemoryErr );

    mico_rtos_init_semaphore( &wait_sem, 1 );

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler,
                                       NULL );
    require_noerr( err, exit );

    /* Create mico system context and read application's config data from flash */
    mico_context = mico_system_context_init( sizeof(application_config_t) );
    app_context->appConfig = mico_system_context_get_user_data( mico_context );

    /* mico system initialize */
    err = mico_system_init( mico_context );
    require_noerr( err, exit );

    app_log("product model: %s", alink_product_model);

    cli_register_commands( user_clis, sizeof(user_clis) / sizeof(struct cli_command) );

    alink_platform_config_func_set( &app_alink_config_read, &app_alink_config_write );

    strcpy( product_info.product_name, alink_product_dev_name );
    strcpy( product_info.product_model, alink_product_model );
    strcpy( product_info.product_key, alink_product_key );
    strcpy( product_info.product_secret, alink_product_secret );
    alink_product_info( &product_info );

    strcpy( product_dev_info.product_dev_version, alink_product_dev_version );
    strcpy( product_dev_info.product_dev_type, alink_product_dev_type );
    strcpy( product_dev_info.product_dev_category, alink_product_dev_category );
    strcpy( product_dev_info.product_dev_manufacturer, alink_product_dev_manufacturer );
    alink_product_dev_info( &product_dev_info  );

#ifdef PASS_THROUGH
    alink_use_easylink_config( );
#endif

    start_aws_config_mode( );

    /* Wait for wlan connection*/
    mico_rtos_get_semaphore( &wait_sem, MICO_WAIT_FOREVER );
    app_log("wifi connected successful");

//    ssl_set_loggingcb(ssl_log);

    start_alink_emb( );

    exit:
    mico_system_notify_remove( mico_notify_WIFI_STATUS_CHANGED,
                               (void *) micoNotify_WifiStatusHandler );
    mico_rtos_deinit_semaphore( &wait_sem );
    mico_rtos_delete_thread( NULL );
    return err;
}

