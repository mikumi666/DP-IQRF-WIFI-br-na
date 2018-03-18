#include "MKL28Z7.h"
#include "wdog.h"

#include "port.h"
#include "pcc.h"
#include "LPUARTHandler.h"
#include "queue.h"
#include "intmux.h"

#include "board.h"
#include "wifi_rn131.h"
#include "usb_vcom.h"


#define PORT_LPUART0			PORTB

#define PORT_LPUART2			PORTD
#define IOIND_LPUART2_TX		2u
#define IOIND_LPUART2_RX		3u



void SYSTEM_Init(void);
void LPUART0_callback(LPUARTHandler_Event_t event);
void LPUART2_callback(LPUARTHandler_Event_t event);

char buff1[50];
char buff2[50];



__attribute__ ((weak)) int main(void)
{
	wdog_set(WDOG_CONF_LPOCLK_PRESC_OFF, 10000);
	int aff = SCG->SOSCCFG;

/* LPUART */
	char testBuff[] = "Ahoj";
	char txt[] = " svete";

	SYSTEM_Init();

	/*LPUARTHandler_Init(LPUARTHandler_Chann_LPUART0, 6, 15, 39, (void*)LPUART0_callback);
	LPUARTHandler_Init(LPUARTHandler_Chann_LPUART2, 6, 15, 39, (void*)LPUART2_callback);

	LPUARTHandler_SendLine(LPUARTHandler_Chann_LPUART0, testBuff);
	LPUARTHandler_SendLine(LPUARTHandler_Chann_LPUART2, testBuff);

	LPUARTHandler_SendData(LPUARTHandler_Chann_LPUART0, txt, 6);
	LPUARTHandler_SendData(LPUARTHandler_Chann_LPUART2, txt, 6);

	PCC_INTMUX0 = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(6);
	INTMUX0->CHANNEL->CHn_IER_31_0 = INTMUX_LPUART2_MASK;
	NVIC_SetPriority(INTMUX0_0_IRQn, 2);
	NVIC_EnableIRQ(INTMUX0_0_IRQn);*/

	/* USB */

	/*PCC_USB0FS = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(PCC_CKFG_PCS_VAL_FIRCLK);	// Povoleni hodin

	memset((void*)FSL_FEATURE_USB_KHCI_USB_RAM_BASE_ADDRESS, 0ul, FSL_FEATURE_USB_KHCI_USB_RAM); // Nulovani RAM pro USB
	*/

	USB_VCOM_Init();


	//USB_devInit(NULL);
	rn131_Init();
uint32_t i = 0;
bool a = false;
bool b = false;

	while (1)
	{
		if(b != Board_MCUBTN_Get(1) && b == false)
		{
			rn131_test();
		}


		b = Board_MCUBTN_Get(1);
		Board_MCULED_Set(1, !b);
		Board_MCULED_Set(2, !Board_MCUBTN_Get(2));
		Board_MCULED_Set(3, !Board_MCUBTN_Get(3));

		if(!Board_MCUBTN_Get(4))
		{
			if(i / 50000)
			{
				a ^= true;
				Board_MCULED_Set(4, a);
				i = 0;
			}
		}
		else
		{
			Board_MCULED_Set(4, false);
		}
		i++;

		USB_VCOM_Service();

		wdog_refresh();
	}

	return 0;
}

void LPUART0_callback(LPUARTHandler_Event_t event)
{
	if (event == LPUARTHandler_Event_ReceiveLine)
	{
		LPURTHandler_ReadLine(LPUARTHandler_Chann_LPUART0, (void*)buff1, 20, 20);
		LPUARTHandler_SendLine(LPUARTHandler_Chann_LPUART2, (void*)buff1);
	}
}

void LPUART2_callback(LPUARTHandler_Event_t event)
{
	if (event == LPUARTHandler_Event_ReceiveLine)
	{
		LPURTHandler_ReadLine(LPUARTHandler_Chann_LPUART2, (void*)buff2, 20, 20);
		LPUARTHandler_SendLine(LPUARTHandler_Chann_LPUART0, (void*)buff2);
	}
}

void SYSTEM_Init(void)
{
	PCC_PORTD = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(3);

	Board_Init();

	rn131_Init();

	int osrTemp, sbrTemp, osr, sbr, tempDiff, calculatedBaud, baud = 9600;
	int srcClock_Hz = 4000000;
	int baudDiff = baud;

	for (osrTemp = 4; osrTemp <= 32; osrTemp++)
	    {
	        /* calculate the temporary sbr value   */
	        sbrTemp = (srcClock_Hz / (baud * osrTemp));
	        /*set sbrTemp to 1 if the sourceClockInHz can not satisfy the desired baud rate*/
	        if (sbrTemp == 0)
	        {
	            sbrTemp = 1;
	        }
	        /* Calculate the baud rate based on the temporary OSR and SBR values */
	        calculatedBaud = (srcClock_Hz / (osrTemp * sbrTemp));

	        tempDiff = calculatedBaud - baud;

	        /* Select the better value between srb and (sbr + 1) */
	        if (tempDiff > (baud - (srcClock_Hz / (osrTemp * (sbrTemp + 1)))))
	        {
	            tempDiff = baud - (srcClock_Hz / (osrTemp * (sbrTemp + 1)));
	            sbrTemp++;
	        }

	        if (tempDiff <= baudDiff)
	        {
	            baudDiff = tempDiff;
	            osr = osrTemp; /* update and store the best OSR value calculated */
	            sbr = sbrTemp; /* update store the best SBR value calculated */
	        }
	    }
baudDiff += 0*155;

	/* LPUART0 a 2 nastaveni portu */

/*
	PORT_LPUART2->PCR[IOIND_LPUART2_TX] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT3);
	PORT_LPUART2->PCR[IOIND_LPUART2_RX] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT3);

	PORTB->PCR[16] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT3);
	PORTB->PCR[17] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_ALT3);*/

}


