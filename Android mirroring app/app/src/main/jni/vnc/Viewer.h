/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_VIEWER_H__
#define __VNC_VIEWER_H__

#include "Common.h"
#include "PixelFormat.h"
#include "DataBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file
 * Creates a VNC-compatible Viewer app for a device you want to control from.
 *
 * A Viewer runs on the device that you want to remotely control @em from.  It
 * provides a @em framebuffer which is kept up to date with the Server device
 * screen, and has functions for sending input events to the Server.
 *
 * Use vnc_Viewer_create() to create one @ref vnc_Viewer for each connection
 * made to a server.  Set up the framebuffer and associated callbacks using
 * vnc_Viewer_setViewerFb() and vnc_Viewer_setFramebufferCallback(), and set up
 * callbacks to provide authentication credentials using
 * vnc_Viewer_setAuthenticationCallback().
 *
 * To connect to a Server, call vnc_Viewer_getConnectionHandler() to obtain a
 * @ref vnc_ConnectionHandler for use with vnc_CloudConnector_connect() (or
 * vnc_CloudListener_create()), and start the process of connecting.
 *
 * When a connection is in progress, any defined
 * vnc_Viewer_ConnectionCallback::connecting callback is called.  Then, any
 * defined vnc_Viewer_PeerVerificationCallback::verifyPeer callback is called.
 * vnc_Viewer_sendPeerVerificationResponse() is used to indicate if the peer is
 * valid.  Next, the vnc_Viewer_AuthenticationCallback::requestUserCredentials
 * callback is called, and the user-supplied credentials are submitted using
 * vnc_Viewer_sendAuthenticationResponse().
 *
 * After authentication is complete, any defined
 * vnc_Viewer_ConnectionCallback::connected callback is called.  The
 * vnc_Viewer_FramebufferCallback::serverFbSizeChanged callback is then called
 * with the size of the server's framebuffer, and the
 * vnc_Viewer_FramebufferCallback::viewerFbUpdated callback is called each time
 * new pixel data is received.  Typically, vnc_Viewer_getViewerFbData() is used
 * to obtain the viewer framebuffer data for window painting purposes.
 *
 * Any keyboard or mouse input to be sent to the server is done using the
 * functions vnc_Viewer_sendKeyDown(), vnc_Viewer_sendKeyUp(),
 * vnc_Viewer_sendPointerEvent() and vnc_Viewer_sendScrollEvent().
 *
 * The viewer can end the connection by calling vnc_Viewer_disconnect().  When
 * the viewer has cleanly disconnected, or the connection was terminated by the
 * peer, the vnc_Viewer_ConnectionCallback::disconnected callback is called.
 * The reason for disconnection is available as a string ID in the callback, and
 * also from vnc_Viewer_getDisconnectReason().
 */

/**
 * Creates and returns a new viewer.
 *
 * @return Returns a new @ref vnc_Viewer.
 */
VNC_SDK_API vnc_Viewer* vnc_Viewer_create();

/**
 * Destroys the viewer.
 */
VNC_SDK_API void vnc_Viewer_destroy(vnc_Viewer* viewer);


/**
 * @name Functions for managing the status of a viewer connection
 * @{
 */

/**
 * Enumeration of connection statuses.  These are returned by
 * vnc_Viewer_getConnectionStatus() to query the status of the connection for
 * a given viewer.
 */
typedef enum {
  /** The viewer is not connected to a server. */
  vnc_Viewer_Disconnected,
  /**
   * The viewer is negotiating a connection to a server (includes
   * authentication).
   */
  vnc_Viewer_Connecting,
  /** The viewer is connected to a server. */
  vnc_Viewer_Connected,
  /**
   * vnc_Viewer_disconnect() has been called but we've not yet disconnected
   * from the server.
   */
   vnc_Viewer_Disconnecting
} vnc_Viewer_ConnectionStatus;

/**
 * Enumeration of disconnection flags.
 */
typedef enum {
  /**
   * The disconnection message could be shown to the user.  This is set if the
   * server sent this flag with its disconnection message, or the viewer
   * disconnected due to an error.
   */
  vnc_Viewer_AlertUser = 1,
  /** The connection has been broken, but a reconnection attempt may succeed. */
  vnc_Viewer_CanReconnect = 2,
} vnc_Viewer_DisconnectFlags;

/** Callback receiving state-change notifications for a Viewer. */
typedef struct {
  /**
   * Notification that is called when the viewer begins a connection attempt to
   * the server.  This callback is optional.
   *
   * When vnc_CloudConnector_connect() is called, the connecting callback will
   * be invoked on the next turn of the event loop, so it is normally not useful
   * to implement this callback, since the application's UI state can be updated
   * straight away.
   *
   * If however the viewer creates connections using a Cloud listener, then the
   * connecting callback is essential to implement.  This is because each viewer
   * object can only handle a single connection at a time, so it makes no sense
   * to continue listening once the viewer has started handling a connection.
   * If a second connection comes in, the viewer will not be able to accept it
   * and an error will be logged.  To stop listening, the Cloud listener should
   * be destroyed from the connecting callback.
   */
  _Callback_default_(void) void
  (*connecting)(void* userData,
                vnc_Viewer* viewer);

  /**
   * Notification that is called when the viewer successfully connects to a
   * server (after authentication).  This callback is optional.
   */
  _Callback_default_(void) void
  (*connected)(void* userData,
               vnc_Viewer* viewer);

  /**
   * Notification that is called when the viewer disconnects from the server.
   * This callback is optional.
   *
   * @param reason See vnc_Viewer_getDisconnectReason() for possible
   *     disconnection reasons.
   * @param flags Zero or more of the @ref vnc_Viewer_DisconnectFlags flags.
   */
  _Callback_default_(void) void
  (*disconnected)(void* userData,
                  vnc_Viewer* viewer,
                  const char* reason,
                  _Enum_mask_(vnc_Viewer_DisconnectFlags) int flags);
} vnc_Viewer_ConnectionCallback;

/**
 * Sets the callbacks for the Viewer to call when various events occur during
 * its lifetime.
 *
 * @param callback The new connection callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The callback is invalid
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_setConnectionCallback(
    vnc_Viewer* viewer,
    _In_opt_ const vnc_Viewer_ConnectionCallback* callback,
    void* userData);

/**
 * Returns the viewer's @ref vnc_ConnectionHandler for accepting connections.
 * The connection handler must not be used after the viewer has been destroyed;
 * any Cloud listener using this viewer must be destroyed before the viewer
 * itself.
 */
VNC_SDK_API vnc_ConnectionHandler*
vnc_Viewer_getConnectionHandler(vnc_Viewer* viewer);

/**
 * Returns the status of the viewer's connection.  See
 * vnc_Viewer_ConnectionStatus() for possible values.
 */
VNC_SDK_API vnc_Viewer_ConnectionStatus
vnc_Viewer_getConnectionStatus(vnc_Viewer* viewer);

/**
 * Returns the address of the viewer's server.  The format of the string is
 * <tt>[vnccloud:\<cloud-address\>]</tt> for VNC Cloud addresses.  The string is
 * only guaranteed to be valid until the next call to any SDK function.
 *
 * @param viewer The Viewer.
 *
 * @return The address as a NULL-terminated string, or NULL in the case of an
 *     error, in which case call vnc_getLastError() to get the error code.
 *
 * @retval UnexpectedCall
 *     The viewer is not connected
 */
VNC_SDK_API const char* vnc_Viewer_getPeerAddress(vnc_Viewer* viewer);

/**
 * Disconnects this viewer from the server.  Sets the disconnection reason to
 * @c ViewerClosedConnection.  This method does nothing if the viewer is not
 * currently connected.  Once disconnected you may subsequently connect the
 * viewer to the same server again.
 *
 * @param viewer The Viewer.
 *
 * @return #vnc_success.
 */
VNC_SDK_API vnc_status_t vnc_Viewer_disconnect(vnc_Viewer* viewer);

/**
 * Returns a string ID representing the reason for the last viewer
 * disconnection.  Returns NULL if there have not been any disconnections, or
 * the viewer has been connected again since the last disconnection.
 *
 * Some of the connection reasons are sent by the server to explain the reason
 * for ending the connection.  These disconnection reasons are free-form and a
 * third-party server may send reasons not on this list.  To help diagnosing the
 * cause of the disconnection, vnc_Viewer_getDisconnectMessage() can be used to
 * get a human-readable message sent by the server which explains the reason.
 * Possible reasons sent by the server are:
 *
 *  - @c AuthFailure: the viewer user did not provide appropriate credentials
 *  - @c ConnTimeout: the connection reached its maximum duration
 *  - @c IdleTimeout: the connection timed out after no activity.  This can
 *    happen when the connection is being established if the requested
 *    credentials are not sent within a reasonable amount of time.
 *  - @c ServerShutdown: the server process has shut down (the disconnection
 *    message may include further details)
 *  - @c ServerSuspended: the server machine is going in suspend mode
 *  - @c TooManySecFail: too many security failures have occurred in a short
 *    space of time, so the server has rejected the connection
 *  - @c UserSwitch: the server machine is switching desktops, causing a
 *    temporary disconnection.
 *
 * In addition, @c ServerClosedConnection indicates a normal disconnection
 * initiated by the server.
 *
 * Some of the reasons occur when the viewer initiates the disconnection, in
 * which case the list of possible disconnection reasons is known in advance.
 * Possible reasons known to the viewer are:
 *
 *  - @c CloudAuthenticationFailed: the Cloud password or address is incorrect
 *  - @c CloudConnectionRefused: the local Cloud address does not have
 *    permission to connect to the server's Cloud address
 *  - @c CloudTargetNotFound: there is no server listening on the Cloud address
 *  - @c DataStoreError: there is no data store
 *  - @c ViewerCancelledAuthentication: authentication was cancelled by calling
 *    vnc_Viewer_sendAuthenticationResponse() with @c ok as #vnc_false
 *  - @c ViewerClosedConnection: the connection was closed by calling
 *    vnc_Viewer_disconnect()
 *  - @c ViewerRejectedConnection: the connection was rejected by the viewer not
 *    accepting the connection security or peer identity via the
 *    vnc_Viewer_PeerVerificationCallback::verifyPeer callback.
 *
 * Finally, some of the reasons can occur both from the viewer or server, or
 * somewhere in between! Possible reasons are:
 *
 *  - @c ConnectivityError: a connection to the server could not be established,
 *    or there was an error reading or writing to the peer
 *  - @c ConnectionDropped: the connection was closed at the transport level,
 *    with no closing handshake
 *  - @c ConnectionNegotiationError: the connection could not be established due
 *    to an error during negotiation with the peer; consult the debug logs for
 *    more information.
 *
 * If cause is not recognized, the reason will be @c Unknown; please consult the
 * debug logs for more information on the underlying cause of the disconnection.
 */
VNC_SDK_API _Ret_opt_z_ const char*
vnc_Viewer_getDisconnectReason(vnc_Viewer* viewer);

/**
 * Returns a human-readable message sent by the server for the last
 * disconnection, or NULL if the last disconnection was not initiated by the
 * server.  If the reason for the disconnection originated at the viewer end
 * then there is no message, since the reason string comes from a known set of
 * possibilities.
 */
VNC_SDK_API _Ret_opt_z_ const char*
vnc_Viewer_getDisconnectMessage(vnc_Viewer* viewer);


/**
 * @}
 * @name Functions for managing the framebuffer
 * @{
 */

/** Callback receiving framebuffer event notifications for a Viewer. */
typedef struct {
  /**
   * Notification that the server's framebuffer size has changed.  You may
   * choose to resize the viewer framebuffer by calling vnc_Viewer_setViewerFb()
   * from this function, for example, to maintain a consistent aspect ratio.
   * This callback is optional.
   *
   * @param w The width of the server's framebuffer.
   * @param h The height of the server's framebuffer.
   */
  _Callback_default_(void) void
  (*serverFbSizeChanged)(void* userData,
                         vnc_Viewer* viewer,
                         int w, int h);

  /**
   * Notification that the given rectangle of the viewer's framebuffer has been
   * updated with new pixel data.  This callback is optional.
   */
  _Callback_default_(void) void
  (*viewerFbUpdated)(void* userData,
                     vnc_Viewer* viewer,
                     int x, int y, int w, int h);
} vnc_Viewer_FramebufferCallback;

/**
 * Sets the framebuffer callback for this viewer.
 *
 * @param viewer The Viewer.
 * @param callback The new framebuffer callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The callback is invalid
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_setFramebufferCallback(
    vnc_Viewer* viewer,
    _In_opt_ const vnc_Viewer_FramebufferCallback* callback,
    void* userData);

/**
 * Sets the viewer framebuffer.  The pixel data received from the server will
 * be rendered into the buffer in the given pixel format, scaled to fit the
 * given size, and with the given stride.
 *
 * @param viewer The Viewer.
 * @param pixels Pointer to the framebuffer to receive pixel data.  The buffer
 *     must be large enough, i.e.  stride * height pixels.  Alternatively, pass
 *     NULL for a buffer owned by the SDK to be allocated for you.
 * @param pixelsBytes The size in bytes of the buffer pointed to by @p pixels
 *     (ignored if @p pixels is NULL).
 * @param pf Pixel format for the pixel data.
 * @param width The width of the framebuffer.
 * @param height The height of the framebuffer
 * @param stride The stride of the framebuffer, i.e.  the number of pixels from
 *     the start of one row to the next.  If specified as zero then the width is
 *     used.  Note that this is specified in terms of pixels, not bytes.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p width or @p height is zero or negative, the stride is invalid, or the
 *     @p pixels buffer is not large enough
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_setViewerFb(vnc_Viewer* viewer,
                       _Out_writes_bytes_(pixelsBytes) void* pixels,
                       int pixelsBytes,
                       const vnc_PixelFormat* pf,
                       int width, int height,
                       int stride);

/** Gets the width of the viewer framebuffer. */
VNC_SDK_API int vnc_Viewer_getViewerFbWidth(vnc_Viewer* viewer);

/** Gets the height of the viewer framebuffer. */
VNC_SDK_API int vnc_Viewer_getViewerFbHeight(vnc_Viewer* viewer);

/** Gets the pixel format of the viewer framebuffer. */
VNC_SDK_API const vnc_PixelFormat*
vnc_Viewer_getViewerFbPixelFormat(vnc_Viewer* viewer);

/**
 * Returns the viewer framebuffer data for the given rectangle.  The returned
 * object is valid until the next call to vnc_Viewer_getViewerFbData() or
 * vnc_Viewer_setViewerFb().  The stride of the data is given by
 * vnc_Viewer_getViewerFbStride().
 *
 * @param viewer The Viewer.
 * @param x The left edge of the rectangle.
 * @param y The top edge of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 *
 * @return Returns the data buffer, or NULL on error, in which case call
 *     vnc_getLastError() to get the error code.
 *
 * @retval InvalidArgument
 *     @c w or @c h are zero or negative, or the rectangle is not fully
 *     contained within the framebuffer
 */
VNC_SDK_API const vnc_DataBuffer*
vnc_Viewer_getViewerFbData(vnc_Viewer* viewer,
                           int x, int y, int w, int h);

/**
 * Returns the stride of the viewer framebuffer data in pixels, that is, the
 * number of pixels from the start of each row until the start of the next.
 *
 * @param viewer The Viewer.
 *
 * @return The stride in pixels.
 */
VNC_SDK_API int vnc_Viewer_getViewerFbStride(vnc_Viewer* viewer);


/**
 * @}
 * @name Functions for managing server events
 * @{
 */


/** Callback receiving server event notifications for a Viewer. */
typedef struct {
  /**
   * Notification that the server's clipboard contains new text.  This
   * callback is optional.
   *
   * @param text The UTF-8 encoded text.
   */
  _Callback_default_(void) void
  (*serverClipboardTextChanged)(void* userData,
                                vnc_Viewer* viewer,
                                const char* text);

  /**
   * Notification that the server's friendly name has changed.  This callback is
   * optional.
   *
   * @param name The UTF-8 encoded name.
   */
  _Callback_default_(void) void
  (*serverFriendlyNameChanged)(void* userData,
                               vnc_Viewer* viewer,
                               const char* name);
} vnc_Viewer_ServerEventCallback;

/**
 * Sets the server event callback for this viewer.
 *
 * @param viewer The Viewer.
 * @param callback The new server event callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The callback is invalid
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_setServerEventCallback(
    vnc_Viewer* viewer,
    _In_opt_ const vnc_Viewer_ServerEventCallback* callback,
    void* userData);


/**
 * @}
 * @name Functions for sending input
 * @{
 */

/**
 * Copies the given text to the server's clipboard.
 *
 * @param viewer The Viewer.
 * @param text The UTF-8 encoded text.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p text is NULL
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_sendClipboardText(vnc_Viewer* viewer,
                             const char* text);

/**
 * Enumeration of mouse buttons.  Here "left" and "right" refer to logical
 * buttons, which may not correspond to the physical orientation of the buttons
 * if using a left-handed mouse.
 */
typedef enum {
  /** The logical left mouse button. */
  vnc_Viewer_MouseButtonLeft = 1,
  /** The logical middle mouse button. */
  vnc_Viewer_MouseButtonMiddle = 2,
  /** The logical right mouse button. */
  vnc_Viewer_MouseButtonRight = 4,
} vnc_Viewer_MouseButton;

/**
 * Sends a pointer event to the server.  If @p rel is #vnc_false then @p x and
 * @p y give the absolute position of the mouse pointer in viewer framebuffer
 * coordinates with 0,0 at the top-left of the framebuffer.  If @p rel is
 * #vnc_true then @p x and @p y give the relative movement delta in "mickeys".
 *
 * @param viewer The Viewer.
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @param buttonState A bitmask containing zero or more of
 *     @ref vnc_Viewer_MouseButtonLeft, @ref vnc_Viewer_MouseButtonMiddle and
 *     @ref vnc_Viewer_MouseButtonRight, indicating which buttons are down, if
 *     any.
 * @param rel Whether the event is relative or absolute.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p buttonState is invalid
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_sendPointerEvent(vnc_Viewer* viewer, int x, int y,
                            _Enum_mask_(vnc_Viewer_MouseButton) int buttonState,
                            vnc_bool_t rel);

/** Enumeration of mouse wheel directions. */
typedef enum {
  /** The horizontal scroll wheel. */
  vnc_Viewer_MouseWheelHorizontal = 1,
  /** The vertical scroll wheel. */
  vnc_Viewer_MouseWheelVertical = 2,
} vnc_Viewer_MouseWheel;

/**
 * Sends a scroll wheel event to the server.
 *
 * @param viewer The Viewer.
 * @param delta The number of scroll wheel "ticks".  You may need to reduce the
 *     value provided by a high-resolution scroll device to get appropriate
 *     behavior (for example, one tick is equivalent to a distance of 120 in the
 *     Win32 API).  If @p delta is negative then the movement is upwards or
 *     towards the left, if positive then the movement is downwards or towards
 *     the right.
 * @param axis The scroll axis (either @ref vnc_Viewer_MouseWheelHorizontal or
 *     @ref vnc_Viewer_MouseWheelVertical).
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p axis is invalid
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_sendScrollEvent(vnc_Viewer* viewer,
                           int delta,
                           vnc_Viewer_MouseWheel axis);

/**
 * Sends a key down (press) event to the server.
 *
 * @param viewer The Viewer.
 * @param keysym The keysym value representing the meaning of the pressed key.
 *   Use vnc_unicodeToKeysym() to convert a unicode character to a keysym.  See
 *   vnc/Keyboard.h for common keysym values for non-character keys.
 * @param keyCode An integer code representing the physical key which has been
 *   pressed.  The SDK only uses this to match up the corresponding call to
 *   vnc_Viewer_sendKeyUp().
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_sendKeyDown(vnc_Viewer* viewer,
                       vnc_uint32_t keysym,
                       int keyCode);

/**
 * Sends a key up (release) event to the server.
 *
 * @param viewer The Viewer.
 * @param keyCode An integer code representing the physical key which has been
 *   released.  If there was no prior call to vnc_Viewer_sendKeyDown() for this
 *   @p keyCode, it is simply ignored.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 */
VNC_SDK_API vnc_status_t vnc_Viewer_sendKeyUp(vnc_Viewer* viewer,
                                              int keyCode);

/**
 * Send key up events for all currently pressed keys.  Typically you call this
 * when the viewer loses keyboard focus.
 *
 * @param viewer The Viewer.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 */
VNC_SDK_API vnc_status_t vnc_Viewer_releaseAllKeys(vnc_Viewer* viewer);


/**
 * @}
 * @name Functions for managing connection negotiation
 * @{
 */

/** Callback receiving requests for authentication for a Viewer. */
typedef struct {
  /**
   * Request for a username and/or password.  Implementors should reply with a
   * username and/or password either immediately or later, by calling
   * vnc_Viewer_sendAuthenticationResponse().  This callback function is
   * required if using a @ref vnc_Viewer_AuthenticationCallback.
   *
   * @param needUser Whether a username must be supplied.
   * @param needPasswd Whether a password must be supplied.
   */
  void (*requestUserCredentials)(void* userData,
                                 vnc_Viewer* viewer,
                                 vnc_bool_t needUser,
                                 vnc_bool_t needPasswd);

  /**
   * Notification to cancel a username and/or password request.  This can happen
   * if the server closes the connection while the Viewer is waiting for the
   * user to provide a username/password.  This callback is optional.
   */
  _Callback_default_(void) void
  (*cancelUserCredentialsRequest)(void* userData,
                                  vnc_Viewer* viewer);
} vnc_Viewer_AuthenticationCallback;

/**
 * Sets the callback to be called when a username and/or password is required.
 * If these are not provided then UI will be rendered into the framebuffer
 * asking the user to provide the required information.
 *
 * @param viewer The Viewer.
 * @param callback The new authentication callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The vnc_Viewer_AuthenticationCallback::requestUserCredentials callback is
 *     NULL
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_setAuthenticationCallback(
    vnc_Viewer* viewer,
    _In_opt_ const vnc_Viewer_AuthenticationCallback* callback,
    void* userData);

/**
 * Provides the SDK with the result of a username/password request.
 *
 * @param viewer The Viewer.
 * @param ok This should be #vnc_false if the user cancelled the password
 *     dialog.
 * @param user This must not be NULL if @p needUser was @p vnc_true in the
 *     username/password request, otherwise it is ignored.
 * @param passwd This must not be NULL if @p needPasswd was @p vnc_true in the
 *     username/password request, otherwise it is ignored.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval UnexpectedCall
 *     The session is not waiting for a username/password response
 *
 * @retval InvalidArgument
 *     @c user or @c passwd was requested but is NULL
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_sendAuthenticationResponse(vnc_Viewer* viewer,
                                      vnc_bool_t ok,
                                      _In_opt_z_ const char* user,
                                      _In_opt_z_ const char* passwd);

/** Callback receiving verification requests to check the peer's identity. */
typedef struct {
  /**
   * Request to verify the identity of the peer (the server).  Implementors
   * should respond by calling vnc_Viewer_sendPeerVerificationResponse().  This
   * callback function is required if using a
   * @ref vnc_Viewer_PeerVerificationCallback.
   *
   * Apart from cancelPeerVerification, no further callbacks (such as
   * disconnected) will be issued for this connection until peer verification
   * has been completed succesfully.
   *
   * @param hexFingerprint The hash of the server's fingerprint, as a string of
   *     hexadecimal digits.
   * @param catchphraseFingerprint The hash of the server's fingerprint, as a
   *     catchphrase.
   * @param serverRsaPublic The server's public key, in the format returned by
   *     vnc_RsaKey_getDetails().
   */
  void
  (*verifyPeer)(void* userData,
                vnc_Viewer* viewer,
                const char* hexFingerprint,
                const char* catchphraseFingerprint,
                const vnc_DataBuffer* serverRsaPublic);

  /**
   * Notification to cancel a prior request for peer verification.  This can
   * happen if the server closes the connection while peer verification is in
   * progress.  This callback is optional.
   */
  _Callback_default_(void) void
  (*cancelPeerVerification)(void* userData,
                            vnc_Viewer* viewer);
} vnc_Viewer_PeerVerificationCallback;

/**
 * Sets the callbacks to be called to verify the identity of the peer (server).
 * If these are not provided, the connection will be accepted.
 *
 * Note that VNC Cloud mutually verifies peer identities for you, so this check
 * is for peace-of-mind only (and only likely to be useful if you have
 * out-of-band access to a list of all potential Server identities).
 *
 * @param viewer The Viewer.
 * @param callback The new peer verification callback.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The vnc_Viewer_PeerVerificationCallback::verifyPeer callback is NULL
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_setPeerVerificationCallback(
    vnc_Viewer* viewer,
    _In_opt_ const vnc_Viewer_PeerVerificationCallback* callback,
    void* userData);

/**
 * Provides the SDK with the response to the
 * vnc_Viewer_PeerVerificationCallback::verifyPeer request.
 *
 * @param viewer The Viewer.
 * @param ok #vnc_true if the connection is approved, #vnc_false if it should
 *    be rejected.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval UnexpectedCall
 *     No peer verification is in progress
 */
VNC_SDK_API vnc_status_t
vnc_Viewer_sendPeerVerificationResponse(vnc_Viewer* viewer,
                                        vnc_bool_t ok);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
