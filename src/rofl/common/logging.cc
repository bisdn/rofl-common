/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * logger.cpp
 *
 *  Created on: Apr 30, 2015
 *      Author: andi
 */

#include "logging.hpp"

using namespace rofl;

/*static*/set_log_level_emerg  logging::emerg;
/*static*/set_log_level_alert  logging::alert;
/*static*/set_log_level_crit   logging::crit;
/*static*/set_log_level_error  logging::error;
/*static*/set_log_level_warn   logging::warn;
/*static*/set_log_level_notice logging::notice;
/*static*/set_log_level_info   logging::info;
/*static*/set_log_level_debug  logging::debug;
/*static*/set_log_level_trace  logging::trace;
/*static*/set_log_level_skip   logging::skip;
/*static*/set_log_level_flush  logging::flush;
