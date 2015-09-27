/*
 * csegmentation.h
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#ifndef CSEGMENTATION_H_
#define CSEGMENTATION_H_

#include <inttypes.h>

#include <iostream>
#include <map>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csegmsg.h"
#include "rofl/common/cthread.hpp"
#include "rofl/common/ctimerid.h"

namespace rofl {

class eSegmentationBase 		: public RoflException {};
class eSegmentationInval		: public eSegmentationBase {};
class eSegmentationNotFound		: public eSegmentationBase {};

class csegmentation :
		public cthread_env
{
public:

	/**
	 *
	 */
	csegmentation(
			time_t check_expiration_interval = DEFAULT_CHECK_EXPIRATION_INTERVAL);

	/**
	 *
	 */
	virtual
	~csegmentation();

	/**
	 *
	 */
	csegmentation(
			csegmentation const& seg);

	/**
	 *
	 */
	csegmentation&
	operator= (csegmentation const& seg);

public:

	/**
	 *
	 */
	csegmsg&
	add_transaction(uint32_t xid);

	/**
	 *
	 */
	void
	drop_transaction(uint32_t xid);

	/**
	 *
	 */
	csegmsg&
	set_transaction(uint32_t xid);

	/**
	 *
	 */
	csegmsg const&
	get_transaction(uint32_t xid) const;

	/**
	 *
	 */
	bool
	has_transaction(uint32_t xid);


private:

	virtual void
	handle_wakeup(
			cthread& thread)
	{};

	virtual void
	handle_timeout(
			cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes);

	virtual void
	handle_read_event(
			cthread& thread, int fd)
	{};

	virtual void
	handle_write_event(
			cthread& thread, int fd)
	{};

private:

	/**
	 *
	 */
	void
	drop_expired_sessions();

public:

	friend std::ostream&
	operator<< (std::ostream& os, csegmentation const& seg) {
		os << rofl::indent(0) << "<csegmentation #transactions:" << (int)seg.segmsgs.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, csegmsg>::const_iterator
				it = seg.segmsgs.begin(); it != seg.segmsgs.end(); ++it) {
			os << it->second;
		}
		return os;
	};

private:

	enum csegmentation_timer_t {
		TIMER_ID_CHECK_EXPIRATION = 1,
	};

	// working thread
	cthread			                    thread;

	// all current pending transactions with fragments
	std::map<uint32_t, csegmsg> 		segmsgs;

	// timer-id
	ctimerid							check_expiration_id;

	time_t								check_expiration_interval;

	static const time_t                 DEFAULT_CHECK_EXPIRATION_INTERVAL = 8; 	// seconds

};

}; // end of namespace rofl

#endif /* CSEGMENTATION_H_ */
