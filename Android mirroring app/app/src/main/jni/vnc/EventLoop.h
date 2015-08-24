/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_EVENTLOOP_H__
#define __VNC_EVENTLOOP_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Run the SDK's event loop.
 *
 * The following functions are used for using the VNC SDK's event loop as the
 * the application's event loop.  In this case, the event loop will respond to
 * the events as they occur inside the SDK, invoking callbacks as appropriate,
 * until vnc_EventLoop_stop() is called inside one of the callbacks, at
 * which point vnc_EventLoop_run() exits on the next turn of the event loop.
 *
 * The other way of using the SDK is to use an event loop external to the SDK.
 * On some platforms such as Android, OS X and iOS, the SDK is able to
 * directly register its timers and sockets with the thread's Android or Core
 * Foundation run loop, so simply using the platform's normal event loop will
 * allow the SDK to process I/O automatically.  On other platforms, the SDK's
 * timers and sockets must instead be manually added to the application's
 * event loop; use the functions in vnc/EventLoopFd.h and vnc/EventLoopWin.h to
 * do this.
 *
 * vnc_EventLoop_run() and vnc_EventLoop_stop() are not available on the HTML5
 * platform.
 */

/**
 * Runs the event loop until vnc_EventLoop_stop() is called.  This function may
 * not be used if the SDK was initialized with @ref vnc_EventLoopType_Android.
 */
VNC_SDK_API _Platform_(!HTML5) void vnc_EventLoop_run();

/**
 * Stops the event loop previously started with vnc_EventLoop_run(), causing
 * vnc_EventLoop_run() to return promptly.  This function can be called on the
 * same thread as vnc_EventLoop_run() from inside an SDK callback, or can also
 * be called from another thread.  (This is the only function in the VNC SDK
 * which can safely be called on a thread other than the one which invoked
 * vnc_init()).  After the event loop has been stopped, is it possible to
 * continue it by calling vnc_EventLoop_run() again.  If vnc_EventLoop_stop()
 * is called when vnc_EventLoop_run() is not currently executing, it will exit
 * immediately the next time it is called.  This function may not be used if the
 * SDK was initialized with @ref vnc_EventLoopType_Android.
 */
VNC_SDK_API _Platform_(!HTML5) void vnc_EventLoop_stop();

#ifdef __cplusplus
}
#endif

#endif
