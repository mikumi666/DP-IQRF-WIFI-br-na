/*
 *==============================================================================
 * Name: startup_MKL28Z7.s
 * Author: Martin Stankus
 *
 * as docs: https://sourceware.org/binutils/docs/as/index.html
 *==============================================================================
 */

	.syntax unified
	.arch armv6-m
	.thumb

	.global Reset_Handler

/*
 *==============================================================================
 * reset handler
 *==============================================================================
 */

	.section .text.Reset_Handler, "ax", %progbits
	.type Reset_Handler, %function
	.thumb_func
Reset_Handler:

	bl SystemInit

	/* data section init */

	ldr r0, =DATA_BEGIN
	ldr r1, =DATA_INIT
	ldr r2, =DATA_END
	subs r2, r0
	bl memcpy

	/* bss section zeroization */

	ldr r0, =BSS_BEGIN
	ldr r1, =0x00
	ldr r2, =BSS_END
	subs r2, r0
	bl memset

	/* stack pattern fill */
	/* this is for debugging purposes */

	ldr r0, =STACK_BOTTOM
	ldr r1, =0xA5
	ldr r2, =STACK_TOP
	subs r2, r0
	bl memset

	bl SystemCoreClockUpdate

	bl main

stop:
	b stop

	.size Reset_Handler, . - Reset_Handler

/*
 *==============================================================================
 * default handler
 *==============================================================================
 */

	.section .text.Default_Handler, "ax", %progbits
	.type Default_Handler, %function
	.thumb_func
Default_Handler:

	bkpt
	b Default_Handler

	.size Default_Handler, . - Default_Handler

/*
 *==============================================================================
 * vector table
 *==============================================================================
 */

	.section .vectab, "a", %progbits

	/* stack init value, system exceptions */
	.long STACK_TOP
	.long Reset_Handler
	.long NMI_Handler
	.long HardFault_Handler
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long SVCall_Handler
	.long 0
	.long 0
	.long PendSV_Handler
	.long SysTick_Handler

	/* interrupts */
	.long DMA0_04_IRQHandler
	.long DMA0_15_IRQHandler
	.long DMA0_26_IRQHandler
	.long DMA0_37_IRQHandler
	.long CTI0_DMA0_Error_IRQHandler
	.long FLEXIO0_IRQHandler
	.long TPM0_IRQHandler
	.long TPM1_IRQHandler
	.long TPM2_IRQHandler
	.long LPIT0_IRQHandler
	.long LPSPI0_IRQHandler
	.long LPSPI1_IRQHandler
	.long LPUART0_IRQHandler
	.long LPUART1_IRQHandler
	.long LPI2C0_IRQHandler
	.long LPI2C1_IRQHandler
	.long Reserved32_IRQHandler
	.long PORTA_IRQHandler
	.long PORTB_IRQHandler
	.long PORTC_IRQHandler
	.long PORTD_IRQHandler
	.long PORTE_IRQHandler
	.long LLWU0_IRQHandler
	.long I2S0_IRQHandler
	.long USB0_IRQHandler
	.long ADC0_IRQHandler
	.long LPTMR0_IRQHandler
	.long RTC_Seconds_IRQHandler
	.long INTMUX0_0_IRQHandler
	.long INTMUX0_1_IRQHandler
	.long INTMUX0_2_IRQHandler
	.long INTMUX0_3_IRQHandler

	/* intmux0 sources */
	.long LPTMR1_IRQHandler
	.long Reserved49_IRQHandler
	.long Reserved50_IRQHandler
	.long Reserved51_IRQHandler
	.long LPSPI2_IRQHandler
	.long LPUART2_IRQHandler
	.long EMVSIM0_IRQHandler
	.long LPI2C2_IRQHandler
	.long TSI0_IRQHandler
	.long PMC_IRQHandler
	.long FTFA_IRQHandler
	.long SCG_IRQHandler
	.long WDOG0_IRQHandler
	.long DAC0_IRQHandler
	.long TRNG_IRQHandler
	.long RCM_IRQHandler
	.long CMP0_IRQHandler
	.long CMP1_IRQHandler
	.long RTC_IRQHandler
	.long Reserved67_IRQHandler
	.long Reserved68_IRQHandler
	.long Reserved69_IRQHandler
	.long Reserved70_IRQHandler
	.long Reserved71_IRQHandler
	.long Reserved72_IRQHandler
	.long Reserved73_IRQHandler
	.long Reserved74_IRQHandler
	.long Reserved75_IRQHandler
	.long Reserved76_IRQHandler
	.long Reserved77_IRQHandler
	.long Reserved78_IRQHandler
	.long Reserved79_IRQHandler

	/* vector table ends here */

	/* vectors without user handlers point to default handler */
	/* handlers are declared weak to make this possible */

	.weak NMI_Handler
	.weak HardFault_Handler
	.weak SVCall_Handler
	.weak PendSV_Handler
	.weak SysTick_Handler
	.weak DMA0_04_IRQHandler
	.weak DMA0_15_IRQHandler
	.weak DMA0_26_IRQHandler
	.weak DMA0_37_IRQHandler
	.weak CTI0_DMA0_Error_IRQHandler
	.weak FLEXIO0_IRQHandler
	.weak TPM0_IRQHandler
	.weak TPM1_IRQHandler
	.weak TPM2_IRQHandler
	.weak LPIT0_IRQHandler
	.weak LPSPI0_IRQHandler
	.weak LPSPI1_IRQHandler
	.weak LPUART0_IRQHandler
	.weak LPUART1_IRQHandler
	.weak LPI2C0_IRQHandler
	.weak LPI2C1_IRQHandler
	.weak Reserved32_IRQHandler
	.weak PORTA_IRQHandler
	.weak PORTB_IRQHandler
	.weak PORTC_IRQHandler
	.weak PORTD_IRQHandler
	.weak PORTE_IRQHandler
	.weak LLWU0_IRQHandler
	.weak I2S0_IRQHandler
	.weak USB0_IRQHandler
	.weak ADC0_IRQHandler
	.weak LPTMR0_IRQHandler
	.weak RTC_Seconds_IRQHandler
	.weak INTMUX0_0_IRQHandler
	.weak INTMUX0_1_IRQHandler
	.weak INTMUX0_2_IRQHandler
	.weak INTMUX0_3_IRQHandler
	.weak LPTMR1_IRQHandler
	.weak Reserved49_IRQHandler
	.weak Reserved50_IRQHandler
	.weak Reserved51_IRQHandler
	.weak LPSPI2_IRQHandler
	.weak LPUART2_IRQHandler
	.weak EMVSIM0_IRQHandler
	.weak LPI2C2_IRQHandler
	.weak TSI0_IRQHandler
	.weak PMC_IRQHandler
	.weak FTFA_IRQHandler
	.weak SCG_IRQHandler
	.weak WDOG0_IRQHandler
	.weak DAC0_IRQHandler
	.weak TRNG_IRQHandler
	.weak RCM_IRQHandler
	.weak CMP0_IRQHandler
	.weak CMP1_IRQHandler
	.weak RTC_IRQHandler
	.weak Reserved67_IRQHandler
	.weak Reserved68_IRQHandler
	.weak Reserved69_IRQHandler
	.weak Reserved70_IRQHandler
	.weak Reserved71_IRQHandler
	.weak Reserved72_IRQHandler
	.weak Reserved73_IRQHandler
	.weak Reserved74_IRQHandler
	.weak Reserved75_IRQHandler
	.weak Reserved76_IRQHandler
	.weak Reserved77_IRQHandler
	.weak Reserved78_IRQHandler
	.weak Reserved79_IRQHandler

	.set NMI_Handler, Default_Handler
	.set HardFault_Handler, Default_Handler
	.set SVCall_Handler, Default_Handler
	.set PendSV_Handler, Default_Handler
	.set SysTick_Handler, Default_Handler
	.set DMA0_04_IRQHandler, Default_Handler
	.set DMA0_15_IRQHandler, Default_Handler
	.set DMA0_26_IRQHandler, Default_Handler
	.set DMA0_37_IRQHandler, Default_Handler
	.set CTI0_DMA0_Error_IRQHandler, Default_Handler
	.set FLEXIO0_IRQHandler, Default_Handler
	.set TPM0_IRQHandler, Default_Handler
	.set TPM1_IRQHandler, Default_Handler
	.set TPM2_IRQHandler, Default_Handler
	.set LPIT0_IRQHandler, Default_Handler
	.set LPSPI0_IRQHandler, Default_Handler
	.set LPSPI1_IRQHandler, Default_Handler
	.set LPUART0_IRQHandler, Default_Handler
	.set LPUART1_IRQHandler, Default_Handler
	.set LPI2C0_IRQHandler, Default_Handler
	.set LPI2C1_IRQHandler, Default_Handler
	.set Reserved32_IRQHandler, Default_Handler
	.set PORTA_IRQHandler, Default_Handler
	.set PORTB_IRQHandler, Default_Handler
	.set PORTC_IRQHandler, Default_Handler
	.set PORTD_IRQHandler, Default_Handler
	.set PORTE_IRQHandler, Default_Handler
	.set LLWU0_IRQHandler, Default_Handler
	.set I2S0_IRQHandler, Default_Handler
	.set USB0_IRQHandler, Default_Handler
	.set ADC0_IRQHandler, Default_Handler
	.set LPTMR0_IRQHandler, Default_Handler
	.set RTC_Seconds_IRQHandler, Default_Handler
	.set INTMUX0_0_IRQHandler, Default_Handler
	.set INTMUX0_1_IRQHandler, Default_Handler
	.set INTMUX0_2_IRQHandler, Default_Handler
	.set INTMUX0_3_IRQHandler, Default_Handler
	.set LPTMR1_IRQHandler, Default_Handler
	.set Reserved49_IRQHandler, Default_Handler
	.set Reserved50_IRQHandler, Default_Handler
	.set Reserved51_IRQHandler, Default_Handler
	.set LPSPI2_IRQHandler, Default_Handler
	.set LPUART2_IRQHandler, Default_Handler
	.set EMVSIM0_IRQHandler, Default_Handler
	.set LPI2C2_IRQHandler, Default_Handler
	.set TSI0_IRQHandler, Default_Handler
	.set PMC_IRQHandler, Default_Handler
	.set FTFA_IRQHandler, Default_Handler
	.set SCG_IRQHandler, Default_Handler
	.set WDOG0_IRQHandler, Default_Handler
	.set DAC0_IRQHandler, Default_Handler
	.set TRNG_IRQHandler, Default_Handler
	.set RCM_IRQHandler, Default_Handler
	.set CMP0_IRQHandler, Default_Handler
	.set CMP1_IRQHandler, Default_Handler
	.set RTC_IRQHandler, Default_Handler
	.set Reserved67_IRQHandler, Default_Handler
	.set Reserved68_IRQHandler, Default_Handler
	.set Reserved69_IRQHandler, Default_Handler
	.set Reserved70_IRQHandler, Default_Handler
	.set Reserved71_IRQHandler, Default_Handler
	.set Reserved72_IRQHandler, Default_Handler
	.set Reserved73_IRQHandler, Default_Handler
	.set Reserved74_IRQHandler, Default_Handler
	.set Reserved75_IRQHandler, Default_Handler
	.set Reserved76_IRQHandler, Default_Handler
	.set Reserved77_IRQHandler, Default_Handler
	.set Reserved78_IRQHandler, Default_Handler
	.set Reserved79_IRQHandler, Default_Handler

	.end
