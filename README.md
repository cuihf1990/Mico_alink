# alink_Demo

本文档通过讲解 Demo 示例程序使用，介绍如何在 MiCO 中实现 alink 通信功能。

* 适用 MiCO OS 版本： v3.0.0及以上
* 支持模块型号： EMW3165，EMW3166，EMW3031, EMW3080B/C. 

## 

* 1.[准备工作](#1.准备工作)
* 2.[Demo_使用](#2.Demo_使用)

   * 2.1 [导入 mico 工程](#21-导入mico工程)
   * 2.2 [修改配置文件参数](#22-修改配置文件参数)
   * 2.3 [alink1.1](#23-alink1_1)
   * 2.4 [alink_emb](#24-alink_emb)
   * 2.5 [alink_sds](#25-alink_sds)
   * 2.6 [zero config](#26-zero_config)
	
* 3.[运行结果](#3.运行结果)

## 1.准备工作

* 选择一个 [MiCO 开发板](http://developer.mico.io/handbooks/6)
* 玩转 [第一个 MiCO 应用程序](http://developer.mico.io/handbooks/11)



## 2.Demo 使用
### 2.1 导入MiCO工程

1. 使用micocube 导入工程。在命令行输入以下指令:`mico import https://code.aliyun.com/MXCHIP_FAE/alink.git`

### 2.2 修改代码
1. 修改配置文件 alink_config.h 根据TRD文件修改 product_model，product_key，product_secret等参数。如果使用透传方式上传数据，请务必打开宏 #define PASS_THROUGH 。


### 2.3 alink1_1

#### 2.3.1 代码注释
```
static void alink_main( uint32_t arg )
{
    struct device_info *main_dev;
    main_dev = platform_malloc( sizeof(struct device_info) );
    alink_sample_mutex = platform_mutex_init( );
    post_sem = platform_semaphore_init( );

    memset( main_dev, 0, sizeof(struct device_info) );
    alink_fill_deviceinfo( main_dev );


//  设置log打印等级 ALINK_LL_NONE为不打印alink库里面的日志。
//    alink_set_loglevel( ALINK_LL_DEBUG | ALINK_LL_INFO | ALINK_LL_ERROR );
    alink_set_loglevel(ALINK_LL_NONE);
// 设置alink状态回调
    main_dev->sys_callback[ALINK_FUNC_SERVER_STATUS] = alink_handler_systemstates_callback;

#ifndef PASS_THROUGH
// 设置获取json数据 和 设置 json 数据接口
    main_dev->dev_callback[ACB_GET_DEVICE_STATUS] = cloud_get_device_json_data;
    main_dev->dev_callback[ACB_SET_DEVICE_STATUS] = cloud_set_device_json_data;
#endif

    /*start alink-sdk */
#ifdef PASS_THROUGH
// 设置透传数据接口
    alink_start_rawdata( main_dev, cloud_get_device_raw_data, cloud_set_device_raw_data );
#else
    alink_start( main_dev );
#endif
    platform_free( main_dev );

    alink_main_log("wait main device login");
    /*wait main device login, -1 means wait forever */
    alink_wait_connect( NULL, ALINK_WAIT_FOREVER );

    while ( 1 )
    {
// 发送设备状态到云端
        if ( get_device_state( ) )
        {
            alink_main_log("device status changed, free memory %d", MicoGetMemoryInfo()->free_memory);
#ifdef PASS_THROUGH
            alink_device_post_raw_data( );
#else
            alink_device_post_json_data();
#endif
        }
        platform_semaphore_wait( post_sem, 500 );
    }

    alink_end( );
    platform_mutex_destroy( alink_sample_mutex );
    platform_semaphore_destroy( post_sem );
    mico_rtos_delete_thread( NULL );
}
```

#### 2.3.2 接口说明

|名称|`void alink_device_reset( void )`|
|:---|:---|
|功能|`设备端解绑 解除app与设备之间的绑定关系`|
|返回|`void`|

|名称|`int get_device_state( void )`|
|:---|:---|
|功能|`获取设备状态`|
|返回|`设备状态是否改变`|

|名称|`int set_device_state( int state )`|
|:---|:---|
|功能|`设置设备状态 如果状态改变则发送到云端`|
|返回|`设备状态是否改变`|

|名称|`void alink_device_post_json_data( void )`|
|:---|:---|
|功能|`上报alink json数据`|
|返回|`void`|

|名称|`int cloud_set_device_json_data( char *json_buffer )`|
|:---|:---|
|功能|`云端下发json数据`|
|返回|`void`|


|名称|`int cloud_get_device_json_data( char *json_buffer )`|
|:---|:---|
|功能|`云端获取设备数据`|
|返回|`void`|



#### 2.3.3 编译下载

* 以3031模块为例：输入指令：`alink.smartled_1_1@MK3031@moc total download run`


### 2.4 alink_emb

* 支持零配功能( 零配功能请参考 [zero config](#25-zero_config) );

#### 2.4.1 代码注释

```
static void alink_main( uint32_t arg )
{
//设置alink打印等级为ALINK_LL_TRACE全开  ALINK_LL_NONE 为全关
    alink_set_loglevel( ALINK_LL_TRACE );

    alink_device_state_mutex = platform_mutex_init( );
    alink_post_data_sem = platform_semaphore_init( );
//设置连云状态回调
    alink_register_callback( ALINK_CLOUD_CONNECTED, &cloud_connected );
    alink_register_callback( ALINK_CLOUD_DISCONNECTED, &cloud_disconnected );

    /*
     * NOTE: register ALINK_GET/SET_DEVICE_STATUS or ALINK_GET/SET_DEVICE_RAWDATA
     */
#ifdef PASS_THROUGH
    /*
     * TODO: before using callback ALINK_GET/SET_DEVICE_RAWDATA,
     * submit product_model_xxx.lua script to ALINK cloud.
     * ALINKTEST_LIVING_LIGHT_SMARTLED_LUA is done with it.
     */
//设置透传状态数据获取和设置接口。
    alink_register_callback( ALINK_GET_DEVICE_RAWDATA, &cloud_get_device_raw_data );
    alink_register_callback( ALINK_SET_DEVICE_RAWDATA, &cloud_set_device_raw_data );
#else
//设置JSON状态数据获取和设置接口。
    alink_register_callback(ALINK_GET_DEVICE_STATUS, &cloud_get_device_json_data);
    alink_register_callback(ALINK_SET_DEVICE_STATUS, &cloud_set_device_json_data);
#endif

//启动emb主线程
    alink_start( );

    alink_wait_connect( ALINK_WAIT_FOREVER );
    alink_main_log("alink connect succeed. \n");

    while ( 1 )
    {
//数据上报接口
        if ( get_device_state( ) )
        {
            alink_main_log("device status changed, free memory %d", MicoGetMemoryInfo()->free_memory);
#ifdef PASS_THROUGH
            alink_device_post_raw_data( );
#else
            alink_device_post_json_data();
#endif
        }
        platform_semaphore_wait( alink_post_data_sem, 500 );
    }

    alink_main_log("alink exit\n");
    alink_end( );

    platform_mutex_destroy( alink_device_state_mutex );
    platform_semaphore_destroy( alink_post_data_sem );
    mico_rtos_delete_thread( NULL );
}
```

#### 2.4.2 接口说明

|名称|`int activate_button_pressed( void )`|
|:---|:---|
|功能|`发送产品激活指令`|
|返回|`发送是否成功`|


#### 2.4.3 编译下载

* 以3031模块为例：输入指令：`alink.smartled_emb@MK3031@moc total download run`

### 2.5 alink_sds
* SDS 功能说明：
给每一台设备增加一个device_secret和device_key信息。支持零配功能（ 零配功能请参考 [zero config](#25-zero_config) ）。其他接口与alink_emb一致。

* 需要在MK文件中定义 ```GLOBAL_DEFINES += ALINK_USE_SDS ```

#### 2.5.1 接口说明

```
#ifdef SUPPORT_SDS

//获取device_secret
char *product_get_device_secret(char secret_str[DEVICE_SECRET_LEN])
{
	uint32_t para_offset = 0x0;
	char secret_flash[DEVICE_SECRET_LEN]={0};
	MicoFlashRead( MICO_PARTITION_SDS, &para_offset, (uint8_t *)&secret_flash, DEVICE_SECRET_LEN-1 );
	return strncpy( secret_str, secret_flash, DEVICE_SECRET_LEN-1 );
}

//获取device_key
char *product_get_device_key(char key_str[DEVICE_KEY_LEN])
{
	uint32_t para_offset = DEVICE_SECRET_LEN-1;
	char key_flash[DEVICE_KEY_LEN]={0};
	MicoFlashRead( MICO_PARTITION_SDS, &para_offset, (uint8_t *)&key_flash, DEVICE_KEY_LEN-1 );
	return strncpy( key_str, key_flash, DEVICE_KEY_LEN-1 );
}

#endif

```
#### 2.5.2 sds文件烧录方法

* flash中sds值烧录方法：
 在bootloader模式下，使用命令 sds write,传入文件 sds.bin


#### 2.5.3 编译下载

* 以3031模块为例：输入指令：`alink.smartled_emb@MK3031@moc total download run`


### 2.6 zero_config 

#### 2.6.1 zero config 流程说明：
* 零配置流程
	A : 待配网设备，enrollee
	B : 已配网设备，registrar
	
	A --> B //第一次握手，A持续广播信息给B，持续广播，基于probe req 或 其他管理帧
	A <-- B //第二次握手，B应答消息给A，应答时间最长为10s，基于probe req 或其他管理帧
	
	A B 通过custom IE交换信息。

* 实现要点：

1. A通常处在配网状态，即 sniffer/monitor 状态，伴随着信道切换
2. awss通过platform_wifi_enable_mgnt_frame（FRAME_PROBE_REQ,buffer,len）来发包。buffer &len已经是一个完整的802.11包含包头以及4字节的FCS,如果驱动需要额外追加其他信息，可以copy这个buffer进行加工，然后最终发送；
3. B是处在station或者AP模式的wifi设备
4. awss通过platform_wifi_enable_mgnt_frame_filter()为B设置了监听管理帧；

* 调试步骤：

1. 使用两块开发板，分别作为A,B. A处在AWSS状态，不断切换信道并发送特定的管理帧（通过OMNIPEEK抓包，包含D896E0的customIE,A的工作即完成）
2. 另一块开发板正常联网后（alink 云通道已连接OK）作为B,ALINK在联网成功后会调用platform_wifi_enable_mgnt_frame_filter()为B开启管理帧监听A发出的包（若B上能打印出监听到A发出的包，B的工作已完成一半）；
3. 采用阿里小智最新版APP，绑定B设备后，在首页点添加设备，停留5S左右会发现一个待添加设备（A），确认添加后，B设备会发出管理帧通知A设备（B上另一半的工作完成），A设备收到信息后，解析出联网密码，即可联网成功。
4. APP提示用户对设备A进行激活，绑定。

#### 2.6.2 编译下载

* 以3031模块为例：输入指令：`alink.smartled_emb@MK3031@moc total download run`





## 3.运行结果 
本Demo可实现： 在MiCO 中实现 alink 通信功能。




