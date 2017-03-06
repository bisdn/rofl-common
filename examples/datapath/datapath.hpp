#ifndef ROFL_EXAMPLES_DATAPATH_H
#define ROFL_EXAMPLES_DATAPATH_H 1

#include <inttypes.h>
#include <map>
#include <signal.h>

#include <rofl/common/rofcommon.h>

namespace rofl {
namespace examples {

/**
 * @ingroup common_howto_datapath
 *
 * @brief	A simple client with OpenFlow endpoint connecting to a control
 * plane
 *
 * A simple example for using rofl to connect to a controller entity.
 */
class datapath : public rofl::crofbase {
public:
  /**
   * @brief	datapath destructor
   */
  virtual ~datapath();

  /**
   * @brief	datapath constructor
   */
  datapath();

  /** @cond EXAMPLES */

public:
  /**
   *
   */
  int run(int argc, char **argv);

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
  virtual void handle_ctl_open(rofl::crofctl &ctl);

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
  virtual void handle_ctl_close(const rofl::cctlid &ctlid);

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * established.
   *
   * @param ctl controller instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_established(rofl::crofctl &ctl,
                                       const rofl::cauxid &auxid);

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * terminated by the peer entity.
   *
   * @param ctl controller instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_terminated(rofl::crofctl &ctl,
                                      const rofl::cauxid &auxid);

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
                                   const rofl::cauxid &auxid);

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
                                  const rofl::cauxid &auxid);

  /**
   * @brief	Called when a negotiation failed with a peer controller entity
   *
   * @param ctl controller instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_negotiation_failed(rofl::crofctl &ctl,
                                              const rofl::cauxid &auxid);

  /**
   * @brief	Called when a congestion situation on the control connection
   * occurs
   *
   * @param ctl controller instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_congestion_occured(rofl::crofctl &ctl,
                                              const rofl::cauxid &auxid);

  /**
   * @brief	Called when a congestion situation on the control connection has
   * been solved
   *
   * @param ctl controller instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_congestion_solved(rofl::crofctl &ctl,
                                             const rofl::cauxid &auxid);

  /**
   * @brief	OpenFlow Features-Request message received.
   *
   * @param ctl   reference to crofctl instance
   * @param auxid OpenFlow auxiliary connection identifier
   * @param msg   Features-Request message received
   */
  virtual void
  handle_features_request(rofl::crofctl &ctl, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_features_request &msg);

  /**
   *
   */
  virtual void handle_port_desc_stats_request(
      rofl::crofctl &ctl, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_port_desc_stats_request &msg);

  /**
   * @brief	OpenFlow Table-Features-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_features_stats_request(
      rofl::crofctl &ctl, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_table_features_stats_request &msg);

  /**
   * @brief	OpenFlow Table-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_table_stats_request(rofl::crofctl &ctl, const rofl::cauxid &auxid,
                             rofl::openflow::cofmsg_table_stats_request &msg);

  /** @endcond */

public:
  friend std::ostream &operator<<(std::ostream &os, const datapath &client) {

    return os;
  };

private:
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
