/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_RSAKEY_H__
#define __VNC_RSAKEY_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file
 * @brief Manage the RSA key used to securely identify this endpoint
 *
 * Each endpoint is identified securely with an RSA key, used for providing end-
 * to-end verification of the peer's identity during connections.  The RSA key
 * is generated automatically when needed by incoming or outgoing connections.
 * The vnc_RsaKey_getDetails() function is used to make an asynchronous request
 * for the fingerprint of the local key, and the results are returned via a
 * vnc_RsaKey_Callback::detailsReady callback.
 */

/** Callback which receives RSA key details. */
typedef struct {
  /**
   * Notification that the SDK's RSA key has been retrieved.  This callback is
   * required for retrieving the RSA key details.
   *
   * @param rsaPublic The RSA public part, encoded using the RFB data format
   *     (N's bit-length as a four-byte big-endian number, followed by N and e
   *     in unsigned big-endian bytes, with e padded to N's byte-length).  This
   *     public part is suitable for hashing with SHA-256 and using for key
   *     verification, if the 64-bit @p hexFingerprint does not meet security
   *     requirements.
   * @param hexFingerprint The SHA-1 hash of the RSA public part, truncated to
   *     64 bits and written as a hexadecimal string.
   * @param catchphraseFingerprint The SHA-1 hash of the RSA public part,
   *     truncated to 64 bits and written using words from a dictionary (this is
   *     a reversible encoding of the @p hexFingerprint string).
   */
  void
  (*detailsReady)(void* userData,
                  const vnc_DataBuffer* rsaPublic,
                  const char* hexFingerprint,
                  const char* catchphraseFingerprint);
} vnc_RsaKey_Callback;

/**
 * Requests the details for the machine's identifying RSA public key.  The
 * results are returned asynchronously via the vnc_RsaKey_Callback::detailsReady
 * callback.
 *
 * The key is read from and stored in the data store.
 *
 * If a callback is already queued with identical @p userData, a second callback
 * will not be queued (that is, the callback will not be invoked twice).
 *
 * @param callback The callback to be invoked when the key details are ready.
 * @param generateIfMissing This will normally be #vnc_true, so that a key is
 *     generated on-demand.  If set to #vnc_false and a key does not yet exist,
 *     a key will not be generated and vnc_RsaKey_getDetails() will return with
 *     a @c NoRsaKeyError error.
 *
 * @return #vnc_success is returned on success, and the callback will be
 *     invoked later.  In the case of an error, #vnc_failure is returned, the
 *     callback not be called, and vnc_getLastError() can be used to get the
 *     error code.
 *
 * @retval NoRsaKeyError
 *     @p generateIfMissing was set to #vnc_false, and no key was found
 *
 * @retval RsaKeyError
 *     The loaded key is invalid
 *
 * @retval DataStoreError
 *     There was an error reading from the data store or there is no data store
 */
VNC_SDK_API vnc_status_t
vnc_RsaKey_getDetails(_In_opt_ const vnc_RsaKey_Callback* callback,
                      void* userData,
                      vnc_bool_t generateIfMissing);

/**
 * Cancels any outstanding notifications for the given callback.  Has no effect
 * if no callbacks with the given @p userData are outstanding.
 *
 * @param userData The callback to be cancelled.
 */
VNC_SDK_API void
vnc_RsaKey_cancelDetails(_Callback_(vnc_RsaKey_Callback) void* userData);

/**
 * Removes any stored RSA key from the data store.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval DataStoreError
 *     There was an error writing to the data store, or there is no data store
 */
VNC_SDK_API vnc_status_t vnc_RsaKey_clear();

#ifdef __cplusplus
}
#endif

#endif
