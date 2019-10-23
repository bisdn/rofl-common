/*
 * cofhelloelems.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef COFHELLOELEMS_H_
#define COFHELLOELEMS_H_

#include <inttypes.h>
#include <map>

#include "rofl/common/openflow/cofhelloelem.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"

namespace rofl {
namespace openflow {

class eHelloElemsBase : public exception {
public:
  eHelloElemsBase(const std::string &__arg = std::string(""))
      : exception(__arg) {
    set_exception("eHelloElemsBase");
  };
};
class eHelloElemsNotFound : public eHelloElemsBase {
public:
  eHelloElemsNotFound(const std::string &__arg = std::string(""))
      : eHelloElemsBase(__arg) {
    set_exception("eHelloElemsNotFound");
  };
};

class cofhelloelems {
public:
  /**
   *
   */
  virtual ~cofhelloelems();

  /**
   *
   */
  cofhelloelems();

  /**
   *
   */
  cofhelloelems(const cofhelloelems &elems);

  /**
   *
   */
  cofhelloelems &operator=(const cofhelloelems &elems);

public:
  /**
   *
   */
  size_t length() const;

  /**
   *
   */
  void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  void clear();

public:
  /**
   *
   */
  cofhello_elem_versionbitmap &add_hello_elem_versionbitmap();

  /**
   *
   */
  cofhello_elem_versionbitmap &set_hello_elem_versionbitmap();

  /**
   *
   */
  const cofhello_elem_versionbitmap &get_hello_elem_versionbitmap() const;

  /**
   *
   */
  void drop_hello_elem_versionbitmap();

  /**
   *
   */
  bool has_hello_elem_versionbitmap() const;

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofhelloelems const &elems) {
    os << "<cofhelloelems #elems:" << (int)elems.elems.size()
       << " length:" << (int)elems.length() << " >" << std::endl;

    for (std::map<uint8_t, cofhello_elem *>::const_iterator it =
             elems.elems.begin();
         it != elems.elems.end(); ++it) {
      const cofhello_elem &elem = *(it->second);
      switch (elem.get_type()) {
      case openflow13::OFPHET_VERSIONBITMAP: {
        os << cofhello_elem_versionbitmap(elem);
      } break;
      default: {
        os << *(it->second);
      } break;
      }
    }
    return os;
  };

private:
  std::map<uint8_t, cofhello_elem *> elems;
};

}; /* namespace openflow */
}; /* namespace rofl */

#endif /* COFHELLOELEMS_H_ */
