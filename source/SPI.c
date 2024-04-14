/**
 * @file    SPI.c
 * @Description: This device driver uses sdk's dspi functions to set the transfer configuration
 * 				 to use the SPI module.
 */

#include "SPI.h"
#include "GPIO.h"
#include "fsl_dspi.h"
#include "fsl_clock.h"
#include "clock_config.h"

void SPI_config(dspi_ctar_selection_t Ctar, uint32_t BAUDRATE, dspi_clock_polarity_t cpol, dspi_clock_phase_t cpha, dspi_which_pcs_t pcs)
{
	uint32_t srcClock_Hz;

	CLOCK_SetSimSafeDivs();

	dspi_master_config_t masterConfig;

	if(kDSPI_Pcs0 == pcs)
	{
		PORT_SetMux(PORT_D, 0, kmux_alt2);
	}
	else
	{
		PORT_SetMux(PORT_C, 3, kmux_alt2);
	}

	 /* Master config */
	masterConfig.whichCtar                                = Ctar;
	masterConfig.ctarConfig.baudRate                      = BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame                  = 8U;
	masterConfig.ctarConfig.cpol                          = cpol;
	masterConfig.ctarConfig.cpha                          = cpha;
	masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 1000U;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 1000U;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000U;
	masterConfig.whichPcs           = pcs;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK        = false;
	masterConfig.enableRxFifoOverWrite      = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

	srcClock_Hz = CLOCK_GetFreq(DSPI0_CLK_SRC);
	DSPI_MasterInit(SPI0, &masterConfig, srcClock_Hz);
}

void SPI_Config0(void)
{
	PORT_SetMux(PORT_D, 1, kmux_alt2);
	PORT_SetMux(PORT_D, 2, kmux_alt2);
	PORT_SetMux(PORT_D, 3, kmux_alt2);
}
