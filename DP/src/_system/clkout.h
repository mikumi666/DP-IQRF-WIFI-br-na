/*
 * Name: clkout.h
 * Author: Martin Stankus
 *
 */

#ifndef _CLKOUT_H_
#define _CLKOUT_H_

#include "MKL28Z7.h"

#define CLKOUT_PCC						PCC_PORTC
#define CLKOUT_PORT						PORTC
#define CLKOUT_IOIND					3
#define CLKOUT_PORT_PCR_MUX_VAL			5

#define SCG_CKOUTCFG_SEL_VAL_BUSCLK		0
#define SCG_CKOUTCFG_SEL_VAL_SOSCCLK	1
#define SCG_CKOUTCFG_SEL_VAL_SIRCCLK	2
#define SCG_CKOUTCFG_SEL_VAL_FIRCCLK	3
#define SCG_CKOUTCFG_SEL_VAL_SPLLCLK	6

typedef enum {
	CLKOUT_CONF_DIS,
	CLKOUT_CONF_BUSCLK,
	CLKOUT_CONF_SOSCCLK,
	CLKOUT_CONF_SIRCCLK,
	CLKOUT_CONF_FIRCCLK,
	CLKOUT_CONF_SPLLCLK
} CLKOUT_CONF;

void clkout_set(CLKOUT_CONF clkout_conf);

#endif /* _CLKOUT_H_ */
