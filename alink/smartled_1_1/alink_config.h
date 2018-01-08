/*************************************************
 ALINK CONFIG
 *************************************************/
#include "common.h"

#pragma once

//#define PASS_THROUGH

//TODO: update these product info
#define alink_product_dev_name        "ADAC-FF-600"
#ifdef PASS_THROUGH
#define alink_product_model         "ALINKTEST_LIVING_LIGHT_SMARTLED_LUA"
#define alink_product_key             "bIjq3G1NcgjSfF9uSeK2"
#define alink_product_secret            "W6tXrtzgQHGZqksvJLMdCPArmkecBAdcr2F5tjuF"
#else
#define alink_product_model         "DCA_LIVING_AIRPURIFIER_ADAC_FF_600"
#define alink_product_key             "H78wD9l59xQaVyEQq5wi"
#define alink_product_secret            "kRhmi1L5F73RHzxVTP9uUyoJky0AjaBJbaG5Ocac"
#endif

#define alink_product_dev_version     "1.1.9"
#define alink_product_dev_type        "AIRPURIFIER"
#define alink_product_dev_category    "LIVING"
#define alink_product_dev_manufacturer "DCA"

typedef struct _alink_config_t
{
    uint8_t alink_config_data[2048];
    bool is_unbind;
} alink_config_t;
