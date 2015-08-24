/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_PRIVATE_H__
#define __VNC_PRIVATE_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * Functions used by RealVNC only.
 */

/**
 * Pass in a buffer containing a HostedConfig.pkg file, which will be used
 * instead of the built-in one.  This is used for testing against environments
 * other than the live deployment.
 */
VNC_SDK_API void
vnc_Private_setCloudDeployment(_In_reads_bytes_(bufSize) const void* buf,
                               int bufSize);

/**
 * Waits for up to timeoutMs for an event to occur and then performs a single
 * iteration of the event loop. Used only for debugging/testing purposes.
 * Pass a timeout of -1 to wait forever. Only supported by Windows and FD
 * event loops.
 */
VNC_SDK_API void
vnc_EventLoop_step(int timeoutMs);

/*
 * Unreleased functions -- don't forget to edit html5.py and java.py as well
 * when these get moved into the public headers, so that they're included in the
 * bindings.
 */

#ifdef __cplusplus
}
#endif

#endif
