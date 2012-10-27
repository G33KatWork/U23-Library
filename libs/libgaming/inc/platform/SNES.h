#ifndef _SNES_CTRL_H_
#define _SNES_CTRL_H_

/*! @addtogroup libgaming
 * @{ */

/*! @addtogroup SNES
 * @brief SNES Controller Interface
 * @{ */

#include <stdint.h>
#include <stm32f4xx/stm32f4xx.h>

#define GPIO_SNES1_LATCH_PIN		(GPIO_Pin_6)
#define GPIO_SNES1_CLOCK_PIN		(GPIO_Pin_7)
#define GPIO_SNES1_DATA_PIN			(GPIO_Pin_5)

#define GPIO_SNES2_LATCH_PIN		(GPIO_Pin_1)
#define GPIO_SNES2_CLOCK_PIN		(GPIO_Pin_3)
#define GPIO_SNES2_DATA_PIN			(GPIO_Pin_0)

#define GPIO_SNES1_LATCH_PORT		(GPIOD)
#define GPIO_SNES1_CLOCK_PORT		(GPIOD)
#define GPIO_SNES1_DATA_PORT		(GPIOD)

#define GPIO_SNES2_LATCH_PORT		(GPIOD)
#define GPIO_SNES2_CLOCK_PORT		(GPIOD)
#define GPIO_SNES2_DATA_PORT		(GPIOD)

#define GPIO_SNES1_LATCH_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES1_CLOCK_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES1_DATA_CLK			RCC_AHB1Periph_GPIOD

#define GPIO_SNES2_LATCH_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES2_CLOCK_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES2_DATA_CLK			RCC_AHB1Periph_GPIOD

/*!
 * @union snes_button_state_t
 * @brief Represents the state of a SNES Controller
 * @field Reserved Reserved for future use
 * @field R Represents right shoulder button
 * @field L Represents left shoulder button 
 * @field X Represents X-Button 
 * @field A Represents A-Button 
 * @field Right Represents Right-Button 
 * @field Left Represents Left-Button 
 * @field Down Represents Down-Button 
 * @field Up Represents Up-Button 
 * @field Start Represents Start-Button 
 * @field Select Represents Select-Button 
 * @field Y Represents Y-Button 
 * @field B Represents B-Button 
 * @field raw raw shift-register data from SNES controller
 */
typedef union {
	struct {
		uint16_t Reserved : 4;
		uint16_t R        : 1;
		uint16_t L        : 1;
		uint16_t X        : 1;
		uint16_t A        : 1;
		uint16_t Right    : 1;
		uint16_t Left     : 1;
		uint16_t Down     : 1;
		uint16_t Up       : 1;
		uint16_t Start    : 1;
		uint16_t Select   : 1;
		uint16_t Y        : 1;
		uint16_t B        : 1;
	} buttons;
	uint16_t raw;
} snes_button_state_t;

/*!
 * @brief Initializes the SNES Controller 
 *
 * This function is automatically called in main().
 * At the moment there is only one Controller supported.
 */
void InitializeSnesController();

/*!
 * @brief Gets the current controller state for Player 1
 * @return snes_button_state_t The current state of the first SNES-Controller
 */
snes_button_state_t GetControllerState1();

/*!
 * @brief Gets the current controller state for Player 2
 * @return snes_button_state_t The current state of the second SNES-Controller
 */
snes_button_state_t GetControllerState2();

/*!
 * @brief Handler for the timer used to make SNES-Handling non-blocking
 *
 * Ignore this, read code in .c or ask staff for more information
 */
void HandleSnesTimerIRQ();

/*! @} */
/*! @} */

#endif
