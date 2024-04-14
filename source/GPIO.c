/*
 * GPIO.c
 *
 *  Created on: 20 feb. 2024
 *      Author: roman
 *
 *  Description: Device driver to configure the GPIO's and ports used, and to
 *  attend its IRQs.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "GPIO.h"
#include "fsl_common.h"
#include "fsl_i2c.h"

volatile static gpio_interrupt_flags_t g_intr_status_flag = {0}; // Interrupt flags for IRQs
volatile static gpio_b_IF_t gb_intr_status_flag = {0};
volatile static gpio_c_IF_t gc_intr_status_flag = {0};
static void (*PTA4_callback)(bool turn_on) = 0; // Pointer to function
static void (*PTD0_callback)(bool read,uint8_t read_reg, uint8_t reg_quantity) = 0; // Pointer to function

volatile static uint8_t B11_counter = 0;
volatile static uint8_t C11_counter = 0;
volatile static bool C10_counter = 0;
static PORT_t *const arr_portBases[] = PORT_BASES;
static GPIO_t *const arr_gpioBases[] = GPIO_PTRS;

void PTA4_callback_init(void (*handler)(bool turn_on))
{
	PTA4_callback = handler;
}
void PTD0_callback_init(void (*handler)(bool read, uint8_t read_reg, uint8_t reg_quantity))
{
	PTD0_callback = handler;
}

uint32_t GPIO_GetInstance(GPIO_t *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(arr_gpioBases); instance++)
    {
        if (arr_gpioBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(arr_gpioBases));

    return instance;
}

uint32_t GPIO_GetPortIF(GPIO_t *base)
{
    uint8_t instance;
    PORT_t *portBase;
    instance = (uint8_t)GPIO_GetInstance(base);
    portBase = arr_portBases[instance];
    return portBase->ISFR;
}

void GPIO_ClearPortIF(GPIO_t *base, uint32_t mask)
{
    uint8_t instance;
    PORT_t *portBase;
    instance = (uint8_t)GPIO_GetInstance(base);
    portBase = arr_portBases[instance];
    portBase->ISFR = mask;
}

void PORT_SetPinAsButton(PORT_t *base, uint32_t pin)
{
	const  port_pin_t button_config = {
			kPull_up,
			kSlow_SR,
			kEnable_pf,
			kDisable_od,
			kLow_ds,
			kmux_as_gpio,
			kNot_locked,
	};
	PORT_SetPinC(base, pin, &button_config);
}

void PORT_SetPinAsI2C(PORT_t *base, uint32_t pin)
{
	const port_pin_t i2c_config = {
			kPull_up,
			kFast_SR,
			kDisable_pf,
			kEnable_od,
			kLow_ds,
			kmux_alt2,
			kNot_locked,
	};
	PORT_SetPinC(base, pin, &i2c_config);
}

void PORT_SetPinAsOutput(PORT_t *base, uint32_t pin)
{
	const  port_pin_t output_config = {
			kPull_disable,
			kFast_SR,
			kDisable_pf,
			kDisable_od,
			kLow_ds,
			kmux_as_gpio,
			kNot_locked,
	};
	PORT_SetPinC(base, pin, &output_config);
}

void PORT_SetPinC(PORT_t *base, uint32_t pin, const port_pin_t *config)
{
    uint32_t addr                = (uint32_t)&base->PCR[pin];
    *(volatile uint16_t *)(addr) = *((const uint16_t *)(const void *)config);
}

void PORT_SetInterrupts(PORT_t *base, uint32_t pin, interrupts_t interrupt_type)
{
	base->PCR[pin] |= interrupt_type <<  16;
}
/* SW3 IRQ (PORT A interrupt request handler) */
void GET_SWA_IRQ(void)
{
	/* Interrupt handler to activate the alarm comparison and horn */
	if(PTA4_callback)
	{
		PTA4_callback(true);
		toggle_RED();
	}
	GPIO_ClearPortIF(GPIO_A, PTA4_MASK);
}
/* Interrupt vector of PORT D, dedicated to PTD0*/
void GET_PORTD_IRQ(void)
{
	if(PTD0_callback)
	{

	}
	GPIO_ClearPortIF(GPIO_D, PTD0_MASK);
}
/* PORT C interrupt request handler */
void GET_SWC_IRQ(void)
{
	/* Algorithm to set software flags by checking which button triggered the IRQ */

	if((PTC11_MASK == GPIO_GetPortIF(GPIO_C)) && !(C11_counter) && !(GPIO_C_get_flag(kPort_C11)))
	{
		gc_intr_status_flag.flag_PTC11 = true;
		GPIO_ClearPortIF(GPIO_C, PTC11_MASK);
		C11_counter = 1;
	}
	else if((PTC11_MASK == GPIO_GetPortIF(GPIO_C)) && (C11_counter))
	{
		GPIO_C_set_flag(kPort_C11);
		GPIO_ClearPortIF(GPIO_C, PTC11_MASK);
		C11_counter = 0;
	}
	else if(PTC6_MASK == GPIO_GetPortIF(GPIO_C))
	{
		gc_intr_status_flag.flag_PTC6 = true;
		GPIO_ClearPortIF(GPIO_C, PTC6_MASK);
	}
	else if(PTC10_MASK == GPIO_GetPortIF(GPIO_C) && !C10_counter)
	{
		gc_intr_status_flag.flag_PTC10 = true;
		GPIO_ClearPortIF(GPIO_C, PTC10_MASK);
		C10_counter = !C10_counter;
	}
	else if(PTC10_MASK == GPIO_GetPortIF(GPIO_C) && C10_counter)
	{
		GPIO_C_set_flag(kPort_C10);
		GPIO_ClearPortIF(GPIO_C, PTC10_MASK);
		C10_counter = !C10_counter;
	}
}

void GET_SWB_IRQ(void)
{
	/* Algorithm to set software flags by checking which button triggered the IRQ */
	if(PTB11_MASK == GPIO_GetPortIF(GPIO_B) && (B11_counter == 0) && !(GPIO_B_get_flag(kPort_B11)))
	{
		gb_intr_status_flag.flag_PTB11=true;
		GPIO_ClearPortIF(GPIO_B, PTB11_MASK);
		B11_counter = 1;
	}
	else if(((PTB10_MASK == GPIO_GetPortIF(GPIO_B)) && (1 == B11_counter)) || ((PTB10_MASK == GPIO_GetPortIF(GPIO_B)) && (GPIO_C_get_flag(kPort_C11))))
	{
		gb_intr_status_flag.flag_PTB10 = true;
		GPIO_ClearPortIF(GPIO_B, PTB10_MASK);
	}
	else if(((PTB3_MASK == GPIO_GetPortIF(GPIO_B)) && (1 == B11_counter)) || ((PTB3_MASK == GPIO_GetPortIF(GPIO_B)) && (GPIO_C_get_flag(kPort_C11))))
	{
		gb_intr_status_flag.flag_PTB3 = true;
		GPIO_ClearPortIF(GPIO_B, PTB3_MASK);
	}
	else if(PTB11_MASK == GPIO_GetPortIF(GPIO_B) && (B11_counter == 1))
	{
		GPIO_B_set_flag(kPort_B11);
		GPIO_ClearPortIF(GPIO_B, PTB11_MASK);
		B11_counter = 0;
	}
	else if((PTB10_MASK == GPIO_GetPortIF(GPIO_B)) && C10_counter)
	{
		gb_intr_status_flag.flag_PTB10 = true;
		GPIO_ClearPortIF(GPIO_B, PTB10_MASK);
	}
	else if((PTB3_MASK == GPIO_GetPortIF(GPIO_B)) && C10_counter)
	{
		gb_intr_status_flag.flag_PTB3 = true;
		GPIO_ClearPortIF(GPIO_B, PTB3_MASK);
	}
	else if(PTB2_MASK == GPIO_GetPortIF(GPIO_B))
	{
		gb_intr_status_flag.flag_PTB2 = true;
		GPIO_ClearPortIF(GPIO_B, PTB2_MASK);
	}
	else
	{
		GPIO_ClearPortIF(GPIO_B, PTB10_MASK);
		GPIO_ClearPortIF(GPIO_B, PTB3_MASK);
	}

}

void GPIO_B_set_flag(PORT_B_PINS_t pin)
{
	if(11 == pin)
	{
		gb_intr_status_flag.flag_PTB11 = false;
	}
	else if(10 == pin)
	{
		gb_intr_status_flag.flag_PTB10 = false;
	}
	else if(3 == pin)
	{
		gb_intr_status_flag.flag_PTB3 = false;
	}
	else
		gb_intr_status_flag.flag_PTB2 = false;
}

uint8_t GPIO_B_get_flag(PORT_B_PINS_t pin)
{
	uint8_t f_status = 0;
	if(11 == pin)
	{
		f_status = gb_intr_status_flag.flag_PTB11;
	}
	else if(10 == pin)
	{
		f_status = gb_intr_status_flag.flag_PTB10;
	}
	else if(3 == pin)
	{
		f_status = gb_intr_status_flag.flag_PTB3;
	}
	else
	{
		f_status = gb_intr_status_flag.flag_PTB2;
	}
	return (f_status);
}

void GPIO_C_set_flag(PORT_C_PINS_t pin)
{
	if(11 == pin)
	{
		gc_intr_status_flag.flag_PTC11 = false;
	}
	else if(10 == pin)
	{
		gc_intr_status_flag.flag_PTC10 = false;
	}
	else
		gc_intr_status_flag.flag_PTC6 = false;
}

uint8_t GPIO_C_get_flag(PORT_C_PINS_t pin)
{
	uint8_t f_status = 0;
	if(11 == pin)
	{
		f_status = gc_intr_status_flag.flag_PTC11;
	}
	else if(10 == pin)
	{
		f_status = gc_intr_status_flag.flag_PTC10;
	}
	else
	{
		f_status = gc_intr_status_flag.flag_PTC6;
	}
	return (f_status);
}

/* Read software interruption flags */
uint8_t GPIO_get_flag(gpio_name_t gpio)
{
	uint8_t f_status = 0;
	if(gpio_A == gpio)
	{
		f_status = g_intr_status_flag.flag_port_a;
	}
	else if(gpio_B == gpio)
	{
		f_status = g_intr_status_flag.flag_port_b;
	}
	else if(gpio_C == gpio)
	{
		f_status = g_intr_status_flag.flag_port_c;
	}
	else if(gpio_D == gpio)
	{
		f_status = g_intr_status_flag.flag_port_d;
	}
	else
	{
		f_status = g_intr_status_flag.flag_port_e;
	}
	return (f_status);
}

/* Clear software interruption flags */
void GPIO_set_flag(gpio_name_t gpio)
{
	if(gpio_A == gpio)
	{
		g_intr_status_flag.flag_port_a = false;
	}
	else if(gpio_B == gpio)
	{
		g_intr_status_flag.flag_port_b = false;
	}
	else if(gpio_C == gpio)
	{
		g_intr_status_flag.flag_port_c = false;
	}
	else if (gpio_D == gpio)
	{
		g_intr_status_flag.flag_port_d = false;
	}
	else
	{
		g_intr_status_flag.flag_port_e = false;
	}
}

/* Assigns user-determined value to modify PORT clock gatings */
void PORT_ClockGatingConfig(uint32_t val)
{
	sim_scgc5 = val;
}

void PORT_SetMux(PORT_t *base, uint32_t pin, mux_t mux)
{
    base->PCR[pin] = (base->PCR[pin] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(mux);
}
/* Assigns user-determined value to modify PIT clock gating*/
void config_pit(uint32_t val)
{
	sim_scgc6 = val;
}

/* pcrx_config functions to specify the port pins configuration */
void pcra_config(uint32_t val)
{
	PCR_PTA4 = val;
}
void pcrc_config(uint32_t val)
{
	PCR_PTC6 = val;
}
void pcrb_config(uint32_t val)
{
	PCR_PTB22 = val;
	PCR_PTB21 = val;
}
void pcrd_config(uint32_t val)
{
	PCR_PTD0 = val;
}
void pcre_config(uint32_t val)
{
	PCR_PTE26 = val;
}

/* config_pddr_x functions to configure purpose of port pins */
void config_pddr_a(uint32_t val)
{
	GPIO_A->PDDR = val;
}
void config_pddr_b(uint32_t val)
{
	GPIO_B->PDDR = val;
	GPIO_B->PSOR = val | 0xFFFFFFFF;
}
void config_pddr_c(uint32_t val)
{
	GPIO_C->PDDR = val;
}
void config_pddr_d(uint32_t val)
{
	GPIO_D->PDDR = val;
}
void config_pddr_e(uint32_t val)
{
	GPIO_E->PDDR = val;
	GPIO_E->PSOR = val | 0xFFFFFFFF;
}

void GPIO_PDDR_Config(GPIO_t *gpio, set_pddr_t pddr_io, uint32_t pin)
{
	gpio->PDDR |= (pddr_io << pin);
}
/* Function to clear hardware GPIO interrupt flags by the PCRx*/
void GPIO_clear_internSW_flags(gpio_name_t gpio)
{
	uint32_t mask_flag = 0x01000000;
	if(gpio_A == gpio)
	{
		pcra_config(PCR_PTA4 | mask_flag);
	}
	else
	{
		pcrc_config(PCR_PTC6 | mask_flag);
	}
}

void GPIO_SetPinValue(GPIO_t *gpio, uint32_t pin)
{
	gpio->PSOR |= (1 << pin);
}

void GPIO_ClearPinValue(GPIO_t *gpio, uint32_t pin)
{
	gpio->PCOR = (1 << pin);
}

void GPIO_TogglePinValue(GPIO_t *gpio, uint32_t pin)
{
	gpio->PTOR |= (1 << pin);
}
/* Support function to turn on the blue LED for testings */
void toggle_RED()
{
	GPIO_B->PTOR = 0x00400000;
}

void toggle_BLUE()
{
	GPIO_B->PTOR = 0x00200000;
}

void turn_off()
{
	GPIO_B->PTOR = 0x00600000;
}
