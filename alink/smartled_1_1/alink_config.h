/*************************************************
 ALINK CONFIG
 *************************************************/
#include "common.h"

#pragma once

#define PASS_THROUGH

//TODO: update these product info
#define alink_product_dev_name        "ALINKTEST"
#ifdef PASS_THROUGH
#define alink_product_model         "ALINKTEST_LIVING_LIGHT_SMARTLED_LUA"
#define alink_product_key             "bIjq3G1NcgjSfF9uSeK2"
#define alink_product_secret            "W6tXrtzgQHGZqksvJLMdCPArmkecBAdcr2F5tjuF"
#else
#define alink_product_model         "ALINKTEST_LIVING_LIGHT_SMARTLED"
#define alink_product_key             "ljB6vqoLzmP8fGkE6pon"
#define alink_product_secret            "YJJZjytOCXDhtQqip4EjWbhR95zTgI92RVjzjyZF"
#endif

#define alink_product_dev_version     "1.1.5"
#define alink_product_dev_type        "LIGHT"
#define alink_product_dev_category    "LIVING"
#define alink_product_dev_manufacturer "ALINKTEST"

typedef struct _alink_config_t
{
    uint8_t alink_config_data[2048];
    bool is_unbind;
} alink_config_t;
