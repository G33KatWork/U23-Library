#ifndef __FONT_H__
#define __FONT_H__

#include <Bitmap.h>
#include <RLEBitmap.h>

/*! @addtogroup libgaming
 * @{ */

/*! @addtogroup Font
 * @brief This header is used to define Fonts and Text-drawing functions.
 * @{ */

/*! @brief 16-Pixel font - black */
extern const RLEBitmap * const fontblack16[256];

/*! @brief 8-Pixel font - black */
extern const RLEBitmap * const fontblack8[256];

/*! @brief 16-Pixel font - white */
extern const RLEBitmap * const fontwhite16[256];

/*! @brief 8-Pixel font - white */
extern const RLEBitmap * const fontwhite8[256];

/*!
 *	@brief Sets the font that is used by DrawText
 *	@param font Pointer to the font that will be used by DrawText
 *
 *	This needs to be called before you can use DrawText.
 */
void setFont(const RLEBitmap * const * font);

/*!
 *	@brief Draws Text to a Bitmap
 *	@param dest The Bitmap the text will be drawn on
 *	@param text String you want to draw
 *	@param x x-coordinate for the text
 *	@param y y-coordinate for the text
 *
 *	Before calling DrawText you need to call setFont once to set a font. Draw Text will use this font until you set a different one.
 *	Newlines are supported, text will continue at x / y + width of font
 */
void DrawText(Bitmap* dest, char *text, int x, int y);

/*! @} */
/*! @} */

#endif
