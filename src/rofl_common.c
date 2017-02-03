#include "rofl_common.h"

#ifndef ROFL_BUILD
// No git detected
const char ROFL_COMMON_VERSION[] = ROFL_COMMON_VERSION_;
const char ROFL_COMMON_BUILD_NUM[] = "";
const char ROFL_COMMON_BUILD_BRANCH[] = "";
const char ROFL_COMMON_BUILD_DESCRIBE[] = "";
#else
const char ROFL_COMMON_VERSION[] = ROFL_COMMON_VERSION_;
const char ROFL_COMMON_BUILD_NUM[] = ROFL_COMMON_BUILD;
const char ROFL_COMMON_BUILD_BRANCH[] = ROFL_COMMON_BRANCH;
const char ROFL_COMMON_BUILD_DESCRIBE[] = ROFL_COMMON_DESCRIBE;

#endif

// C++ extern C
#ifdef __cplusplus
extern "C" {
#endif

// autoconf AC_CHECK_LIB helper function as C-declaration
void librofl_is_present(void){};

// C++ extern C
#ifdef __cplusplus
}
#endif
