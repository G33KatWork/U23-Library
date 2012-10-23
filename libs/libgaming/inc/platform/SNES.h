#ifndef _SNES_CTRL_H_
#define _SNES_CTRL_H_

/*! @header SNES.h
    @discussion SNES Controller header
 */

#include <stdint.h>

/*! @union snes_button_state_t
	@field Reserved Reserved for future use
	@field R Represents right shoulder button
	@field L Represents left shoulder button 
	@field X Represents X-Button 
	@field A Represents A-Button 
	@field Right Represents Right-Button 
	@field Left Represents Left-Button 
	@field Down Represents Down-Button 
	@field Up Represents Up-Button 
	@field Start Represents Start-Button 
	@field Select Represents Select-Button 
	@field Y Represents Y-Button 
	@field B Represents B-Button 
	@field raw raw shift-register data from SNES controller
    @discussion snes_button_state_t represents the state of a SNES Controller
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
	@function InitializeSnesController
	Initializes the SNES Controller 
	@discussion This function is called in Main.c
	@discussion At the moment there is only one Controller supported
 */
void InitializeSnesController();

/*!
	@function GetControllerState
	Initializes the SNES Controller 
	@returns snes_button_state_t The current state of the SNES-Controller connected
	@discussion At the moment there is only one Controller supported
 */
snes_button_state_t GetControllerState();

/*!
	@function HandleSnesTimerIRQ
	Handler for the timer used to make SNES-Handling non-blocking
	@discussion Do not care, read code in .c or ask staff for more information
 */
void HandleSnesTimerIRQ();

#endif
