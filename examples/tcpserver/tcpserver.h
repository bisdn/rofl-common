#ifndef ROFL_EXAMPLES_TCPSERVER_H
#define ROFL_EXAMPLES_TCPSERVER_H 1

#include <inttypes.h>
#include <map>
#include <signal.h>

#include <rofl/common/crofbase.h>

namespace rofl {
namespace examples {

/**
 * @ingroup common_howto_tcpserver
 *
 * @brief	A simple server with OpenFlow endpoint connecting to a control
 * plane
 *
 * A simple example for using rofl to connect to a controller entity.
 */
class tcpserver : public rofl::crofbase {
public:
  /**
   * @brief	tcpserver destructor
   */
  virtual ~tcpserver(){

  };

  /**
   * @brief	tcpserver constructor
   */
  tcpserver()
      : keep_on_running(true){

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
    vbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
    vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);

    crofbase::set_versionbitmap(vbitmap);
    crofbase::dpt_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 6653));
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
  virtual void handle_dpt_open(rofl::crofdpt &dpt) {
    std::cerr << "datapath attached dptid=" << dpt.get_dptid() << std::endl;
    dpt.send_port_desc_stats_request(rofl::cauxid(0), 0, 5);
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
  virtual void handle_dpt_close(const rofl::cdptid &dptid) {
    std::cerr << "datapath detached dptid=" << dptid << std::endl;

    stop();
  };

  /**
   * @brief	OpenFlow Features-Request message received.
   *
   * @param ctl   reference to crofctl instance
   * @param auxid OpenFlow auxiliary connection identifier
   * @param msg   Features-Request message received
   */
  virtual void handle_port_desc_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_port_desc_stats_reply &msg) {
    std::cerr << "port description received; " << dpt.get_ports() << std::endl;

    switch (dpt.get_version()) {
    case rofl::openflow13::OFP_VERSION: {
      dpt.send_table_features_stats_request(auxid, 0, 5);
    } break;
    case rofl::openflow12::OFP_VERSION: {
      dpt.send_table_stats_request(auxid, 0, 5);
    } break;
    default: { /* never occurs */
    };
    }
  };

  /**
   * @brief	Timer expired while waiting for OpenFlow Port-Desc-Stats-Reply
   * message.
   *
   * No Port-Desc-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_port_desc_stats_reply_timeout(rofl::crofdpt &dpt,
                                                    uint32_t xid) {
    std::cerr << "Port-Desc-Stats timeout occurred, xid: " << (unsigned int)xid
              << std::endl;
  };

  /**
   * @brief	OpenFlow Table-Features-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_features_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_table_features_stats_reply &msg) {
    std::cerr << "Table-Features-Stats received; " << dpt.get_tables()
              << std::endl;
  };

  /**
   * @brief	Timer expired while waiting for OpenFlow
   * Table-Features-Stats-Reply message.
   *
   * No Table-Features-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_table_features_stats_reply_timeout(rofl::crofdpt &dpt,
                                                         uint32_t xid) {
    std::cerr << "Table-Features-Stats timeout occurred, xid: "
              << (unsigned int)xid << std::endl;
  };

  /**
   * @brief	OpenFlow Table-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_table_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg_table_stats_reply &msg) {
    std::cerr << "Table-Stats received; " << dpt.get_tables() << std::endl;
  };

  /**
   * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply
   * message.
   *
   * No Table-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_table_stats_reply_timeout(rofl::crofdpt &dpt,
                                                uint32_t xid) {
    std::cerr << "Table-Stats timeout occurred, xid: " << (unsigned int)xid
              << std::endl;
  };

  /** @endcond */

private:
  bool keep_on_running;
};

}; // namespace examples
}; // namespace rofl

#endif /* ROFL_EXAMPLES_TCPSERVER_H */
