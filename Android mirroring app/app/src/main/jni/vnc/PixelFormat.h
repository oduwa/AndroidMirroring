/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_PIXELFORMAT_H__
#define __VNC_PIXELFORMAT_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Configure a format for the framebuffer's pixels
 *
 * A pixel format describes the representation of individual pixels in a
 * framebuffer.  This includes how many bits are used for each red, green and
 * blue value, and in what order these are placed.  It also includes the total
 * number of bits per pixel (since there may be some unused bits to allow for
 * efficient alignment in memory).
 *
 * Each pixel value is taken to be an integer, and the color components are
 * specified in terms of the maximum value and the "shift", the offset of the
 * component in the pixel value.  When stored in memory, each pixel is written
 * using the CPU's native endianness.  For example, the "rgb888" pixel format
 * uses the bottom eight bits of a four-byte integer to represent the blue
 * component (<tt>(R<<16)|(G<<8)|B</tt>), but on a little-endian machine blue is
 * the first byte in memory (<tt>[B,G,R,X]</tt>).
 *
 * Note that Intel x86 and x64 are little-endian; Android and iOS use ARM in
 * little-endian mode; and the VNC SDK on HTML5 represents pixels using
 * little-endian ordering.
 */


/** @name Standard pixel formats
 *  @{ */

/** 32 bits per pixel stored as XXXXXXXXRRRRRRRRGGGGGGGGBBBBBBBB in most
 *  significant to least significant bit order */
VNC_SDK_API _Ret_valid_ const vnc_PixelFormat* vnc_PixelFormat_rgb888();

/** 32 bits per pixel stored as XXXXXXXXBBBBBBBBGGGGGGGGRRRRRRRR in most
 *  significant to least significant bit order */
VNC_SDK_API _Ret_valid_ const vnc_PixelFormat* vnc_PixelFormat_bgr888();

/** 16 bits per pixel stored as RRRRRGGGGGGBBBBB in most significant to least
 *  significant bit order */
VNC_SDK_API _Ret_valid_ const vnc_PixelFormat* vnc_PixelFormat_rgb565();

/** 16 bits per pixel stored as XRRRRRGGGGGBBBBB in most significant to least
 *  significant bit order */
VNC_SDK_API _Ret_valid_ const vnc_PixelFormat* vnc_PixelFormat_rgb555();

/** @} */

/** @name Accessor functions for pixel format parameters
 *  @{ */

/**
 * Gets the total number of bits per pixel.
*/
VNC_SDK_API int vnc_PixelFormat_bpp(const vnc_PixelFormat* pf);

/**
 * Gets the number of significant bits that are used to store pixel data.
 */
VNC_SDK_API int vnc_PixelFormat_depth(const vnc_PixelFormat* pf);

/**
 * Gets the maximum value for the red pixel value.
 */
VNC_SDK_API int vnc_PixelFormat_redMax(const vnc_PixelFormat* pf);

/**
 * Gets the maximum value for the green pixel value.
 */
VNC_SDK_API int vnc_PixelFormat_greenMax(const vnc_PixelFormat* pf);

/**
 * Gets the maximum value for the blue pixel value.
 */
VNC_SDK_API int vnc_PixelFormat_blueMax(const vnc_PixelFormat* pf);

/**
 * Gets the number of bits the red pixel value is shifted.
 */
VNC_SDK_API int vnc_PixelFormat_redShift(const vnc_PixelFormat* pf);

/**
 * Gets the number of bits the green pixel value is shifted.
 */
VNC_SDK_API int vnc_PixelFormat_greenShift(const vnc_PixelFormat* pf);

/**
 * Gets the number of bits the blue pixel value is shifted.
 */
VNC_SDK_API int vnc_PixelFormat_blueShift(const vnc_PixelFormat* pf);

/** @} */

/** @name Custom pixel formats
 *  @{ */

/**
 * Creates a custom pixel format based on the given parameters.
 *
 * @param bitsPerPixel The total number of bits per pixel (a multiple of eight).
 * @param redMax The maximum value for the red pixel value.
 * @param greenMax The maximum value for the red pixel value.
 * @param blueMax The maximum value for the red pixel value.
 * @param redShift The number of bits the red pixel is shifted.
 * @param greenShift The number of bits the green pixel is shifted.
 * @param blueShift The number of bits the blue pixel is shifted.
 */
VNC_SDK_API vnc_PixelFormat*
vnc_PixelFormat_create(int bitsPerPixel, int redMax, int greenMax, int blueMax,
                       int redShift, int greenShift, int blueShift);

/**
 * Destroy a custom pixel format.
 *
 * @param pf The pixel format object.
 */
VNC_SDK_API void
vnc_PixelFormat_destroy(vnc_PixelFormat* pf);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
