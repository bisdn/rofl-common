/*
 * crofcoretest.hpp
 *
 *  Created on: May 17, 2015
 *      Author: andi
 */

#ifndef TEST_UNIT_TEST_HPP_
#define TEST_UNIT_TEST_HPP_

#include <inttypes.h>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/crofbase.h"

class ccontroller :
	public rofl::crofbase
{
public:

	/**
	 *
	 */
	~ccontroller();

	/**
	 *
	 */
	ccontroller();

public:

	/**
	 *
	 */
	bool
	keep_running() const
	{ return __keep_running; };

	/**
	 *
	 */
	const rofl::cdptid&
	get_dptid() const
	{ return dptid; };

private:

	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);

	virtual void
	handle_features_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_reply& msg);

	virtual void
	handle_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_reply& msg);

	virtual void
	handle_table_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_reply& msg);

	virtual void
	handle_port_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_reply& msg);

	virtual void
	handle_barrier_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_reply& msg);

	virtual void
	handle_barrier_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid);

private:

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;

	// bind address
	rofl::csockaddr         baddr;

	// dptid
	rofl::cdptid            dptid;

	// keep test running
	bool                    __keep_running;
};



class cdatapath :
		public rofl::crofbase
{
public:

	/**
	 *
	 */
	~cdatapath();

	/**
	 *
	 */
	cdatapath();

public:

	/**
	 *
	 */
	void
	test_start();

public:

	/**
	 *
	 */
	const rofl::cctlid&
	get_ctlid() const
	{ return ctlid; };

private:

	virtual void
	handle_ctl_open(
			rofl::crofctl& ctl);

	virtual void
	handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg);

	virtual void
	handle_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request& msg);

	virtual void
	handle_table_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request& msg);

	virtual void
	handle_port_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request& msg);

	virtual void
	handle_barrier_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_request& msg);

private:

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;

	// controller address
	rofl::csockaddr             raddr;

	// id for main crofctl instance
	rofl::cctlid                ctlid;

	uint64_t                    dpid;
	uint32_t                    n_buffers;
	uint8_t                     n_tables;
	uint32_t                    capabilities;
	uint16_t                    flags;
	uint16_t                    miss_send_len;
	rofl::openflow::coftables   tables;
	rofl::openflow::cofports    ports;
};



class crofbasetest :
		public CppUnit::TestFixture,
		public rofl::cthread_env
{

	CPPUNIT_TEST_SUITE( crofbasetest );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test();

private:

	virtual void
	handle_wakeup(
			rofl::cthread& thread);

	virtual void
	handle_timeout(
			rofl::cthread& thread, uint32_t timer_id);

	virtual void
	handle_read_event(
			rofl::cthread& thread, int fd)
	{};
	virtual void
	handle_write_event(
			rofl::cthread& thread, int fd)
	{};

private:

	// test controller
	ccontroller             controller;

	// test datapath
	cdatapath               datapath;

private:

};

#endif /* TEST_USG_UNIT_TEST_HPP_ */
