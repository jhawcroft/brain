# process this file with autoconf to produce a configure script

AC_PREREQ(2.59)
AC_INIT([BRAIN], [0.1], [dev@joshhawcroft.org])

# check source directory is correct
AC_CONFIG_SRCDIR([src/config.h])

AM_INIT_AUTOMAKE

AC_LANG([C])


# check for programs
AC_PROG_CC


# check for libraries
AC_SEARCH_LIBS( [initscr], [ncurses], [], [AC_ERROR([ncurses cannot be found])] )
AC_SEARCH_LIBS( [new_field], [form], [], [AC_ERROR([ncurses forms cannot be found])] )

AC_CHECK_HEADER([unistr.h],
                    AC_CHECK_LIB([unistring],
                                 [u8_strcoll],
                                 [have_unistring=yes],[have_unistring=no]))
    if test "x$have_unistring" = "xyes"; then
        AC_DEFINE([HAVE_UNISTRING],[1],[Use libunistring for collation])
        BRAIND_INTL_LIB="-lunistring"
        LIBS+=" -lunistring"
    fi


# check for system header files
AC_CHECK_HEADERS([fcntl.h locale.h stdlib.h string.h sys/socket.h syslog.h unistd.h])

# checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T


# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 memmove memset select setenv setlocale socket strdup strndup strstr])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT


