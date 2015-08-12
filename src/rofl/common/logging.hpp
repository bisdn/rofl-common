/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * logger.hpp
 *
 *  Created on: Apr 29, 2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_LOGGING_HPP_
#define SRC_ROFL_COMMON_LOGGING_HPP_

#include <deque>
#include <iostream>
#include <sstream>

#include "rofl/common/locking.hpp"

namespace rofl {

enum loglevel_t {
	_EMERG_  = 0,
    _ALERT_  = 1,
	_CRIT_   = 2,
	_ERROR_  = 3,
	_WARN_   = 4,
	_NOTICE_ = 5,
	_INFO_   = 6,
	_DEBUG_  = 7,
	_TRACE_  = 8,
	_SKIP_   = 9,
};

class logentry {
public:

	/**
	 *
	 */
	~logentry()
	{};

	/**
	 *
	 */
	logentry() :
		loglevel(loglevel_t::_DEBUG_)
	{};

	/**
	 *
	 */
	logentry(
			loglevel_t loglevel,
			const std::string& msg = std::string("")) :
				loglevel(loglevel),
				msg(msg)
	{};

	/**
	 *
	 */
	logentry(
			const logentry& entry)
	{ *this = entry; };

	/**
	 *
	 */
	logentry&
	operator= (
			const logentry& entry) {
		if (this == &entry)
			return *this;
		loglevel  = entry.loglevel;
		msg       = entry.msg;
		return *this;
	};

public:

	/**
	 *
	 */
	loglevel_t
	get_loglevel() const
	{ return loglevel; };

	/**
	 *
	 */
	const std::string&
	get_msg() const
	{ return msg; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const logentry& entry) {
		switch (entry.get_loglevel()) {
		case loglevel_t::_EMERG_ : os << "log<emergency> : "; break;
		case loglevel_t::_ALERT_ : os << "log<alert> : ";     break;
		case loglevel_t::_CRIT_  : os << "log<critical> : ";  break;
		case loglevel_t::_ERROR_ : os << "log<error> : ";     break;
		case loglevel_t::_WARN_  : os << "log<warning> : ";   break;
		case loglevel_t::_NOTICE_: os << "log<notice> : ";    break;
		case loglevel_t::_INFO_  : os << "log<info> : ";      break;
		case loglevel_t::_DEBUG_ : os << "log<debug> : ";     break;
		case loglevel_t::_TRACE_ : os << "log<trace> : ";     break;
		default: os << "log<unknown> : "; break;
		}
		os << entry.get_msg();
		return os;
	};

protected:

	// TODO for the future: json encoded and machine readable entry
	loglevel_t   loglevel;
	std::string  msg;
};


// forward declarations of manipulators, see below
class set_log_level_emerg;
class set_log_level_alert;
class set_log_level_crit;
class set_log_level_error;
class set_log_level_warn;
class set_log_level_notice;
class set_log_level_info;
class set_log_level_debug;
class set_log_level_trace;
class set_log_level_skip;
class set_log_level_flush;



class logging : private std::stringbuf, public std::ostream {
private:

    virtual int
	sync() {
    	store(logentry(active_loglevel, str())); str("");
    	active_loglevel = loglevel_t::_SKIP_;
    	return 0; // success
    };

public:

	/**
	 *
	 */
	~logging()
	{ pubsync(); };

	/**
	 *
	 */
	logging(
			loglevel_t loglevel = loglevel_t::_EMERG_,
			size_t queue_limit = DEFAULT_NUM_LOGENTRIES_LIMIT) :
				std::ostream(this),
				loglevel(loglevel),
				queue_limit(queue_limit),
				active_loglevel(loglevel_t::_SKIP_)
	{};

	/**
	 *
	 */
	const logentry&
	operator[] (
			size_t index) const {
		AcquireReadLock lock(logger_lock);
		if ((queue.size() - 1) < index)
			throw;
		return queue.at((queue.size() - 1) - index);
	};

public:

	/**
	 *
	 */
	void
	store(
			const logentry& entry) {
		AcquireReadWriteLock lock(logger_lock);
		if (loglevel < entry.get_loglevel()) {
			return;
		}
		queue.push_back(entry);
		while (queue.size() > queue_limit) {
			queue.pop_front();
		}
	};

	/**
	 *
	 */
	size_t
	size() const {
		AcquireReadLock lock(logger_lock);
		return queue.size();
	};

	/**
	 *
	 */
	void
	set_queue_limit(
			size_t limit)
	{ this->queue_limit = limit; };

	/**
	 *
	 */
	void
	set_loglevel(
			loglevel_t loglevel)
	{ this->loglevel = loglevel; };

	/**
	 *
	 */
	logging&
	set_active_loglevel(
			loglevel_t active_loglevel)
	{ this->active_loglevel = active_loglevel; return *this; };

private:

	friend class set_log_level_emerg;
	friend class set_log_level_alert;
	friend class set_log_level_crit;
	friend class set_log_level_error;
	friend class set_log_level_warn;
	friend class set_log_level_notice;
	friend class set_log_level_info;
	friend class set_log_level_debug;
	friend class set_log_level_trace;
	friend class set_log_level_skip;
	friend class set_log_level_flush;

public:

	friend std::ostream&
	operator<< (std::ostream& os, const logging& log) {
		os << "[logging stack: ]" << std::endl;
		for (auto entry : log.queue) {
			os << entry << std::endl;
		}
		return os;
	};

	static set_log_level_emerg   emerg;
	static set_log_level_alert   alert;
	static set_log_level_crit    crit;
	static set_log_level_error   error;
	static set_log_level_warn    warn;
	static set_log_level_notice  notice;
	static set_log_level_info    info;
	static set_log_level_debug   debug;
	static set_log_level_trace   trace;
	static set_log_level_skip    skip;
	static set_log_level_flush   flush;

private:

	crwlock            logger_lock;
	loglevel_t			 loglevel;
	static const size_t  DEFAULT_NUM_LOGENTRIES_LIMIT = 128;
	size_t               queue_limit;
	std::deque<logentry> queue;
	loglevel_t			 active_loglevel;
};

/*
 * manipulators
 */

class set_log_level_emerg {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_emerg& m)
	{ return log.set_active_loglevel(loglevel_t::_EMERG_); };
};

class set_log_level_alert {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_alert& m)
	{ return log.set_active_loglevel(loglevel_t::_ALERT_); };
};

class set_log_level_crit {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_crit& m)
	{ return log.set_active_loglevel(loglevel_t::_CRIT_); };
};

class set_log_level_error {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_error& m)
	{ return log.set_active_loglevel(loglevel_t::_ERROR_); };
};

class set_log_level_warn {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_warn& m)
	{ return log.set_active_loglevel(loglevel_t::_WARN_); };
};

class set_log_level_notice {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_notice& m)
	{ return log.set_active_loglevel(loglevel_t::_NOTICE_); };
};

class set_log_level_info {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_info& m)
	{ return log.set_active_loglevel(loglevel_t::_INFO_); };
};

class set_log_level_debug {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_debug& m)
	{ return log.set_active_loglevel(loglevel_t::_DEBUG_); };
};

class set_log_level_trace {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_trace& m)
	{ return log.set_active_loglevel(loglevel_t::_TRACE_); };
};

class set_log_level_skip {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_skip& m)
	{ return log.set_active_loglevel(loglevel_t::_SKIP_); };
};

class set_log_level_flush {
public:
	friend logging&
	operator<< (logging& log, const set_log_level_flush& m)
	{ dynamic_cast<std::ostream&>(log).flush(); return log; };
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_LOGGING_HPP_ */

