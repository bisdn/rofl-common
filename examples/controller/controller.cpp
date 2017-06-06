#include "controller.hpp"

using namespace rofl::examples;

bool keep_on_running = true;

void signal_handler(int signal) {
  switch (signal) {
  case SIGINT: {
    keep_on_running = false;
  } break;
  }
}

controller::~controller() {}

controller::controller() : dptid(0) {}

int controller::run(int argc, char **argv) {
  /* rofl-common supports OpenFlow versions 1.0, 1.2 and 1.3
   * Specify acceptable versions in a versionbitmap element
   * and update the element stored in base class rofl::crofbase */

  rofl::openflow::cofhello_elem_versionbitmap vbitmap;
  vbitmap.add_ofp_version(rofl::openflow10::OFP_VERSION);
  vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
  crofbase::set_versionbitmap(vbitmap);

  /* rofl-common provides all means for managing sockets for
   * incoming OpenFlow connections. For accepting connections
   * from remote datapath elements, rofl::crofbase provides
   * method 'dpt_sock_listen()'. It accepts an instance of
   * class rofl::csockaddr. */

  // register the initially used OpenFlow port number 6633
  crofbase::dpt_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 6633));

  /* You may open an arbitrary number of listening sockets */

  // register the IANA assigned OpenFlow port number 6653
  crofbase::dpt_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 6653));

  /* rofl::crofbase also provides method 'ctl_sock_listen()' for
   * accepting connection requests from controller entities.
   * Role of controller/datapath and client/server mode is
   * orthogonal in rofl-common, so you may actively connect()
   * or passively accept() TCP connections either in datapath
   * or controller role. */

  // register e.g. port number 3566 to act as a proxy
  crofbase::ctl_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 3566));

  /* rofl-common's OpenFlow protocol engine uses Posix threads
   * for its internal operations. rofl creates and destroys
   * threads automatically as needed, but make sure
   * that your main function keeps running, otherwise the process
   * ends too aggressively ... :) */

  while (keep_on_running) {
    struct timespec ts;
    ts.tv_sec = 2;
    ts.tv_nsec = 0;
    pselect(0, NULL, NULL, NULL, &ts, NULL);
  }

  return 0;
};

void controller::handle_dpt_open(rofl::crofdpt &dpt) {
  std::cerr << "EVENT: datapath attached" << std::endl;

  /* Class rofl::crofbase provides various notification handlers
   * for specific events including attachment/detachment of OpenFlow
   * peer entities. Method rofl::crofbase::handle_dpt_open() indicates
   * a new successful connection establishment to a remote datapath
   * entity. Method rofl::crofbase::handle_ctl_open() is the complemetary
   * function indicating attachment of a remote controller entity. */

  /* The rofl::crofdpt instance also provides the OpenFlow version
   * negotiated during connection establishment. */

  // dump the negotiated OpenFlow version
  std::cerr << "OpenFlow version negotiated: " << (int)dpt.get_version()
            << std::endl;

  // Dump the peer's OpenFlow datapath identifier (dpid), ...
  std::cerr << "datapath attached, dpid: 0x" << dpt.get_dpid() << std::endl;

  /* rofl-common maintains unique internal identifiers as handles
   * to rofl::crofdpt and rofl::crofctl instances in classes
   * rofl::cdptid and rofl::cctlid, respectively. Please note,
   * that these internal handles are not associated to OpenFlow's
   * datapath identifier (dpid).
   *
   * Store this identifier for later reference to a particular
   * instance. rofl::crofbase provides convenience methods for
   * obtaining a reference to a rofl::crofdpt/rofl::crofctl instance
   *
   * Please note, a rofl::crofdpt instance may be destroyed automatically
   * upon termination of the underlying OpenFlow control channel, so
   * DO NOT store references (or even pointers) to rofl::crofdpt
   * instances. Use rofl-common's internal handle only to get access. */

  // get rofl-common's internal handle to rofl::crofdpt instance 'dpt'
  const rofl::cdptid &reference_to_dptid = dpt.get_dptid();

  // get const reference to associated rofl::crofdpt instance from handle
  const rofl::crofdpt &const_reference_to_dpt = rofl::crofbase::get_dpt(dptid);

  // get non-const reference
  rofl::crofdpt &reference_to_dpt = rofl::crofbase::set_dpt(dptid);

  // store copy of rofl::cdptid
  this->dptid = dpt.get_dptid();

  // make compiler happy ...
  (void)reference_to_dpt;
  (void)reference_to_dptid;
  (void)const_reference_to_dpt;
}

void controller::handle_dpt_close(const rofl::cdptid &dptid) {
  std::cerr << "EVENT: datapath detached" << std::endl;

  /* Don't access rofl::crofdpt instance referenced by 'dptid'
   * when handle rofl::crofbase::handle_dpt_close() is called.
   * The underlying instance may be destroyed by rofl-common's
   * internal garbage collection any time.
   * Use this call for cleaning up any application state associated
   * with 'dptid'. */
}

void controller::handle_conn_established(rofl::crofdpt &dpt,
                                         const rofl::cauxid &auxid) {
  std::cerr << "EVENT: connection established, auxid:" << (int)auxid.get_id()
            << std::endl;

  /* Since version 1.3 OpenFlow defines the concept of a control
   * channel consisting of multiple control connections for load
   * distribution. rofl-common provides related handlers indicating
   * status changes for specific control connections. A control
   * connection is identified by an auxiliary identifier represented
   * by class rofl::cauxid. */

  /* When operating in controller role, rofl-common sends a Features.Request
   * message during connection establishment in order to determine the
   * datapath's identifier and the auxiliary identifier assigned to the
   * particular OpenFlow control connection. */

  // dump all control connection identifiers seen so far
  std::stringstream ss;
  for (auto const &id : dpt.get_conn_ids()) {
    ss << (int)id.get_id() << ", ";
  }
  std::cerr << "connection identifiers: " << ss.str() << std::endl;

  /* As indicated above, rofl-common maintains a journal for a control
   * channel and its control connections. This journal does not log
   * individual messages by default. However you may enable packet
   * tracing on a per control connection basis, which will generate
   * a log entry for each message received and sent by the control
   * connection. */

  // auxid(0) always represents the main control connection
  if (rofl::cauxid(0) == auxid) {

    /* OpenFlow defines various messages for querying a datapath
     * for its capabilities, ports, tables and group-features.
     * While rofl-common does not send queries automatically to
     * a remote datapath, it may store information received
     * from traversing reply messages, if initiated by the
     * higher layer application. This feature is called
     * snooping and is enabled by default. However, you may
     * disable snooping to save some memory, but keeping
     * it enabled allows you to use rofl::crofdpt as a data store
     * for ports, tables and group-features. */

    // enable snooping and store ports, tables and group-features (the default)
    dpt.set_snooping(true);

    /* Classes rofl::crofdpt and rofl::crofctl provide various
     * methods for sending OpenFlow messages to the remote peer.
     * Both synchronous and asynchronous messages may be sent.
     * You may select any of the active control connections to send
     * a request, but replies should always use the control connection
     * the request was received on. When acting in controller role and
     * sending a request message, a default timeout of 5 seconds is
     * used. You may specify another timeout value as needed. */

    // send an OpenFlow Features.Request message
    dpt.send_features_request(auxid);

    // send an OpenFlow GetConfig.Request message
    dpt.send_get_config_request(auxid);

    switch (dpt.get_version()) {
    case rofl::openflow13::OFP_VERSION: {

      // send an OpenFlow PortDescMultipart.Request message
      dpt.send_port_desc_stats_request(auxid, /*stats-flags*/ 0, /*timeout=*/5);

      // send an OpenFlow TableFeaturesMultipart.Request message
      dpt.send_table_features_stats_request(auxid, /*stats-flags*/ 0,
                                            /*timeout=*/5);

    } break;
    default: { /* ... */
    };
    }
  }
}

void controller::handle_conn_terminated(rofl::crofdpt &dpt,
                                        const rofl::cauxid &auxid) {
  std::cerr << "EVENT: connection terminated, auxid:" << (int)auxid.get_id()
            << std::endl;

  /* A peer entity may shutdown a single OpenFlow connection
   * inside an OpenFlow control channel any time. Unfortunately,
   * OpenFlow lacks proper means to signal the cause for a shutdown.
   * When a control connection terminates, rofl-common calls
   * method rofl::crofbase::handle_conn_terminated() indicating
   * the particular control connection via the rofl::cauxid instance.
   * When the main control connection terminates, rofl-common
   * terminates the channel's other auxiliary connections automatically.
   *
   * When acting in TCP server mode, rofl-common deletes the underlying
   * rofl::crofconn connection objects automatically. When acting in TCP
   * client mode, rofl::crofconn connection objects are not destroyed
   * and may be used for reestablishing the control connection.
   *
   * As a rule of thumb: Connections (rofl::crofconn instances) created by
   * the higher layer application must be destroyed by the application as well.
   * Connection instances created by server side sockets are automatically
   * removed upon connection shutdown.
   */
}

void controller::handle_conn_congestion_occurred(rofl::crofdpt &dpt,
                                                const rofl::cauxid &auxid) {
  std::cerr << "EVENT: congestion occured, auxid:" << (int)auxid.get_id()
            << std::endl;

  /* An OpenFlow control connection may suffer from insufficient
   * bandwidth for sending and receiving control messages. rofl-common
   * implements a sophisticated flow control mechanism to avoid
   * extensive buffering of messages inside the OpenFlow layer thus
   * avoiding buffer-bloat like situations.
   *
   * Each connection's transmission buffer size adapts automatically to
   * the bandwidth offered by the underlying TCP connection. If the
   * TCP socket indicates a blocking condition, rofl-common will call
   * method rofl::crofbase::handle_conn_congestion_occured(). The
   * application should stop sending further control messages via this
   * connection until a congestion resolution indication is received
   * from the library (see below). If an application persists on sending
   * further messages, such messages will be dropped and an exception of
   * type rofl::eRofQueueFull will be thrown, so you should throttle
   * your message generation rate and prepare your code for handling
   * exceptions properly. */
}

void controller::handle_conn_congestion_solved(rofl::crofdpt &dpt,
                                               const rofl::cauxid &auxid) {
  std::cerr << "EVENT: congestion resolved, auxid:" << (int)auxid.get_id()
            << std::endl;

  /* This is the indication used by rofl-common once a congestion situation
   * has been solved and complements the congestion indication method (see
   * above).
   * When called, the application may send further messages towards the peer
   * entity. */
}

void controller::handle_features_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_features_reply &msg) {
  /* rolf-common abstracts all OpenFlow messages in C++ classes
   * for simple and easy handling of their content.
   * There are also individual notification methods for each
   * OpenFlow message (including requests and replies, asynchronous
   * and synchronous messages). See rofl::crofbase for the comprehensive
   * list of these methods and overwrite those you are interested in.
   *
   * Here, we have overwritten rofl::crofbase::handle_features_reply()
   * and obtain a reference to a features reply message. You must not refer
   * to this message once scope of this handler has concluded. If
   * you want to keep the message, please create a copy and store it
   * in the application layer.
   *
   * All rofl-common handlers are executed within scope of rofl-internal
   * threads, so please provide proper locking of application data
   * structures when manipulating those inside the handler methods. */

  // rofl::openflow::cofmsg_features_reply C++ API

  // datapath's number of packet buffers, ...
  uint32_t n_buffers = msg.get_n_buffers();

  // datapath pipeline's number of tables, ...
  uint8_t n_tables = msg.get_n_tables();

  // datapath pipeline's capabilities, ...
  uint32_t capabilities = msg.get_capabilities();

  // datapath dpid, ...
  uint64_t dpid = msg.get_dpid();

  // datapath ports (only valid for Openflow 1.0 and 1.2)
  const rofl::openflow::cofports &ports = msg.get_ports();

  /* When snooping has been enabled on an OpenFlow control connection,
   * rofl-common stores data from various OpenFlow requests in an
   * instance of type rofl::crofdpt for the connection's lifetime.
   * So you may acquire these data by querying the rofl::crofdpt
   * instance as well. */

  // datapath's number of packet buffers, ...
  std::cerr << "#buffers: " << (unsigned int)dpt.get_n_buffers() << std::endl;

  // datapath pipeline's number of tables, ...
  std::cerr << "#tables: " << (unsigned int)dpt.get_n_tables() << std::endl;

  // datapath pipeline's capabilities, ...
  std::cerr << "capabilities: " << (unsigned int)dpt.get_capabilities()
            << std::endl;

  // datapath hwaddr derived from dpid, ...
  std::cerr << "hwaddr: " << dpt.get_hwaddr().str() << std::endl;

  (void)n_buffers;
  (void)n_tables;
  (void)capabilities;
  (void)dpid;
  (void)ports;
}

void controller::handle_features_reply_timeout(rofl::crofdpt &dpt,
                                               uint32_t xid) {
  /* rofl-common maintains timers for pending requests and provides
   * notification methods indicating timer expirations. Overwrite
   * those notifiers you are interested in, e.g., for a Features.Request
   * method rofl::crofbase::hanle_features_reply_timeout(). */

  // transaction identifier of Features.Request whose timer expired
  (void)xid;
}

void controller::handle_get_config_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_get_config_reply &msg) {
  // the OpenFlow configuration flags, ...
  std::cerr << "configuration flags: " << (unsigned int)dpt.get_flags()
            << std::endl;

  // the datapath pipeline's miss_send_len value, ...
  std::cerr << "miss-send-len: " << (unsigned int)dpt.get_miss_send_len()
            << std::endl;
}

void controller::handle_get_config_reply_timeout(rofl::crofdpt &dpt,
                                                 uint32_t xid) {
  /* rofl-common maintains timers for pending requests and provides
   * notification methods indicating timer expirations. Overwrite
   * those notifiers you are interested in, e.g., for a Get-Config.Request
   * method rofl::crofbase::hanle_get_config_reply_timeout(). */

  // transaction identifier of Get-Config.Request whose timer expired
  (void)xid;
}

void controller::handle_port_desc_stats_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_port_desc_stats_reply &msg) {
  std::cerr << "port description received; " << dpt.get_ports() << std::endl;

  /* Class rofl::openflow::cofports defines a container for storing
   * a set of OpenFlow port descriptions. Its keys() method returns
   * a list of valid 32bit port numbers for this instance. */

  for (auto portno : msg.get_ports().keys()) {

    /* Class rofl::openflow::cofport defines an abstraction of
     * a single OpenFlow port. */
    const rofl::openflow::cofport &port = msg.get_ports().get_port(portno);

    // OpenFlow port number ...
    uint32_t port_no = port.get_port_no();

    // OpenFlow port config field ...
    uint32_t config = port.get_config();

    // OpenFlow port state field ...
    uint32_t state = port.get_state();

    // OpenFlow port name ...
    const std::string &name = port.get_name();

    // OpenFlow port link layer address ...
    const rofl::caddress_ll &hwaddr = port.get_hwaddr();

    // OpenFlow port statistics ...
    const rofl::openflow::cofport_stats_reply &stats = port.get_port_stats();

    // OpenFlow port ethernet parameters ...
    const rofl::openflow::cofportdesc_prop_ethernet &ethernet =
        port.get_ethernet();

    /*uint32_t*/ ethernet.get_advertised();
    /*uint32_t*/ ethernet.get_curr();
    /*uint32_t*/ ethernet.get_curr_speed();
    /*uint32_t*/ ethernet.get_max_speed();
    /*uint32_t*/ ethernet.get_peer();
    /*uint32_t*/ ethernet.get_supported();
    /*uint16_t*/ ethernet.get_type();

    // make compiler happy ...
    (void)port_no;
    (void)config;
    (void)state;
    (void)name;
    (void)hwaddr;
    (void)stats;
  }

  /* OpenFlow is a binary protocol and provides many (deeply) nested
   * data structures. rofl-common provides a homogeneous API following
   * some core principles to simplify use of OpenFlow.
   *
   * OpenFlow defines various containers for storing elements of the same kind,
   * so it is inevitable to conduct CRUD like operations on OpenFlow instances
   * frequently. rofl-common defines the following CRUD operations and you
   * will encounter these basic methods almost everywhere in rofl-common:
   *
   * - Adding a new or resetting an existing element:
   *   element& add_element();
   *
   * - Returning an existing or creating a new element if none exists:
   *   element& set_element();
   *
   * - Returning a const reference to an existing element or throw an exception:
   *   const element& get_element() const;
   *
   * - Remove an element (fails silently, if non exists):
   *   bool drop_element();
   *
   * - Check for existence of specific element:
   *   bool has_element() const;
   */
  rofl::openflow::cofports ports(rofl::openflow13::OFP_VERSION);

  // add a new rofl::openflow::cofport instance
  rofl::openflow::cofport &a_new_port = ports.add_port(/*portno=*/1);

  // get reference to existing port or create new empty one
  rofl::openflow::cofport &an_existing_port = ports.set_port(/*portno=*/1);

  // get const reference to existing port
  try {
    const rofl::openflow::cofport &an_existing_port =
        ports.get_port(/*portno=*/1);
    (void)an_existing_port;

  } catch (rofl::openflow::ePortsNotFound &e) {
    /* ... */
  }

  // remove existing port
  ports.drop_port(/*portno=*/1);

  // check for port
  if (ports.has_port(/*portno=*/1)) {
    /* ... */
  }

  (void)a_new_port;
  (void)an_existing_port;
}

void controller::handle_port_desc_stats_reply_timeout(rofl::crofdpt &dpt,
                                                      uint32_t xid) {
  std::cerr << "Port-Desc-Stats timeout occured, xid: " << (unsigned int)xid
            << std::endl;
}

void controller::handle_table_features_stats_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_table_features_stats_reply &msg) {
  std::cerr << "Table-Features-Stats received; " << dpt.get_tables()
            << std::endl;
}

void controller::handle_table_features_stats_reply_timeout(rofl::crofdpt &dpt,
                                                           uint32_t xid) {
  std::cerr << "Table-Features-Stats timeout occured, xid: "
            << (unsigned int)xid << std::endl;
}

void controller::handle_table_stats_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_table_stats_reply &msg) {
  std::cerr << "Table-Stats received; " << dpt.get_tables() << std::endl;
}

void controller::handle_table_stats_reply_timeout(rofl::crofdpt &dpt,
                                                  uint32_t xid) {
  std::cerr << "Table-Stats timeout occured, xid: " << (unsigned int)xid
            << std::endl;
}
