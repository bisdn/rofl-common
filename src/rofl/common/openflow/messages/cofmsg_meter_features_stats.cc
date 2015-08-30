/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"

using namespace rofl::openflow;

cofmsg_meter_features_stats_request::~cofmsg_meter_features_stats_request()
{}



cofmsg_meter_features_stats_request::cofmsg_meter_features_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_METER_FEATURES, stats_flags)
{}



cofmsg_meter_features_stats_request::cofmsg_meter_features_stats_request(
		const cofmsg_meter_features_stats_request& msg)
{
	*this = msg;
}



cofmsg_meter_features_stats_request&
cofmsg_meter_features_stats_request::operator= (
		const cofmsg_meter_features_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	return *this;
}




size_t
cofmsg_meter_features_stats_request::length() const
{
	switch (get_version()) {
	default: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request));
	};
	}
	return 0;
}



void
cofmsg_meter_features_stats_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_meter_features_stats_request::length())
		throw eMsgInval("cofmsg_group_stats_request::pack() buf too short");
}



void
cofmsg_meter_features_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_METER_FEATURES)
			throw eMsgInval("cofmsg_meter_features_stats_request::unpack() invalid statistics type");
	};
	}

	if (buflen < cofmsg_meter_features_stats_request::length())
		throw eBadRequestBadLen("cofmsg_meter_features_stats_request::unpack() buf too short");

	if (get_length() < cofmsg_meter_features_stats_request::length())
		throw eBadRequestBadLen("cofmsg_meter_features_stats_request::unpack() buf too short");
}







cofmsg_meter_features_stats_reply::~cofmsg_meter_features_stats_reply()
{}



cofmsg_meter_features_stats_reply::cofmsg_meter_features_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_features_reply& meter_features) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_METER_FEATURES, stats_flags),
				meter_features(meter_features)
{
	this->meter_features.set_version(version);
}



cofmsg_meter_features_stats_reply::cofmsg_meter_features_stats_reply(
		const cofmsg_meter_features_stats_reply& msg)
{
	*this = msg;
}



cofmsg_meter_features_stats_reply&
cofmsg_meter_features_stats_reply::operator= (
		const cofmsg_meter_features_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	meter_features	= msg.meter_features;
	return *this;
}



size_t
cofmsg_meter_features_stats_reply::length() const
{
	switch (get_version()) {
	default: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + meter_features.length());
	};
	}
}



void
cofmsg_meter_features_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_meter_features_stats_reply::length())
		throw eMsgInval("cofmsg_meter_features_stats_reply::pack() buf too short");

	switch (get_version()) {
	default: {
		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		meter_features.pack(hdr->body, meter_features.length());
	};
	}
}



void
cofmsg_meter_features_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	meter_features.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_meter_features_stats_reply::length())
		throw eBadRequestBadLen("cofmsg_meter_features_stats_reply::unpack() buf too short");

	switch (get_version()) {
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_METER_FEATURES)
			throw eMsgInval("cofmsg_meter_features_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			meter_features.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
		}
	};
	}

	if (get_length() < cofmsg_meter_features_stats_reply::length())
		throw eBadRequestBadLen("cofmsg_meter_features_stats_reply::unpack() buf too short");
}


