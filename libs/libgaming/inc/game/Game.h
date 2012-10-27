#ifndef _GAME_H_
#define _GAME_H_

#include <stdint.h>
#include <platform/SNES.h>
#include <platform/LED.h>
#include <platform/SNES.h>
#include <platform/SysTick.h>
#include <platform/VGA.h>
#include <platform/Random.h>

#include <Drawing.h>

/*! @addtogroup libgaming
 * @{ */

/*! @addtogroup Game
 * @brief This header is used to define the basic structure of games.
 * @{ */

/*!
 * @struct Gamestate
 * @brief Represents a Gamestate for a Game
 */
 typedef struct Gamestate {
	/** @brief Called once for every state.
	@param state the current Gamestate*/
	void (*Init)(struct Gamestate* state);
	/** @brief Called every time a state is entered.
	@param state the current Gamestate*/
	void (*OnEnter)(struct Gamestate* state);
	/** @brief Called every time a state is left.
	@param state the current Gamestate*/
	void (*OnLeave)(struct Gamestate* state);
	/** @brief Called every frame for the active state.
	@param time since last Update call in 10 milliseconds*/
	void (*Update)(uint32_t);
	/** @brief Called every frame for the active state after Update.
	@param surface The Surface use for drawing.*/
	void (*Draw)(Bitmap* surface);
	/** @brief ExitState will return to this state.*/
	struct Gamestate *previousState;
	/** @brief Is set after Init was called for this state.*/
	bool initialized;
} Gamestate;

/*!
 * @struct Game
 * @brief Represents a Game
 */
typedef struct {
	/** @brief The Current state that is used in the Udpate/Draw loop.*/
	Gamestate *currentState;
} Game;

/*! @brief The only Game you'll ever need. */
extern Game* TheGame;

/*!
 *	@brief Used to switch to a different Gamestate
 *	@param state Pointer to the Gamestate to switch to
 *	@return returns -1 if you did something stupid.
 *
 *	Use this in a Gamestates Update function. After the Update call is finished the Draw and OnLeave function of the current state will be called. After that the Init and OnEnter functions of the new state will be called. Init will be called only once for every state. The Game then returns to the Update/Draw loop of the new state. Gamestates are organized in a Stack. DO NOT change to a state again if you did not exit that state before.
 */
int ChangeState(Gamestate *state);

/*!
 *	@brief Used to return from a state to the previous state.
 *
 *	Use this in a Gamestates Update function. After the Update call is finished the Draw and OnLeave function of the current state will be called. After that the OnEnter functions of the state that you entered the current state from will be called. The Game then return to the Update/Draw loop of the previous state.
 */
void ExitState();

/*! @} */
/*! @} */

#endif
