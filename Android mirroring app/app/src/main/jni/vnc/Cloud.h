/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_CLOUD_H__
#define __VNC_CLOUD_H__

#include "Common.h"

/** Listener used to join VNC Cloud and listen for a connection. */
typedef struct vnc_CloudListener vnc_CloudListener;
/** Connector used to join VNC Cloud and establish an outgoing connection. */
typedef struct vnc_CloudConnector vnc_CloudConnector;
/** Monitor used to query the availability of VNC Cloud addresses. */
typedef struct vnc_CloudAddressMonitor vnc_CloudAddressMonitor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * Listens for or establishes connections using VNC Cloud.
 */

/**
 * Enumeration of listening statuses.
 */
typedef enum {
  /**
   * The listener is in the process of establishing an association with VNC
   * Cloud.  Incoming connections are not yet possible.
   */
  vnc_CloudListener_StatusSearching,
  /** The listener is available for incoming connections. */
  vnc_CloudListener_StatusOnline,
} vnc_CloudListener_Status;

/** 
 * Callback for a @ref vnc_CloudListener. 
 * 
 * @see vnc_CloudListener_create()
 */
typedef struct {
  /**
   * Notification that the Cloud listener has stopped listening and is unable
   * to accept any more connections.  This callback is required.
   *
   * The Cloud listener internally handles errors due to transient networking
   * conditions, but if a connection to VNC Cloud cannot be promptly
   * re-established, the Cloud listener stops, leaving the application to choose
   * whether to retry or to present an error message to the user.  In a
   * long-running application where the user may not be present, it is normal to
   * retry wherever possible; in a short-lived "helpdesk" application it may be
   * desirable not to retry at all when there are errors but simply present a
   * message to the user.
   *
   * Some errors are unrecoverable, such as invalid Cloud credentials, and in
   * this case the user must be notified rather than creating a new Cloud
   * listener using the same credentials.  In this case, @p retryTimeSecs is
   * -1.
   *
   * For recoverable errors (such as a lack of network connectivity), is is
   * possible to retry listening.  This is done by destroying the Cloud
   * listener, then creating a new one after a delay.  The delay should not be
   * too short, or excessive bandwidth and needless load on the VNC Cloud will
   * be generated.  The @p retryTimeSecs parameter indicates a safe delay to
   * use, starting at five seconds and gradually increasing to a maximum
   * with each failure.  Do not retry more rapidly than indicated by
   * @p retryTimeSecs, or service may be impaired for other users of VNC Cloud
   * at times of peak load.
   *
   * @param cloudError A specific error string describing the cause.  Possible
   *     errors are: @c CloudAuthenticationFailed, @c CloudConnectivityError.
   * @param retryTimeSecs The time in seconds to wait before attempting to
   *     listen again using this Cloud address, or -1 if the error is fatal
   *     and automatic recovery after a delay is not possible.
   */
  void
  (*listeningFailed)(void* userData,
                     vnc_CloudListener* listener,
                     const char* cloudError,
                     int retryTimeSecs);

  /**
   * Notification to provide address-based filtering of incoming connections.
   * This callback is optional.
   *
   * @param peerCloudAddress The address of the peer requesting a connection.
   *
   * @return #vnc_true to allow the connection, or #vnc_false to deny, in
   *     which case the connection will be closed.
   */
  _Callback_default_(true) vnc_bool_t
  (*filterConnection)(void* userData,
                      vnc_CloudListener* listener,
                      const char* peerCloudAddress);

  /**
   * Notification that the listener status has changed.  This callback is
   * optional.
   *
   * After creating a listener, it is initially unable to receive connections
   * until it has successfully checked-in with VNC Cloud.  If network
   * connectivity is temporarily lost, the status can also change from
   * @ref vnc_CloudListener_StatusOnline back to
   * @ref vnc_CloudListener_StatusSearching.
   *
   * @param status The new status of the Cloud listener.
   */
  _Callback_default_(void) void
  (*listeningStatusChanged)(void* userData,
                            vnc_CloudListener* listener,
                            vnc_CloudListener_Status status);
} vnc_CloudListener_Callback;

/**
 * Begins listening for incoming connections on the given Cloud address.  To
 * stop listening, destroy the Cloud listener.
 *
 * The Cloud listener's status is initially
 * @ref vnc_CloudListener_StatusSearching while it is locating a nearby Cloud
 * server to use, then changes to @ref vnc_CloudListener_StatusOnline once it is
 * ready to receive connections; when this happens the
 * vnc_CloudListener_Callback::listeningStatusChanged function is called.  It is
 * also possible for the listener to briefly switch back to
 * @ref vnc_CloudListener_StatusSearching during normal operation.
 *
 * If an error occurs, then the listener reaches a terminal error state; when
 * this happens, the vnc_CloudListener_Callback::listeningFailed function
 * is called and the vnc_CloudListener_Callback::listeningStatusChanged function
 * is not notified.  The only thing that can be done after an error is to
 * destroy the listener and create a new one after a delay.
 *
 * @param localCloudAddress The cloud address on which to listen for incoming
 *     connections (typically a string with four components of the form
 *     <tt>XXX.XXX.XXX.XXX</tt>).
 * @param localCloudPassword The password associated with @p localCloudAddress
 *     (typically a short alphanumeric string).
 * @param connectionHandler The object which shall handle incoming connections
 *     (if not rejected by the filter function).  It is either a server or a
 *     viewer.  It must be destroyed after the Cloud listener.
 * @param callback The callback is required, in order to receive notification of
 *     errors.
 *
 * @return Returns a new @ref vnc_CloudListener on success, or NULL on error.
 *     If an error is returned, vnc_getLastError() can be used to get the error
 *     code.
 *
 * @retval InvalidArgument
 *     The given Cloud address and password are malformed
 */
VNC_SDK_API vnc_CloudListener*
vnc_CloudListener_create(const char* localCloudAddress,
                         const char* localCloudPassword,
                         vnc_ConnectionHandler* connectionHandler,
                         const vnc_CloudListener_Callback* callback,
                         void* userData);

/**
 * Destroys the Cloud listener.  Any ongoing connections are not affected, but
 * new connections will not be accepted.
 *
 * @param listener The listener.
 */
VNC_SDK_API void
vnc_CloudListener_destroy(vnc_CloudListener* listener);

/**
 * Creates a connector, which is used used to create connections to Cloud
 * addresses.
 *
 * @param localCloudAddress The cloud address representing our end for outgoing
 *     connections (typically a string with four components of the form
 *     <tt>XXX.XXX.XXX.XXX</tt>).
 * @param localCloudPassword The password associated with @p localCloudAddress
 *     (typically a short alphanumeric string).
 *
 * @return Returns a new @ref vnc_CloudConnector on success, or NULL on error.
 *     If an error is returned, vnc_getLastError() can be used to get the error
 *     code.
 *
 * @retval InvalidArgument
 *     The given Cloud address or password is malformed
 */
VNC_SDK_API vnc_CloudConnector*
vnc_CloudConnector_create(const char* localCloudAddress,
                          const char* localCloudPassword);

/**
 * Destroys the Cloud connector.
 *
 * @param connector The connector.
 */
VNC_SDK_API void
vnc_CloudConnector_destroy(vnc_CloudConnector* connector);

/**
 * Begins an outgoing connection to the given Cloud address.  The connection
 * will be handled by the @p connectionHandler.
 *
 * The Cloud connector can be used to create connections to any number of
 * different peer addresses, without having to wait for earlier connections to
 * finish (note however that a viewer cannot be used as a connection handler if
 * it is already connected).  Two simultaneous connections to the same peer
 * Cloud address are not supported.  The connector can be destroyed immediately
 * after creating a connection, without affecting the connection: the connection
 * handler immediately takes ownership of it.
 *
 * Connection errors are notified using the server or viewer's callbacks
 * (depending on the type of the connection handler).
 *
 * @param connector The connector.
 * @param peerCloudAddress The cloud address to make a connection to.
 * @param connectionHandler The object which shall handle the outgoing
 *     connection.  It is either a server or a viewer.  Connection errors are
 *     notified via the connection handler's callbacks.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     The given Cloud address or password is malformed
 * @retval UnexpectedCall
 *     The connection handler is already connected
 */
VNC_SDK_API vnc_status_t
vnc_CloudConnector_connect(vnc_CloudConnector* connector,
                           const char* peerCloudAddress,
                           vnc_ConnectionHandler* connectionHandler);

/**
 * Sets whether new connections created by the connector wait for the peer to
 * start listening.  The default behaviour is for connections to fail if the
 * peer is not currently listening.
 *
 * @param connector The connector.
 * @param waitForPeer If set to #vnc_true, then the connection attempt will
 *     wait for the peer to create a Cloud listener.  The application must
 *     decide how long to wait, and cancel the connection attempt after a
 *     suitable timeout by destroying the viewer.
 */
VNC_SDK_API void
vnc_CloudConnector_setWaitForPeer(vnc_CloudConnector* connector,
                                  vnc_bool_t waitForPeer);

/**
 * Set the bandwidth limit applied to relayed Cloud connections.
 *
 * Connections made via VNC Cloud are either @em direct or @em relayed.  A
 * direct connection will be made where possible, but if one of the peers is
 * behind a restrictive NAT or firewall, then VNC Cloud will be used to relay
 * the data.  In this case, a bandwidth limit may be set to control use of the
 * relay.
 *
 * After calling vnc_CloudConnector_create(), the connector's initial bandwidth
 * limit is unlimited.  If a new limit is set on the connector, it will be
 * applied to all subsequent connections started using
 * vnc_CloudConnector_connect().
 *
 * @param relayBandwidthLimit The limit in bytes per second, or -1 for
 *     unlimited.  The allowed values are in the range 1024 to 999999999.
 *
 * @return @c vnc_success is returned on success or @c vnc_failure in the case
 *     of an error, in which case vnc_getLastError() can be used to get the
 *     error code.
 *
 * @retval InvalidArgument
 *     The bandwidth limit is out of range
 */
VNC_SDK_API vnc_status_t
vnc_CloudConnector_setRelayBandwidthLimit(vnc_CloudConnector* connector,
                                          int relayBandwidthLimit);

/** Enumeration of availabilities for a Cloud address. */
typedef enum {
  /** A peer is currently listening on the Cloud address. */
  vnc_CloudAddressMonitor_Available,
  /** No peer is currently listening on the Cloud address. */
  vnc_CloudAddressMonitor_Unavailable,
  /** The Cloud address monitor is waiting for the Cloud services to return the
   *  availability for the peer address, or an error occurred. */
  vnc_CloudAddressMonitor_UnknownAvailability,
} vnc_CloudAddressMonitor_Availability;


/** 
 * Callback for a @ref vnc_CloudAddressMonitor. 
 * 
 * @see vnc_CloudAddressMonitor_create()
 */
typedef struct {
  /**
   * Notification that the availability information for a Cloud address has
   * changed.  This callback is required.
   *
   * @param cloudAddress The address whose availability has changed.
   * @param availability The new value for the availability.
   */
  void
  (*availabilityChanged)(void* userData,
                         vnc_CloudAddressMonitor* monitor,
                         const char* cloudAddress,
                         vnc_CloudAddressMonitor_Availability availability);

  /**
   * Notification that monitoring has been paused by an outgoing connection
   * starting.  This callback is optional.
   *
   * The monitor should not normally need to be restarted -- in particular, if
   * availability information is not being displayed to the user the monitor
   * should not be resumed.
   */
  _Callback_default_(void) void
  (*monitorPaused)(void* userData,
                   vnc_CloudAddressMonitor* monitor);
} vnc_CloudAddressMonitor_Callback;

/**
 * Creates a monitor, which queries whether a list of cloud addresses is
 * available.  (An address is "available" if there is a Cloud listener currently
 * accepting connections on that address.)  The monitor will notify its
 * callback whenever the availability information for an address is updated.
 *
 * Each Cloud connector can have at most one associated monitor at one time.
 *
 * @param connector The connector used by the monitor for queries to VNC Cloud.
 * @param cloudAddresses An array of cloud addresses to be monitored.
 * @param nAddresses The length of the @p cloudAddresses array.
 * @param callback The required callback for receiving availability information.
 *
 * @return Returns a new @ref vnc_CloudAddressMonitor on success, or NULL on
 *     error.  If an error is returned, vnc_getLastError() can be used to get
 *     the error code.
 *
 * @retval UnexpectedCall
 *     The given Cloud connector already has a monitor
 * @retval InvalidArgument
 *     One of the given Cloud addresses is malformed
 */
VNC_SDK_API vnc_CloudAddressMonitor*
vnc_CloudAddressMonitor_create(
    vnc_CloudConnector* connector,
    _In_reads_(nAddresses) const char** cloudAddresses,
    int nAddresses,
    const vnc_CloudAddressMonitor_Callback* callback,
    void* userData);

/**
 * Destroys the Cloud monitor.
 *
 * @param monitor The monitor.
 */
VNC_SDK_API void
vnc_CloudAddressMonitor_destroy(vnc_CloudAddressMonitor* monitor);

/**
 * Pauses the Cloud monitor.  While paused, updates will not be fetched for
 * the peer addresses, however, the callback may still be notified of changes in
 * availability for a short while after pausing.  If a list of cloud addresses
 * is being shown with their availabilities, it would be appropriate to pause
 * the monitor while the window is not focused, for example.
 *
 * @param monitor The monitor.
 */
VNC_SDK_API void
vnc_CloudAddressMonitor_pause(vnc_CloudAddressMonitor* monitor);

/**
 * Resumes the Cloud monitor.
 *
 * @param monitor The monitor.
 */
VNC_SDK_API void
vnc_CloudAddressMonitor_resume(vnc_CloudAddressMonitor* monitor);

/**
 * Sets whether or not the Cloud monitor pauses automatically when a connection
 * is established.  Normally, a Cloud monitor does not need to run during a
 * connection, so the default behaviour is to reduce bandwidth by pausing the
 * monitor when a connection is established.  If @p pauseOnConnect is set to
 * #vnc_false, the user must be careful to pause the monitor when possible,
 * rather than continuously polling the network.
 *
 * @param monitor The monitor.
 * @param pauseOnConnect Whether to pause when a new incoming connection is
 *     established.
 */
VNC_SDK_API void
vnc_CloudAddressMonitor_setPauseOnConnect(vnc_CloudAddressMonitor* monitor,
                                          vnc_bool_t pauseOnConnect);

#ifdef __cplusplus
}
#endif

#endif
