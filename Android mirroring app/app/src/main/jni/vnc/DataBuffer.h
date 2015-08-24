/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_DATABUFFER_H__
#define __VNC_DATABUFFER_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Access memory owned by the SDK, or pass data to the SDK.
 *
 * Data buffers are used by various SDK functions to encapsulate
 * arbitrarily-sized pieces of data, for example, strings or RSA keys.
 *
 * Given a @ref vnc_DataBuffer, the contained data can be accessed via the
 * vnc_DataBuffer_getData() function, which also gives the size of the stored
 * data.
 */

/**
 * Creates a data buffer containing a copy of the given data.
 *
 * @param data A pointer to untyped data.
 * @param sizeBytes The size of the data.
 *
 * @return Returns a new @ref vnc_DataBuffer on success, or NULL on
 *     error.  If an error is returned, vnc_getLastError() can be used to get
 *     the error code.
 *
 * @retval InvalidArgument
 *     @p sizeBytes is negative, or @p data is NULL and @p sizeBytes is non-zero
 */
VNC_SDK_API vnc_DataBuffer*
vnc_DataBuffer_create(_In_reads_bytes_(sizeBytes) const void* data,
                      int sizeBytes);

/**
 * Gets the data contained in the buffer.
 *
 * @param buffer The buffer.
 * @param bufferSize An optional output parameter used to return the size of the
 *     data in the data buffer.
 *
 * @return A pointer to the data stored in the data buffer, or NULL if the size
 *     of the data buffer is zero.
 */
VNC_SDK_API _Ret_opt_bytecount_(*bufferSize) const void*
vnc_DataBuffer_getData(const vnc_DataBuffer* buffer,
                       _Out_ int* bufferSize);

/**
 * Destroys the data buffer.
 *
 * @param buffer The buffer.
 */
VNC_SDK_API void
vnc_DataBuffer_destroy(vnc_DataBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
