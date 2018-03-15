/*
 * Name: io_frdm.h
 * Author: Martin Stankus
 *
 */

#ifndef _IO_FRDM_H_
#define _IO_FRDM_H_

#include "MKL28Z7.h"

// rgb led *********************************************************************

#define PORT_LEDR				PORTE
#define GPIO_LEDR				GPIOE
#define IOIND_LEDR				29
#define IOMASK_LEDR				(1ul << IOIND_LEDR)

#define PORT_LEDG				PORTC
#define GPIO_LEDG				GPIOC
#define IOIND_LEDG				4
#define IOMASK_LEDG				(1ul << IOIND_LEDG)

#define PORT_LEDB				PORTE
#define GPIO_LEDB				GPIOE
#define IOIND_LEDB				31
#define IOMASK_LEDB				(1ul << IOIND_LEDB)

// lpspi2 **********************************************************************

#define PORT_LPSPI2_PCS0		PORTB
#define IOIND_LPSPI2_PCS0		20

#define PORT_LPSPI2_SCK			PORTB
#define IOIND_LPSPI2_SCK		21

#define PORT_LPSPI2_SOUT		PORTB
#define IOIND_LPSPI2_SOUT		22

#define PORT_LPSPI2_SIN			PORTB
#define IOIND_LPSPI2_SIN		23

// lpuart2 *********************************************************************

#define PORT_LPUART2_RTS		PORTD
#define IOIND_LPUART2_RTS		0

#define PORT_LPUART2_CTS		PORTD
#define IOIND_LPUART2_CTS		1

#define PORT_LPUART2_RX			PORTD
#define IOIND_LPUART2_RX		2

#define PORT_LPUART2_TX			PORTD
#define IOIND_LPUART2_TX		3

// light sensor ****************************************************************

#define ADC_CHAN_LSENS			6
#define ADC_MUX_LSENS			ADC_CFG2_MUXSEL_MASK

#endif /* _IO_FRDM_H_ */
