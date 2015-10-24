/*
 * crofqueue.h
 *
 *  Created on: 23.11.2014
 *      Author: andreas
 */

#ifndef CROFQUEUE_H_
#define CROFQUEUE_H_

#include <list>
#include <ostream>

#include "rofl/common/locking.hpp"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/logging.h"
#include "rofl/common/exception.hpp"

namespace rofl {


class eRofQueueBase : public exception {
public:
	eRofQueueBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};
class eRofQueueInvalid : public eRofQueueBase {
public:
	eRofQueueInvalid(
			const std::string& __arg) :
				eRofQueueBase(__arg)
	{};
};
class eRofQueueNotFound : public eRofQueueBase {
public:
	eRofQueueNotFound(
			const std::string& __arg) :
				eRofQueueBase(__arg)
	{};
};
class eRofQueueFull : public eRofQueueBase {
public:
	eRofQueueFull(
			const std::string& __arg) :
				eRofQueueBase(__arg)
	{};
};


class crofqueue {
public:

	/**
	 *
	 */
	crofqueue() :
		queue_max_size(QUEUE_MAX_SIZE_DEFAULT)
	{};

	/**
	 *
	 */
	~crofqueue()
	{ clear(); };

public:

	/**
	 *
	 */
	bool
	empty() const {
		AcquireReadLock rwlock(queue_lock);
		return queue.empty();
	};

	/**
	 *
	 */
	size_t
	size() const {
		AcquireReadLock rwlock(queue_lock);
		return queue.size();
	};

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock rwlock(queue_lock);
		while (not queue.empty()) {
			rofl::openflow::cofmsg* msg = queue.front(); queue.pop_front();
			std::cerr << "[rofl-common][crofqueue][clear] msg: " << std::endl << *msg;
			delete msg;
		}
	};

	/**
	 *
	 */
	size_t
	store(rofl::openflow::cofmsg* msg, bool enforce = false) {
		AcquireReadWriteLock rwlock(queue_lock);
		//std::cerr << "[rofl-common][crofqueue][store] msg: " << std::endl << *msg;
		if ((not enforce) && (queue.size() >= queue_max_size)) {
			throw eRofQueueFull("crofqueue::store() queue max size exceeded");
		}
		queue.push_back(msg);
		return queue.size();
	};

	/**
	 *
	 */
	rofl::openflow::cofmsg*
	retrieve() {
		rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;
		AcquireReadWriteLock rwlock(queue_lock);
		if (queue.empty()) {
			return msg;
		}
		msg = queue.front(); queue.pop_front();
		//std::cerr << "[rofl-common][crofqueue][retrieve] msg: " << std::endl << *msg;
		return msg;
	};

	/**
	 *
	 */
	rofl::openflow::cofmsg*
	front() {
		rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;
		AcquireReadWriteLock rwlock(queue_lock);
		if (queue.empty()) {
			return msg;
		}
		msg = queue.front();
		//std::cerr << "[rofl-common][crofqueue][front] msg: " << std::endl << *msg;
		return msg;
	};

	/**
	 *
	 */
	void
	pop() {
		AcquireReadWriteLock rwlock(queue_lock);
		if (queue.empty()) {
			return;
		}
		queue.pop_front();
		//std::cerr << "[rofl-common][crofqueue][pop] " << std::endl;
	};

	/**
	 *
	 */
	size_t
	capacity() const
	{
		AcquireReadLock rwlock(queue_lock);
		return (queue.size() < queue_max_size) ? (queue_max_size - queue.size()) : 0;
	};

public:

	/**
	 *
	 */
	size_t
	get_queue_max_size() const
	{ return queue_max_size; };

	/**
	 *
	 */
	crofqueue&
	set_queue_max_size(
			size_t queue_max_size)
	{ this->queue_max_size = queue_max_size; return *this; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofqueue& queue) {
		AcquireReadLock rwlock(queue.queue_lock);
		os << rofl::indent(0) << "<crofqueue size #" << queue.queue.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::list<rofl::openflow::cofmsg*>::const_iterator
				it = queue.queue.begin(); it != queue.queue.end(); ++it) {
			os << *(*it);
		}
		return os;
	};

private:

	std::list<rofl::openflow::cofmsg*> 	queue;
	mutable crwlock                     queue_lock;
	size_t                              queue_max_size;
	static const size_t                 QUEUE_MAX_SIZE_DEFAULT = 32768;
};

}; // end of namespace rofl

#endif /* CROFQUEUE_H_ */
