AC_PREREQ(2.59)
AC_INIT([tokamak_draw], [1.0], [bd512@york.ac.uk])

AM_INIT_AUTOMAKE([1.9 foreign])

######### Configure options


######### Programs

AC_PROG_CC

######### Libraries

AC_CHECK_LIB([glut], [glutInit], , [
   # Glut not found
   echo "ERROR: Glut library required"
   echo "    Make sure it's in your LD_LIBRARY_PATH"
   exit
])

######### Headers

AC_CHECK_HEADERS([GL/glut.h ctype.h sys/types.h stdarg.h time.h float.h], , [
   # One of the headers not found
   echo "ERROR: Required header not found"
   exit
])

######### Output

AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
