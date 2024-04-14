/*
 * SPI.h
 *
 *  Created on: 19 jun. 2021
 *      Author: jlpe
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include "fsl_dspi.h"

#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN16_IDX                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */

typedef enum _spi_num
{
	kSPI0 = 0U,
	kSPI1 = 1U,
}SPIx_t;

typedef enum _spi_pcsx
{
	kSPI_PCS0 = 0U,
	kSPI_PCS1 = 1U,
	kSPI_PCS2 = 2U,
	kSPI_PCS3 = 3U,
	kSPI_PCS4 =	4U,
}SPI_PCSx_t;

void SPI_config(dspi_ctar_selection_t Ctar, uint32_t BAUDRATE, dspi_clock_polarity_t cpol, dspi_clock_phase_t cpha, dspi_which_pcs_t pcs);
void SPI_Config0(void);

#endif /* SPI_H_ */
