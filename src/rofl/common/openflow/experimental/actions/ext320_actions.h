/*
 * ext320_actions.h
 *
 *  Created on: 23.10.2019
 *      Author: andreas
 */

#ifndef EXT320_ACTIONS_H_
#define EXT320_ACTIONS_H_

#include <inttypes.h>
#include <iostream>

#include <rofl/common/cmemory.h>
#include <rofl/common/openflow/openflow_common.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace ext320 {

enum onf_act_exp_id_t {
  EXT320_EXP_ID = 0x4F4E4600,
};

enum onf_act_exp_type {
  ONFACT_ET_COPY_FIELD = 3200,
};

struct onf_act_copy_field_hdr {
  uint16_t exp_type;       /* ONFACT_ET_COPY_FIELD */
  uint8_t  pad2[2];
  uint16_t n_bits;         /* Number of bits to copy. */
  uint16_t src_offset;     /* Starting bit offset in source */
  uint16_t dst_offset;     /* Starting bit offset in destination */
  uint8_t  pad[2];         /* Align to 32 bits. */
  /* Followed by:
   *   - Exactly 8,12 or 16 bytes containing the oxm_ids, then
   *   - Enough all-zero bytes (either 0 or 4) to make the action a whole
   *     multiple of 8 bytes in length */
  uint8_t  data[0];        /* Source and destination OXM ids in any f...ing weird combination. Who was so dumb to define this s...? */ 
} __attribute__((packed));
OFP_ASSERT(sizeof(struct onf_act_copy_field_hdr) == 12);


class cofaction_exp_body_copy_field : public rofl::cmemory {
public:
  /**
   *
   */
  cofaction_exp_body_copy_field(
		  uint16_t n_bits = 0, 
		  uint16_t src_offset = 0, 
		  uint16_t dst_offset = 0, 
		  uint32_t src_oxm_id = 0, 
		  uint32_t src_oxm_exp_id = 0, 
		  uint32_t dst_oxm_id = 0, 
		  uint32_t dst_oxm_exp_id = 0) {
    size_t memlen = sizeof(struct onf_act_copy_field_hdr) + 
	    ((src_oxm_exp_id == 0) ? sizeof(uint32_t) : 2*sizeof(uint32_t)) + 
	    ((dst_oxm_exp_id == 0) ? sizeof(uint32_t) : 2*sizeof(uint32_t));
    if (memlen % sizeof(uint64_t))
        memlen += sizeof(uint64_t) - (memlen % sizeof(uint64_t)); // padding to 8 bytes boundary
    rofl::cmemory::resize(memlen);

    set_exp_type(ONFACT_ET_COPY_FIELD);
    set_n_bits(n_bits);
    set_src_offset(src_offset);
    set_dst_offset(dst_offset);
    set_oxm_ids(src_oxm_id, src_oxm_exp_id, dst_oxm_id, dst_oxm_exp_id);
  };

  /**
   *
   */
  ~cofaction_exp_body_copy_field(){};

  /**
   *
   */
  cofaction_exp_body_copy_field(const cofaction_exp_body_copy_field &body) {
    *this = body;
  };

  /**
   *
   */
  cofaction_exp_body_copy_field &operator=(const cofaction_exp_body_copy_field &body) {
    if (this == &body)
      return *this;
    rofl::cmemory::operator=(body);
    return *this;
  };

  /**
   *
   */
  cofaction_exp_body_copy_field(const rofl::cmemory &body) { *this = body; };

  /**
   *
   */
  cofaction_exp_body_copy_field &operator=(const rofl::cmemory &body) {
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
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_exp_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())
            ->exp_type);
  };

  /**
   *
   */
  void set_exp_type(uint16_t exp_type) {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::set_exp_type()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->exp_type =
        htobe16(exp_type);
  };

  /**
   *
   */
  uint16_t get_n_bits() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_n_bits()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())
            ->n_bits);
  };

  /**
   *
   */
  void set_n_bits(uint16_t n_bits) {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::set_n_bits()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->n_bits =
        htobe16(n_bits);
  };

  /**
   *
   */
  uint16_t get_src_offset() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_src_offset()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())
            ->src_offset);
  };

  /**
   *
   */
  void set_src_offset(uint16_t src_offset) {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::set_src_offset()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->src_offset =
        htobe16(src_offset);
  };

  /**
   *
   */
  uint16_t get_dst_offset() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_dst_offset()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return be16toh(
        ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())
            ->dst_offset);
  };

  /**
   *
   */
  void set_dst_offset(uint16_t dst_offset) {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::set_dst_offset()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->dst_offset =
        htobe16(dst_offset);
  };

  /**
   *
   */
  uint32_t get_src_oxm_id() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_id() too short, no header found")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint8_t *data =  ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->data;
    size_t datalen = rofl::cmemory::memlen() - sizeof(struct onf_act_copy_field_hdr);

    if (datalen < sizeof(uint32_t))
      throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_id() too short, no src_oxm_id found")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint16_t oxm_class = (uint16_t)((data[0] << 8) + data[1]);

    switch (oxm_class) {
    case OFPXMC_EXPERIMENTER:
    case OFPXMC_OPENFLOW_BASIC:
    case 0x8001 /* TODO: to be replaced with constant once the latter is defined */: {
        if (datalen < sizeof(uint32_t))
            throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_id()")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	return be32toh(*((uint32_t*)data));
    } break;
    default: {
      throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_id() unsupported src-oxm-id class")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    }
    }
    return 0;
  };

  /**
   *
   */
  uint32_t get_src_oxm_exp_id() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_exp_id() too short")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint8_t *data =  ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->data;
    size_t datalen = rofl::cmemory::memlen() - sizeof(struct onf_act_copy_field_hdr);

    if (datalen < sizeof(uint32_t))
      throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_exp_id() too short, no src_oxm_id found")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint16_t oxm_class = (uint16_t)((data[0] << 8) + data[1]);

    switch (oxm_class) {
    case OFPXMC_EXPERIMENTER: {
        if (datalen < sizeof(uint64_t))
            throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_exp_id() too short, no src_oxm_id experimenter id found")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	return be32toh(*((uint32_t*)(data + sizeof(uint32_t))));
    } break;
    default: {
      return 0; /* return 0 for non-experimental oxm_ids */
    }
    }
    return 0;
  };

  /**
   *
   */
  uint32_t get_dst_oxm_id() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_id() too short, no header found")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint8_t *data =  ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->data;
    size_t datalen = rofl::cmemory::memlen() - sizeof(struct onf_act_copy_field_hdr);

    uint32_t src_oxm_exp_id = get_src_oxm_exp_id();

    /* adjust offset */
    if (src_oxm_exp_id > 0) {
	if (datalen < (2*sizeof(uint32_t) + sizeof(uint32_t)/* 3rd 32bits */)) {
            throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_id() too short, no dst_oxm_id found")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}
        data    += 2*sizeof(uint32_t);
	datalen -= 2*sizeof(uint32_t);
    } else
    if (src_oxm_exp_id == 0) {
	if (datalen < (1*sizeof(uint32_t) + sizeof(uint32_t)/* 2nd 32bits */)) {
            throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_id() too short, no dst_oxm_id found")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}
        data    += 1*sizeof(uint32_t);
	datalen -= 1*sizeof(uint32_t);
    }

    uint16_t oxm_class = (uint16_t)((data[0] << 8) + data[1]);

    switch (oxm_class) {
    case OFPXMC_EXPERIMENTER:
    case OFPXMC_OPENFLOW_BASIC:
    case 0x8001 /* TODO: to be replaced with constant once the latter is defined */: {
        if (datalen < sizeof(uint32_t))
            throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_id()")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	return be32toh(*((uint32_t*)data));
    } break;
    default: {
      throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_id() unsupported dst-oxm-id class")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    }
    }
    return 0;
  };

  /**
   *
   */
  uint32_t get_dst_oxm_exp_id() const {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_exp_id() too short, no header found")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint8_t *data =  ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->data;
    size_t datalen = rofl::cmemory::memlen() - sizeof(struct onf_act_copy_field_hdr);

    uint32_t src_oxm_exp_id = get_src_oxm_exp_id();

    /* adjust offset */
    if (src_oxm_exp_id > 0) {
	if (datalen < (2*sizeof(uint32_t) + sizeof(uint32_t)/* 3rd 32bits */)) {
            throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_exp_id() too short, no dst_oxm_id found")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}
        data    += 2*sizeof(uint32_t);
	datalen -= 2*sizeof(uint32_t);
    } else
    if (src_oxm_exp_id == 0) {
	if (datalen < (1*sizeof(uint32_t) + sizeof(uint32_t)/* 2nd 32bits */)) {
            throw eInvalid("cofaction_exp_body_copy_field::get_dst_oxm_exp_id() too short, no dst_oxm_id found")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}
        data    += 1*sizeof(uint32_t);
	datalen -= 1*sizeof(uint32_t);
    }

    uint16_t oxm_class = (uint16_t)((data[0] << 8) + data[1]);

    switch (oxm_class) {
    case OFPXMC_EXPERIMENTER: {
        if (datalen < sizeof(uint64_t))
            throw eInvalid("cofaction_exp_body_copy_field::get_src_oxm_exp_id() too short, no dst_oxm_id experimenter id found")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	return be32toh(*((uint32_t*)(data + sizeof(uint32_t))));
    } break;
    default: {
      return 0; /* return 0 for non-experimental oxm_ids */
    }
    }
    return 0;
  };

  /**
   *
   */
  void set_oxm_ids(
	  uint32_t src_oxm_id = 0, 
	  uint32_t src_oxm_exp_id = 0/* should be 0 for non-experimental oxm_ids */,
	  uint32_t dst_oxm_id = 0,
	  uint32_t dst_oxm_exp_id = 0/* should be 0 for non-experimental oxm_ids */) {
    if (rofl::cmemory::memlen() < sizeof(struct onf_act_copy_field_hdr))
      throw eInvalid("cofaction_exp_body_copy_field::set_src_offset()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);

    uint8_t *data =  ((struct onf_act_copy_field_hdr *)rofl::cmemory::somem())->data;
    size_t datalen = rofl::cmemory::memlen() - sizeof(struct onf_act_copy_field_hdr);

    size_t offset = 0;

    /* src_oxm_id */
    if (src_oxm_exp_id > 0) {
        if (datalen < (offset + sizeof(uint64_t))) {
            throw eInvalid("cofaction_exp_body_copy_field::set_oxm_id() too short, not enough space for storing 64bit src_oxm_id")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
        }

        *(uint32_t*)(data + offset + 0*sizeof(uint32_t)) = htobe32(src_oxm_id);
        *(uint32_t*)(data + offset + 1*sizeof(uint32_t)) = htobe32(src_oxm_exp_id);

	offset = 2*sizeof(uint32_t);
    } else
    if (src_oxm_exp_id == 0) {
        if (datalen < sizeof(uint64_t)) {
            throw eInvalid("cofaction_exp_body_copy_field::set_oxm_id() too short, not enough space for storing 32bit src_oxm_id")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}

        *(uint32_t*)(data + offset + 0*sizeof(uint32_t)) = htobe32(src_oxm_id);

	offset = 1*sizeof(uint32_t);
    }

    /* dst_oxm_id */
    if (dst_oxm_exp_id > 0) {
        if (datalen < (offset + sizeof(uint64_t))) {
            throw eInvalid("cofaction_exp_body_copy_field::set_oxm_id() too short, not enough space for storing 64bit dst_oxm_id")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}

        *(uint32_t*)(data + offset + 0*sizeof(uint32_t)) = htobe32(dst_oxm_id);
        *(uint32_t*)(data + offset + 1*sizeof(uint32_t)) = htobe32(dst_oxm_exp_id);

	offset = 2*sizeof(uint32_t);
    } else
    if (dst_oxm_exp_id == 0) {
        if (datalen < sizeof(uint64_t)) {
            throw eInvalid("cofaction_exp_body_copy_field::set_oxm_id() too short, not enough space for storing 32bit dst_oxm_id")
                .set_func(__FUNCTION__)
                .set_line(__LINE__);
	}

        *(uint32_t*)(data + offset + 0*sizeof(uint32_t)) = htobe32(dst_oxm_id);

	offset = 1*sizeof(uint32_t);
    }

    /* padding was calculated in constructor */
  };


public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_exp_body_copy_field &body) {
    os << "<cofaction_exp_body_copy_field exp-type: 0x" 
	    << std::hex
       	    << "exp-type: 0x" << (unsigned int)body.get_exp_type() << ", "
	    << std::dec 
	    << "src_offset: " << (unsigned int)body.get_src_offset() << ", "
	    << "dst_offset: " << (unsigned int)body.get_dst_offset() << ", "
	    << ">" << std::endl;

    os << dynamic_cast<const rofl::cmemory &>(body);
    return os;
  };
};

}; // end of namespace ext320
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* EXT320_ACTIONS_H_ */
