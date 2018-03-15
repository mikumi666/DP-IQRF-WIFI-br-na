/*
 * board.h
 *
 *  Created on: 10. 3. 2018
 *      Author: mirek
 */

#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <stdbool.h>
#include "MKL28Z7.h"
#include "port.h"
#include "intmux.h"

/* MCU LED */
#define PORT_MCU_LEDS			PORTB
#define GPIO_MCU_LEDS			GPIOB

#define IOIND_MCU_LED1			10u
#define IOIND_MCU_LED2			9u
#define IOIND_MCU_LED3			8u
#define IOIND_MCU_LED4			7u

#define MCU_LED1_MASK			(1u << IOIND_MCU_LED1)
#define MCU_LED2_MASK			(1u << IOIND_MCU_LED2)
#define MCU_LED3_MASK			(1u << IOIND_MCU_LED3)
#define MCU_LED4_MASK			(1u << IOIND_MCU_LED4)

#define MCU_LEDS_MASK			(MCU_LED1_MASK | MCU_LED2_MASK | MCU_LED3_MASK | MCU_LED4_MASK)

/* MCU BTN */
#define PORT_MCU_BTNS			PORTB
#define GPIO_MCU_BTNS			GPIOB

#define IOIND_MCU_BTN1			3u
#define IOIND_MCU_BTN2			2u
#define IOIND_MCU_BTN3			1u
#define IOIND_MCU_BTN4			0u

#define MCU_BTN1_MASK			(1u << IOIND_MCU_BTN1)
#define MCU_BTN2_MASK			(1u << IOIND_MCU_BTN2)
#define MCU_BTN3_MASK			(1u << IOIND_MCU_BTN3)
#define MCU_BTN4_MASK			(1u << IOIND_MCU_BTN4)

#define MCU_BTNS_MASK			(MCU_BTN1_MASK | MCU_BTN2_MASK | MCU_BTN3_MASK | MCU_BTN4_MASK)

/* MCU SW */
#define PORT_MCU_SW				PORTB
#define GPIO_MCU_SW				GPIOB

#define IOIND_MCU_SW1			18u
#define IOIND_MCU_SW2			17u
#define IOIND_MCU_SW3			16u
#define IOIND_MCU_SW4			15u

#define MCU_SW1_MASK			(1u << IOIND_MCU_SW1)
#define MCU_SW2_MASK			(1u << IOIND_MCU_SW2)
#define MCU_SW3_MASK			(1u << IOIND_MCU_SW3)
#define MCU_SW4_MASK			(1u << IOIND_MCU_SW4)

#define MCU_SWS_MASK			(MCU_SW1_MASK | MCU_SW2_MASK | MCU_SW3_MASK | MCU_SW4_MASK)

/* */
#define PORT_WIFI				PORTD
#define GPIO_WIFI				GPIOD

#define IOIND_WIFI_EN			4u
#define IOIND_WIFI_TX			2u
#define IOIND_WIFI_RX			3u
#define IOIND_WIFI_RESET		5u
#define IOIND_WIFI_WAKE			6u
#define IOIND_WIFI_IO			7u





void Board_Init(void);
void Board_MCULED_Set(uint8_t led, bool value);
bool Board_MCUBTN_Get(uint8_t btn);

#endif /* SRC_BOARD_H_ */
