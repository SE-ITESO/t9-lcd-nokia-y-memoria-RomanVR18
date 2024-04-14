/*
 * PIT.h
 *
 *  Created on: 24 feb. 2024
 *      Author: Roman y Paulina
 *      Description: Device driver to configure the PIT channels used and to
 *  	attend its IRQs.
 */

#ifndef PIT_H_
#define PIT_H_

#include <stdint.h>
#include <stdbool.h>
#include "GPIO.h"
#include "fsl_pit.h"


#define CLK_EN_PIT (0x00800000) // Clock gating for PIT
#define WDOG_256_CLK	(13U)
/* Base struct of PIT */
typedef struct {
	uint32_t PIT_MCR;
	uint8_t Reserved[252];
	struct{
		uint32_t LVAL;
		uint32_t CURR_VAL;
		uint32_t TIMER_CTRL;
		uint32_t TIMER_FLG;
	} PIT_CHANNEL[4];
} PIT_t;

/* Channel selector for PIT */
typedef enum
{
	kChannel0 = 0U,
	kChannel1 = 1U,
	kChannel2 = 2U,
	kChannel3 = 3U,
} pit_channel_t;

#define PIT0_IRQ_ID	PIT0_IRQn 	// Re-definition of interruption vector
#define PIT1_IRQ_ID PIT1_IRQn
#define PIT2_IRQ_ID PIT2_IRQn
#define PIT3_IRQ_ID PIT3_IRQn

#define PIT_CLK_FRQ 10500000	// Basic PIT clock frequency

#define PIT_BASEADDR PIT

#define PIT_CHANNEL0 kPIT_Chnl_0
#define PIT_CHANNEL1 kPIT_Chnl_1
#define PIT_CHANNEL2 kPIT_Chnl_2
#define PIT_CHANNEL3 kPIT_Chnl_3

#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk) // Gets automatically PIT clock frequency
#define PIT_ENABLED_MASK	(0x00000003)

#define PIT0_IR   		PIT0_IRQHandler
#define PIT1_IR			PIT1_IRQHandler
#define PIT2_IR			PIT2_IRQHandler
#define PIT3_IR			PIT3_IRQHandler

void PIT_enable_interrupts();
void PIT_set_timer_period(pit_channel_t CHANNEL,uint32_t time_us);
void PIT_set_interrupts();

void PIT_callback_init(uint8_t (*handler)(bool is_on));
void PIT1_callback_init(uint8_t (*handler)(bool is_on));

uint8_t PIT_IsActive();

void PIT0_IR(void);
void PIT1_IR(void);
void PIT2_IR(void);
void PIT3_IR(void);


#endif /* PIT_H_ */
