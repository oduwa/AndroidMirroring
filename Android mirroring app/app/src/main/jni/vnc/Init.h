/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_INIT_H__
#define __VNC_INIT_H__

#include "Common.h"
#include "Version.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * Initialize the SDK.
 */

/** Enumeration of types of event loop used by the SDK. */
typedef enum {
  /**
   * The default event loop is a Windows event loop on Windows, a
   * file-descriptor event loop on Linux, a Core Foundation event loop on OS X
   * and iOS, and an Android event loop on Android.  There is no event loop in
   * HTML5.
   */
  vnc_EventLoopType_Default = 0,
  /** The Windows event loop is only available on Windows. */
  vnc_EventLoopType_Win,
  /**
   * The file-descriptor event loop is available on Linux, OS X, iOS, and
   * Android.
   */
  vnc_EventLoopType_Fd,
  /** The Core Foundation event loop is available on OS X and iOS. */
  vnc_EventLoopType_Cf,
  /** The Android "Looper" event loop is only available on Android. */
  vnc_EventLoopType_Android,
} vnc_EventLoopType;

/**
 * Initializes the SDK with the default @ref vnc_EventLoopType for the platform.
 * Only call functions in vnc/DataStore.h or vnc/Logger.h before you call this
 * function.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval VersionError
 *     @p majorVersion or @p minorVersion do not match the run-time version.
 */
#define vnc_init() \
  vnc_initInternal(VNC_SDK_MAJOR_VERSION, VNC_SDK_MINOR_VERSION, \
                   VNC_SDK_PATCH_VERSION, VNC_SDK_BUILD_NUMBER, \
                   vnc_EventLoopType_Default)

/**
 * Initializes the SDK, as for vnc_init().  This is used in the case where the
 * default event loop type is not desired, for example to create a
 * file-descriptor event loop on OS X for the purpose of integrating with a
 * third-party event loop such as Qt.
 */
#define vnc_initWithLoopType(eventLoopType) \
  vnc_initInternal(VNC_SDK_MAJOR_VERSION, VNC_SDK_MINOR_VERSION, \
                   VNC_SDK_PATCH_VERSION, VNC_SDK_BUILD_NUMBER, \
                   eventLoopType)

/**
 * @internal
 * This should not be called directly, instead the vnc_init() macro should be
 * used.
 */
VNC_SDK_API vnc_status_t vnc_initInternal(
  int majorVersion, int minorVersion, int patchVersion, int buildNumber,
  vnc_EventLoopType eventLoopType);

#ifdef __cplusplus
}
#endif

#endif
