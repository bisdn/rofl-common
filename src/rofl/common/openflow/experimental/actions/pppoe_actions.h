#ifndef PPPOE_ACTIONS_H
#define PPPOE_ACTIONS_H 1

#include <inttypes.h>
#include <iostream>

#include <rofl/common/cmemory.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace pppoe {

enum pppoe_exp_id_t {
  PPPOE_EXP_ID = 0x5555a780,
};

enum pppoe_action_type_t {
  PPPOE_ACTION_PUSH_PPPOE,
  PPPOE_ACTION_POP_PPPOE,
};

struct ofp_exp_pppoe_action_body_hdr {
  uint16_t exptype;
  uint16_t explen;
  uint8_t data[0];
} __attribute__((packed));

struct ofp_exp_pppoe_action_body_push_pppoe {
  uint16_t exptype;
  uint16_t explen;
  uint16_t ethertype;
} __attribute__((packed));

struct ofp_exp_pppoe_action_body_pop_pppoe {
  uint16_t exptype;
  uint16_t explen;
  uint16_t ethertype;
} __attribute__((packed));

class cofaction_exp_body_pppoe : public rofl::cmemory {
public:
  /**
   *
   */
  cofaction_exp_body_pppoe(
      uint16_t exptype = 0,
      size_t bodylen = sizeof(struct ofp_exp_pppoe_action_body_hdr))
      : rofl::cmemory(bodylen) {
    set_exp_type(exptype);
    set_exp_len(bodylen);
  };

  /**
   *
   */
  ~cofaction_exp_body_pppoe(){};

  /**
   *
   */
  cofaction_exp_body_pppoe(const cofaction_exp_body_pppoe &body)
      : cmemory(body) {}

  /**
   *
   */
  cofaction_exp_body_pppoe &operator=(const cofaction_exp_body_pppoe &body) {
    if (this == &body)
      return *this;
    rofl::cmemory::operator=(body);
    return *this;
  };

  /**
   *
   */
  cofaction_exp_body_pppoe(const rofl::cmemory &body) { *this = body; };

  /**
   *
   */
  cofaction_exp_body_pppoe &operator=(const rofl::cmemory &body) {
    if (this == &body)
      return *this;
    rofl::cmemory::operator=(body);
    return *this;
  };

public:
  /**
   *
   */
  uint16_t get_exp_type() const {
    if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_pppoe_action_body_hdr))
      throw eInvalid("cofaction_exp_body_pppoe::get_exp_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct ofp_exp_pppoe_action_body_hdr *)rofl::cmemory::somem())
            ->exptype);
  };

  /**
   *
   */
  void set_exp_type(uint16_t exptype) {
    if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_pppoe_action_body_hdr))
      throw eInvalid("cofaction_exp_body_pppoe::set_exp_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct ofp_exp_pppoe_action_body_hdr *)rofl::cmemory::somem())->exptype =
        htobe16(exptype);
  };

  /**
   *
   */
  uint16_t get_exp_len() const {
    if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_pppoe_action_body_hdr))
      throw eInvalid("cofaction_exp_body_pppoe::get_exp_len()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct ofp_exp_pppoe_action_body_hdr *)rofl::cmemory::somem())
            ->explen);
  };

  /**
   *
   */
  void set_exp_len(uint16_t explen) {
    if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_pppoe_action_body_hdr))
      throw eInvalid("cofaction_exp_body_pppoe::set_exp_len()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct ofp_exp_pppoe_action_body_hdr *)rofl::cmemory::somem())->explen =
        htobe16(explen);
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_exp_body_pppoe &body) {
    os << "<cofaction_exp_body_pppoe exp-type: 0x" << std::hex
       << (unsigned int)body.get_exp_type() << std::dec << ">" << std::endl;

    os << dynamic_cast<const rofl::cmemory &>(body);
    return os;
  };
};

class cofaction_exp_body_push_pppoe : public cofaction_exp_body_pppoe {
public:
  /**
   *
   */
  cofaction_exp_body_push_pppoe(uint16_t ethertype = 0)
      : cofaction_exp_body_pppoe(
            PPPOE_ACTION_PUSH_PPPOE,
            sizeof(struct ofp_exp_pppoe_action_body_push_pppoe)) {
    set_ether_type(ethertype);
  };

  /**
   *
   */
  ~cofaction_exp_body_push_pppoe(){};

  /**
   *
   */
  cofaction_exp_body_push_pppoe(const cofaction_exp_body_push_pppoe &action)
      : cofaction_exp_body_pppoe(action) {}

  /**
   *
   */
  cofaction_exp_body_push_pppoe &
  operator=(const cofaction_exp_body_push_pppoe &body) {
    if (this == &body)
      return *this;
    rofl::cmemory::operator=(body);
    return *this;
  };

  /**
   *
   */
  cofaction_exp_body_push_pppoe(const rofl::cmemory &body) { *this = body; };

  /**
   *
   */
  cofaction_exp_body_push_pppoe &operator=(const rofl::cmemory &action) {
    if (this == &action)
      return *this;
    rofl::cmemory::operator=(action);
    return *this;
  };

public:
  /**
   *
   */
  uint16_t get_ether_type() const {
    if (rofl::cmemory::memlen() <
        sizeof(struct ofp_exp_pppoe_action_body_push_pppoe))
      throw eInvalid("cofaction_exp_body_push_pppoe::get_ether_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct ofp_exp_pppoe_action_body_push_pppoe *)rofl::cmemory::somem())
            ->ethertype);
  };

  /**
   *
   */
  void set_ether_type(uint16_t ethertype) {
    if (rofl::cmemory::memlen() <
        sizeof(struct ofp_exp_pppoe_action_body_push_pppoe))
      throw eInvalid("cofaction_exp_body_push_pppoe::set_ether_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct ofp_exp_pppoe_action_body_push_pppoe *)rofl::cmemory::somem())
        ->ethertype = htobe16(ethertype);
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_exp_body_push_pppoe &action) {
    os << "<cofaction_push_pppoe ether-type: 0x" << std::hex
       << (unsigned int)action.get_ether_type() << std::dec << ">" << std::endl;

    os << dynamic_cast<const rofl::cmemory &>(action);
    return os;
  };
};

class cofaction_exp_body_pop_pppoe : public cofaction_exp_body_pppoe {
public:
  /**
   *
   */
  cofaction_exp_body_pop_pppoe(uint16_t ethertype = 0)
      : cofaction_exp_body_pppoe(
            PPPOE_ACTION_POP_PPPOE,
            sizeof(struct ofp_exp_pppoe_action_body_pop_pppoe)) {
    set_ether_type(ethertype);
  };

  /**
   *
   */
  ~cofaction_exp_body_pop_pppoe(){};

  /**
   *
   */
  cofaction_exp_body_pop_pppoe(const cofaction_exp_body_pop_pppoe &action)
      : cofaction_exp_body_pppoe(action) {}

  /**
   *
   */
  cofaction_exp_body_pop_pppoe &
  operator=(const cofaction_exp_body_pop_pppoe &body) {
    if (this == &body)
      return *this;
    rofl::cmemory::operator=(body);
    return *this;
  };

  /**
   *
   */
  cofaction_exp_body_pop_pppoe(const rofl::cmemory &body) { *this = body; };

  /**
   *
   */
  cofaction_exp_body_pop_pppoe &operator=(const rofl::cmemory &action) {
    if (this == &action)
      return *this;
    rofl::cmemory::operator=(action);
    return *this;
  };

public:
  /**
   *
   */
  uint16_t get_ether_type() const {
    if (rofl::cmemory::memlen() <
        sizeof(struct ofp_exp_pppoe_action_body_pop_pppoe))
      throw eInvalid("cofaction_exp_body_pop_pppoe::get_ether_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct ofp_exp_pppoe_action_body_pop_pppoe *)rofl::cmemory::somem())
            ->ethertype);
  };

  /**
   *
   */
  void set_ether_type(uint16_t ethertype) {
    if (rofl::cmemory::memlen() <
        sizeof(struct ofp_exp_pppoe_action_body_pop_pppoe))
      throw eInvalid("cofaction_exp_body_pop_pppoe::set_ether_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct ofp_exp_pppoe_action_body_pop_pppoe *)rofl::cmemory::somem())
        ->ethertype = htobe16(ethertype);
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_exp_body_pop_pppoe &action) {
    os << "<cofaction_pop_pppoe ether-type: 0x" << std::hex
       << (unsigned int)action.get_ether_type() << std::dec << ">" << std::endl;

    os << dynamic_cast<const rofl::cmemory &>(action);
    return os;
  };
};

}; // end of namespace pppoe
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
