#include "bsp_key.h"
#include "fsl_common.h"

volatile bool g_InputSignal = false;

void BSP_KeyInit(void)
{
	gpio_pin_config_t sw_config={
		kGPIO_DigitalInput,
		0,
		kGPIO_IntRisingEdge,
	};
	EnableIRQ(EXAMPLE_SW_IRQ);
  GPIO_PinInit(EXAMPLE_SW_GPIO, EXAMPLE_SW_GPIO_PIN, &sw_config);
	/* Enable GPIO pin interrupt */
  GPIO_PortEnableInterrupts(EXAMPLE_SW_GPIO, 1U << EXAMPLE_SW_GPIO_PIN);
}

void EXAMPLE_GPIO_IRQHandler(void)
{
    /* clear the interrupt status */
    GPIO_PortClearInterruptFlags(EXAMPLE_SW_GPIO, 1U << EXAMPLE_SW_GPIO_PIN);
    /* Change state of switch. */
    g_InputSignal = true;
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

	