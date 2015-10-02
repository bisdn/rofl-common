/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crandom.h"

using namespace rofl;

/*static*/const std::string crandom::DEV_URANDOM("/dev/urandom");


crandom::~crandom()
{}


crandom::crandom() :
				seedp(1)
{
	/* open /dev/urandom and read sizeof(unsigned int)
	 * as seed to rand_r
	 */
	int fd = 0;
	int rc = 0;

	if ((fd = open(DEV_URANDOM.c_str(), O_RDONLY, "r")) < 0) {
		throw eSysCall("crandom::crandom() open() for /dev/urandom failed");
	}

	if ((rc = read(fd, &seedp, sizeof(seedp))) < 0) {
		throw eSysCall("crandom::crandom() read() for /dev/urandom failed");
	}

	close(fd);

	/* seed random number generator */
	srand(seedp);
}


double
crandom::rand()
{
	return ((double)rand_r(&seedp) / RAND_MAX);
}


uint8_t
crandom::uint8()
{
	return (std::numeric_limits<uint8_t>::max() * rand());
}


uint16_t
crandom::uint16()
{
	return (std::numeric_limits<uint16_t>::max() * rand());
}


uint32_t
crandom::uint32()
{
	return (std::numeric_limits<uint32_t>::max() * rand());
}


uint64_t
crandom::uint64()
{
	return (std::numeric_limits<uint64_t>::max() * rand());
}



