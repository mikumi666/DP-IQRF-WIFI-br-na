/*
 * fram_prv.h
 *	FRAM: CY15B104Q
 *  Created on: 26. 3. 2018
 *      Author: mirek
 */

#ifndef FRAM_PRV_H_
#define FRAM_PRV_H_

/******************************Defines******************************/
/////////////////////////////////////////////////////////////////////

	/*********PORT & GPIO defs*********/
#define FRAM_PCC_LPSPI				PCC_LPSPI0
#define FRAM_PCC_PORT				PCC_PORTC
#define PORT_FRAM					PORTC
#define GPIO_FRAM					GPIOC

#define IOIND_FRAM_EN				16u
#define IOIND_FRAM_WP				17u
#define IOIND_FRAM_HOLD				18u
#define IOIND_FRAM_CS				4u
#define IOIND_FRAM_SCK				5u
#define IOIND_FRAM_MOSI				6u
#define IOIND_FRAM_MISO				7u

#define FRAM_EN_MASK				(1u << IOIND_FRAM_EN)
#define FRAM_WP_MASK				(1u << IOIND_FRAM_WP)
#define	FRAM_HOLD_MASK				(1u << IOIND_FRAM_HOLD)

	/*************SPI defs*************/
#define FRAM_LPSPI					LPSPI0
#define FRAM_LPSPI_PRI 				2

#define FRAM_LPSPI_CLK_PCS			PCC_CKFG_PCS_VAL_FIRCLK
#define FRAM_LPSPI_CCR_SP			8u
#define FRAM_LPSPI_CCR_PS			8u
#define FRAM_LPSPI_CCR_DBT			8u
#define FRAM_LPSPI_CCR_SCKD			0u

#define FRAM_LPSPI_PCS				0u
#define FRAM_LPSPI_TCR_PRESC		0b000

#define FRAM_LPSPI_TXQ_LEN_EXP		((FRAM_LPSPI->PARAM & LPSPI_PARAM_TXFIFO_MASK) >> LPSPI_PARAM_TXFIFO_SHIFT)
#define FRAM_LPSPI_TXQ_LEN			(1u << FRAM_LPSPI_TXQ_LEN_EXP)


/*****************************Commands******************************/
/////////////////////////////////////////////////////////////////////

#define FRAM_OPCODE_WREN			(0b00000110)
#define FRAM_OPCODE_WRDI			(0b00000100)
#define FRAM_OPCODE_RDSR			(0b00000101)
#define FRAM_OPCODE_WRSR			(0b00000001)
#define FRAM_OPCODE_READ			(0b00000011)
#define FRAM_OPCODE_FSTRD			(0b00001011)
#define FRAM_OPCODE_WRITE			(0b00000010)
#define FRAM_OPCODE_SLEEP			(0b10111001)
#define FRAM_OPCODE_RDID			(0b10011111)

#define FRAM_CMD_READ(addr)			((FRAM_OPCODE_READ << 24) | addr)
#define FRAM_CMD_WRITE(addr)		((FRAM_OPCODE_WRITE << 24) | addr)


/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef enum
{
	FRAM_State_Disable, FRAM_State_Enable, FRAM_State_Reading, FRAM_State_Writing
}FRAM_State_t;


#endif /* FRAM_PRV_H_ */
