/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cmemory.h"
#include <assert.h>

using namespace rofl;

/*static*/std::set<cmemory*> 	cmemory::cmemory_list;
/*static*/pthread_mutex_t       cmemory::memlock;
/*static*/int 			cmemory::memlockcnt = 0;

cmemory::cmemory(
		size_t len) :
		data(std::make_pair<uint8_t*, size_t>(NULL,0))
{
#if 0
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_init(&cmemory::memlock, NULL);
	}
	++cmemory::memlockcnt;
#endif

	if (len > 0) {
		mallocate(len);
	}
}



cmemory::cmemory(
		uint8_t *data,
		size_t datalen) :
		data(std::make_pair<uint8_t*, size_t>(NULL,0))
{
#if 0
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_init(&cmemory::memlock, NULL);
	}
	++cmemory::memlockcnt;
#endif

	if (datalen > 0) {
		mallocate(datalen);
		memcpy(somem(), data, datalen);
	}
}



cmemory::cmemory(const cmemory& m) :
		data(std::make_pair<uint8_t*, size_t>(NULL,0))
{
#if 0
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_init(&cmemory::memlock, NULL);
	}
	++cmemory::memlockcnt;
#endif

	*this = m;
}



cmemory::~cmemory()
{
#if 0
	--cmemory::memlockcnt;
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_destroy(&cmemory::memlock);
	}
#endif

	mfree();
}



cmemory&
cmemory::operator= (
		const cmemory& m)
{
	if (this == &m)
		return *this;

	mallocate(m.memlen());

	memcpy(this->somem(), m.somem(), m.memlen());

	return *this;
}



uint8_t*
cmemory::somem() const
{
	return data.first;
}



size_t
cmemory::memlen() const
{
	return data.second;
}



uint8_t&
cmemory::operator[] (
		size_t index) const
{
	if (index >= data.second) {
		throw eOutOfRange("cmemory::operator[] ()").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return *(somem() + index);
}



bool
cmemory::operator< (
		const cmemory& m) const
{
	if (this->memlen() < m.memlen()) {
		return true;
	}
	else if (this->memlen() > m.memlen()) {
		return false;
	}

	int rc = memcmp(this->somem(), m.somem(), this->memlen());

	if (rc < 0) {
		return true;
	} else {
		return false;
	}

	return true; // never reached
}



bool
cmemory::operator> (
		const cmemory& m) const
{
	if (this->memlen() > m.memlen()) {
		return true;
	}
	else if (this->memlen() < m.memlen()) {
		return false;
	}

	int rc = memcmp(this->somem(), m.somem(), this->memlen());

	if (rc > 0) {
		return true;
	} else {
		return false;
	}

	return true; // never reached
}



cmemory
cmemory::operator& (
		const cmemory& m) const
{
	if (memlen() != m.memlen()) {
		throw eInvalid("cmemory::operator& ()").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	cmemory tmp(*this);

	for (size_t i = 0; i < memlen(); ++i) {
		tmp[i] &= m[i];
	}

	return tmp;
}



cmemory&
cmemory::operator+= (
		const cmemory& m)
{
	size_t offset = memlen();

	resize(memlen() + m.memlen());

	memcpy(somem() + offset, m.somem(), m.memlen());

	return *this;
}



cmemory
cmemory::operator+ (
		const cmemory& m)
{
	cmemory tmp(*this);

	tmp += m;

	return tmp;
}



bool
cmemory::operator== (const cmemory& m) const
{
	if (this->memlen() == m.memlen()) {
		if (!memcmp(this->somem(), m.somem(), this->memlen())) {
			return true;
		}
	}
	return false;
}



bool
cmemory::operator!= (
		const cmemory& m) const
{
	return (not operator== (m));
}



void
cmemory::assign(
		uint8_t *buf,
		size_t buflen)
{
	resize(buflen);
	clear();
	memcpy(somem(), buf, buflen);
}



uint8_t*
cmemory::resize(
		size_t len)
{
	if (0 == len) {
		mfree();
	} else if (len <= data.second) {
		if ((data.first = (uint8_t*)realloc(data.first, len)) == 0) {
			throw eSysCall("realloc syscall failed").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
		}
		//memset(data.first + len, 0x00, data.second);

		// adjust data
		data.second = len;
	} else {
		if ((data.first = (uint8_t*)realloc(data.first, len)) == 0) {
			throw eSysCall("realloc syscall failed").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
		}
		memset(data.first + data.second, 0x00, len - data.second);

		// adjust data
		data.second = len;
	}
	return data.first;
}



void
cmemory::clear()
{
	if (!data.first || !data.second) {
		return;
	}
	memset(data.first, 0, data.second);
}



void
cmemory::pack(
		uint8_t *buf,
		size_t buflen)
{
	if (buflen < memlen())
		throw eInvalid("cmemory::pack()");
	memcpy(buf, somem(), memlen());
}



std::string
cmemory::toString() const
{
	return std::string((const char*)somem(), memlen());
}



void
cmemory::mallocate(
		size_t len)
{
	if (data.first) {
		mfree();
	}
	data.second = len;


	if ((data.first = (uint8_t*)calloc(1, data.second)) == 0) {
		throw eSysCall("calloc syscall failed").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	memset(data.first, 0, data.second);
}



void
cmemory::mfree()
{
	if (data.first) {
		memset(data.first, 0, data.second);
		free(data.first);
	}
	data = std::make_pair<uint8_t*, size_t>(NULL, 0);
}



uint8_t*
cmemory::insert(
		uint8_t *ptr,
		size_t len)
{
	if (not ((ptr >= data.first) && (ptr < (data.first + data.second)))) {
		throw eOutOfRange("cmemory::insert()").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	return insert(ptr - data.first, len);
}



uint8_t*
cmemory::insert(
		unsigned int offset,
		size_t len)
{
	if (offset > data.second) {
		throw eOutOfRange("offset > data.second").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	if (0 == data.first) {
		mallocate(len);
		return somem();
	}

	uint8_t *p_ptr = (uint8_t*)0;
	size_t p_len = data.second + len;


	if ((p_ptr = (uint8_t*)calloc(1, p_len)) == 0) {
		throw eSysCall("calloc syscall failed").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	memcpy(p_ptr, data.first, offset);
	memset(p_ptr + offset, 0x00, len);
	memcpy(p_ptr + offset + len, data.first + offset, data.second - offset);

	free(data.first);

	data.first = p_ptr;
	data.second = p_len;

	return (somem() + offset);
}



void
cmemory::remove(
		uint8_t *ptr,
		size_t len)
{
	if (not ((ptr >= data.first) && (ptr < (data.first + data.second)))) {
		throw eOutOfRange("cmemory::remove()").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	if (not (((ptr + len) >= data.first) && ((ptr + len) < (data.first + data.second)))) {
		throw eOutOfRange("cmemory::remove()").
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}

	remove(ptr - data.first, len);
}



void
cmemory::remove(
		unsigned int offset,
		size_t len)
{
	len = ((offset + len) > data.second) ? data.second - offset : len;

	memmove(data.first + offset, data.first + offset + len, data.second - (offset + len));
	memset(data.first + data.second - len, 0x00, len);

	data.second -= len;
}



unsigned int
cmemory::find_first_of(
		uint8_t value,
		unsigned int start)
{
	for (unsigned int i = start; i < memlen(); i++) {
		if (operator[] (i) == value) {
			return i;
		}
	}
	throw eMemNotFound("cmemory::find_first_of()").
			set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
}





