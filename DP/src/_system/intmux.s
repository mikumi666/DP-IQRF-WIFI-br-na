/*
 *==============================================================================
 * Name: intmux.s
 * Author: Martin Stankus
 *
 * as docs: https://sourceware.org/binutils/docs/as/index.html
 *
 *==============================================================================
 */

	.syntax unified
	.arch armv6-m
	.thumb

	.global INTMUX0_0_IRQHandler
	.global INTMUX0_1_IRQHandler
	.global INTMUX0_2_IRQHandler
	.global INTMUX0_3_IRQHandler

	.set INTMUX0_0_IRQHandler, INTMUX0_IRQHandler
	.set INTMUX0_1_IRQHandler, INTMUX0_IRQHandler
	.set INTMUX0_2_IRQHandler, INTMUX0_IRQHandler
	.set INTMUX0_3_IRQHandler, INTMUX0_IRQHandler

	.set IPSR_INTMUX0_MASK, 0x00000003
	.set IPSR_INTMUX0_SHFT, 6
	.set VEC_BASE_ADDR, 0x40024004
	.set VTOR_ADDR, 0xE000ED08

/*
 *==============================================================================
 * intmux0 handler
 *==============================================================================
 */

	.section .text.INTMUX0_IRQHandler, "ax", %progbits
	.type INTMUX0_IRQHandler, %function
	.thumb_func
INTMUX0_IRQHandler:

	push {lr}
	mrs r0, ipsr
	ldr r1, =IPSR_INTMUX0_MASK
	ands r0, r1
	lsls r0, #IPSR_INTMUX0_SHFT
	ldr r1, =VEC_BASE_ADDR
	orrs r0, r1
	ldr r0, [r0]
	movs r0, r0
	beq INTMUX0_IRQHandler_end
	ldr r1, =VTOR_ADDR
	ldr r1, [r1]
	adds r0, r1
	ldr r0, [r0]
	blx r0
INTMUX0_IRQHandler_end:
	pop {pc}

	.size INTMUX0_IRQHandler, . - INTMUX0_IRQHandler

	.end
