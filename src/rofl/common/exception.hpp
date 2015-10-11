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
#include <sstream>
#include <stdexcept>

namespace rofl {

class exception : public std::runtime_error {
public:
	exception(
			const std::string& __arg) :
				std::runtime_error(__arg)
	{};
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
	{};
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
