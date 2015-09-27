/*
 * csegmentation.cc
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegmentation.h"

using namespace rofl;


csegmentation::csegmentation(
		time_t check_expiration_interval) :
				thread(this),
				check_expiration_interval(check_expiration_interval)
{
	/* start thread */
	thread.start();

	thread.add_timer(TIMER_ID_CHECK_EXPIRATION, ctimespec().expire_in(check_expiration_interval));
}


csegmentation::~csegmentation()
{

}


csegmentation::csegmentation(
		csegmentation const& seg) :
				thread(this)
{
	*this = seg;
}


csegmentation&
csegmentation::operator= (
		csegmentation const& seg)
{
	if (this == &seg)
		return *this;

	// TODO

	return *this;
}



csegmsg&
csegmentation::add_transaction(uint32_t xid)
{
	if (segmsgs.find(xid) != segmsgs.end()) {
		segmsgs.erase(xid);
	}
	return segmsgs[xid];
}



void
csegmentation::drop_transaction(uint32_t xid)
{
	if (segmsgs.find(xid) == segmsgs.end()) {
		return;
	}
	segmsgs.erase(xid);
}



csegmsg&
csegmentation::set_transaction(uint32_t xid)
{
	if (segmsgs.find(xid) == segmsgs.end()) {
		(void)segmsgs[xid];
	}
	return segmsgs[xid];
}



csegmsg const&
csegmentation::get_transaction(uint32_t xid) const
{
	if (segmsgs.find(xid) == segmsgs.end()) {
		throw eSegmentationNotFound();
	}
	return segmsgs.at(xid);
}



bool
csegmentation::has_transaction(uint32_t xid)
{
	return (not (segmsgs.find(xid) == segmsgs.end()));
}



void
csegmentation::handle_timeout(
		cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
{
	switch (timer_id) {
	case TIMER_ID_CHECK_EXPIRATION: {
		drop_expired_sessions();
	} break;
	default: {
		// do nothing
	};
	}
}



void
csegmentation::drop_expired_sessions()
{
restart:
	for (std::map<uint32_t, csegmsg>::iterator
			it = segmsgs.begin(); it != segmsgs.end(); ++it) {
		csegmsg& segmsg = it->second;

		if (segmsg.has_expired()) {
			std::cerr << "[rofl][csegmentation] dropping multipart segment:" << std::endl << segmsg;
			segmsgs.erase(it);
			goto restart;
		}
	}

	// re-add timer
	thread.add_timer(TIMER_ID_CHECK_EXPIRATION, ctimespec().expire_in(check_expiration_interval));
}



