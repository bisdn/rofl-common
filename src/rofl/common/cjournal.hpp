/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cjournal.hpp
 *
 *  Created on: Oct 17, 2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_CJOURNAL_HPP_
#define SRC_ROFL_COMMON_CJOURNAL_HPP_

#include <inttypes.h>
#include <map>
#include <set>
#include <string>
#include <cstdarg>

#include "rofl/common/clocking.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/ctimespec.hpp"
#include "rofl/common/cjrnentry.hpp"

namespace rofl {

class cjournal; /* forward declaration */

class eJournalBase : public exception {
public:
	eJournalBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};
class eJournalNotFound : public eJournalBase {
public:
	eJournalNotFound(
			const std::string& __arg) :
				eJournalBase(__arg)
	{};
};
class eJournalExists : public eJournalBase {
public:
	eJournalExists(
			const std::string& __arg) :
				eJournalBase(__arg)
	{};
};
class eJournalInval : public eJournalBase {
public:
	eJournalInval(
			const std::string& __arg) :
				eJournalBase(__arg)
	{};
};



class cjournal_env {
	friend class cjournal;
public:
	static
	cjournal_env&
	call_env(cjournal_env* env) {
		AcquireReadLock lock(cjournal_env::journal_envs_lock);
		if (cjournal_env::journal_envs.find(env) == cjournal_env::journal_envs.end()) {
			throw eJournalNotFound("cjournal_env::call_env() cjournal_env instance not found");
		}
		return *(env);
	};
public:
	virtual
	~cjournal_env() {
		AcquireReadWriteLock lock(cjournal_env::journal_envs_lock);
		cjournal_env::journal_envs.erase(this);
	};
	cjournal_env() {
		AcquireReadWriteLock lock(cjournal_env::journal_envs_lock);
		cjournal_env::journal_envs.insert(this);
	};

protected:

	virtual void
	handle_exception(
			const cjrnentry& entry)
	{};

private:
	static std::set<cjournal_env*>  journal_envs;
	static crwlock                  journal_envs_lock;
};



class cjournal
{
public:

	/**
	 *
	 */
	~cjournal()
	{ clear(); };

	/**
	 *
	 */
	cjournal(
			cjournal_env* env, uint32_t id) :
				env(env),
				id(id),
				last_entry_id(0),
				max_entries(cjournal::MAX_ENTRIES_DEFAULT)
	{
		start();
		(void)this->env;
	};

public:

	/**
	 * @brief	Return local identifier of this cjournal instance
	 */
	uint32_t
	get_id() const
	{ return id; };

	/**
	 * @brief	Returns const reference to starttime of this journal
	 */
	const ctimespec&
	get_starttime() const
	{ return starttime; };

	/**
	 * @brief	Returns const reference to stoptime of this journal
	 */
	const ctimespec&
	get_stoptime() const
	{ return stoptime; };

public:

	/**
	 *
	 */
	unsigned int
	get_max_entries() const
	{ return max_entries; };

	/**
	 *
	 */
	cjournal&
	set_max_entries(
			unsigned int max_entries)
	{ this->max_entries = max_entries; return *this; };

public:

	/**
	 *
	 */
	void
	start()
	{ starttime = ctimespec::now(CLOCK_REALTIME); };

	/**
	 *
	 */
	void
	stop()
	{ stoptime = ctimespec::now(CLOCK_REALTIME); };

public:

	/**
	 *
	 */
	cjrnentry&
	log(
			cjournal_level_t level, const std::string& event)
	{ return add_entry().log(level, event); };

	/**
	 *
	 */
	cjrnentry&
	log(
			cjournal_level_t level, const char* format, ...)
	{
		char tmp[256];
		va_list args;
		va_start(args, format);
		vsnprintf(tmp, sizeof(tmp), format, args);
		va_end (args);
		return add_entry().log(level, tmp);
	};

public:

	/**
	 *
	 */
	std::list<uint32_t>
	get_entry_ids() const {
		std::list<uint32_t> ids;
		AcquireReadLock rlock(entries_rwlock);
		for (auto it : entries) {
			ids.push_back(it.first);
		}
		return ids;
	};

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock rwlock(entries_rwlock);
		for (auto it : entries) {
			delete it.second;
		}
		entries.clear();
		stop();
		start();
	};

	/**
	 *
	 */
	cjrnentry&
	add_entry() {
		AcquireReadWriteLock rwlock(entries_rwlock);
		while (entries.size() > max_entries) {
			uint32_t entry_id = entries.begin()->first;
			delete entries[entry_id];
			entries.erase(entry_id);
		}
		while (entries.find(last_entry_id) != entries.end()) {
			last_entry_id++;
		}
		return *(entries[last_entry_id] = new cjrnentry(last_entry_id));
	};

public:

	/**
	 *
	 */
	cjrnentry&
	add_entry(
			uint32_t id) {
		AcquireReadWriteLock rwlock(entries_rwlock);
		while (entries.size() > max_entries) {
			uint32_t entry_id = entries.begin()->first;
			delete entries[entry_id];
			entries.erase(entry_id);
		}
		if (entries.find(id) != entries.end()) {
			delete entries[id];
		}
		return *(entries[id] = new cjrnentry(id));
	};

	/**
	 *
	 */
	cjrnentry&
	set_entry(
			uint32_t id) {
		AcquireReadWriteLock rwlock(entries_rwlock);
		while (entries.size() > max_entries) {
			uint32_t entry_id = entries.begin()->first;
			delete entries[entry_id];
			entries.erase(entry_id);
		}
		if (entries.find(id) == entries.end()) {
			entries[id] = new cjrnentry(id);
		}
		return *(entries[id]);
	};

	/**
	 *
	 */
	const cjrnentry&
	get_entry(
			uint32_t id) const {
		AcquireReadLock rlock(entries_rwlock);
		if (entries.find(id) == entries.end()) {
			eJournalNotFound("cjournal::get_entry() id not found");
		}
		return *(entries.at(id));
	};

	/**
	 *
	 */
	bool
	drop_entry(
			uint32_t id) {
		AcquireReadWriteLock rwlock(entries_rwlock);
		if (entries.find(id) == entries.end()) {
			return false;
		}
		delete entries[id];
		entries.erase(id);
		return true;
	};

	/**
	 *
	 */
	bool
	has_entry(
			uint32_t id) const {
		AcquireReadLock rlock(entries_rwlock);
		return (not (entries.find(id) == entries.end()));
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const cjournal& journal) {
		for (auto it : journal.entries) {
			os << *(it.second) << std::endl;
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

	// environment for this cjournal instance
	cjournal_env*                           env;

	// local identifier for this cjournal instance
	uint32_t                                id;

	// start time (journal has been started)
	ctimespec                               starttime;

	// end time (journal has been stopped)
	ctimespec                               stoptime;

	// list of journal entries
	std::map<uint32_t, cjrnentry*>          entries;

	// rwlock for journal entries
	crwlock                                 entries_rwlock;

	// last entry id assigned
	uint32_t                                last_entry_id;

	// maximum number of entries in log
	unsigned int                            max_entries;
	static const unsigned int               MAX_ENTRIES_DEFAULT;
};

}; // end of namespace mme
}; // end of namespace spring

#endif /* SRC_ROFL_COMMON_CJOURNAL_HPP_ */
