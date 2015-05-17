/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofbase.h"
#include "crofcore.h"

using namespace rofl;

crofbase::crofbase(
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap)
{
	rofl::crofcore::register_rofbase(this);
	rofl::crofcore::set_rofcore(this).set_versionbitmap() = versionbitmap;
};



crofbase::~crofbase()
{
	rofl::crofcore::deregister_rofbase(this);
};



void
crofbase::set_num_of_workers(
		unsigned int n)
{
	rofl::crofcore::set_num_of_workers(n);
};



rofl::openflow::cofhello_elem_versionbitmap&
crofbase::set_versionbitmap()
{
	return rofl::crofcore::set_rofcore(this).set_versionbitmap();
};



void
crofbase::set_versionbitmap(
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap)
{
	rofl::crofcore::set_rofcore(this).set_versionbitmap() = versionbitmap;
};



const rofl::openflow::cofhello_elem_versionbitmap&
crofbase::get_versionbitmap() const
{
	return rofl::crofcore::set_rofcore(this).set_versionbitmap();
};



uint8_t
crofbase::get_highest_supported_ofp_version() const
{
	return rofl::crofcore::set_rofcore(this).get_highest_supported_ofp_version();
};



bool
crofbase::is_ofp_version_supported(
		uint8_t ofp_version) const
{
	return rofl::crofcore::set_rofcore(this).is_ofp_version_supported(ofp_version);
};



void
crofbase::set_log_level(
		unsigned int level)
{
	rofl::logging::set_debug_level(level);
};



void
crofbase::close_dpt_listening()
{
	rofl::crofcore::set_rofcore(this).close_dpt_listening();
};



rofl::csocket&
crofbase::add_dpt_listening(
		unsigned int sockid,
		enum rofl::csocket::socket_type_t socket_type,
		const rofl::cparams& params)
{
	return rofl::crofcore::set_rofcore(this).add_dpt_listening(sockid, socket_type, params);
};



rofl::csocket&
crofbase::set_dpt_listening(
		unsigned int sockid,
		enum rofl::csocket::socket_type_t socket_type,
		const rofl::cparams& params)
{
	return rofl::crofcore::set_rofcore(this).set_dpt_listening(sockid, socket_type, params);
};



const rofl::csocket&
crofbase::get_dpt_listening(
		unsigned int sockid) const
{
	return rofl::crofcore::set_rofcore(this).get_dpt_listening(sockid);
};



void
crofbase::drop_dpt_listening(
		unsigned int sockid)
{
	rofl::crofcore::set_rofcore(this).drop_dpt_listening(sockid);
};



bool
crofbase::has_dpt_listening(
		unsigned int sockid)
{
	return rofl::crofcore::set_rofcore(this).has_dpt_listening(sockid);
};



void
crofbase::close_ctl_listening()
{
	rofl::crofcore::set_rofcore(this).close_ctl_listening();
};



rofl::csocket&
crofbase::add_ctl_listening(
		unsigned int sockid,
		enum rofl::csocket::socket_type_t socket_type,
		const rofl::cparams& params)
{
	return rofl::crofcore::set_rofcore(this).add_ctl_listening(sockid, socket_type, params);
};



rofl::csocket&
crofbase::set_ctl_listening(
		unsigned int sockid,
		enum rofl::csocket::socket_type_t socket_type,
		const rofl::cparams& params)
{
	return rofl::crofcore::set_rofcore(this).set_ctl_listening(sockid, socket_type, params);
};



const rofl::csocket&
crofbase::get_ctl_listening(
		unsigned int sockid) const
{
	return rofl::crofcore::set_rofcore(this).get_ctl_listening(sockid);
};



void
crofbase::drop_ctl_listening(
		unsigned int sockid)
{
	rofl::crofcore::set_rofcore(this).drop_ctl_listening(sockid);
};



bool
crofbase::has_ctl_listening(
		unsigned int sockid)
{
	return rofl::crofcore::set_rofcore(this).has_ctl_listening(sockid);
};



rofl::cdptid
crofbase::get_idle_dptid() const
{
	return rofl::crofcore::set_rofcore(this).get_idle_dptid();
};



void
crofbase::drop_dpts()
{
	rofl::crofcore::set_rofcore(this).drop_dpts();
};



rofl::crofdpt&
crofbase::add_dpt(
	const rofl::cdptid& dptid,
	const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
	bool remove_on_channel_close,
	const rofl::cdpid& dpid)
{
	return rofl::crofcore::set_rofcore(this).add_dpt(dptid, versionbitmap, remove_on_channel_close, dpid);
};



rofl::crofdpt&
crofbase::set_dpt(
	const rofl::cdptid& dptid,
	const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
	bool remove_on_channel_close,
	const rofl::cdpid& dpid)
{
	return rofl::crofcore::set_rofcore(this).set_dpt(dptid, versionbitmap, remove_on_channel_close, dpid);
};



rofl::crofdpt&
crofbase::set_dpt(
		const rofl::cdptid& dptid)
{
	return rofl::crofcore::set_rofcore(this).set_dpt(dptid);
};



const rofl::crofdpt&
crofbase::get_dpt(
		const rofl::cdptid& dptid) const
{
	return rofl::crofcore::set_rofcore(this).get_dpt(dptid);
};



void
crofbase::drop_dpt(
	rofl::cdptid dptid)
{
	rofl::crofcore::set_rofcore(this).drop_dpt(dptid);
};



bool
crofbase::has_dpt(
	const rofl::cdptid& dptid) const
{
	return rofl::crofcore::set_rofcore(this).has_dpt(dptid);
};



rofl::cctlid
crofbase::get_idle_ctlid() const
{
	return rofl::crofcore::set_rofcore(this).get_idle_ctlid();
};



void
crofbase::drop_ctls()
{
	rofl::crofcore::set_rofcore(this).drop_ctls();
};



rofl::crofctl&
crofbase::add_ctl(
	const rofl::cctlid& ctlid,
	const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
	bool remove_on_channel_close)
{
	return rofl::crofcore::set_rofcore(this).add_ctl(ctlid, versionbitmap, remove_on_channel_close);
};



rofl::crofctl&
crofbase::set_ctl(
	const rofl::cctlid& ctlid,
	const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
	bool remove_on_channel_close)
{
	return rofl::crofcore::set_rofcore(this).set_ctl(ctlid, versionbitmap, remove_on_channel_close);
};



rofl::crofctl&
crofbase::set_ctl(
		const rofl::cctlid& ctlid)
{
	return rofl::crofcore::set_rofcore(this).set_ctl(ctlid);
};



const rofl::crofctl&
crofbase::get_ctl(
		const rofl::cctlid& ctlid) const
{
	return rofl::crofcore::set_rofcore(this).get_ctl(ctlid);
};



void
crofbase::drop_ctl(
	rofl::cctlid ctlid)
{
	rofl::crofcore::set_rofcore(this).drop_ctl(ctlid);
};



bool
crofbase::has_ctl(
	const rofl::cctlid& ctlid) const
{
	return rofl::crofcore::set_rofcore(this).has_ctl(ctlid);
};



void
crofbase::send_packet_in_message(
		const cauxid& auxid,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF1.0
		rofl::openflow::cofmatch &match,
		uint8_t *data,
		size_t datalen)
{
	rofl::crofcore::set_rofcore(this).send_packet_in_message(
			auxid,
			buffer_id,
			total_len,
			reason,
			table_id,
			cookie,
			in_port,
			match,
			data,
			datalen);
}



void
crofbase::send_flow_removed_message(
		const cauxid& auxid,
		rofl::openflow::cofmatch& match,
		uint64_t cookie,
		uint16_t priority,
		uint8_t reason,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count)
{
	rofl::crofcore::set_rofcore(this).send_flow_removed_message(
			auxid,
			match,
			cookie,
			priority,
			reason,
			table_id,
			duration_sec,
			duration_nsec,
			idle_timeout,
			hard_timeout,
			packet_count,
			byte_count);
}



void
crofbase::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	rofl::crofcore::set_rofcore(this).send_port_status_message(
			auxid,
			reason,
			port);
}


