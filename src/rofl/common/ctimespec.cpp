/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimespec.cpp
 *
 *  Created on: May 26, 2015
 *      Author: andi
 */

#include "ctimespec.hpp"

using namespace rofl;

/*static*/ctimespec ctimespec::current_time;


ctimespec
ctimespec::operator+ (
		const ctimespec& t) const
{
	ctimespec timer;

	timer.tspec.tv_sec		= tspec.tv_sec  + t.tspec.tv_sec;
	timer.tspec.tv_nsec 	= tspec.tv_nsec + t.tspec.tv_nsec;

	if (timer.tspec.tv_nsec > CC_TIMER_ONE_SECOND_NS) {
		timer.tspec.tv_sec 	+=  CC_TIMER_ONE_SECOND_S;
		timer.tspec.tv_nsec -=  CC_TIMER_ONE_SECOND_NS;
	}

	return timer;
}



ctimespec
ctimespec::operator- (
		const ctimespec& t) const
{
	ctimespec timer(::timespec {0,0});

	if (tspec.tv_sec > t.tspec.tv_sec) {
		if (t.tspec.tv_nsec > tspec.tv_nsec) {
			timer.tspec.tv_nsec = tspec.tv_nsec - t.tspec.tv_nsec + CC_TIMER_ONE_SECOND_NS;
			timer.tspec.tv_sec  = tspec.tv_sec  - t.tspec.tv_sec  - CC_TIMER_ONE_SECOND_S;
		} else {
			timer.tspec.tv_nsec = tspec.tv_nsec - t.tspec.tv_nsec;
			timer.tspec.tv_sec  = tspec.tv_sec  - t.tspec.tv_sec;
		}
	}

	return timer;
}



ctimespec&
ctimespec::operator+= (
		const ctimespec& t)
{
	tspec.tv_sec	+= t.tspec.tv_sec;
	tspec.tv_nsec 	+= t.tspec.tv_nsec;

	if (tspec.tv_nsec >  CC_TIMER_ONE_SECOND_NS) {
		tspec.tv_sec 	+= CC_TIMER_ONE_SECOND_S;
		tspec.tv_nsec 	-= CC_TIMER_ONE_SECOND_NS;
	}

	return *this;
}



ctimespec&
ctimespec::operator-= (
		const ctimespec& t)
{
	if (tspec.tv_sec > t.tspec.tv_sec) {
		if (t.tspec.tv_nsec > tspec.tv_nsec) {
			tspec.tv_nsec = tspec.tv_nsec - t.tspec.tv_nsec + CC_TIMER_ONE_SECOND_NS;
			tspec.tv_sec  = tspec.tv_sec  - t.tspec.tv_sec  - CC_TIMER_ONE_SECOND_S;
		} else {
			tspec.tv_nsec = tspec.tv_nsec - t.tspec.tv_nsec;
			tspec.tv_sec  = tspec.tv_sec  - t.tspec.tv_sec;
		}
	}

	return *this;
}



bool
ctimespec::operator!= (
		const ctimespec& t) const
{
	return not (*this == t);
}



bool
ctimespec::operator< (
		const ctimespec& t) const
{
	if (tspec.tv_sec < t.tspec.tv_sec) {
		return true;
	} else
	if (tspec.tv_sec > t.tspec.tv_sec) {
		return false;
	}

	// here: tspec.tv_sec == t.tspec.tv_sec

	if (tspec.tv_nsec < t.tspec.tv_nsec) {
		return true;
	} else
	if (tspec.tv_nsec > t.tspec.tv_nsec) {
		return false;
	}

	// here: tspec.tv_nsec == t.tspec.tv_nsec

	return false;
}



bool
ctimespec::operator<= (
		const ctimespec& t) const
{
	return ((*this < t) || (*this == t));
}


bool
ctimespec::operator> (
		const ctimespec& t) const
{
	if (tspec.tv_sec > t.tspec.tv_sec) {
		return true;
	} else
	if (tspec.tv_sec < t.tspec.tv_sec) {
		return false;
	}

	// here: tspec.tv_sec == t.tspec.tv_sec

	if (tspec.tv_nsec > t.tspec.tv_nsec) {
		return true;
	} else
	if (tspec.tv_nsec < t.tspec.tv_nsec) {
		return false;
	}

	// here: tspec.tv_nsec == t.tspec.tv_nsec

	return false;
}


bool
ctimespec::operator>= (
		const ctimespec& t) const
{
	return ((*this > t) || (*this == t));
}



bool
ctimespec::operator== (
		const ctimespec& t) const
{
	return ((not (*this < t)) && (not (t < *this)));
}

