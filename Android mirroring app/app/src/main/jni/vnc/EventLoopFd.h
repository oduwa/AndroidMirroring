/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_EVENTLOOPFD_H__
#define __VNC_EVENTLOOPFD_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Implement a custom file-descriptor-based event loop.
 *
 * The following functions are used for integrating the VNC SDK into an
 * existing file-descriptor-based event loop.  This could either be an
 * application's event loop implemented directly using poll or select, or
 * a third party event loop where the SDK's file descriptors can be registered
 * (for example using Qt's QSocketNotifier).
 *
 * The functions in this file are all only available on Linux, OS X, iOS, and
 * Android, and an assertion is used to verify that vnc_init() was used to
 * create a file-descriptor event loop.
 *
 * See the basicViewerX11 sample for an example of how to integrate this into an
 * existing application event loop using select.
 *
 * To integrate into a third-party event loop (e.g.  using a framework such as
 * Qt), implement the vnc_EventLoopFd_Callback::eventUpdated and 
 * vnc_EventLoopFd_Callback::timerUpdated callbacks to create, update
 * or destroy the appropriate notifiers for the framework.  On receiving
 * notifications, call vnc_EventLoopFd_markEvents() (in the case of an fd event)
 * and then call vnc_EventLoopFd_handleEvents().
 */

/** Enumeration of file descriptor events for event selection. */
typedef enum {
  /**
   * Monitor using @c readfds with @c select() (the second argument), or
   *  @c POLLIN with @c poll().
   */
  vnc_EventLoopFd_Read = 0x01,
  /**
   * Monitor using @c writefds with @c select() (the third argument), or
   *  @c POLLOUT with @c poll().
   */
  vnc_EventLoopFd_Write = 0x02,
  /**
   * Monitor using @c exceptfds with @c select() (the fourth argument), or
   *  @c POLLPRI with @c poll().
   */
  vnc_EventLoopFd_Except = 0x04,
} vnc_EventLoopFd_Event;



/** 
 * Callback receiving notifications for a file-descriptor-based event loop. 
 * 
 * @see vnc_EventLoopFd_setCallback()
 */
typedef struct {
  /**
   * Notification that a file descriptor's event mask has changed.  This
   * callback is required.
   *
   * @param fd The file descriptor whose event mask has changed.
   * @param eventMask Specifies the events we are interested in for this
   *     descriptor.  When one of the specified events occurs, the
   *     implementation should call vnc_EventLoopFd_markEvents() to mark which
   *     event(s) occurred, then call vnc_EventLoopFd_handleEvents().  An
   *     @p eventMask of 0 indicates the SDK is no longer using this file
   *     descriptor; it should be removed immediately from the monitored set of
   *     descriptors.
   */
  void
  (*eventUpdated)(void* userData,
                  int fd,
                  _Enum_mask_(vnc_EventLoopFd_Event) int eventMask);

  /**
   * Notification that the timer expiry period has been updated.
   * When the specified expiry time has passed, the implementation should call
   * vnc_EventLoopFd_handleEvents().
   *
   * This callback is optional, and is only needed if integrating with a
   * third-party event loop rather than calling vnc_EventLoopFd_handleEvents()
   * directly to a create a custom blocking event loop.
   *
   * @param expiryMs The maximum time that should be allowed to pass before the
   *     next call to vnc_EventLoopFd_handleEvents(), in milliseconds
   */
  _Callback_default_(void) void
  (*timerUpdated)(void* userData,
                  int expiryMs);
} vnc_EventLoopFd_Callback;

/**
 * Sets the event loop callback.
 *
 * @param callback The new event loop callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The vnc_EventLoopFd_Callback::eventUpdated callback is NULL
 */
VNC_SDK_API _Platform_(Linux|Darwin|iOS|Android) vnc_status_t
vnc_EventLoopFd_setCallback(_In_opt_ const vnc_EventLoopFd_Callback* callback,
                            void* userData);

/**
 * Marks event(s) that occurred on the specified file descriptor.
 *
 * @param fd The file descriptor.
 * @param events A mask of events to be marked for handling.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p fd is not recognized by the SDK
 */
VNC_SDK_API _Platform_(Linux|Darwin|iOS|Android) vnc_status_t
vnc_EventLoopFd_markEvents(int fd,
                           _Enum_mask_(vnc_EventLoopFd_Event) int events);

/**
 * Handles events on the file descriptors and process expired timers.
 *
 * @return The number of milliseconds until the next timer expires
 */
VNC_SDK_API _Platform_(Linux|Darwin|iOS|Android) int
vnc_EventLoopFd_handleEvents();

#ifdef __cplusplus
}
#endif

#endif
