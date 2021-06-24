## File:      configure.ac

dnl  To rebuild the `configure' script from this, execute the command
dnl	autoconf
dnl  in the directory containing this script.
dnl
dnl


dnl Change the definition of four vars:   
dnl   xsbinclude: point to the include files for XSB installation (its emu directory)
dnl   pythoninclude: point to the include files for the python installation 
dnl   pythonlib: main python library name - the actual file name prepends "lib" and appends ".so" to this, e.g. "libpython.so"
dnl		pythonlibdir: point to the main python library location 


AC_PREREQ(2.12)
#AC_INIT([xpy],[1.0])
AC_INIT([xsbpy],[1.0])

AC_CONFIG_MACRO_DIR([./m4])
AC_CHECK_HEADERS(pcre.h , with_pcre=yes, pcre=yes)

source configvars.sh

AX_PYTHON

echo
echo The Python configuration variables have been set as follows:
echo PYTHON_BIN=$PYTHON_BIN
echo PYTHON_LIB=$PYTHON_LIB
echo PYTHON_LIBDIR=`dirname $PYTHON_LIB`
echo PYTHON_INCLUDE_DIR=$PYTHON_INCLUDE_DIR
echo XSBCONFIG_LIBDIR=$XSBCONFIG_LIBDIR
echo XSBCONFIG_EMUDIR=$XSBCONFIG_EMUDIR
echo


ldflags="-l$PYTHON_LIB -L$PYTHON_LIBDIR"
runflags="-Wl,-rpath=$PYTHON_LIBDIR"
ccflags="-I$XSBCONFIG_EMUDIR -I$PYTHON_INCLUDE_DIR $ldflags $runflags"

AC_SUBST(ccflags)
AC_SUBST(ldflags)
AC_SUBST(runflags)

AC_SUBST(PYTHON_INCLUDE_DIR)
AC_SUBST(PYTHON_LIB)
AC_SUBST(PYTHON_LIBDIR)
AC_SUBST(PYTHON_BIN)
AC_SUBST(XSBCONFIG_LIBDIR)
AC_SUBST(XSBCONFIG_EMUDIR)

LIBS="$ldflags"
echo ccflags=$ccflags
echo ldflags=$LIBS

support=no
AC_SEARCH_LIBS([Py_Initialize], [], [support=yes], [AC_MSG_WARN([Py_Initialize not found], [1])], [])

# NO FILEs ld_pymod.P.in or xsbpym.H.in!!!
#generated_files="ld_pymod.P xsbpym.H"
dnl AC_OUTPUT([$generated_files])
dnl chmod a-w $generated_files

(
echo "  XSBPY support (XSB to Python connector):        $support"
if test "$support" = "yes"; then
   cat <<EOT
  XSBPY C flags:                                        $ccflags
  XSBPY loader flags:                                   $ldflags

EOT
else
    echo "       XSBPY: Py_Initialize not found"
fi
) | tee Installation_summary
