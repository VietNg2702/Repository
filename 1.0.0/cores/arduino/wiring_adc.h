#ifndef WIRING_ADC_H__
#define WIRING_ADC_H__

#include <stdint.h>
#include <stdbool.h>
#include "RE01_256KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "pin.h"

uint32_t adc_read(void);

#endif