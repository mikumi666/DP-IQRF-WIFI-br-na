/*
 * fram.c
 *
 *  Created on: 19. 3. 2018
 *      Author: mirek
 */

#include "fram.h"

#include <stddef.h>
#include "MKL28Z7.h"
#include "pcc.h"
#include "mem.h"
#include "board.h"

#define FRAM_LPSPI_PRI 				2
#define FRAM_LPSPI					LPSPI0
#define FRAM_PCC_LPSPI				PCC_LPSPI0
#define FRAM_PCC_PORT				PCC_PORTC

#define FRAM_LPSPI_CLK_PCS			PCC_CKFG_PCS_VAL_SIRCLK
#define FRAM_LPSPI_CCR_SP			64u
#define FRAM_LPSPI_CCR_PS			64u
#define FRAM_LPSPI_CCR_DBT			64u
#define FRAM_LPSPI_CCR_SCKD			64u

#define FRAM_LPSPI_PCS				0u
#define FRAM_LPSPI_TCR_PRESC		0b011

#define FRAM_LPSPI_TXQ_LEN_EXP		((FRAM_LPSPI->PARAM & LPSPI_PARAM_TXFIFO_MASK) >> LPSPI_PARAM_TXFIFO_SHIFT)
#define FRAM_LPSPI_TXQ_LEN			(1u << FRAM_LPSPI_TXQ_LEN_EXP)


#define FRAM_LPSPI_CMD_READ(addr)	((3u << 24) | (len))

static bool state_en = false;
static uint8_t* rx_buff;
static uint16_t rx_rec_size;
static uint16_t rx_buff_size;

static uint8_t* tx_buff;
static uint32_t tx_addr;
static uint16_t	tx_send_size;
static uint16_t tx_buff_size;

static void (*event)(FRAM_Event_t e);

void __attribute__ ((interrupt)) LPSPI0_IRQHandler(void)
{
	if((FRAM_LPSPI->IER & LPSPI_IER_TDIE_MASK) && (FRAM_LPSPI->SR & LPSPI_SR_TDF_MASK))
	{
		uint16_t tx_len;
		if((tx_buff_size - tx_send_size) < 4)
		{
			tx_len = (tx_buff_size - tx_send_size)%4;
		}
		else
		{
			tx_len = 4;
		}
		uint32_t temp_TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
							LPSPI_TCR_CONTC_MASK | LPSPI_TCR_FRAMESZ(tx_len*8 - 1) |
							LPSPI_TCR_BYSW_MASK;

		if((tx_send_size + tx_len) < tx_buff_size)
		{
			temp_TCR |= LPSPI_TCR_CONT_MASK;
		}
		else
		{
			FRAM_LPSPI->IER &= ~LPSPI_IER_TDIE_MASK;
		}

		FRAM_LPSPI->TCR = temp_TCR;
		/*FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK | LPSPI_TCR_FRAMESZ(63u);*/
		uint32_t yy;
		memcpy((void*)&yy, (void*)(tx_buff + tx_send_size), tx_len);

		FRAM_LPSPI->TDR;


		tx_send_size += tx_len;

		/*uint16_t tx_len = (tx_buff_size - tx_send_size)%4;
		memcpy((void*)&FRAM_LPSPI->TDR, (void*)(tx_buff + tx_send_size), tx_len);
		tx_send_size += tx_len;
		if((tx_buff_size - tx_send_size) <= 0)
		{
			if(event != NULL)
			{
				event(FRAM_Event_TxComplete);
			}
		}
		FRAM_LPSPI->SR = LPSPI_SR_TDF_MASK;*/
	}
	else if(FRAM_LPSPI->SR & LPSPI_SR_RDF_MASK)
	{
		uint16_t rx_len = (rx_buff_size - rx_rec_size)%4;
		memcpy((void*)(rx_buff + rx_rec_size), (void*)&FRAM_LPSPI->RDR, rx_len);
		rx_rec_size += rx_len;
		if((rx_buff_size - rx_rec_size) <= 0)
		{
			if (event != NULL)
			{
				event(FRAM_Event_RxComplete);
			}
		}
		FRAM_LPSPI->SR = LPSPI_SR_RDF_MASK;
	}
	else if((FRAM_LPSPI->IER & LPSPI_IER_WCIE_MASK) & (FRAM_LPSPI->SR & LPSPI_SR_WCF_MASK))
	{
		/*send write opcode after WREN and enable TDIE*/
		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(31u);
		FRAM_LPSPI->TDR = (uint32_t)((1u << 25) | 0);

		FRAM_LPSPI->IER = LPSPI_IER_TDIE_MASK;
	}
	/*else if(FRAM_LPSPI->SR & LPSPI_SR_RDF_MASK)
	{
		uint16_t rx_len = (rx_buff_size - rx_rec_size)%4;
		memcpy((void*)(rx_buff + rx_rec_size), (void*)&FRAM_LPSPI->RDR, rx_len);
		rx_rec_size += rx_len;
		if((rx_buff_size - rx_rec_size) <= 0)
		{
			if (event != NULL)
			{
				event(FRAM_Event_RxComplete);
			}
		}
		FRAM_LPSPI->SR = LPSPI_SR_RDF_MASK;
	}*/
	/*if(FRAM_LPSPI->SR & LPSPI_SR_TDF_MASK)
	{
		FRAM_LPSPI->IER &= ~LPSPI_IER_TDIE_MASK;
		/*uint16_t tx_len = (tx_buff_size - tx_send_size)%4;
		memcpy((void*)&FRAM_LPSPI->TDR, (void*)(tx_buff + tx_send_size), tx_len);
		tx_send_size += tx_len;
		if((tx_buff_size - tx_send_size) <= 0)
		{
			if(event != NULL)
			{
				event(FRAM_Event_TxComplete);
			}
		}
		FRAM_LPSPI->SR = LPSPI_SR_TDF_MASK;*/
	/*}*/
	if(FRAM_LPSPI->SR & LPSPI_SR_TCF_MASK)
	{
		FRAM_LPSPI->SR = LPSPI_SR_TCF_MASK;
	}
}

void FRAM_Init(void (*FRAM_Event)(FRAM_Event_t))
{
	event = FRAM_Event;
	NVIC_SetPriority(LPSPI0_IRQn, FRAM_LPSPI_PRI);
	NVIC_EnableIRQ(LPSPI0_IRQn);
}

void FRAM_Enable(void)
{
	if(state_en == false)
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
		state_en = true;
	}

}

void FRAM_Disable(void)
{
	if(state_en == true)
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

		state_en = false;
	}
}

bool FRAM_Write(uint32_t addr, void* data, uint16_t len)
{
	bool retVal = false;

	if(state_en == true)
	{
		tx_buff = (uint8_t*)data;
		tx_addr = addr;
		tx_buff_size = len;
		tx_send_size = 0;

		FRAM_LPSPI->IER |= LPSPI_IER_WCIE_MASK;

		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_FRAMESZ(7u);
		FRAM_LPSPI->TDR = 0b00000110;

		/*for (int i = 0; i < 200; i++);

		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(31u);
		FRAM_LPSPI->TDR = (uint32_t)((1u << 25) | 0);

		FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK | LPSPI_TCR_FRAMESZ(31u);
		FRAM_LPSPI->TDR = (uint32_t)0xFFFFFFFF;*/


		/*FRAM_LPSPI->TCR = LPSPI_TCR_PCS(FRAM_LPSPI_CLK_PCS) | LPSPI_TCR_RXMSK_MASK |
						  LPSPI_TCR_CONTC_MASK | LPSPI_TCR_FRAMESZ(31u);*/



	}

	return retVal;
}

bool FRAM_Read(uint32_t addr, void* data, uint16_t len)
{
	bool retVal = false;

	if(state_en == true)
	{
		int a = FRAM_LPSPI_CMD_READ(addr);
		FRAM_LPSPI->TCR = LPSPI_TCR_PCS(FRAM_LPSPI_PCS) | LPSPI_TCR_RXMSK_MASK |
				 LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(31);
		FRAM_LPSPI->TDR = (3u << 24);

		FRAM_LPSPI->TCR = LPSPI_TCR_PCS(FRAM_LPSPI_PCS) | LPSPI_TCR_TXMSK_MASK |
				LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK |
				LPSPI_TCR_FRAMESZ(31u);

		FRAM_LPSPI->IER = LPSPI_IER_RDIE_MASK;

	}

	return retVal;
}

bool FRAM_ReadStat()
{
	if(state_en == true)
		{
			FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_PCS(FRAM_LPSPI_PCS) | LPSPI_TCR_RXMSK_MASK |
					 LPSPI_TCR_CONT_MASK | LPSPI_TCR_FRAMESZ(7u);
			FRAM_LPSPI->TDR = 5;

			FRAM_LPSPI->TCR = LPSPI_TCR_PRESCALE(FRAM_LPSPI_TCR_PRESC) | LPSPI_TCR_PCS(FRAM_LPSPI_PCS) | LPSPI_TCR_TXMSK_MASK |
					LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK |
					LPSPI_TCR_FRAMESZ(7u);

			FRAM_LPSPI->IER = LPSPI_IER_RDIE_MASK;
		}
}
