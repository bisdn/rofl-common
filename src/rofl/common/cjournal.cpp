/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cjournal.cpp
 *
 *  Created on: Oct 17, 2015
 *      Author: andi
 */

#include "cjournal.hpp"

using namespace rofl;

/*static*/std::set<cjournal_env*>  cjournal_env::journal_envs;
/*static*/crwlock                  cjournal_env::journal_envs_lock;
/*static*/const unsigned int       cjournal::MAX_ENTRIES_DEFAULT = 128;

