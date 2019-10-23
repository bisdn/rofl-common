#ifndef EXT244_MATCHES
#define EXT244_MATCHES 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace extensions {
namespace ext244 {

/**
 * @brief OXM_OF_EXT244
 *
 */
class coxmatch_packet_register : public coxmatch_64 {
public:
  coxmatch_packet_register(uint8_t pkt_reg)
      : coxmatch_64((OFPXMC_PACKET_REGS << 16) | (pkt_reg << 9) |
                    sizeof(uint64_t)){};
  coxmatch_packet_register(uint8_t pkt_reg, uint64_t value)
      : coxmatch_64((OFPXMC_PACKET_REGS << 16) | (pkt_reg << 9) |
                        sizeof(uint64_t),
                    value){};
  coxmatch_packet_register(uint8_t pkt_reg, uint64_t value, uint64_t mask)
      : coxmatch_64((OFPXMC_PACKET_REGS << 16) | (pkt_reg << 9) |
                        HAS_MASK_FLAG | 2 * sizeof(uint64_t),
                    value, mask){};
  coxmatch_packet_register(const coxmatch_64 &oxm) : coxmatch_64(oxm){};
  virtual ~coxmatch_packet_register(){};

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_packet_register &oxm) {
    os << dynamic_cast<const coxmatch_64 &>(oxm);
    // oxm-field represents the packet register index
    os << "<ext244-packet-register(" << (int)oxm.get_oxm_field()
       << "): " << std::hex << "0x" << (unsigned long long)oxm.get_u64value()
       << "/"
       << "0x" << (unsigned long long)oxm.get_u64mask() << " >" << std::dec
       << std::endl;
    return os;
  };
};

}; // end of namespace ext244
}; // end of namespace extensions
}; // end of namespace openflow
}; // end of namespace rofl

#endif
