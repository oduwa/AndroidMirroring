/* Copyright (C) 2015 RealVNC Limited. All rights reserved.
 */

#ifndef __VNC_VNC_H__
#define __VNC_VNC_H__

/**
 * @file
 * Includes all the other VNC SDK headers.
 *
 * Note Server.h applies only to Servers, and Viewer.h and Keyboard.h
 * apply only to Viewers.  All other headers apply equally to Servers and
 * Viewers.
 */

/*
 * Headers for both Viewers and Servers:
 */
#include "Cloud.h"
#include "DataBuffer.h"
#include "DataStore.h"
#include "Errors.h"
#include "EventLoop.h"
#include "EventLoopFd.h"
#include "EventLoopWin.h"
#include "Init.h"
#include "Logger.h"
#include "RsaKey.h"

/*
 * Headers just for Servers:
 */
#include "Server.h"

/*
 * Headers just for Viewers:
 */
#include "Keyboard.h"
#include "Viewer.h"

#endif
