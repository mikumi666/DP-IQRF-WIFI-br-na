/*
 *==============================================================================
 * Name: tstmr.s
 * Author: Martin Stankus
 *
 * as docs: https://sourceware.org/binutils/docs/as/index.html
 *
 *==============================================================================
 */

	.syntax unified
	.arch armv6-m
	.thumb

	.global tstmr_get_val

	.set TSTMR_L_ADDR, 0x400750F0
	.set TSTMR_H_ADDR, 0x400750F4

/*
 *==============================================================================
 * tstmr_get_val
 *==============================================================================
 */

	.section .text.tstmr_get_val, "ax", %progbits
	.type tstmr_get_val, %function
	.thumb_func
tstmr_get_val:

	mrs r3, primask
	cpsid i
	ldr r2, =TSTMR_L_ADDR
	ldr r0, [r2]
	ldr r2, =TSTMR_H_ADDR
	ldr r1, [r2]
	msr primask, r3
	bx lr

	.size tstmr_get_val, . - tstmr_get_val

	.end
