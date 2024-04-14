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
#include "S25FL164K.h"
#include "LCD_nokia.h"
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

int main(void)
{
	uint8_t *IMG_DATA_ARRS[NUM_IMGS] = {IMG1_DATA, IMG2_DATA, IMG3_DATA, IMG4_DATA, IMG5_DATA};
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);
	EnableIRQ(PIT0_IRQ_ID);

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
	while(1)
    {
		//LCD_nokia_bitmap(); /*! It prints an array that hold an image, in this case is the initial picture*/
    }

    return 0 ;
}
