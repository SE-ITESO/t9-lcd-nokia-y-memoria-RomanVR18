/*
 * GPIO.h
 *
 *  Created on: 20 feb. 2024
 *      Author: roman
 *
 *  Description: Device driver to configure the GPIO's and ports used, and to
 *  attend its IRQs.
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "fsl_i2c.h"
//#include "fsl_port.h"

#define MUX_CONFIG_AS_GPIO (0x00000100)	// Configuration of PCR to just set port pin as GPIO
#define PCR_CONFIG_INPUT_INT (0x000A0103)	// Configuration of PCR with pull-up resistor, GPIO and falling edge detection
#define CLK_EN_ALL_PORTS (0x00003E00)	// Clock gating configuration for all ports
#define CLK_EN_PORT_E (0x00002000)
#define CLK_EN_PORT_B (0x00000400)
#define CLK_EN_PORT_D (0x00001000)
#define PORT_CONFIG_AS_INPUT (0x00000000)	// Value to charge to PDDR
#define TEST_LED (0x00600000)	// Configure the turn on of both LEDs

/* Definition of the masks to identify port pin interruptions*/
#define PTB11_MASK (0x00000800)
#define PTB10_MASK (0x00000400)
#define PTB3_MASK (0x00000008)
#define PTB2_MASK (0x00000004)
#define PTC11_MASK (0x00000800)
#define PTC10_MASK (0x00000400)
#define PTC6_MASK (0x00000040)
#define PTA4_MASK (0x00000010)
#define PTD0_MASK (0x00000001)

#define sim_scgc4 (*((volatile uint32_t *) 0x40048034)) // Clock gating for VREF
#define sim_scgc5 (*((volatile uint32_t *) 0x40048038)) // Clock gating for ports
#define sim_scgc6 (*((volatile uint32_t *) 0x4004803C)) // Clock gating for DAC0 and PIT
#define PCR_PTB22 (*((volatile uint32_t *) 0x4004A058)) // PCR for Red and Blue LEDs
#define PCR_PTB21 (*((volatile uint32_t *) 0x4004A054))
#define PCR_PTE26 (*((volatile uint32_t *) 0x4004D068))	// PCR for Green LED
#define PCR_PTC6 (*((volatile uint32_t *) 0x4004B018))  // PCR for SW3
#define PCR_PTA4 (*((volatile uint32_t *) 0x40049010))  // PCR for SW2
#define PCR_PTD0 (*((volatile uint32_t *) 0x4004C000))  // PCR address for PTD0

/* Re-definition of interrupt vectors */
#define PORTA_IRQ_ID		PORTA_IRQn
#define PORTB_IRQ_ID		PORTB_IRQn
#define PORTC_IRQ_ID		PORTC_IRQn
#define PORTD_IRQ_ID		PORTD_IRQn

/* Re-definition of interrupt handlers */
#define GET_SWA_IRQ 	PORTA_IRQHandler
#define GET_SWB_IRQ		PORTB_IRQHandler
#define GET_SWC_IRQ		PORTC_IRQHandler
#define GET_PORTD_IRQ	PORTD_IRQHandler

/* Base addresses for ports */
#define GPIO_A_BASE_ADDR (((volatile uint32_t *) 0x400FF000u))
#define GPIO_B_BASE_ADDR (((volatile uint32_t *) 0x400FF040u))
#define GPIO_C_BASE_ADDR (((volatile uint32_t *) 0x400FF080u))
#define GPIO_D_BASE_ADDR (((volatile uint32_t *) 0x400FF0C0u))
#define GPIO_E_BASE_ADDR (((volatile uint32_t *) 0x400FF100u))

/* Masks used for GPIO configuration characteristics */
#define PULL_SELECT_MASK (3U)
#define SLEW_RATE_MASK (4U)
#define PASSIVE_FILTER_MASK (16U)
#define OPEN_DRAIN_MASK (32U)
#define DRIVE_STRENGTH_MASK (64U)

/* Enumeration of PCR interrupt types */
typedef enum interrupts_t
{
    kinterrupt_disabled 		= 0x0U,
    kisf_flag_DMA_risingedge 	= 0x1U,
	kisf_flag_DMA_fallingedge 	= 0x2U,
	kisf_flag_DMA_eitheredge 	= 0x3U,
	kisf_flag_int_zerologic 	= 0x8U,
	kisf_flag_int_risingedge 	= 0x9U,
	kisf_flag_int_fallingedge 	= 0xAU,
	kisf_flag_int_eitheredge 	= 0xBU,
	kisf_flag_int_onelogic 		= 0xCU,
} interrupts_t;

/* Enumeration of mux-set choices */
typedef enum mux_t
{
	kpin_disabled 	= 0U,
	kmux_as_gpio 	= 1U,
	kmux_alt2 		= 2U,
	kmux_alt3		= 3U,
	kmux_alt4		= 4U,
	kmux_alt5		= 5U,
	kmux_alt6		= 6U,
	kmux_alt7		= 7U,
} mux_t;

/* Enumeration of resistor type pull */
enum pull_selector
{
	kPull_disable = 0U,
	kPull_down = 2U,
	kPull_up = 3U,
};

/* Enumeration of slew rate choices */
enum slew_rate
{
	kFast_SR = 0U,
	kSlow_SR = 1U,
};

/* Enumeration of passive filter enable/disable */
enum passive_filter
{
	kDisable_pf = 0U,
	kEnable_pf = 1U,
};

/* Enumeration of open drain enable/disable */
enum open_drain
{
	kDisable_od = 0U,
	kEnable_od = 1U,
};

/* Enumeration of drive strength choices */
enum drive_strength
{
	kLow_ds = 0U,
	kHigh_ds = 1U,
};

/* Enumeration of lock registers choices */
enum lock_reg
{
	kNot_locked = 0U,
	kLocked = 1U,
};

/* PCR configuration struct */
typedef struct port_pin_cfgn
{
	uint16_t ps : 2;	// Pull select bit
	uint16_t sr : 1; // Slew rate
	uint16_t : 1;
	uint16_t pf : 1; // Passive filter
	uint16_t od : 1; // Open drain
	uint16_t ds : 1; // Drive strength
	uint16_t : 1;
	uint16_t mux : 3;
	uint16_t lock : 1;
} port_pin_t;

typedef struct		// Registers struct
{
	uint32_t PDOR;
	uint32_t PSOR;
	uint32_t PCOR;
	uint32_t PTOR;
	uint32_t PDIR;
	uint32_t PDDR;
}GPIO_t;

/* Port configuration characteristics */
typedef struct {
  uint32_t PCR[32];                           /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  uint32_t GPCLR;                             /**< Global Pin Control Low Register, offset: 0x80 */
  uint32_t GPCHR;                             /**< Global Pin Control High Register, offset: 0x84 */
  uint8_t RESERVED_0[24];
  uint32_t ISFR;                              /**< Interrupt Status Flag Register, offset: 0xA0 */
  uint8_t RESERVED_1[28];
  uint32_t DFER;                              /**< Digital Filter Enable Register, offset: 0xC0 */
  uint32_t DFCR;                              /**< Digital Filter Clock Register, offset: 0xC4 */
  uint32_t DFWR;                              /**< Digital Filter Width Register, offset: 0xC8 */
} PORT_t;

typedef enum{gpio_A, /*!< Definition to select GPIO A */
			 gpio_B, /*!< Definition to select GPIO B */
			 gpio_C, /*!< Definition to select GPIO C */
			 gpio_D, /*!< Definition to select GPIO D */
			 gpio_E, /*!< Definition to select GPIO E */
			} gpio_name_t;

/* Enumeration to configure GPIO as I/O */
typedef enum input_or_output
{
	kSet_input = 0U,
	kSet_output = 1U,
} set_pddr_t;

/* Enumeration of port B used pins */
typedef enum _port_b_int
{
	kPort_B11 = 11U,
	kPort_B10 = 10U,
	kPort_B3  = 3U,
	kPort_B2  = 2U,
} PORT_B_PINS_t;

/* Enumeration of port C used pins */
typedef enum _port_c_int
{
	kPort_C11 = 11U,
	kPort_C10 = 10U,
	kPort_C6  = 6U,
} PORT_C_PINS_t;

/* Flag bit-field */
typedef struct
{
	uint8_t flag_port_a : 1;
	uint8_t flag_port_b : 1;
	uint8_t flag_port_c : 1;
	uint8_t flag_port_d : 1;
	uint8_t flag_port_e : 1;
} gpio_interrupt_flags_t;

/* Interrupt flag struct of port B*/
typedef struct
{
	uint8_t flag_PTB11 : 1;
	uint8_t flag_PTB10 : 1;
	uint8_t flag_PTB3  : 1;
	uint8_t flag_PTB2  : 1;
} gpio_b_IF_t;

/* Interrupt flag struct of port C*/
typedef struct
{
	uint8_t flag_PTC11 : 1;
	uint8_t flag_PTC10 : 1;
	uint8_t flag_PTC6  : 1;
}gpio_c_IF_t;


/* Pointers to structs */
#define GPIO_A ((GPIO_t*) GPIO_A_BASE_ADDR)
#define GPIO_B ((GPIO_t*) GPIO_B_BASE_ADDR)
#define GPIO_C ((GPIO_t*) GPIO_C_BASE_ADDR)
#define GPIO_D ((GPIO_t*) GPIO_D_BASE_ADDR)
#define GPIO_E ((GPIO_t*) GPIO_E_BASE_ADDR)

#define GPIO_PTRS		{ GPIO_A, GPIO_B, GPIO_C, GPIO_D, GPIO_E } // GPIO pointers array

/* Port base addresses */
#define PORT_A_BASE_ADDR	(((void *) 0x40049000u))
#define PORT_B_BASE_ADDR    (((void *) 0x4004A000u))
#define PORT_C_BASE_ADDR    (((void *) 0x4004B000u))
#define PORT_D_BASE_ADDR    (((void *) 0x4004C000u))
#define PORT_E_BASE_ADDR	(((void *) 0x4004D000u))

/* GPIO base addresses array */
#define GPIO_BASES     { GPIO_A_BASE_ADDR, GPIO_B_BASE_ADDR, GPIO_C_BASE_ADDR, GPIO_D_BASE_ADDR, GPIO_E_BASE_ADDR }

#define PORT_A		((PORT_t *) PORT_A_BASE_ADDR)
#define PORT_B      ((PORT_t *) PORT_B_BASE_ADDR)
#define PORT_C      ((PORT_t *) PORT_C_BASE_ADDR)
#define PORT_D		((PORT_t *) PORT_D_BASE_ADDR)
#define PORT_E		((PORT_t *) PORT_E_BASE_ADDR)

#define PORT_BASES	{ PORT_A_BASE_ADDR, PORT_B_BASE_ADDR, PORT_C_BASE_ADDR, PORT_D_BASE_ADDR, PORT_E_BASE_ADDR }

/* Array initializer of PORT peripheral base pointers */
#define PORT_PTRS 	{ PORT_A, PORT_B, PORT_C, PORT_D, PORT_E }

void PORT_ClockGatingConfig(uint32_t val); // Function to configure ports
void config_pit(uint32_t val); // Function to enable PIT clock
void pcra_config(uint32_t val);	 // Function to configure PCR characteristics of ports
void pcrb_config(uint32_t val);
void pcrc_config(uint32_t val);
void pcrd_config(uint32_t val);
void pcre_config(uint32_t val);

/* Interrupt SW handlers */
void GET_SWC_IRQ(void);
void GET_SWA_IRQ(void);
void GET_SWB_IRQ(void);
void GET_PORTD_IRQ(void);

void config_pddr_a(uint32_t val);	// Function to configure GPIOs as input or output
void config_pddr_b(uint32_t val);
void config_pddr_c(uint32_t val);
void config_pddr_d(uint32_t val);
void config_pddr_e(uint32_t val);

void GPIO_clear_internSW_flags(gpio_name_t gpio);

/* Functions to handle software interrupt flags */
uint8_t GPIO_get_flag(gpio_name_t gpio);
void GPIO_set_flag(gpio_name_t gpio);
void GPIO_B_set_flag(PORT_B_PINS_t pin);
uint8_t GPIO_B_get_flag(PORT_B_PINS_t pin);
void GPIO_C_set_flag(PORT_C_PINS_t pin);
uint8_t GPIO_C_get_flag(PORT_C_PINS_t pin);

/* Functions to get and clear hardware interrupts of a whole port */
uint32_t GPIO_GetPortIF(GPIO_t *base);
void GPIO_ClearPortIF(GPIO_t *base, uint32_t mask);

/* Function to get port instance */
uint32_t GPIO_GetInstance(GPIO_t *base);

/* Functions to configure pin input or output */
void PORT_SetPinAsButton(PORT_t *base, uint32_t pin);
void PORT_SetPinAsOutput(PORT_t *base, uint32_t pin);
void PORT_SetPinAsI2C(PORT_t *base, uint32_t pin);

/* Functions to set a certain pin configuration and interrupts */
void PORT_SetPinC(PORT_t *base, uint32_t pin, const port_pin_t *config);
void PORT_SetMux(PORT_t *base, uint32_t pin, mux_t mux);
void PORT_SetInterrupts(PORT_t *base, uint32_t pin, interrupts_t interrupt_type);
void GPIO_PDDR_Config(GPIO_t *gpio, set_pddr_t pddr_io, uint32_t pin);

/* Functions to set, clear or toggle a certain port pin */
void GPIO_SetPinValue(GPIO_t *gpio, uint32_t pin);
void GPIO_ClearPinValue(GPIO_t *gpio, uint32_t pin);
void GPIO_TogglePinValue(GPIO_t *gpio, uint32_t pin);

/* Port A callback initialization */
void PTA4_callback_init(void (*handler)(bool turn_on));

/* Port D callback initialization*/
void PTD0_callback_init(void (*handler)(bool read, uint8_t read_reg, uint8_t reg_quantity));

/* Indicator LEDs control */
void toggle_RED();
void toggle_BLUE();
void turn_off();

#endif /* GPIO_H_ */
