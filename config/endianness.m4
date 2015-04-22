##Determines endianness and other useful things... 

#Check endianness
AC_C_BIGENDIAN(
	AC_DEFINE([BIG_ENDIAN_DETECTED], [1]), 
	AC_DEFINE([LITTLE_ENDIAN_DETECTED], [1]), 
	AC_MSG_ERROR([Unkown endianness]), 
	AC_MSG_ERROR([Unkown endianness])
)


AC_CHECK_HEADER(byteswap.h,
    [AC_DEFINE([BYTESWAP_HEADER_DETECTED], [1])],
    [AC_WARN([Could not find or include byteswap.h])]
    )

