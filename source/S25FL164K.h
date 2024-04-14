/*
 * S25FL164K.h
 *
 *  Created on: 13 abr. 2024
 *      Author: roman
 */

#ifndef S25FL164K_H_
#define S25FL164K_H_

#include <stdint.h>
#include <stdbool.h>
#include "SPI.h"

#define IMAGE1_ADDR		0x040000 //28672
#define IMAGE2_ADDR		0x041000 //32768
#define IMAGE3_ADDR		0x042000//36864
#define IMAGE4_ADDR		0x043000 //40960
#define IMAGE5_ADDR		0x044000 //45056

#define NUM_IMGS		5U

#define TRANSFER_SIZE     	4U     /* Transfer dataSize */
#define S25FL_READ_CODE		0x03


#define S25_TRANSFER_BAUDRATE 2000000U /*! Transfer baudrate - 500k */
#define IMAGE_DATA_SIZE		504U

void S25_SPI_Config(void);
void S25_Read(uint32_t image_addr, uint8_t *read_arr);

#endif /* S25FL164K_H_ */
