# Check for debug mode - MUST BE THE FIRST CHECK
AC_MSG_CHECKING(whether to enable debug mode)
debug_default="no"
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug], [turn on debug mode [default=no]])
		, , enable_debug=$debug_default)
if test "$enable_debug" = "yes"; then
	CFLAGS="$CFLAGS -g "
	CXXFLAGS="$CXXFLAGS -g "
	AC_DEFINE([DEBUG], [], [Description])
	AC_MSG_RESULT(yes)
else
	AC_DEFINE([NDEBUG], [], [Description])
	AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(DEBUG, test "$enable_debug" = yes)


