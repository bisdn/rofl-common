# Check for google's address sanitizer - MUST BE THE FIRST CHECK
AC_MSG_CHECKING(whether to enable google's address sanitizer)
asan_default="no"
AC_ARG_ENABLE(asan,
	AS_HELP_STRING([--enable-asan], [compile with google's address sanitizer [default=no]])
		, , enable_asan=$asan_default)
if test "$enable_asan" = "yes"; then
	CFLAGS="$CFLAGS -fsanitize=address -fomit-frame-pointer"
	CXXFLAGS="$CXXFLAGS -fno-inline -fsanitize=address -fomit-frame-pointer"
        LDFLAGS="$LDFLAGS -fsanitize=address -fomit-frame-pointer"
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
	CFLAGS="$CFLAGS -fsanitize=thread"
	CXXFLAGS="$CXXFLAGS -fno-inline -fsanitize=thread"
        LDFLAGS="$LDFLAGS -fsanitize=thread"
	AC_DEFINE([TSAN], [], [Description])
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(TSAN, test "$enable_tsan" = yes)


