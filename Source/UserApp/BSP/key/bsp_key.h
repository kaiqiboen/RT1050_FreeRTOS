#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "board.h"


#define EXAMPLE_SW_GPIO          BOARD_USER_BUTTON_GPIO
#define EXAMPLE_SW_GPIO_PIN      BOARD_USER_BUTTON_GPIO_PIN
#define EXAMPLE_SW_IRQ           BOARD_USER_BUTTON_IRQ
#define EXAMPLE_GPIO_IRQHandler  BOARD_USER_BUTTON_IRQ_HANDLER
#define EXAMPLE_SW_NAME          BOARD_USER_BUTTON_NAME




void BSP_KeyInit(void);





#endif