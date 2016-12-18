#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <inttypes.h>
#include <signal.h>
#include <map>

#include <rofl/common/crofbase.h>
#include <rofl/common/cthread.hpp>

#include "cfibtable.h"
#include "cflowtable.h"
#include "cunixenv.h"
#include "cdaemon.h"

namespace rofl {
namespace examples {
namespace ethswctld {



/**
 * @ingroup common_howto_ethswctld
 *
 * @brief	A very simple controller for forwarding Ethernet flows.
 *
 * A simple controller application capable of switching Ethernet
 * frames in a flow-based manner.
 */
class cetherswitch :
		public cflowtable_env,
		public rofl::crofbase,
		public virtual rofl::cthread_env
{
public:

	/**
	 * @brief	Static main routine for class cetherswitch
	 *
	 * Runs main event loop. Does not return.
	 *
	 * @param argc number of arguments given to main function
	 * @param argv array of pointers to arguments given to main function
	 */
	static
	int
	run(
			int argc, char** argv);

	/**
	 *
	 */
	static
	void
	stop()
	{ cetherswitch::keep_on_running = false; };

private:

	/**
	 * @brief	cetherswitch constructor
	 */
	cetherswitch(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap);

	/**
	 * @brief	cetherswitch destructor
	 */
	virtual
	~cetherswitch();


	/** @cond EXAMPLES */

private:

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofdpt instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_close(
			const rofl::cdptid& dptid);

	/**
	 * @brief	OpenFlow Packet-In message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_in(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_in& msg);

	/**
	 * @brief	OpenFlow Flow-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_reply& msg);

	/**
	 * @brief	Timer expired while waiting for OpenFlow Flow-Stats-Reply message.
	 *
	 * No Flow-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_flow_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid);

	/** @endcond */

private:

	/**
	 * @brief	Handler for timer events.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param opaque expired timer type
	 * @param data pointer to opaque data
	 */
	virtual void
	handle_timeout(
			cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes);

	/**
	 * @brief	Dump an Ethernet frame received via an OpenFlow Packet-In message.
	 */
	void
	dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg);

	/**
	 *
	 */
	virtual rofl::crofdpt&
	set_dpt(
			const rofl::cdptid& dptid)
	{ return rofl::crofbase::set_dpt(dptid); };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cetherswitch& sw) {
		try {
			os << "<ethswitch dpid: "
					<< sw.get_dpt(sw.dptid).get_dpid() << " >" << std::endl;
		} catch (rofl::eRofDptNotFound& e) {
			os << "<ethswitch dptid: " << sw.dptid << " >" << std::endl;
		}
		
		try {
			os << cfibtable::set_fib(sw.dptid);
			os << cflowtable::get_flowtable(sw.dptid);
		} catch (rofl::examples::ethswctld::exceptions::eFlowNotFound& e) {};
		return os;
	};

private:

	enum cetherswitch_timer_t {
		TIMER_ID_DUMP_FIB          = 1,
		TIMER_ID_GET_FLOW_STATS    = 2,
	};

	static bool					keep_on_running;

	rofl::cthread               thread;

	rofl::cdptid                dptid;

	unsigned int                dump_fib_interval;
	static const unsigned int   DUMP_FIB_DEFAULT_INTERVAL = 60; // seconds

	unsigned int                get_flow_stats_interval;
	static const unsigned int   GET_FLOW_STATS_DEFAULT_INTERVAL = 30; // seconds
};

}; // namespace ethswctld
}; // namespace examples
}; // namespace rofl

#endif
