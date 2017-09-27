/*************************************************
 ALINK CONFIG
 *************************************************/
#include "common.h"

#pragma once

/* raw data device means device post byte stream to cloud,
 * cloud translate byte stream to json value by lua script
 * for each product model, vendor need to sumbit a lua script
 * doing this job
 *
 * undefine RAW_DATA_DEVICE, sample will post json string to cloud
 */
//#define PASS_THROUGH

#define Method_PostData    "postDeviceData"
#define Method_PostRawData "postDeviceRawData"
#define Method_GetAlinkTime "getAlinkTime"

//TODO: update these product info

#ifdef PASS_THROUGH
/*  使用easylink 配网测试 model
 * #define alink_product_dev_name        "alink_product"
 * #define alink_product_model         "ALINKTEST_LIVING_LIGHT_SMARTLED_LUA"
 * #define alink_product_key             "bIjq3G1NcgjSfF9uSeK2"
 * #define alink_product_secret            "W6tXrtzgQHGZqksvJLMdCPArmkecBAdcr2F5tjuF"
 */
#define alink_product_dev_name        "ALINK_TEST_LUA"
#define alink_product_model           "ALINKTEST_LIVING_LIGHT_ALINK_TEST_LUA"
#define alink_product_key             "PpvWt4yEJhWW9f987meF"
#define alink_product_secret          "c2IGOfujoX7JrbixF1yHNf4tcD8J2rAOIv7AszV8"
#else
#define alink_product_dev_name        "alink_product"
#define alink_product_model           "ALINKTEST_LIVING_LIGHT_ALINK_TEST"
#define alink_product_key             "5gPFl8G4GyFZ1fPWk20m"
#define alink_product_secret          "ngthgTlZ65bX5LpViKIWNsDPhOf2As9ChnoL9gQb"
#endif

#define alink_product_dev_version     "1.3"
#define alink_product_dev_type        "LIGHT"
#define alink_product_dev_category    "LIVING"
#define alink_product_dev_manufacturer "ALINKTEST"

typedef struct _alink_config_t
{
    uint8_t alink_config_data[2048];
} alink_config_t;
