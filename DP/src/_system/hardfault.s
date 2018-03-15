/*
 *==============================================================================
 * Name: hardfault.s
 * Author: Martin Stankus
 *
 * as docs: https://sourceware.org/binutils/docs/as/index.html
 *
 * "It's a trap!"
 *==============================================================================
 */

	.syntax unified
	.arch armv6-m
	.thumb

	.global HardFault_Handler

/*
 *==============================================================================
 * hardfault handler
 *==============================================================================
 */

	.section .text.HardFault_Handler, "ax", %progbits
	.type HardFault_Handler, %function
	.thumb_func
HardFault_Handler:

	bkpt
	b HardFault_Handler

	.size HardFault_Handler, . - HardFault_Handler

	.end
