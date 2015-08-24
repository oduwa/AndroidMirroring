/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_EVENTLOOPWIN_H__
#define __VNC_EVENTLOOPWIN_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* HANDLE;

/**
 * @file
 * @brief Implement a custom Windows HANDLE-based event loop.
 *
 * The following functions are used for integrating the VNC SDK into an
 * existing Windows HANDLE-based event loop.  This could either be an
 * application's event loop implemented directly using
 * @c MsgWaitForMultipleObjects, or a third party event loop where the SDK's
 * event HANDLEs can be registered (for example using Qt's QWinEventNotifier).
 *
 * The functions in this file are all only available on Windows, and an
 * assertion is used to verify that vnc_init() was used to create a Windows
 * event loop.
 *
 * See the basicViewerWin sample for an example of how to integrate this into an
 * existing Windows event loop using @c WaitForMultipleObjects.
 *
 * To integrate into a third-party event loop (for use with a framework such as 
 * Qt), implement the vnc_EventLoopWin_Callback::eventUpdated and 
 * vnc_EventLoopWin_Callback::timerUpdated callbacks to create, update or
 * destroy the appropriate notifiers for the framework.  On receiving
 * notifications, call vnc_EventLoopWin_handleEvent().
 */

/** 
 * Callback receiving notifications for a Windows HANDLE-based event loop. 
 * 
 * @see vnc_EventLoopWin_setCallback()
 */
typedef struct {
  /**
   * Notification that a Windows event is being added or removed.  This callback
   * is required.
   *
   * The implementation should call vnc_EventLoopWin_handleEvent() whenever a
   * HANDLE owned by the SDK is signalled.  This callback does not get called
   * for events that were created by the SDK prior to the callback being set;
   * the implementation should use vnc_EventLoopWin_getEvents() to query those
   * when vnc_EventLoopWin_setCallback() is called.
   *
   * @param event The HANDLE of the event being added or removed.
   * @param add Set to #vnc_true to signify that the SDK wishes to be
   *     notified when the given event occurs.  Set to #vnc_false when the
   *     SDK is no longer interested in the given event; it must be removed
   *     immediately from the set of monitored HANDLEs.
   */
  void
  (*eventUpdated)(void* userData,
                  HANDLE event,
                  vnc_bool_t add);

  /**
   * Notification that the timer expiry period has been updated.  When the
   * specified expiry time has passed, the implementation should call
   * vnc_EventLoopWin_handleEvent().
   *
   * This callback is optional, and is only needed if integrating with a
   * third-party event loop rather than calling vnc_EventLoopWin_handleEvent()
   * directly to a create a custom blocking event loop.
   *
   * @param expiryMs The maximum time that should be allowed to pass before the
   *     next call to vnc_EventLoopWin_handleEvent(), in milliseconds
   */
  _Callback_default_(void) void
  (*timerUpdated)(void* userData,
                  int expiryMs);
} vnc_EventLoopWin_Callback;

/**
 * Sets the event loop callback.
 *
 * @param callback The new event loop callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The vnc_EventLoopWin_Callback::eventUpdated callback is NULL
 */
VNC_SDK_API _Platform_(Windows) vnc_status_t
vnc_EventLoopWin_setCallback(_In_opt_ const vnc_EventLoopWin_Callback* callback,
                             void* userData);

/**
 * Gets the array of events that the SDK currently wishes to be notified of.
 *
 * @param events An output array which will be filled with the list of events
 *     the SDK is using.  The array pointed to by @p events must be
 *     @c MAXIMUM_WAIT_OBJECTS in size.
 *
 * @return The number of events set in the output array
 */
VNC_SDK_API _Platform_(Windows) int
vnc_EventLoopWin_getEvents(HANDLE* events);

/**
 * Handles the given event (if any) and process expired timers.  This should be
 * called whenever a timer expires or an event occurs.  If a timer expires and
 * no event has been signalled, pass NULL as the handle to process timer events
 * only.
 *
 * @param event The event which has been signalled.
 *
 * @return The number of milliseconds until the next timer expires
 */
VNC_SDK_API _Platform_(Windows) int
vnc_EventLoopWin_handleEvent(HANDLE event);

#ifdef __cplusplus
}
#endif

#endif
