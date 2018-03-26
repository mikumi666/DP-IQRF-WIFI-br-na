/*
 * fram.c
 *	FRAM: CY15B104Q
 *  Created on: 19. 3. 2018
 *      Author: mirek
 */

#include "fram.h"
#include "fram_prv.h"

#include <stddef.h>

#include "MKL28Z7.h"
#include "mem.h"
#include "pcc.h"
#include "port.h"


/*****************************Variables*****************************/
/////////////////////////////////////////////////////////////////////

static FRAM_State_t state = FRAM_State_Disable;

static uint8_t* rx_buff;
static uint16_t rx_rec_size;
static uint16_t rx_buff_size;

static uint8_t* tx_buff;
static uint32_t tx_addr;
static uint16_t	tx_send_size;
static uint16_t tx_buff_size;

static void (*event)(FRAM_Event_t e);


/****************************IRQ handler****************************/
/////////////////////////////////////////////////////////////////////

void __attribute__ ((interrupt)) LPSPI0_IRQHandler(void)
{
	if((FRAM_LPSPI->IER & LPSPI_IER_TDIE_MASK) && (FRAM_LPSPI->SR & LPSPI_SR_TDF_MASK))
	{
		uint16_t tx_len;
		if((tx_buff_size - tx_send_size) < 4)
		{
			tx_len = tx_buff_size - tx_send_size;
			uint32_t data = 0;
			memcpy((void*)(&data), (void*)(tx_buff + tx_send_size), tx_len);

			FRAM_LPSPI->TDR = data << (4 - tx_len)*8;
		}
		else
		{
			tx_len = 4;
			uint32_t yy = 0;
			memcpy((void*)(&yy), (void*)(tx_buff + tx_send_size), tx_len);

			FRAM_LPSPI->TDR = *(uint32_t*)(tx_buff + tx_send_size);
		}

		tx_send_size += tx_len;

		if(tx_send_size == tx_buff_size)
		{
			state = FRAM_State_Enable;
			if(event != NULL)
			{
				event(FRAM_Event_TxComplete);
			}
			FRAM_LPSPI->IER &= LPSPI_IER_TDIE_MASK;
		}

	}
	else if((FRAM_LPSPI->IER & LPSPI_IER_RDIE_MASK) && (FRAM_LPSPI->SR & LPSPI_SR_RDF_MASK))
	{
		uint16_t rx_len;
		if((rx_buff_size - rx_rec_size) < 4)
		{
			rx_len = rx_buff_size - rx_rec_size;
			uint32_t data = FRAM_LPSPI->RDR >> (4 - rx_len)*8;
			memcpy((void*)(rx_buff + rx_rec_size), (void*)&data, rx_len);
		}
		else
		{
			rx_len = 4;
			*(uint32_t*)(rx_buff + rx_rec_size) = FRAM_LPSPI->RDR;
		}
		rx_rec_size += rx_len;
		if((rx_buff_size - rx_rec_size) <= 0)
		{
			state = FRAM_State_Enable;
			if (event != NULL)
			{
				event(FRAM_Event_RxComplete);
			}
			FRAM_LPSPI->IER &= ~LPSPI_IER_RDIE_MASK;
		}
	}
	else if((FRAM_LPSPI->IER & LPSPI_IER_WCIE_MASK) && (FRAM_LPSPI->SR & LPSPI_SR_WCF_MASK))
	{
		/*send write opcode after WREN and enable TDIE*/
		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(31u);
		FRAM_LPSPI->TDR = FRAM_CMD_WRITE(tx_addr);

		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK |
						  LPSPI_TCR_FRAMESZ(tx_buff_size*8-1);

		FRAM_LPSPI->IER = LPSPI_IER_TDIE_MASK;
	}
}


/*****************************functions*****************************/
/////////////////////////////////////////////////////////////////////

void FRAM_Init(void (*FRAM_Event)(FRAM_Event_t))
{
	event = FRAM_Event;
	NVIC_SetPriority(LPSPI0_IRQn, FRAM_LPSPI_PRI);
	NVIC_EnableIRQ(LPSPI0_IRQn);
}

bool FRAM_Enable(void)
{
	bool retVal = false;
	if(state == FRAM_State_Disable)
	{
		FRAM_PCC_PORT = PCC_CLKCFG_CGC_MASK;

		GPIO_FRAM->PCOR = FRAM_EN_MASK;
		GPIO_FRAM->PSOR = FRAM_WP_MASK | FRAM_HOLD_MASK;
		GPIO_FRAM->PDDR = FRAM_EN_MASK | FRAM_WP_MASK | FRAM_HOLD_MASK;

		PORT_FRAM->PCR[IOIND_FRAM_EN] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
		PORT_FRAM->PCR[IOIND_FRAM_WP] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
		PORT_FRAM->PCR[IOIND_FRAM_HOLD] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
		PORT_FRAM->PCR[IOIND_FRAM_CS] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT2);
		PORT_FRAM->PCR[IOIND_FRAM_SCK] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT2);
		PORT_FRAM->PCR[IOIND_FRAM_MISO] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT2);
		PORT_FRAM->PCR[IOIND_FRAM_MOSI] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT2);

		FRAM_PCC_LPSPI = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(PCC_CKFG_PCS_VAL_SIRCLK);

		FRAM_LPSPI->CCR = LPSPI_CCR_SCKPCS(FRAM_LPSPI_CCR_SP) | LPSPI_CCR_PCSSCK(FRAM_LPSPI_CCR_PS) |
				LPSPI_CCR_DBT(FRAM_LPSPI_CCR_DBT) | LPSPI_CCR_SCKDIV(FRAM_LPSPI_CCR_SCKD);

		FRAM_LPSPI->FCR = LPSPI_FCR_TXWATER(FRAM_LPSPI_TXQ_LEN - 1u);

		FRAM_LPSPI->CFGR1 = LPSPI_CFGR1_MASTER_MASK;
		FRAM_LPSPI->CR = LPSPI_CR_MEN_MASK;
		state = FRAM_State_Enable;
		retVal = true;
	}
	return retVal;
}

bool FRAM_Disable(void)
{
	bool retVal = false;
	if(state == FRAM_State_Enable)
	{
		PORT_FRAM->PCR[IOIND_FRAM_CS] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
		PORT_FRAM->PCR[IOIND_FRAM_SCK] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
		PORT_FRAM->PCR[IOIND_FRAM_MISO] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
		PORT_FRAM->PCR[IOIND_FRAM_MOSI] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

		PORT_FRAM->PCR[IOIND_FRAM_HOLD] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
		PORT_FRAM->PCR[IOIND_FRAM_WP] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
		PORT_FRAM->PCR[IOIND_FRAM_EN] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

		FRAM_PCC_PORT = 0u;

		FRAM_LPSPI->CR = LPSPI_CR_RST_MASK;
		FRAM_LPSPI->CR = 0u;

		FRAM_PCC_LPSPI = 0ul;

		state = FRAM_State_Disable;
		retVal = true;
	}
	return retVal;
}

bool FRAM_Write(uint32_t addr, void* data, uint16_t len)
{
	bool retVal = false;

	if(state == FRAM_State_Enable)
	{
		tx_buff = (uint8_t*)data;
		tx_addr = addr;
		tx_buff_size = len;
		tx_send_size = 0;

		FRAM_LPSPI->IER |= LPSPI_IER_WCIE_MASK;

		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_FRAMESZ(7u);
		FRAM_LPSPI->TDR = FRAM_OPCODE_WREN;

		state = FRAM_State_Writing;
	}

	return retVal;
}

bool FRAM_Read(uint32_t addr, void* data, uint16_t len)
{
	bool retVal = false;
	rx_buff = (uint8_t*)data;
	rx_buff_size = len;

	if(state == FRAM_State_Enable)
	{
		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_PCS(FRAM_LPSPI_PCS) |
						  LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(31);
		FRAM_LPSPI->TDR = FRAM_CMD_READ(addr);

		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_PCS(FRAM_LPSPI_PCS) |
						  LPSPI_TCR_TXMSK_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK |
						  LPSPI_TCR_FRAMESZ(len*8 - 1);

		state = FRAM_State_Reading;

		FRAM_LPSPI->IER = LPSPI_IER_RDIE_MASK;
	}

	return retVal;
}

bool FRAM_ReadStat(uint8_t* data)
{
	bool retVal = false;
	if(state == FRAM_State_Enable)
		{
			FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_PCS(FRAM_LPSPI_PCS) | LPSPI_TCR_RXMSK_MASK |
					 LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(7u);
			FRAM_LPSPI->TDR = FRAM_OPCODE_RDSR;

			FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_PCS(FRAM_LPSPI_PCS) | LPSPI_TCR_TXMSK_MASK |
					LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK |
					LPSPI_TCR_FRAMESZ(7u);

			for(uint32_t i = 0; i < 5000; i++)
			{
				if(FRAM_LPSPI->SR & LPSPI_SR_RDF_MASK)
				{
					*data = (uint8_t)(FRAM_LPSPI->RDR >> 24);
					retVal = true;
					break;
				}
			}
		}
	return retVal;
}
