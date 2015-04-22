/*
 * cworkflow_test.h
 *
 *  Created on: 12.04.2015
 *      Author: andreas
 */

#ifndef CWORKFLOW_TEST_H_
#define CWORKFLOW_TEST_H_

#include <vector>
#include "rofl/common/crofbase.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


class controller_t : public rofl::crofbase {
public:
	controller_t()
	{};
	virtual
	~controller_t()
	{};
public:
	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);
	virtual void
	handle_dpt_close(
			const rofl::cdptid& dptid);
};

class datapath_t : public rofl::crofbase {
public:
	datapath_t(
			uint64_t dpid,
			uint32_t n_buffers,
			uint32_t n_tables,
			uint32_t capabilities) :
				dpid(dpid),
				n_buffers(n_buffers),
				n_tables(n_tables),
				capabilities(capabilities)
	{};
	virtual
	~datapath_t()
	{};
public:
	virtual void
	handle_ctl_open(
			rofl::crofctl& ctl);
	virtual void
	handle_ctl_close(
			const rofl::cctlid& ctlid);
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
private:
	uint64_t dpid;
	uint32_t n_buffers;
	uint32_t n_tables;
	uint32_t capabilities;
};


class cworkflow_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cworkflow_test );
	CPPUNIT_TEST( testRoflImpl );
	CPPUNIT_TEST_SUITE_END();


public:
	void setUp();
	void tearDown();

	void testRoflImpl();

private:

	std::map<unsigned int, controller_t*> ctls;
	std::map<unsigned int, datapath_t*>   dpts;
};

#endif /* CWORKFLOW_TEST_H_ */
