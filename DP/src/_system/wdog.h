/*
 * Name: wdog.h
 * Author: Martin Stankus
 *
 */

#ifndef _WDOG_H_
#define _WDOG_H_

#include "MKL28Z7.h"

#define WDOG_REFRESH_SEQ		0xB480A602ul
#define WDOG_UNLOCK_SEQ			0xD928C520ul

#define WDOG_CONF_DELAY			64

typedef enum {
	WDOG_CONF_DIS,
	WDOG_CONF_BUSCLK_PRESC_OFF,
	WDOG_CONF_BUSCLK_PRESC_ON,
	WDOG_CONF_LPOCLK_PRESC_OFF,
	WDOG_CONF_LPOCLK_PRESC_ON,
	WDOG_CONF_SIRCCLK_PRESC_OFF,
	WDOG_CONF_SIRCCLK_PRESC_ON,
	WDOG_CONF_SOSCCLK_PRESC_OFF,
	WDOG_CONF_SOSCCLK_PRESC_ON
} WDOG_CONF;

void wdog_set(WDOG_CONF wdog_conf, uint16_t timeout);

static inline void wdog_refresh(void)
{
	WDOG0->CNT = WDOG_REFRESH_SEQ;
}

#endif /* _WDOG_H_ */
