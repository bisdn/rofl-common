#include "datapath.hpp"

using namespace rofl::examples;

bool keep_on_running = true;

void signal_handler(int signal) {
  switch (signal) {
  case SIGINT: {
    keep_on_running = false;
  } break;
  }
}

datapath::~datapath() {}

datapath::datapath()
    : ctlid(0), auxid(0), dpid(0x0102030405060708), n_buffers(0xffff),
      n_tables(8), capabilities(0xa1a2a3a4) {}

int datapath::run(int argc, char **argv) {
  /* rofl-common supports OpenFlow versions 1.0, 1.2 and 1.3
   * Specify acceptable versions in a versionbitmap element
   * and update the element stored in base class rofl::crofbase */

  rofl::openflow::cofhello_elem_versionbitmap vbitmap;
  vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);

  ctlid = rofl::cctlid(0);
  auxid = rofl::cauxid(0);

  for (uint8_t i = 0; i < 4; i++) {
    rofl::crofbase::set_ctl(ctlid)
        .add_conn(rofl::cauxid(i))
        .set_laddr(rofl::csockaddr(AF_INET, "0.0.0.0", 0))
        .set_raddr(rofl::csockaddr(AF_INET, "127.0.0.1", 6653))
        .tcp_connect(vbitmap, rofl::crofconn::MODE_DATAPATH, true);
  }

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
}

void datapath::handle_ctl_open(rofl::crofctl &ctl) {
  std::cerr << "controller attached ctlid=" << ctl.get_ctlid() << std::endl;
}

void datapath::handle_ctl_close(const rofl::cctlid &ctlid) {
  std::cerr << "controller detached ctlid=" << ctlid << std::endl;
}

void datapath::handle_conn_established(rofl::crofctl &ctl,
                                       const rofl::cauxid &auxid) {
  std::cerr << "connection established ctlid=" << ctlid.get_ctlid_s()
            << " auxid=" << (int)auxid.get_id() << std::endl;
}

void datapath::handle_conn_terminated(rofl::crofctl &ctl,
                                      const rofl::cauxid &auxid) {
  std::cerr << "channel terminated ctlid=" << ctlid << std::endl;
}

void datapath::handle_conn_refused(rofl::crofctl &ctl,
                                   const rofl::cauxid &auxid) {
  std::cerr << "channel connect refused ctlid=" << ctlid << std::endl;
}

void datapath::handle_conn_failed(rofl::crofctl &ctl,
                                  const rofl::cauxid &auxid) {
  std::cerr << "channel connect failed ctlid=" << ctlid << std::endl;
}

void datapath::handle_conn_negotiation_failed(rofl::crofctl &ctl,
                                              const rofl::cauxid &auxid) {
  std::cerr << "channel failed ctlid=" << ctlid << std::endl;
}

void datapath::handle_conn_congestion_occurred(rofl::crofctl &ctl,
                                              const rofl::cauxid &auxid) {
  std::cerr << "channel congestion occured, ctlid=" << ctlid << std::endl;
}

void datapath::handle_conn_congestion_solved(rofl::crofctl &ctl,
                                             const rofl::cauxid &auxid) {
  std::cerr << "channel congestion solved, ctlid=" << ctlid << std::endl;
}

void datapath::handle_features_request(
    rofl::crofctl &ctl, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_features_request &msg) {
  std::cerr << "received Features-Request ctlid=" << ctlid.get_ctlid_s()
            << " auxid=" << (int)auxid.get_id() << std::endl;

  ctl.send_features_reply(auxid, msg.get_xid(), dpid, n_buffers, n_tables,
                          capabilities, auxid.get_id());
}

void datapath::handle_port_desc_stats_request(
    rofl::crofctl &ctl, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_port_desc_stats_request &msg) {
  std::cerr << "received Port-Desc-Stats-Request ctlid=" << ctlid.get_ctlid_s()
            << " auxid=" << (int)auxid.get_id() << std::endl;

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
}

void datapath::handle_table_features_stats_request(
    rofl::crofctl &ctl, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_table_features_stats_request &msg) {
  std::cerr << "received Table-Features-Stats-Request ctlid="
            << ctlid.get_ctlid_s() << " auxid=" << (int)auxid.get_id()
            << std::endl;

  rofl::openflow::coftables tables;

  tables.add_table(0).set_max_entries(1024);
  tables.set_table(0).set_name("table#0");
  tables.add_table(1).set_max_entries(1024);
  tables.set_table(1).set_name("table#1");
  tables.add_table(2).set_max_entries(1024);
  tables.set_table(2).set_name("table#2");

  ctl.send_table_features_stats_reply(auxid, msg.get_xid(), tables);
}

void datapath::handle_table_stats_request(
    rofl::crofctl &ctl, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_table_stats_request &msg) {
  std::cerr << "received Table-Stats-Request ctlid=" << ctlid.get_ctlid_s()
            << " auxid=" << (int)auxid.get_id() << std::endl;

  rofl::openflow::coftablestatsarray tablestatsarray;

  tablestatsarray.add_table_stats(0).set_name("table#0");
  tablestatsarray.set_table_stats(0).set_max_entries(1024);
  tablestatsarray.add_table_stats(1).set_name("table#1");
  tablestatsarray.set_table_stats(1).set_max_entries(1024);
  tablestatsarray.add_table_stats(2).set_name("table#2");
  tablestatsarray.set_table_stats(2).set_max_entries(1024);

  ctl.send_table_stats_reply(auxid, msg.get_xid(), tablestatsarray);
};
