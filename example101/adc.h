#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include "fnd.h"

void adc_init();
uint16_t adc_get_result(uint8_t channel_num);

#endif 
