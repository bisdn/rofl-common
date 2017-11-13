# Check for google's address sanitizer - MUST BE THE FIRST CHECK
AC_MSG_CHECKING(whether to enable google's address sanitizer)
asan_default="no"
AC_ARG_ENABLE(asan,
	AS_HELP_STRING([--enable-asan], [compile with google's address sanitizer [default=no]])
		, , enable_asan=$asan_default)
if test "$enable_asan" = "yes"; then
	CFLAGS="$CFLAGS -g -fsanitize=address -fno-omit-frame-pointer"
	CXXFLAGS="$CXXFLAGS -g -fno-inline -fsanitize=address -fno-omit-frame-pointer"
        LDFLAGS="$LDFLAGS -g -fsanitize=address -fno-omit-frame-pointer"
	AC_DEFINE([ASAN], [], [Description])
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(ASAN, test "$enable_asan" = yes)


# Check for google's thread sanitizer - MUST BE THE FIRST CHECK
AC_MSG_CHECKING(whether to enable google's thread sanitizer)
tsan_default="no"
AC_ARG_ENABLE(tsan,
	AS_HELP_STRING([--enable-tsan], [compile with google's thread sanitizer [default=no]])
		, , enable_tsan=$tsan_default)
if test "$enable_tsan" = "yes"; then
	CFLAGS="$CFLAGS -g -fsanitize=thread"
	CXXFLAGS="$CXXFLAGS -g -fno-inline -fsanitize=thread"
        LDFLAGS="$LDFLAGS -g -fsanitize=thread"
	AC_DEFINE([TSAN], [], [Description])
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(TSAN, test "$enable_tsan" = yes)


