/*
 * Name: wdog.c
 * Author: Martin Stankus
 *
 */

#include "MKL28Z7.h"
#include "wdog.h"
#include "tstmr.h"

void wdog_set(WDOG_CONF wdog_conf, uint16_t timeout)
{
	uint64_t delay_begin, delay_end;
	uint32_t primask;

	if (timeout == 0) {
		timeout = 0xFFFFu;
	}

	primask = __get_PRIMASK();
	__disable_irq();

	WDOG0->CNT = WDOG_UNLOCK_SEQ;
	WDOG0->TOVAL = timeout;

	switch (wdog_conf) {
	case WDOG_CONF_DIS:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_BUSCLK_PRESC_OFF:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_BUSCLK |
					WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_BUSCLK_PRESC_ON:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_BUSCLK |
					WDOG_CS_PRES_MASK | WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_LPOCLK_PRESC_OFF:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_LPOCLK |
					WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_LPOCLK_PRESC_ON:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_LPOCLK |
					WDOG_CS_PRES_MASK | WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_SIRCCLK_PRESC_OFF:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_SIRCCLK |
					WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_SIRCCLK_PRESC_ON:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_SIRCCLK |
					WDOG_CS_PRES_MASK | WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_SOSCCLK_PRESC_OFF:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_SOSCCLK |
					WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	case WDOG_CONF_SOSCCLK_PRESC_ON:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_CLK_VAL_SOSCCLK |
					WDOG_CS_PRES_MASK | WDOG_CS_EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	default:
		WDOG0->CS = WDOG_CS_CMD32EN_MASK | WDOG_CS_UPDATE_MASK;
		break;
	}

	delay_begin = tstmr_get_val();

	do {
		delay_end = tstmr_get_val();
	} while ((delay_end - delay_begin) < WDOG_CONF_DELAY);

	__set_PRIMASK(primask);
}
