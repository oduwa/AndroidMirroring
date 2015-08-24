/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_COMMON_H__
#define __VNC_COMMON_H__

/**
 * @file
 * @brief Definitions and types common to all parts of the SDK.
 */
#ifndef VNC_SDK_API
#ifdef WIN32
#define VNC_SDK_API __declspec(dllimport)
#include <sal.h>
#else
#define VNC_SDK_API
#endif
#endif

/** @name Common primitive types */
/** @{ */
typedef signed char vnc_int8_t;
typedef signed short vnc_int16_t;
typedef signed int vnc_int32_t;
typedef signed long long vnc_int64_t;
typedef unsigned char vnc_uint8_t;
typedef unsigned short vnc_uint16_t;
typedef unsigned int vnc_uint32_t;
typedef unsigned long long vnc_uint64_t;
/** @} */

/**
 * @name Boolean type
 * @details Note that #vnc_true and #vnc_false are guaranteed to be defined as
 * 1 and 0 respectively in all future versions of the SDK.  There is no need to
 * explicitly reference these constants, unless you prefer that coding style.
 */
/** @{ */
/** A boolean, valid values are #vnc_true and #vnc_false */
typedef int vnc_bool_t;
#define vnc_true 1        /**< Evaluates to 1 */
#define vnc_false 0       /**< Evaluates to 0 */
/** @} */

/**
 * @name Status type
 * @details Note that #vnc_success and #vnc_failure are guaranteed to be defined
 * as 1 and 0 respectively in all future versions of the SDK.  There is no need
 * to explicitly reference these constants, unless you prefer that coding style;
 * either of these is acceptable:
 *
 * @code
 * if (!vnc_Logger_createFileLogger("logfile.txt"))
 *     handleError();
 * @endcode
 *
 * or:
 * @code
 * if (vnc_Logger_createFileLogger("logfile.txt") != vnc_success)
 *     handleError();
 * @endcode
 *
 */
/** @{ */
/** Function return status, valid values are #vnc_success and #vnc_failure */
typedef int vnc_status_t;
#define vnc_success 1     /**< Evaluates to 1 */
#define vnc_failure 0     /**< Evaluates to 0 */
/** @} */

/** @name Object types */
/** @{ */
/** Opaque type for a connection, identifying a connected Viewer to a Server.*/
typedef struct vnc_Connection vnc_Connection;
/**
 * Opaque type for a connection handler, enabling a Viewer or Server to perform
 * a connection operation.
 */
typedef struct vnc_ConnectionHandler vnc_ConnectionHandler;
/** Buffer containing data managed by the SDK. */
typedef struct vnc_DataBuffer vnc_DataBuffer;
/** Server which provides control of the machine to remote users. */
typedef struct vnc_Server vnc_Server;
/** Viewer which connects to and controls a remote machine. */
typedef struct vnc_Viewer vnc_Viewer;
/** Description of how pixels are stored in a Viewer framebuffer. */
typedef struct vnc_PixelFormat vnc_PixelFormat;
/** @} */

/*
 * Source annotations
 *
 * See https://msdn.microsoft.com/en-us/library/ms182032.aspx for details.
 */
#ifndef _In_reads_
#define _In_reads_(x)
#endif
#ifndef _In_reads_bytes_
#define _In_reads_bytes_(x)
#endif
#ifndef _In_opt_
#define _In_opt_
#endif
#ifndef _In_opt_z_
#define _In_opt_z_
#endif
#ifndef _Out_
#define _Out_
#endif
#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(x)
#endif
#ifndef _Ret_opt_bytecount_
#define _Ret_opt_bytecount_(x)
#endif
#ifndef _Ret_opt_z_
#define _Ret_opt_z_
#endif
#ifndef _Ret_opt_
#define _Ret_opt_
#endif
#ifndef _Ret_valid_
#define _Ret_valid_
#endif
#define _Enum_mask_(x)
#define _Callback_(x)
#define _Callback_default_(x)
#define _Platform_(x)

/* If these cause compile errors, it means that one or more of the integer
 * types defined by the compiler is not of a size that we support. */
typedef char vnc_assert_int8_t  [sizeof(vnc_int8_t)   == 1 ? 1 : -1];
typedef char vnc_assert_int16_t [sizeof(vnc_int16_t)  == 2 ? 1 : -1];
typedef char vnc_assert_int32_t [sizeof(vnc_int32_t)  == 4 ? 1 : -1];
typedef char vnc_assert_int64_t [sizeof(vnc_int64_t)  == 8 ? 1 : -1];
typedef char vnc_assert_uint8_t [sizeof(vnc_uint8_t)  == 1 ? 1 : -1];
typedef char vnc_assert_uint16_t[sizeof(vnc_uint16_t) == 2 ? 1 : -1];
typedef char vnc_assert_uint32_t[sizeof(vnc_uint32_t) == 4 ? 1 : -1];
typedef char vnc_assert_uint64_t[sizeof(vnc_uint64_t) == 8 ? 1 : -1];
typedef enum { vnc_DummyEnum1, vnc_DummyEnum2 } vnc_DummyEnum;
typedef char vnc_assert_DummyEnum[sizeof(vnc_DummyEnum) == 4 ? 1 : -1];

#endif
