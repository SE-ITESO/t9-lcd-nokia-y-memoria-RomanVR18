/*
 * S25FL164K.c
 *
 *  Created on: 13 abr. 2024
 *      Author: roman
 * 		@Description: This device driver has the functions to configurate the SPI
 * 					  settings to use the flash memory by SPI and get the readings
 * 					  of the addresses desired.
 */

#include "S25FL164K.h"

void S25_SPI_Config(void)
{
	SPI_config(kDSPI_Ctar1, S25_TRANSFER_BAUDRATE, kDSPI_ClockPolarityActiveLow, kDSPI_ClockPhaseSecondEdge, kDSPI_Pcs1);
}

void S25_Read(uint32_t image_addr, uint8_t *read_arr)
{
	dspi_half_duplex_transfer_t masterXfer;
	uint8_t FuncCodeAndAddress[TRANSFER_SIZE] = {S25FL_READ_CODE, image_addr >> 16, (uint8_t) image_addr >> 8, (uint8_t) image_addr};

	masterXfer.txData = FuncCodeAndAddress;
	masterXfer.rxData = read_arr;
	masterXfer.txDataSize = TRANSFER_SIZE;
	masterXfer.rxDataSize = IMAGE_DATA_SIZE;
	masterXfer.isTransmitFirst = true;
	masterXfer.isPcsAssertInTransfer = true;
	masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;

	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);
}
