/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_DATASTORE_H__
#define __VNC_DATASTORE_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Creates a persistent data store.
 *
 * For both a Viewer and a Server, you must create a data store in which the SDK
 * can store an automatically-generated RSA public/private key pair and
 * retrieve it on demand.  The RSA key is required for encryption and identity
 * verification, and connections cannot be established without it.  Note the SDK
 * also stores other data, for example VNC Cloud discovery information.
 *
 * You can either call an appropriate method to let the SDK create and
 * manage a suitable data store for you, or else call
 * vnc_DataStore_createCustomStore() to create your own store in which you 
 * put and get data when notified by the SDK.
 *
 * Only one data store can exist in your Viewer or Server at a time.  If you
 * create a new data store, an existing data store is automatically replaced.
 *
 * Any SDK functions that read or write to the global data store will produce
 * a @c DataStoreError while no data store exists.
 */

/**
 * Callback providing custom storage of data used by the SDK.
 *
 * @see vnc_DataStore_createCustomStore().
 */
typedef struct {
  /**
   * Requests storing data in your custom data store.  This callback is
   * required for a custom data store.
   *
   * In your implementation, associate the data contained in @p value with the
   * given key.  If a value with the same key exists already, overwrite it.  If
   * @p value is NULL, remove any existing value stored for the key.  Note the
   * @ref vnc_DataBuffer is destroyed when the callback function returns, so
   * copy data out of the buffer rather than storing the buffer itself.
   *
   * @param key A NULL-terminated string.
   * @param value A @ref vnc_DataBuffer to store, or NULL to remove an existing
   *     value with the same key from your custom data store.
   */
  void
  (*put)(void* userData,
         const char* key,
         const vnc_DataBuffer* value);

  /**
   * Requests data stored in your custom data store.  This callback is required
   * for a custom data store.
   *
   * In your implementation, you should retrieve the value associated with
   * @p key and return it as a @ref vnc_DataBuffer, or else NULL if the key
   * could not be found.
   *
   * @param key A NULL-terminated string.
   *
   * @return A @ref vnc_DataBuffer.
   */
  _Ret_opt_ vnc_DataBuffer*
  (*get)(void* userData,
         const char* key);
} vnc_DataStore_Callback;

/**
 * Creates a custom data store.  You are responsible for storing and retrieving
 * data in the correct format when notified by the SDK.
 *
 * @param callback A @ref vnc_DataStore_Callback structure.
 * @param userData Custom data to pass through to callback function
 *     implementations.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *      One or more mandatory callbacks in @p callback is @c NULL.
 */
VNC_SDK_API vnc_status_t
vnc_DataStore_createCustomStore(const vnc_DataStore_Callback* callback,
                                void* userData);

/**
 * Creates a file data store.
 *
 * The SDK manages storing and retrieving data for you.  Note this type of
 * store is not available on the HTML5 platform.
 *
 * Note that the data store contains sensitive information such as the RSA
 * private key, and therefore it must not be readable by any potential attacker
 * on the system.  For this reason it is essential that the parent
 * directory/folder of the given path has been created with appropriate
 * permissions which do not allow any access to other users on the system.
 *
 * @param path A full path to and file name for the data store.  You can specify
 *     any file extension; a binary file will be created.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p path is invalid.
 * @retval DataStoreError
 *     The file could not be opened or created.
 */
VNC_SDK_API _Platform_(!HTML5) vnc_status_t
vnc_DataStore_createFileStore(const char* path);

/**
 * Creates a registry data store.
 *
 * The SDK manages storing and retrieving data for you.  Note this type of
 * store is only available on Windows.
 *
 * Note that the data store contains sensitive information such as the RSA
 * private key, and therefore it must not be readable by any potential attacker
 * on the system.  For this reason it is essential that the registry key have 
 * appropriate permissions which do not allow any access to other users on the
 * system.
 *
 * @param registryPath A suitable path to and name for a registry key, for
 *     example @c HKEY_LOCAL_USER\\SOFTWARE\\Company\\Application\\MyKey.
 *
 * @return #vnc_success or #vnc_failure, in which case call vnc_getLastError()
 *     to get the error code.
 *
 * @retval InvalidArgument
 *     @p registryPath is invalid.
 * @retval DataStoreError
 *     The registry key could not be opened.
 */
VNC_SDK_API _Platform_(Windows) vnc_status_t
vnc_DataStore_createRegistryStore(const char* registryPath);

/**
 * Creates a web browser data store, in DOM @c localStorage.
 *
 * The SDK manages storing and retrieving data for you.  Note this type of
 * store is only available on the HTML5 platform.
 *
 * @param prefix A prefix prepending to the @c localStorage key names, so that
 *     the SDK's keys do not conflict with other libraries.
 */
VNC_SDK_API _Platform_(HTML5) void
vnc_DataStore_createBrowserStore(const char* prefix);

/**
 * Destroys the current data store.
 */
VNC_SDK_API void
vnc_DataStore_destroyStore();

#ifdef __cplusplus
}
#endif

#endif
