/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_KEYBOARD_H__
#define __VNC_KEYBOARD_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief For a Viewer, convert keyboard input to platform-independent symbols.
 *
 * Keyboard keys are represented by keysyms, as used in the X Window System.
 * This file provides the vnc_unicodeToKeysym() function to convert unicode
 * characters to keysyms, as well as defining macros for most common
 * non-character keysyms.  For further information on keysyms, see
 * http://www.x.org/releases/X11R7.6/doc/xproto/x11protocol.html#keysym_encoding
 */

/**
 * Converts a unicode character to a keysym, suitable for passing to
 * vnc_Viewer_sendKeyDown().
 *
 * @param unicodeChar The unicode character.
 *
 * @return The keysym value corresponding to @p unicodeChar, or zero if not
 *     recognized.
 */
VNC_SDK_API vnc_uint32_t vnc_unicodeToKeysym(vnc_uint32_t unicodeChar);

#ifdef __cplusplus
}
#endif

/** @name Non-character keys on the main part of the keyboard
 *  @{ */

#define XK_BackSpace                     0xff08  /**< Back space, back char */
#define XK_Tab                           0xff09
#define XK_Return                        0xff0d  /**< Return, enter */
#define XK_Pause                         0xff13  /**< Pause, hold */
#define XK_Scroll_Lock                   0xff14
#define XK_Sys_Req                       0xff15
#define XK_Escape                        0xff1b
#define XK_Delete                        0xffff  /**< Delete, rubout */

#define XK_Home                          0xff50
#define XK_Left                          0xff51  /**< Move left, left arrow */
#define XK_Up                            0xff52  /**< Move up, up arrow */
#define XK_Right                         0xff53  /**< Move right, right arrow */
#define XK_Down                          0xff54  /**< Move down, down arrow */
#define XK_Page_Up                       0xff55
#define XK_Page_Down                     0xff56
#define XK_End                           0xff57

#define XK_Print                         0xff61
#define XK_Insert                        0xff63  /**< Insert, insert here */
#define XK_Menu                          0xff67
#define XK_Break                         0xff6b

/** @} */

/** @name Keys on the numeric keypad
 *  @{ */

#define XK_KP_Enter                      0xff8d  /**< Enter */
#define XK_KP_Home                       0xff95
#define XK_KP_Left                       0xff96
#define XK_KP_Up                         0xff97
#define XK_KP_Right                      0xff98
#define XK_KP_Down                       0xff99
#define XK_KP_Page_Up                    0xff9a
#define XK_KP_Page_Down                  0xff9b
#define XK_KP_End                        0xff9c
#define XK_KP_Insert                     0xff9e
#define XK_KP_Delete                     0xff9f
#define XK_KP_Multiply                   0xffaa
#define XK_KP_Add                        0xffab
#define XK_KP_Separator                  0xffac  /**< Separator, often comma */
#define XK_KP_Subtract                   0xffad
#define XK_KP_Decimal                    0xffae
#define XK_KP_Divide                     0xffaf

#define XK_KP_0                          0xffb0
#define XK_KP_1                          0xffb1
#define XK_KP_2                          0xffb2
#define XK_KP_3                          0xffb3
#define XK_KP_4                          0xffb4
#define XK_KP_5                          0xffb5
#define XK_KP_6                          0xffb6
#define XK_KP_7                          0xffb7
#define XK_KP_8                          0xffb8
#define XK_KP_9                          0xffb9

/** @} */

/** @name Function keys
 *  @{ */

#define XK_F1                            0xffbe
#define XK_F2                            0xffbf
#define XK_F3                            0xffc0
#define XK_F4                            0xffc1
#define XK_F5                            0xffc2
#define XK_F6                            0xffc3
#define XK_F7                            0xffc4
#define XK_F8                            0xffc5
#define XK_F9                            0xffc6
#define XK_F10                           0xffc7
#define XK_F11                           0xffc8
#define XK_F12                           0xffc9

/** @} */

/** @name Modifier keys
 *  @{ */

#define XK_Shift_L                       0xffe1  /**< Left shift */
#define XK_Shift_R                       0xffe2  /**< Right shift */
#define XK_Control_L                     0xffe3  /**< Left control */
#define XK_Control_R                     0xffe4  /**< Right control */

#define XK_Alt_L                         0xffe9  /**< Left alt / Mac Command */
#define XK_Alt_R                         0xffea  /**< Right alt */
#define XK_Super_L                       0xffeb  /**< Left Windows key */
#define XK_Super_R                       0xffec  /**< Right Windows key */

#define XK_ISO_Level3_Shift              0xfe03  /**< AltGr / Mac Option(Alt) */

/** @} */

#endif
