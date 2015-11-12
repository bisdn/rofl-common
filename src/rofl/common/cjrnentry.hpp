/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cjrnentry.hpp
 *
 *  Created on: Oct 17, 2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_CJRNENTRY_HPP_
#define SRC_ROFL_COMMON_CJRNENTRY_HPP_

#include <inttypes.h>
#include <ostream>
#include <sstream>
#include <cstdarg>
#include <string>
#include <map>

#include "rofl/common/ctimespec.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"

namespace rofl {

enum cjournal_level_t {
	LOG_EXCEPTION     = 0,
	LOG_CRIT_ERROR    = 1,
	LOG_RUNTIME_ERROR = 2,
	LOG_NOTICE        = 3,
	LOG_INFO          = 4,
	LOG_TRACE         = 5,
};

class cjrnentry
{
public:

	/**
	 *
	 */
	~cjrnentry()
	{};

	/**
	 *
	 */
	cjrnentry(
			uint32_t id) :
				id(id),
				logtime(ctimespec::now(CLOCK_REALTIME))
	{};

	/**
	 *
	 */
	cjrnentry(
			const cjrnentry& jrnentry)
	{ *this = jrnentry; };

	/**
	 *
	 */
	cjrnentry&
	operator= (
			const cjrnentry& jrnentry) {
		if (this == &jrnentry)
			return *this;
		AcquireReadWriteLock rwlock(kvmap_lock);
		for (auto key : jrnentry.keys()) {
			set_key(key, jrnentry.get_key(key));
		}
		logtime = jrnentry.logtime;
		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_id() const
	{ return id; };

	/**
	 *
	 */
	const ctimespec&
	get_logtime() const
	{ return logtime; };

public:

	/**
	 *
	 */
	cjrnentry&
	log(
			cjournal_level_t severity, const std::string& event)
	{ set_severity(severity); set_key("event", event); return *this; };

	/**
	 *
	 */
	cjrnentry&
	log(
			cjournal_level_t severity, const char* format, ...)
	{
		char tmp[1024];
		va_list args;
		va_start(args, format);
		vsnprintf(tmp, sizeof(tmp), format, args);
		va_end(args);
		return log(severity, std::string(tmp));
	};

	/**
	 *
	 */
	cjrnentry&
	log(
			const rofl::exception& e)
	{
		set_severity(LOG_EXCEPTION);
		set_key("event", e.what());
		for (auto key : e.keys()) {
			set_key(key, e.get_key(key));
		}
		return *this;
	};

public:

	/**
	 *
	 */
	cjournal_level_t
	get_severity() const
	{
		int severity = 0;
		std::istringstream ss(get_key("severity")); ss >> severity; return (cjournal_level_t)severity;
	};

	/**
	 *
	 */
	cjrnentry&
	set_severity(
			cjournal_level_t severity)
	{
		std::ostringstream ss; ss << severity;
		set_key("severity", ss.str());
		return *this;
	};

public:

	/**
	 *
	 */
	const std::string&
	get_caller() const
	{ return get_key("caller"); };

	/**
	 *
	 */
	cjrnentry&
	set_caller(
			const std::string& s_caller)
	{ set_key("caller", s_caller); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_action() const
	{ return get_key("action"); };

	/**
	 *
	 */
	cjrnentry&
	set_action(
			const std::string& s_action)
	{ set_key("action", s_action); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_func() const
	{ return get_key("func"); };

	/**
	 *
	 */
	cjrnentry&
	set_func(
			const std::string& s_func)
	{ set_key("func", s_func); return *this; };

public:

	/**
	 *
	 */
	int
	get_line() const
	{
		std::istringstream ss(get_key("line"));
		int line = 0; ss >> line;
		return line;
	};

	/**
	 *
	 */
	cjrnentry&
	set_line(
			int line)
	{
		std::stringstream ss; ss << line;
		set_key("line", ss.str());
		return *this;
	};

public:

	/**
	 *
	 */
	const std::string&
	get_class() const
	{ return get_key("class"); };

	/**
	 *
	 */
	cjrnentry&
	set_class(
			const std::string& s_class)
	{ set_key("class", s_class); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_method() const
	{ return get_key("method"); };

	/**
	 *
	 */
	cjrnentry&
	set_method(
			const std::string& method)
	{ set_key("method", method); return *this; };

public:

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock rwlock(kvmap_lock);
		kvmap.clear();
	};

	/**
	 *
	 */
	std::vector<std::string>
	keys() const {
		AcquireReadLock rlock(kvmap_lock);
		std::vector<std::string> vkeys;
		for (auto it : kvmap) {
			vkeys.push_back(it.first);
		}
		return vkeys;
	};

	/**
	 *
	 */
	cjrnentry&
	set_key(
			const std::string& key, int value) {
		AcquireReadWriteLock rwlock(kvmap_lock);
		std::stringstream ss; ss << value;
		kvmap[key] = ss.str(); return *this;
	};

	/**
	 *
	 */
	cjrnentry&
	set_key(
			const std::string& key, unsigned int value) {
		AcquireReadWriteLock rwlock(kvmap_lock);
		std::stringstream ss; ss << value;
		kvmap[key] = ss.str(); return *this;
	};

	/**
	 *
	 */
	cjrnentry&
	set_key(
			const std::string& key, const std::string& value) {
		AcquireReadWriteLock rwlock(kvmap_lock);
		kvmap[key] = value; return *this;
	};

	/**
	 *
	 */
	std::string&
	set_key(
			const std::string& key) {
		AcquireReadWriteLock rwlock(kvmap_lock);
		return kvmap[key];
	};

	/**
	 *
	 */
	const std::string&
	get_key(
			const std::string& key) const {
		AcquireReadLock rlock(kvmap_lock);
		return kvmap.at(key);
	};

	/**
	 *
	 */
	bool
	drop_key(
			const std::string& key) {
		AcquireReadWriteLock rwlock(kvmap_lock);
		if (kvmap.find(key) == kvmap.end()) {
			return false;

		}
		kvmap.erase(key);
		return true;
	};

	/**
	 *
	 */
	bool
	has_key(
			const std::string& key) const {
		AcquireReadLock rlock(kvmap_lock);
		return (not (kvmap.find(key) == kvmap.end()));
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const cjrnentry& jrnentry) {
		os << "[" << jrnentry.logtime.str() << "] ";
		std::vector<std::string> keys = jrnentry.keys();
		for (auto key : keys) {
			os << key << "=" << jrnentry.get_key(key) << ", ";
		}
		return os;
	};

	/**
	 *
	 */
	std::string
	str() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	};

private:

	// local identifier for this journal entry
	uint32_t                            id;

	// time this journal entry was created
	ctimespec                           logtime;

	// key:value pairs
	std::map<std::string, std::string>  kvmap;

	// associated rwlock
	crwlock                             kvmap_lock;
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CJRNENTRY_HPP_ */
