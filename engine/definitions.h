/**
 * @file    definitions.h
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

/////////////////
// Special Key //
/////////////////

/**
 * @name Special Key Codes
 * @brief Key codes used by the engine for keyboard and special key input callbacks.
 *
 * These codes are typically used for function keys, arrow keys, and modifier keys,
 * often derived from the GLUT or FreeGLUT standard.
 */
 ///@{
#define ENGINE_KEY_F1			0x0001 ///< Function Key 1
#define ENGINE_KEY_F2			0x0002 ///< Function Key 1
#define ENGINE_KEY_F3			0x0003 ///< Function Key 1
#define ENGINE_KEY_F4			0x0004 ///< Function Key 1
#define ENGINE_KEY_F5			0x0005 ///< Function Key 1
#define ENGINE_KEY_F6			0x0006 ///< Function Key 1
#define ENGINE_KEY_F7			0x0007 ///< Function Key 1
#define ENGINE_KEY_F8			0x0008 ///< Function Key 1
#define ENGINE_KEY_F9			0x0009 ///< Function Key 1
#define ENGINE_KEY_F10			0x000A ///< Function Key 1
#define ENGINE_KEY_F11			0x000B ///< Function Key 1
#define ENGINE_KEY_F12			0x000C ///< Function Key 1
#define ENGINE_KEY_LEFT			0x0064 ///< Left Arrow Key
#define ENGINE_KEY_UP			0x0065 ///< Up Arrow Key
#define ENGINE_KEY_RIGHT		0x0066 ///< Right Arrow Key
#define ENGINE_KEY_DOWN			0x0067 ///< Bottom Arrow Key
#define ENGINE_KEY_PAGE_UP		0x0068 ///< Page Up Key
#define ENGINE_KEY_PAGE_DOWN	0x0069 ///< Page Down Key
#define ENGINE_KEY_HOME			0x006A ///< Home Key
#define ENGINE_KEY_END			0x006B ///< End Key
#define ENGINE_KEY_INSERT		0x006C ///< Insert Key

#define ENGINE_KEY_NUM_LOCK		0x006D ///< Num Lock Key
#define ENGINE_KEY_BEGIN		0x006E ///< Begin Key (often synonymous with Home)
#define ENGINE_KEY_DELETE		0x006F ///< Delete Key
#define ENGINE_KEY_SHIFT_L		0x0070 ///< Left Shift Modifier
#define ENGINE_KEY_SHIFT_R		0x0071 ///< Right Shift Modifier
#define ENGINE_KEY_CTRL_L		0x0072 ///< Left Control Modifier
#define ENGINE_KEY_CTRL_R		0x0073 ///< Right Control Modifier
#define ENGINE_KEY_ALT_L		0x0074 ///< Left Alt Modifier
#define ENGINE_KEY_ALT_R		0x0075 ///< Right Alt Modifier
#define ENGINE_KEY_SUPER_L		0x0076 ///< Left Super (Windows/Command) Modifier
#define ENGINE_KEY_SUPER_R		0x0077 ///< Right Super (Windows/Command) Modifier
///@}

//////////////////
// Camera Ratio //
//////////////////

/**
 * @name Common Camera Aspect Ratios
 * @brief Predefined floating-point constants for common display and video aspect ratios.
 *
 * These definitions simplify setting up the projection matrix for cameras, ensuring
 * proper display geometry.
 */
 ///@{
#define RATIO_1_1				1.0f / 1.0f ///< Square aspect ratio (1:1)
#define RATIO_3_4				3.0f / 4.0f ///< Tall aspect ratio (e.g., older monitors/portrait)
#define RATIO_9_16				9.0f / 16.0f ///< Portrait HD aspect ratio
#define RATIO_2_3				2.0f / 3.0f ///< Tall aspect ratio
#define RATIO_3_5				3.0f / 5.0f ///< Aspect ratio (3:5)
#define RATIO_4_5				4.0f / 5.0f ///< Aspect ratio (4:5)
#define RATIO_5_7				5.0f / 7.0f ///< Aspect ratio (5:7)
#define RATIO_4_3				4.0f / 3.0f ///< Standard definition (SD) aspect ratio
#define RATIO_16_9				16.0f / 9.0f ///< High definition (HD) widescreen aspect ratio
#define RATIO_3_2				3.0f / 2.0f ///< Photography aspect ratio
#define RATIO_5_3				5.0f / 3.0f ///< Aspect ratio (5:3)
#define RATIO_5_4				5.0f / 4.0f ///< Aspect ratio (5:4)
#define RATIO_7_5				7.0f / 5.0f ///< Aspect ratio (7:5)
///@}
