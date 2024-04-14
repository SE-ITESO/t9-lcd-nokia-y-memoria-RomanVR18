/*
 * PIT.c
 *
 *  Created on: 24 feb. 2024
 *      Author: Roman y Paulina
 *      Description: Device driver to configure the PIT channels used and to
 *  	attend its IRQs.
 */

#include "PIT.h"
#include <stdint.h>
#include <stdbool.h>
#include "fsl_pit.h"

static uint8_t (*PIT_callback)(bool is_on) = 0; // Pointer to function
static uint8_t (*PIT1_callback)(bool is_on) = 0; // Pointer to function

static bool temp = false;
static bool PIT_IR_Flag;

/* Interrupt enables */
void PIT_enable_interrupts()
{
	EnableIRQ(PIT0_IRQ_ID);
	EnableIRQ(PIT1_IRQ_ID);
	EnableIRQ(PIT2_IRQ_ID);
	EnableIRQ(PIT3_IRQ_ID);
}

/* Activate PIT interrupts */
void PIT_set_interrupts()
{
	PIT_EnableInterrupts(PIT_BASEADDR, PIT_CHANNEL0, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT_BASEADDR, PIT_CHANNEL1, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT_BASEADDR, PIT_CHANNEL2, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT_BASEADDR, PIT_CHANNEL3, kPIT_TimerInterruptEnable);
}

/*PIT 0 interrupt handler*/
void PIT0_IR(void)
{
	PIT_IR_Flag = true;
	PIT_ClearStatusFlags(PIT_BASEADDR, PIT_CHANNEL0, kPIT_TimerFlag);
}

/*PIT 1 interrupt handler*/
void PIT1_IR(void)
{
	if(PIT1_callback)
	{
		PIT1_callback(temp);
		temp = !temp;
		PIT_ClearStatusFlags(PIT_BASEADDR, PIT_CHANNEL1, kPIT_TimerFlag);
	}
}

/* Function to load a count value to the timer */
void PIT_set_timer_period(pit_channel_t CHANNEL,uint32_t time_us)
{
	PIT->CHANNEL[CHANNEL].LDVAL = USEC_TO_COUNT(time_us, PIT_CLK_FRQ);
}

/* Initialization of callback functions */
void PIT_callback_init(uint8_t (*handler)(bool is_on))
{
	PIT_callback = handler;
}

void PIT1_callback_init(uint8_t (*handler)(bool is_on))
{
	PIT1_callback = handler;
}

/* Checks if any of the PITs is active */
uint8_t PIT_IsActive()
{
	if(PIT_ENABLED_MASK == PIT_BASEADDR->CHANNEL[PIT_CHANNEL2].TCTRL)
	{
		return kPIT_Chnl_2;
	}
	else if(PIT_ENABLED_MASK ==  PIT_BASEADDR->CHANNEL[PIT_CHANNEL3].TCTRL)
	{
		return kPIT_Chnl_3;
	}
	else
	{
		return 0;
	}
}

/* Control of PIT 0 flags */
bool PIT0_GetIRFlag()
{
	return(PIT_IR_Flag);
}

void PIT0_SetIRFlag(bool set)
{
	PIT_IR_Flag = set;
}
