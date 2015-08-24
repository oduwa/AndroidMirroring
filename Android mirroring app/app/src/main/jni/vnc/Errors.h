/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_ERRORS_H__
#define __VNC_ERRORS_H__

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Handle SDK errors.
 */

/**
 * Gets the error produced by the last API call.
 *
 * @return A string ID indicating the type of error.  See individual function
 *   documentation for details of which error types each function can produce.
 * @retval 0 No error occurred
 * @retval 'Unknown' An error occurred but the exact cause is not known
 */
VNC_SDK_API _Ret_opt_z_ const char* vnc_getLastError();

#ifdef __cplusplus
}
#endif

#endif
