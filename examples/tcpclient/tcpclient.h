#ifndef ROFL_EXAMPLES_TCPCLIENT_H
#define ROFL_EXAMPLES_TCPCLIENT_H 1

#include <inttypes.h>
#include <signal.h>
#include <map>

#include <rofl/common/crofbase.h>

namespace rofl {
namespace examples {



/**
 * @ingroup common_howto_tcpclient
 *
 * @brief	A simple client with OpenFlow endpoint connecting to a control plane
 *
 * A simple example for using rofl to connect to a controller entity.
 */
class tcpclient :
		public rofl::crofbase
{
public:

	/**
	 * @brief	tcpclient destructor
	 */
	virtual
	~tcpclient()
	{

	};

	/**
	 * @brief	tcpclient constructor
	 */
	tcpclient() :
		keep_on_running(true),
		ctlid(0),
		auxid(0),
		dpid(0x0102030405060708),
		n_buffers(0xffff),
		n_tables(1),
		capabilities(0)
	{

	};

	/** @cond EXAMPLES */

public:

	/**
	 *
	 */
	int
	run(
			int argc, char** argv)
	{
		start();

		while (keep_on_running) {
			struct timespec ts;
			ts.tv_sec  = 2;
			ts.tv_nsec = 0;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		}

		return 0;
	};

	/**
	 * @brief	Static main routine for class cetherswitch
	 *
	 * Runs main event loop. Does not return.
	 *
	 * @param argc number of arguments given to main function
	 * @param argv array of pointers to arguments given to main function
	 */
	void
	start()
	{
		rofl::openflow::cofhello_elem_versionbitmap vbitmap;
		vbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
		vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);

		crofbase::
			add_ctl(ctlid).
				add_conn(auxid).
					set_laddr(rofl::csockaddr(AF_INET, "0.0.0.0",   0)).
					set_raddr(rofl::csockaddr(AF_INET, "127.0.0.1", 6653)).
					set_trace(true).
					tcp_connect(vbitmap, rofl::crofconn::MODE_DATAPATH, true);
	};

	/**
	 *
	 */
	void
	stop()
	{
		keep_on_running = false;
	};

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
	handle_ctl_open(
			rofl::crofctl& ctl)
	{

	};

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
	handle_ctl_close(
			const rofl::cctlid& ctlid)
	{

	};

	/**
	 * @brief	OpenFlow Features-Request message received.
	 *
	 * @param ctl   reference to crofctl instance
	 * @param auxid OpenFlow auxiliary connection identifier
	 * @param msg   Features-Request message received
	 */
	virtual void
	handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg)
	{
		ctl.send_features_reply(
				auxid,
				msg.get_xid(),
				dpid,
				n_buffers,
				n_tables,
				capabilities,
				auxid.get_id());

		stop();
	};

	/** @endcond */

public:

	friend std::ostream&
	operator<< (std::ostream& os, const tcpclient& client) {

		return os;
	};

private:

	bool                        keep_on_running;

	rofl::cctlid                ctlid;

	rofl::cauxid                auxid;

	uint64_t                    dpid;

	uint32_t                    n_buffers;

	uint8_t                     n_tables;

	uint32_t                    capabilities;
};

}; // namespace examples
}; // namespace rofl

#endif /* ROFL_EXAMPLES_TCPCLIENT_H */
