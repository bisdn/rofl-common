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

#include "rofl/common/thread_helper.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {


class eRofQueueBase : public RoflException {
public:
	eRofQueueBase(
			const std::string& __arg) :
				RoflException(__arg)
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
	empty() {
		RwLock rwlock(queuelock, RwLock::RWLOCK_READ);
		return queue.empty();
	};

	/**
	 *
	 */
	size_t
	size() const {
		RwLock rwlock(queuelock, RwLock::RWLOCK_READ);
		return queue.size();
	};

	/**
	 *
	 */
	void
	clear() {
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		for (auto msg : queue) {
			delete msg;
		}
		queue.clear();
	};

	/**
	 *
	 */
	size_t
	store(rofl::openflow::cofmsg* msg, bool enforce = false) {
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
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
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
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
		RwLock rwlock(queuelock, RwLock::RWLOCK_READ);
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
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		if (queue.empty()) {
			return;
		}
		queue.pop_front();
		//std::cerr << "[rofl-common][crofqueue][pop] " << std::endl;
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
		RwLock rwlock(queue.queuelock, RwLock::RWLOCK_READ);
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
	mutable PthreadRwLock				queuelock;
	size_t                              queue_max_size;
	static const size_t                 QUEUE_MAX_SIZE_DEFAULT = 32768;
};

}; // end of namespace rofl

#endif /* CROFQUEUE_H_ */
