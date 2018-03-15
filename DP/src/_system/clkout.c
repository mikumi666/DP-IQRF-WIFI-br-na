/*
 * Name: clkout.c
 * Author: Martin Stankus
 *
 */

#include "MKL28Z7.h"
#include "clkout.h"

void clkout_set(CLKOUT_CONF clkout_conf)
{
	CLKOUT_PCC = PCC_CLKCFG_CGC_MASK;
	CLKOUT_PORT->PCR[CLKOUT_IOIND] = 0;

	switch (clkout_conf) {
	case CLKOUT_CONF_DIS:
		break;
	case CLKOUT_CONF_BUSCLK:
		SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(SCG_CKOUTCFG_SEL_VAL_BUSCLK);
		CLKOUT_PORT->PCR[CLKOUT_IOIND] = PORT_PCR_MUX(CLKOUT_PORT_PCR_MUX_VAL);
		break;
	case CLKOUT_CONF_SOSCCLK:
		SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(SCG_CKOUTCFG_SEL_VAL_SOSCCLK);
		CLKOUT_PORT->PCR[CLKOUT_IOIND] = PORT_PCR_MUX(CLKOUT_PORT_PCR_MUX_VAL);
		break;
	case CLKOUT_CONF_SIRCCLK:
		SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(SCG_CKOUTCFG_SEL_VAL_SIRCCLK);
		CLKOUT_PORT->PCR[CLKOUT_IOIND] = PORT_PCR_MUX(CLKOUT_PORT_PCR_MUX_VAL);
		break;
	case CLKOUT_CONF_FIRCCLK:
		SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(SCG_CKOUTCFG_SEL_VAL_FIRCCLK);
		CLKOUT_PORT->PCR[CLKOUT_IOIND] = PORT_PCR_MUX(CLKOUT_PORT_PCR_MUX_VAL);
		break;
	case CLKOUT_CONF_SPLLCLK:
		SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(SCG_CKOUTCFG_SEL_VAL_SPLLCLK);
		CLKOUT_PORT->PCR[CLKOUT_IOIND] = PORT_PCR_MUX(CLKOUT_PORT_PCR_MUX_VAL);
		break;
	default:
		break;
	}
}