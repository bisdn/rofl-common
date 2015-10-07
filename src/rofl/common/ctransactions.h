/*
 * ctransactions.h
 *
 *  Created on: 04.01.2014
 *      Author: andi
 */

#ifndef CTRANSACTIONS_H_
#define CTRANSACTIONS_H_

#include <inttypes.h>
#include <map>

#include "rofl/common/thread_helper.h"
#include "rofl/common/cthread.hpp"
#include "rofl/common/ctransaction.h"
#include "rofl/common/crandom.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eTransactionBase		: public RoflException {};
class eTransactionNotFound	: public eTransactionBase {};

class ctransactions;

class ctransactions_env {
public:
	virtual ~ctransactions_env() {};
protected:
	friend class ctransactions;
	virtual void ta_expired(ctransactions& tas, ctransaction& ta) = 0;
};

class ctransactions :
		public std::list<ctransaction>,
		public cthread_env
{
public:

	/**
	 *
	 */
	virtual
	~ctransactions();

	/**
	 *
	 */
	ctransactions(
			ctransactions_env *env);

	/**
	 *
	 */
	ctransactions(
			const ctransactions& tas) :
				env(0), thread(this), nxid(0), work_interval(0)
	{ *this = tas; };

	/**
	 *
	 */
	ctransactions&
	operator= (
			const ctransactions& tas) {
		if (this == &tas)
			return *this;
		std::list<ctransaction>::operator= (tas);
		work_interval = tas.work_interval;
		nxid = tas.nxid;
		if (not empty())
			thread.add_timer(TIMER_ID_WORK_ON_TA_QUEUE, ctimespec().expire_in(work_interval));
		return *this;
	};

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	uint32_t
	add_ta(
			cclock const& delta = cclock(0, 0), uint8_t msg_type = 0, uint16_t msg_subtype = 0);

	/**
	 *
	 */
	void
	drop_ta(
			uint32_t xid);

	/**
	 *
	 */
	uint32_t
	get_async_xid()
	{ return ++nxid; };

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
	get_next_ta(ctransaction& ta);

	/**
	 *
	 */
	void
	work_on_ta_queue();

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ctransactions& tas) {
		os << indent(0) << "<transactions #ta:" << tas.size() << " >" << std::endl;
		indent i(2);
		for (std::list<ctransaction>::const_iterator
				it = tas.begin(); it != tas.end(); ++it) {
			os << (*it);
		}
		return os;
	};

private:

	ctransactions_env			*env;
	cthread                     thread;
	uint32_t					nxid;			// next xid
	crandom						rand;
	unsigned int				work_interval; 	// time interval for checking work-queue
	PthreadRwLock				queuelock;		// rwlock for work-queue

	enum ctransactions_timer_t {
		TIMER_ID_WORK_ON_TA_QUEUE 	= 1,	// lookup all expired TAs in list
	};
};

}; // namespace rofl



#endif /* CTRANSACTIONS_H_ */
