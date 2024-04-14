/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    LCD_Nokia_EEPROM.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <stdbool.h>
#include "SPI.h"
#include "GPIO.H"
#include "PIT.h"
#include "S25FL164K.h"
#include "LCD_nokia.h"
#include "LCD_nokia_images.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
/*
 * @brief   Application entry point.
 */

static uint8_t IMG1_DATA[IMAGE_DATA_SIZE] = {0};
static uint8_t IMG2_DATA[IMAGE_DATA_SIZE] = {0};
static uint8_t IMG3_DATA[IMAGE_DATA_SIZE] = {0};
static uint8_t IMG4_DATA[IMAGE_DATA_SIZE] = {0};
static uint8_t IMG5_DATA[IMAGE_DATA_SIZE] = {0};

static uint32_t IMG_ADDRS[NUM_IMGS] = {IMAGE1_ADDR, IMAGE2_ADDR, IMAGE3_ADDR, IMAGE4_ADDR, IMAGE5_ADDR};

static uint8_t img_counter;

/*! This array hold the initial picture that is shown in the LCD. Note that extern should be avoided*/
extern const uint8_t ITESO[504];

int main(void)
{
	uint8_t *IMG_DATA_ARRS[NUM_IMGS] = {IMG1_DATA, IMG2_DATA, IMG3_DATA, IMG4_DATA, IMG5_DATA};
    pit_config_t pitConfig;

    PORT_ClockGatingConfig(CLK_EN_ALL_PORTS);

	EnableIRQ(PIT0_IRQ_ID);

    PIT_GetDefaultConfig(&pitConfig);
    PIT_Init(PIT_BASEADDR, &pitConfig);
	PIT_EnableInterrupts(PIT_BASEADDR, PIT_CHANNEL0, kPIT_TimerInterruptEnable);
	PIT_set_timer_period(PIT_CHANNEL0, DISPLAY_TIME);

	SPI_Config0();

	S25_SPI_Config();
	LCD_SPI_Config();

	LCD_nokia_init();

	for(uint8_t i = 0; i < NUM_IMGS; i++)
	{
		S25_Read(IMG_ADDRS[i], IMG_DATA_ARRS[i]);
	}

	LCD_nokia_clear();/*! It clears the information printed in the LCD*/

	PIT_StartTimer(PIT_BASEADDR, PIT_CHANNEL0);
	while(1)
    {
		if(PIT0_GetIRFlag())
		{
			img_counter++;
			PIT0_SetIRFlag(false);
		}
		switch(img_counter)
		{
			case 1:
				LCD_nokia_bitmap(ITESO);
			break;
			case 2:
				LCD_nokia_bitmap(IMG1_DATA);
			break;
			case 3:
			break;
		}
    }

    return 0 ;
}
