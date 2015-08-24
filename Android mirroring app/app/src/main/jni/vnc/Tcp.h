/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_TCP_H__
#define __VNC_TCP_H__

#include "Common.h"

/** Opaque type for a TCP listener */
typedef struct vnc_TcpListener vnc_TcpListener;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the default port for VNC connections (5900).
 */
VNC_SDK_API int
vnc_Tcp_defaultPort();

/** Callback for filtering incoming TCP connections. */
typedef struct {
  /**
   * Notification to provide address-based filtering of incoming connections.
   * This callback is optional.
   *
   * @return #vnc_true to allow the connection, or #vnc_false to deny, in which
   *     case the connection will be closed.
   */
  _Callback_default_(true) vnc_bool_t
  (*filterConnection)(void* userData,
                      vnc_TcpListener* listener,
                      const char* ipAddress,
                      int port);
} vnc_TcpListener_Callback;

/**
 * Begins listening for incoming TCP connections on the given port (IPv4 and
 * IPv6).  To stop listening, destroy the TCP listener.
 *
 * @param localhostListen Listen on the localhost addresses only, rather than
 *     all available addresses.
 * @param port The port number to listen on.
 * @param connectionHandler The object which shall handle incoming connections
 *     (if not rejected by the filter function).  It is either a server or a
 *     viewer.  It must be destroyed after the TCP listener.
 * @param callback An optional callback to filter incoming connections.
 *
 * @return Returns a new TCP listener on success, or NULL in the case of an
 *     error, in which case vnc_getLastError() can be used to get the error
 *     code.
 *
 * @retval AddressError
 *     There was an error listening on the specified address
 */
VNC_SDK_API _Platform_(!HTML5) vnc_TcpListener*
vnc_TcpListener_create(vnc_bool_t localhostListen,
                       int port,
                       vnc_ConnectionHandler* connectionHandler,
                       _In_opt_ const vnc_TcpListener_Callback* callback,
                       void* userData);

/**
 * Destroys the TCP listener.  Any ongoing connections are not affected, but
 * new connections will not be accepted.
 */
VNC_SDK_API _Platform_(!HTML5) void
vnc_TcpListener_destroy(vnc_TcpListener* listener);

/**
 * Begins an outgoing TCP connection to the given hostname or IP address.  The
 * connection will be handled by the @c connectionHandler.  This function is
 * not available on the HTML5 platform.
 *
 * Connection errors are notified using the server or viewer's callbacks
 * (depending on the type of the connection handler).
 *
 * @param hostOrIpAddress The DNS hostname or IP address to connect to.
 * @param port The port number to connect to.
 * @param connectionHandler The handler (a Viewer or a Server) to handle the
 *     connection.
 *
 * @return #vnc_success is returned on success or #vnc_failure in the case of
 *     an error, in which case vnc_getLastError() can be used to get the error
 *     code.
 *
 * @retval InvalidArgument
 *     @p port is zero or negative, or @p hostOrIpAddress is malformed
 * @retval UnexpectedCall
 *     The connection handler cannot accept a connection
 */
VNC_SDK_API _Platform_(!HTML5) vnc_status_t
vnc_Tcp_connect(const char* hostOrIpAddress,
                int port,
                vnc_ConnectionHandler* connectionHandler);

/**
 * Begin an outgoing WebSocket connection to the given WebSocket URL.  The
 * connection will be handled by the connectionHandler.  This function is only
 * available on the HTML5 platform.
 *
 * @param url A WebSocket URL to connect to.
 * @param connectionHandler The handler (a Viewer or a Server) to handle the
 *     connection.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The URL is invalid
 * @retval UnexpectedCall
 *     The connection handler cannot accept a connection
 */
VNC_SDK_API _Platform_(HTML5) vnc_status_t
vnc_Websocket_connect(const char* url,
                      vnc_ConnectionHandler* connectionHandler);

#ifdef __cplusplus
}
#endif

#endif
