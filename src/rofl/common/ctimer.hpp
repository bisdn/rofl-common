/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimer.hpp
 *
 *  Created on: 25.05.2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_CTIMER_HPP_
#define SRC_ROFL_COMMON_CTIMER_HPP_

#include <time.h>
#include <errno.h>
#include <algorithm>
#include <ostream>
#include <list>

#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"
#include "rofl/common/ctimespec.hpp"

namespace rofl {

class eTimerBase : public exception {
public:
	eTimerBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};

class eTimerSysCall : public eTimerBase {
public:
	eTimerSysCall(
			const std::string& __arg) :
				eTimerBase(__arg)
	{};
};


class ctimer {
public:

	/**
	 *
	 */
	static
	const ctimespec&
	now(
			cclockid clk_id = cclockid(CLOCK_MONOTONIC)) {
		return ctimespec::now(clk_id);
	};

public:

	/**
	 *
	 */
	~ctimer()
	{};

	/**
	 *
	 */
	ctimer(
			uint32_t timer_id,
			const ctimespec& tspec) :
				timer_id(timer_id),
				tspec(tspec)
	{};

	/**
	 *
	 */
	ctimer(
			const ctimer& ts)
	{ *this = ts; };

	/**
	 *
	 */
	ctimer&
	operator= (
			const ctimer& ts) {
		if (this == &ts)
			return *this;

		timer_id = ts.timer_id;
		tspec = ts.tspec;

		clear();
		for (auto ttype : ts.ttypes)
			add_timer_type(ttype);

		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_timer_id() const
	{ return timer_id; };

	/**
	 *
	 */
	const ctimespec&
	get_tspec() const
	{ return tspec; };

	/**
	 *
	 */
	void
	expire_in(
			time_t tv_sec = 0, long tv_nsec = 0)
	{ tspec.expire_in(tv_sec, tv_nsec); };

	/**
	 *
	 */
	int
	get_relative_timeout() const
	{ return get_tspec().get_relative_timeout(); };

	/**
	 *
	 */
	const std::list<unsigned int>&
	get_timer_types() const
	{ return ttypes; };

public:

	/**
	 *
	 */
	bool
	empty() const {
		AcquireReadLock lock(tlock);
		return ttypes.empty();
	};

	/**
	 *
	 */
	void
	clear() {
		AcquireReadLock lock(tlock);
		ttypes.clear();
	};

	/**
	 *
	 */
	ctimer&
	add_timer_type(
			unsigned int timer_type) {
		AcquireReadWriteLock lock(tlock);
		ttypes.push_back(timer_type);
		return *this;
	};

	/**
	 *
	 */
	void
	drop_timer_type(
			unsigned int timer_type) {
		AcquireReadWriteLock lock(tlock);
		std::list<unsigned int>::iterator it;
		while ((it = find_if(ttypes.begin(), ttypes.end(),
				std::bind2nd(std::equal_to<unsigned int>(), timer_type))) != ttypes.end()) {
			ttypes.remove(timer_type);
		}
	};

	/**
	 *
	 */
	int
	front() {
		AcquireReadLock lock(tlock);
		return ttypes.front();
	};

	/**
	 *
	 */
	void
	pop_front() {
		AcquireReadWriteLock lock(tlock);
		ttypes.pop_front();
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const ctimer& ts) {
		os << "<ctimer ttypes: ";
		for (auto ttype : ts.ttypes)
			os << ttype << " ";
		os << ">";
		return os;
	};

private:

	uint32_t                timer_id;
	crwlock                 tlock;
	ctimespec               tspec;
	std::list<unsigned int> ttypes; // timer types
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CTIMER_HPP_ */
