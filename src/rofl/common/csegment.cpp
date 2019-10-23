/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * csegment.cpp
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegment.hpp"

using namespace rofl;

void csegment::clone(const rofl::openflow::cofmsg &msg_stats) {
  if (NULL != msg) {
    delete msg;
    msg = NULL;
  }

  uint16_t stats_type = 0;
  if (dynamic_cast<const rofl::openflow::cofmsg_stats_request *>(&msg_stats)) {
    stats_type =
        dynamic_cast<const rofl::openflow::cofmsg_stats_request &>(msg_stats)
            .get_stats_type();
  } else if (dynamic_cast<const rofl::openflow::cofmsg_stats_reply *>(
                 &msg_stats)) {
    stats_type =
        dynamic_cast<const rofl::openflow::cofmsg_stats_reply &>(msg_stats)
            .get_stats_type();
  }

  switch (msg_stats.get_type()) {
  case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
    switch (stats_type) {
    case rofl::openflow13::OFPMP_DESC: {
      msg = new rofl::openflow::cofmsg_desc_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_FLOW: {
      msg = new rofl::openflow::cofmsg_flow_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_AGGREGATE: {
      msg = new rofl::openflow::cofmsg_aggr_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_TABLE: {
      msg = new rofl::openflow::cofmsg_table_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_table_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_PORT_STATS: {
      msg = new rofl::openflow::cofmsg_port_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_port_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_QUEUE: {
      msg = new rofl::openflow::cofmsg_queue_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_queue_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_GROUP: {
      msg = new rofl::openflow::cofmsg_group_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_group_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_GROUP_DESC: {
      msg = new rofl::openflow::cofmsg_group_desc_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_GROUP_FEATURES: {
      msg = new rofl::openflow::cofmsg_group_features_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_METER: {
      msg = new rofl::openflow::cofmsg_meter_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_meter_stats_request const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_METER_CONFIG: {
      msg = new rofl::openflow::cofmsg_meter_config_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_METER_FEATURES: {
      msg = new rofl::openflow::cofmsg_meter_features_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_TABLE_FEATURES: {
      msg = new rofl::openflow::cofmsg_table_features_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_PORT_DESC: {
      msg = new rofl::openflow::cofmsg_port_desc_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request const &>(
              msg_stats));
    } break;
    default: {
      msg = new rofl::openflow::cofmsg_stats_request(
          dynamic_cast<rofl::openflow::cofmsg_stats_request const &>(
              msg_stats));
    };
    }

    if (NULL != msg) {
      if (dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg)) {
        rofl::openflow::cofmsg_stats_request &req =
            dynamic_cast<rofl::openflow::cofmsg_stats_request &>(*msg);
        req.set_stats_flags(req.get_stats_flags() &
                            ~rofl::openflow13::OFPMPF_REQ_MORE);
      } else if (dynamic_cast<rofl::openflow::cofmsg_stats_reply *>(msg)) {
        rofl::openflow::cofmsg_stats_reply &rep =
            dynamic_cast<rofl::openflow::cofmsg_stats_reply &>(*msg);
        rep.set_stats_flags(rep.get_stats_flags() &
                            ~rofl::openflow13::OFPMPF_REQ_MORE);
      }
    }

  } break;
  case rofl::openflow13::OFPT_MULTIPART_REPLY: {
    switch (stats_type) {
    case rofl::openflow13::OFPMP_DESC: {
      msg = new rofl::openflow::cofmsg_desc_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_FLOW: {
      msg = new rofl::openflow::cofmsg_flow_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_AGGREGATE: {
      msg = new rofl::openflow::cofmsg_aggr_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_TABLE: {
      msg = new rofl::openflow::cofmsg_table_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_PORT_STATS: {
      msg = new rofl::openflow::cofmsg_port_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_QUEUE: {
      msg = new rofl::openflow::cofmsg_queue_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_GROUP: {
      msg = new rofl::openflow::cofmsg_group_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_GROUP_DESC: {
      msg = new rofl::openflow::cofmsg_group_desc_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_GROUP_FEATURES: {
      msg = new rofl::openflow::cofmsg_group_features_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_METER: {
      msg = new rofl::openflow::cofmsg_meter_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_METER_CONFIG: {
      msg = new rofl::openflow::cofmsg_meter_config_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply const &>(
              msg_stats));
    } break;
    case rofl::openflow13::OFPMP_METER_FEATURES: {
      msg = new rofl::openflow::cofmsg_meter_features_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_reply const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_TABLE_FEATURES: {
      msg = new rofl::openflow::cofmsg_table_features_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply const
                           &>(msg_stats));
    } break;
    case rofl::openflow13::OFPMP_PORT_DESC: {
      msg = new rofl::openflow::cofmsg_port_desc_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply const &>(
              msg_stats));
    } break;
    default: {
      msg = new rofl::openflow::cofmsg_stats_reply(
          dynamic_cast<rofl::openflow::cofmsg_stats_reply const &>(msg_stats));
    };
    }

    if (NULL != msg) {
      if (dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg)) {
        rofl::openflow::cofmsg_stats_request &req =
            dynamic_cast<rofl::openflow::cofmsg_stats_request &>(*msg);
        req.set_stats_flags(req.get_stats_flags() &
                            ~rofl::openflow13::OFPMPF_REQ_MORE);
      } else if (dynamic_cast<rofl::openflow::cofmsg_stats_reply *>(msg)) {
        rofl::openflow::cofmsg_stats_reply &rep =
            dynamic_cast<rofl::openflow::cofmsg_stats_reply &>(*msg);
        rep.set_stats_flags(rep.get_stats_flags() &
                            ~rofl::openflow13::OFPMPF_REQ_MORE);
      }
    }

  } break;
  default: {
    // oops
  };
  }
}

void csegment::store_and_merge_msg(const rofl::openflow::cofmsg &msg_stats) {
  if (NULL == msg) {

    csegment::clone(msg_stats);

  } else {

    /* sanity checks */

    /* message type must match */
    if (msg_type != msg_stats.get_type()) {
      throw eSegmentedMessageInvalid(
          "csegment::store_and_merge_msg(), invalid msg type")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    }

    /* message multpart type must match */
    const rofl::openflow::cofmsg_stats_request *mp_req = nullptr;
    if ((mp_req = dynamic_cast<const rofl::openflow::cofmsg_stats_request *>(
             &msg_stats)) != nullptr) {
      if (msg_multipart_type != mp_req->get_stats_type()) {
        throw eSegmentedMessageInvalid(
            "csegment::store_and_merge_msg(), invalid msg multipart type (REQ)")
            .set_func(__FUNCTION__)
            .set_line(__LINE__);
      }
    }

    /* message multpart type must match */
    const rofl::openflow::cofmsg_stats_reply *mp_rep = nullptr;
    if ((mp_rep = dynamic_cast<const rofl::openflow::cofmsg_stats_reply *>(
             &msg_stats)) != nullptr) {
      if (msg_multipart_type != mp_rep->get_stats_type()) {
        throw eSegmentedMessageInvalid(
            "csegment::store_and_merge_msg(), invalid msg multipart type (REP)")
            .set_func(__FUNCTION__)
            .set_line(__LINE__);
      }
    }

    switch (msg_type) {
    case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

      switch (msg_multipart_type) {
      case rofl::openflow13::OFPMP_TABLE_FEATURES: {

        rofl::openflow::cofmsg_table_features_stats_request *msg_table =
            dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request *>(
                msg);

        rofl::openflow::cofmsg_table_features_stats_request const
            &msg_table_stats = dynamic_cast<
                rofl::openflow::cofmsg_table_features_stats_request const &>(
                msg_stats);

        msg_table->set_tables() += msg_table_stats.get_tables();

      } break;
      default: {
        // cannot be fragmented, as request is too short
      };
      }

    } break;
    case rofl::openflow13::OFPT_MULTIPART_REPLY: {

      switch (msg_multipart_type) {
      case rofl::openflow13::OFPMP_FLOW: {

        rofl::openflow::cofmsg_flow_stats_reply *msg_flow =
            dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply *>(msg);

        rofl::openflow::cofmsg_flow_stats_reply const &msg_flow_stats =
            dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const &>(
                msg_stats);

        msg_flow->set_flow_stats_array() +=
            msg_flow_stats.get_flow_stats_array();

      } break;
      case rofl::openflow13::OFPMP_TABLE: {

        rofl::openflow::cofmsg_table_stats_reply *msg_table =
            dynamic_cast<rofl::openflow::cofmsg_table_stats_reply *>(msg);

        rofl::openflow::cofmsg_table_stats_reply const &msg_table_stats =
            dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const &>(
                msg_stats);

        msg_table->set_table_stats_array() +=
            msg_table_stats.get_table_stats_array();

      } break;
      case rofl::openflow13::OFPMP_PORT_STATS: {

        rofl::openflow::cofmsg_port_stats_reply *msg_port =
            dynamic_cast<rofl::openflow::cofmsg_port_stats_reply *>(msg);

        rofl::openflow::cofmsg_port_stats_reply const &msg_port_stats =
            dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const &>(
                msg_stats);

        msg_port->set_port_stats_array() +=
            msg_port_stats.get_port_stats_array();

      } break;
      case rofl::openflow13::OFPMP_QUEUE: {

        rofl::openflow::cofmsg_queue_stats_reply *msg_queue =
            dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply *>(msg);

        rofl::openflow::cofmsg_queue_stats_reply const &msg_queue_stats =
            dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const &>(
                msg_stats);

        msg_queue->set_queue_stats_array() +=
            msg_queue_stats.get_queue_stats_array();

      } break;
      case rofl::openflow13::OFPMP_GROUP: {

        rofl::openflow::cofmsg_group_stats_reply *msg_group =
            dynamic_cast<rofl::openflow::cofmsg_group_stats_reply *>(msg);

        rofl::openflow::cofmsg_group_stats_reply const &msg_group_stats =
            dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const &>(
                msg_stats);

        msg_group->set_group_stats_array() +=
            msg_group_stats.get_group_stats_array();

      } break;
      case rofl::openflow13::OFPMP_GROUP_DESC: {

        rofl::openflow::cofmsg_group_desc_stats_reply *msg_group =
            dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply *>(msg);

        rofl::openflow::cofmsg_group_desc_stats_reply const &msg_group_stats =
            dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const &>(
                msg_stats);

        msg_group->set_group_desc_stats_array() +=
            msg_group_stats.get_group_desc_stats_array();

      } break;
      case rofl::openflow13::OFPMP_METER: {

        rofl::openflow::cofmsg_meter_stats_reply *msg_meter =
            dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply *>(msg);

        rofl::openflow::cofmsg_meter_stats_reply const &msg_meter_stats =
            dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply const &>(
                msg_stats);

        msg_meter->set_meter_stats_array() +=
            msg_meter_stats.get_meter_stats_array();

      } break;
      case rofl::openflow13::OFPMP_METER_CONFIG: {

        rofl::openflow::cofmsg_meter_config_stats_reply *msg_meter =
            dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply *>(
                msg);

        rofl::openflow::cofmsg_meter_config_stats_reply const &msg_meter_stats =
            dynamic_cast<
                rofl::openflow::cofmsg_meter_config_stats_reply const &>(
                msg_stats);

        msg_meter->set_meter_config_array() +=
            msg_meter_stats.get_meter_config_array();

      } break;
      case rofl::openflow13::OFPMP_TABLE_FEATURES: {

        rofl::openflow::cofmsg_table_features_stats_reply *msg_table =
            dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply *>(
                msg);

        rofl::openflow::cofmsg_table_features_stats_reply const
            &msg_table_stats = dynamic_cast<
                rofl::openflow::cofmsg_table_features_stats_reply const &>(
                msg_stats);

        msg_table->set_tables() += msg_table_stats.get_tables();

      } break;
      case rofl::openflow13::OFPMP_PORT_DESC: {

        rofl::openflow::cofmsg_port_desc_stats_reply *msg_port_desc =
            dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply *>(msg);

        rofl::openflow::cofmsg_port_desc_stats_reply const
            &msg_port_desc_stats = dynamic_cast<
                rofl::openflow::cofmsg_port_desc_stats_reply const &>(
                msg_stats);

        msg_port_desc->set_ports() += msg_port_desc_stats.get_ports();

      } break;
      default: {
        // cannot be fragmented, as request is too short
      };
      }

    } break;
    default: {
    };
    }

  } // end else block
}

rofl::openflow::cofmsg *csegment::retrieve_and_detach_msg() {
  rofl::openflow::cofmsg *tmp = msg;
  msg = NULL;
  return tmp;
}
