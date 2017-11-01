#Detecting crypto and ssl
ssl_detected="yes"
 
AC_CHECK_LIB(ssl, SSL_accept, , ssl_detected="no")
AC_CHECK_LIB(crypto, ERR_get_error, , ssl_detected="no")

AC_MSG_CHECKING(for availability of openssl and crypto libraries(SSL/TLS))
if test "$ssl_detected" = "yes"; then
	AC_MSG_RESULT(found)
	AC_DEFINE_UNQUOTED([CACERT],["$(pwd)/../tools/xca/ca.rofl-core.crt.pem"], [""])
	AC_DEFINE_UNQUOTED([CLICERT],["$(pwd)/../tools/xca/client.crt.pem"], [""])
	AC_DEFINE_UNQUOTED([CLIKEY],["$(pwd)/../tools/xca/client.key.pem"], [""])
	AC_DEFINE_UNQUOTED([SRVCERT],["$(pwd)/../tools/xca/server.crt.pem"], [""])
	AC_DEFINE_UNQUOTED([SRVKEY],["$(pwd)/../tools/xca/server.key.pem"], [""])
else
	AC_MSG_RESULT(not found)
fi

AM_CONDITIONAL([HAVE_OPENSSL],  [test "$ssl_detected" = yes])
