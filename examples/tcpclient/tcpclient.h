#ifndef ROFL_EXAMPLES_TCPCLIENT_H
#define ROFL_EXAMPLES_TCPCLIENT_H 1

#include <inttypes.h>
#include <map>
#include <signal.h>

#include <rofl/common/crofbase.h>

namespace rofl {
namespace examples {

/**
 * @ingroup common_howto_tcpclient
 *
 * @brief	A simple client with OpenFlow endpoint connecting to a control
 * plane
 *
 * A simple example for using rofl to connect to a controller entity.
 */
class tcpclient : public rofl::crofbase {
public:
  /**
   * @brief	tcpclient destructor
   */
  virtual ~tcpclient(){

  };

  /**
   * @brief	tcpclient constructor
   */
  tcpclient()
      : keep_on_running(true), ctlid(0), auxid(0), dpid(0x0102030405060708),
        n_buffers(0xffff), n_tables(1), capabilities(0){

                                        };

  /** @cond EXAMPLES */

public:
  /**
   *
   */
  int run(int argc, char **argv) {
    start();

    while (keep_on_running) {
      struct timespec ts;
      ts.tv_sec = 2;
      ts.tv_nsec = 0;
      pselect(0, NULL, NULL, NULL, &ts, NULL);
    }

    crofbase::set_ctl(ctlid).close();

    crofbase::drop_ctl(ctlid);

    unsigned int shutdown = 4;
    while (--shutdown > 0) {
      std::cerr << ".";
      sleep(1);
    }
    std::cerr << std::endl;

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
  void start() {
    rofl::openflow::cofhello_elem_versionbitmap vbitmap;
    vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);

    crofbase::add_ctl(ctlid)
        .add_conn(auxid)
        .set_laddr(rofl::csockaddr(AF_INET, "0.0.0.0", 0))
        .set_raddr(rofl::csockaddr(AF_INET, "127.0.0.1", 6653))
        .tcp_connect(vbitmap, rofl::crofconn::MODE_DATAPATH, true);
  };

  /**
   *
   */
  void stop() { keep_on_running = false; };

private:
  /**
   * @brief	Called after establishing the associated OpenFlow control
   * channel.
   *
   * This method is called once the associated OpenFlow control channel has
   * been established, i.e., its main connection has been accepted by the remote
   * site.
   *
   * @param dpt datapath instance
   */
  virtual void handle_ctl_open(rofl::crofctl &ctl) {
    std::cerr << "controller attached ctlid=" << ctl.get_ctlid() << std::endl;
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
  virtual void handle_ctl_close(const rofl::cctlid &ctlid) {
    std::cerr << "controller detached ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * established.
   *
   * @param ctl controller instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_established(rofl::crofctl &ctl,
                                       const rofl::cauxid &auxid) {
    std::cerr << "channel established ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * terminated by the peer entity.
   *
   * @param ctl controller instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_terminated(rofl::crofctl &ctl,
                                      const rofl::cauxid &auxid) {
    std::cerr << "channel terminated ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * been
   * refused.
   *
   * This event occurs when the C-library's connect() system call fails
   * with the ECONNREFUSED error code. This indicates typically a problem on
   * the remote site.
   *
   * @param ctl controller instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_refused(rofl::crofctl &ctl,
                                   const rofl::cauxid &auxid) {
    std::cerr << "channel connect refused ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * been
   * failed.
   *
   * This event occurs when some failure occures while calling the underlying
   * C-library connect() system call, e.g., no route to destination, etc. This
   * may
   * indicate a local configuration problem inside or outside of the
   * application.
   *
   * @param ctl controller instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_failed(rofl::crofctl &ctl,
                                  const rofl::cauxid &auxid) {
    std::cerr << "channel connect failed ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief	Called when a negotiation failed with a peer controller entity
   *
   * @param ctl controller instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_negotiation_failed(rofl::crofctl &ctl,
                                              const rofl::cauxid &auxid) {
    std::cerr << "channel failed ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief	Called when a congestion situation on the control connection
   * occurs
   *
   * @param ctl controller instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_congestion_occurred(rofl::crofctl &ctl,
                                              const rofl::cauxid &auxid) {
    std::cerr << "channel congestion occured, ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief	Called when a congestion situation on the control connection has
   * been solved
   *
   * @param ctl controller instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_congestion_solved(rofl::crofctl &ctl,
                                             const rofl::cauxid &auxid) {
    std::cerr << "channel congestion solved, ctlid=" << ctlid << std::endl;
  };

  /**
   * @brief	OpenFlow Features-Request message received.
   *
   * @param ctl   reference to crofctl instance
   * @param auxid OpenFlow auxiliary connection identifier
   * @param msg   Features-Request message received
   */
  virtual void
  handle_features_request(rofl::crofctl &ctl, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_features_request &msg) {
    std::cerr << "received Features-Request" << std::endl;

    ctl.send_features_reply(auxid, msg.get_xid(), dpid, n_buffers, n_tables,
                            capabilities, auxid.get_id());
  };

  /**
   *
   */
  virtual void handle_port_desc_stats_request(
      rofl::crofctl &ctl, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_port_desc_stats_request &msg) {
    std::cerr << "received Port-Desc-Stats-Request" << std::endl;

    ports.set_version(ctl.get_version());

    ports.add_port(0).set_hwaddr(rofl::caddress_ll("00:11:11:11:11:11"));
    ports.set_port(0).set_name("port#0");
    ports.set_port(0).set_ethernet().set_advertised(0xa1a2a3a4);
    ports.set_port(0).set_ethernet().set_max_speed(0xb1b2b3b4);

    ports.add_port(1).set_hwaddr(rofl::caddress_ll("00:22:22:22:22:22"));
    ports.set_port(1).set_name("port#1");
    ports.set_port(1).set_ethernet().set_advertised(0xa1a2a3a4);
    ports.set_port(1).set_ethernet().set_max_speed(0xb1b2b3b4);

    ctl.send_port_desc_stats_reply(auxid, msg.get_xid(), ports);
  };

  /**
   * @brief	OpenFlow Table-Features-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_features_stats_request(
      rofl::crofctl &ctl, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_table_features_stats_request &msg) {
    rofl::openflow::coftables tables;

    tables.add_table(0).set_max_entries(1024);
    tables.set_table(0).set_name("table#0");
    tables.add_table(1).set_max_entries(1024);
    tables.set_table(1).set_name("table#1");
    tables.add_table(2).set_max_entries(1024);
    tables.set_table(2).set_name("table#2");

    ctl.send_table_features_stats_reply(auxid, msg.get_xid(), tables);

    stop();
  };

  /**
   * @brief	OpenFlow Table-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_table_stats_request(rofl::crofctl &ctl, const rofl::cauxid &auxid,
                             rofl::openflow::cofmsg_table_stats_request &msg) {
    rofl::openflow::coftablestatsarray tablestatsarray;

    tablestatsarray.add_table_stats(0).set_name("table#0");
    tablestatsarray.set_table_stats(0).set_max_entries(1024);
    tablestatsarray.add_table_stats(1).set_name("table#1");
    tablestatsarray.set_table_stats(1).set_max_entries(1024);
    tablestatsarray.add_table_stats(2).set_name("table#2");
    tablestatsarray.set_table_stats(2).set_max_entries(1024);

    ctl.send_table_stats_reply(auxid, msg.get_xid(), tablestatsarray);

    stop();
  };

  /** @endcond */

public:
  friend std::ostream &operator<<(std::ostream &os, const tcpclient &client) {

    return os;
  };

private:
  bool keep_on_running;

  rofl::cctlid ctlid;

  rofl::cauxid auxid;

  uint64_t dpid;

  uint32_t n_buffers;

  uint8_t n_tables;

  uint32_t capabilities;

  rofl::openflow::cofports ports;
};

}; // namespace examples
}; // namespace rofl

#endif /* ROFL_EXAMPLES_TCPCLIENT_H */
