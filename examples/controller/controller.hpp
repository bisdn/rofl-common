#ifndef ROFL_EXAMPLES_CONTROLLER_H
#define ROFL_EXAMPLES_CONTROLLER_H 1

#include <inttypes.h>
#include <map>
#include <signal.h>

#include <rofl/common/rofcommon.h>

namespace rofl {
namespace examples {

/**
 * @ingroup common_howto_controller
 *
 * @brief	A simple server with OpenFlow endpoint connecting to a control
 * plane
 *
 * A simple example for using rofl to connect to a controller entity.
 */
class controller : public rofl::crofbase {
public:
  /**
   * @brief	controller destructor
   */
  virtual ~controller();

  /**
   * @brief	controller constructor
   */
  controller();

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
  virtual void handle_dpt_open(rofl::crofdpt &dpt);

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
  virtual void handle_dpt_close(const rofl::cdptid &dptid);

  /**
   * @brief	OpenFlow Features-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_features_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                        rofl::openflow::cofmsg_features_reply &msg);

  /**
   * @brief	Timer expired while waiting for OpenFlow Features-Reply message.
   *
   * No Features-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_features_reply_timeout(rofl::crofdpt &dpt, uint32_t xid);

  /**
   * @brief	OpenFlow Get-Config-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_get_config_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_get_config_reply &msg);

  /**
   * @brief	Timer expired while waiting for OpenFlow Get-Config-Reply
   * message.
   *
   * No Get-Config-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_get_config_reply_timeout(rofl::crofdpt &dpt,
                                               uint32_t xid);

  /**
   * @brief	OpenFlow Features-Request message received.
   *
   * @param ctl   reference to crofctl instance
   * @param auxid OpenFlow auxiliary connection identifier
   * @param msg   Features-Request message received
   */
  virtual void handle_port_desc_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_port_desc_stats_reply &msg);

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
                                                    uint32_t xid);

  /**
   * @brief	OpenFlow Table-Features-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_features_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_table_features_stats_reply &msg);

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
                                                         uint32_t xid);

  /**
   * @brief	OpenFlow Table-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_table_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg_table_stats_reply &msg);

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
                                                uint32_t xid);

  /** @endcond */

private:
  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * established.
   *
   * @param dpt datapath instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_established(rofl::crofdpt &dpt,
                                       const rofl::cauxid &auxid);

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * terminated by the peer entity.
   *
   * @param dpt datapath instance
   * @param auxid connection identifier (main: 0)
   */
  virtual void handle_conn_terminated(rofl::crofdpt &dpt,
                                      const rofl::cauxid &auxid);

  /**
   * @brief	Called when a congestion situation on the control connection
   * occurs
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_congestion_occurred(rofl::crofdpt &dpt,
                                               const rofl::cauxid &auxid);

  /**
   * @brief	Called when a congestion situation on the control connection has
   * been solved
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier (main: 0)
   */
  virtual void handle_conn_congestion_solved(rofl::crofdpt &dpt,
                                             const rofl::cauxid &auxid);

private:
  rofl::cdptid dptid;
};

}; // namespace examples
}; // namespace rofl

#endif /* ROFL_EXAMPLES_TCPSERVER_H */
