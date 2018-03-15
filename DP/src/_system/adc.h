/*
 * Name: adc.h
 * Author: Martin Stankus
 *
 */

#ifndef _ADC_H_
#define _ADC_H_

#define ADC_CFG1_ADIV_VAL_DIV1			0
#define ADC_CFG1_ADIV_VAL_DIV2			1
#define ADC_CFG1_ADIV_VAL_DIV4			2
#define ADC_CFG1_ADIV_VAL_DIV8			3

#define ADC_CFG1_MODE_VAL_8BIT			0
#define ADC_CFG1_MODE_VAL_10BIT			2
#define ADC_CFG1_MODE_VAL_12BIT			1
#define ADC_CFG1_MODE_VAL_16BIT			3

#define ADC_CFG1_ICLK_VAL_BUSCLK		0
#define ADC_CFG1_ICLK_VAL_BUSCLK2		1
#define ADC_CFG1_ICLK_VAL_ALTCLK		2
#define ADC_CFG1_ICLK_VAL_ADACK			3

#define ADC_CFG2_LSTS_VAL_24CLK			0
#define ADC_CFG2_LSTS_VAL_16CLK			1
#define ADC_CFG2_LSTS_VAL_10CLK			2
#define ADC_CFG2_LSTS_VAL_6CLK			3

#define ADC_SC2_RFSL_VAL_DEF			0
#define ADC_SC2_RFSL_VAL_ALT			1

#define ADC_SC3_AVGS_VAL_4SAMP			0
#define ADC_SC3_AVGS_VAL_8SAMP			1
#define ADC_SC3_AVGS_VAL_16SAMP			2
#define ADC_SC3_AVGS_VAL_32SAMP			3

#endif /* _ADC_H_ */
