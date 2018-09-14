/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * @file rofl_common.h
 * @author Marc Sune<marc.sune (at) bisdn.de>
 * @author Andreas Koepsel <andreas.koepsel (at) bisdn.de>
 *
 * @brief Main Revised OpenFlow Library set (ROFL-common) header
 */

#ifndef __ROFL_COMMON_H__
#define __ROFL_COMMON_H__

#include "rofl_common_conf.h"
#include <stdlib.h>

/**
 * ROFL-common version number (git tag)
 */
extern const char ROFL_COMMON_VERSION[];

/**
 * ROFL-common build number (git hash)
 */
extern const char ROFL_COMMON_BUILD_NUM[];

/**
 * Branch where ROFL-common was compiled in (git branch)
 */
extern const char ROFL_COMMON_BUILD_BRANCH[];

/**
 * ROFL-common describe summary (git describe)
 */
extern const char ROFL_COMMON_BUILD_DESCRIBE[];

#endif //__ROFL_COMMON_H__
