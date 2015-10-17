/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * exception.hpp
 *
 *  Created on: Apr 23, 2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_EXCEPTION_HPP_
#define SRC_ROFL_COMMON_EXCEPTION_HPP_

#include <errno.h>
#include <string.h>

#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace rofl {

class exception : public std::runtime_error {
public:
	exception(
			const std::string& __arg) :
				std::runtime_error(__arg)
	{};

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
	exception&
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
	exception&
	set_method(
			const std::string& s_method)
	{ set_key("method", s_method); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_reason() const
	{ return get_key("reason"); };

	/**
	 *
	 */
	exception&
	set_reason(
			const std::string& s_reason)
	{ set_key("reason", s_reason); return *this; };

public:

	/**
	 *
	 */
	int
	get_errnum() const
	{
		int errnum = 0;
		std::istringstream ss(get_key("errno")); ss >> errnum; return errnum;
	};

	/**
	 *
	 */
	exception&
	set_errnum(
			int errnum)
	{
		std::ostringstream ss; ss << errnum;
		set_key("errno", ss.str());
		set_key("errstr", strerror(errnum));
		return *this;
	};

public:

	/**
	 *
	 */
	std::vector<std::string>
	keys() const {
		std::vector<std::string> vkeys;
		for (auto it : kvmap) {
			vkeys.push_back(it.first);
		}
		return vkeys;
	}

	/**
	 *
	 */
	exception&
	set_key(
			const std::string& key, int value) {
		std::stringstream ss; ss << value;
		kvmap[key] = ss.str(); return *this;
	};

	/**
	 *
	 */
	exception&
	set_key(
			const std::string& key, unsigned int value) {
		std::stringstream ss; ss << value;
		kvmap[key] = ss.str(); return *this;
	};

	/**
	 *
	 */
	exception&
	set_key(
			const std::string& key, const std::string& value) {
		kvmap[key] = value; return *this;
	};

	/**
	 *
	 */
	std::string&
	set_key(
			const std::string& key) {
		return kvmap[key];
	};

	/**
	 *
	 */
	const std::string&
	get_key(
			const std::string& key) const {
		return kvmap.at(key);
	};

	/**
	 *
	 */
	bool
	drop_key(
			const std::string& key) {
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
		return (not (kvmap.find(key) == kvmap.end()));
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const exception& e) {
		os << "exception: " << e.what() << " ";
		for (auto it : e.kvmap) {
			os << it.first << ":" << it.second << ", ";
		}
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	};

private:

	std::map<std::string, std::string> kvmap;
};


class eOutOfRange : exception {
public:
	eOutOfRange(
			const std::string& __arg) :
				exception(__arg)
	{};
};

class eOutOfMem : exception {
public:
	eOutOfMem(
			const std::string& __arg) :
					exception(__arg)
	{};
};

class eInvalid : exception {
public:
	eInvalid(
			const std::string& __arg) :
					exception(__arg)
	{};
};

class eSysCall : exception {
public:
	eSysCall(
			const std::string& __arg) :
				exception(__arg),
				__errno(errno)
	{ set_errnum(errno); };
    virtual
	const char*
    what() const noexcept {
    	std::stringstream ss;
    	ss << exception::what() <<  " errno: " << __errno << " (" << strerror(__errno) << ")";
    	return ss.str().c_str();
    }
private:
    int __errno;
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_EXCEPTION_HPP_ */
