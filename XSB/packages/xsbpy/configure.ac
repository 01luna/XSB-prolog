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

source configvars.sh

AX_PYTHON

PYTHON_LIBDIR=`dirname $PYTHON_LIB`
PYTHON_LIB_NAME=`basename $PYTHON_LIB`

echo
echo The Python configuration variables have been set as follows:
echo PYTHON_BIN=$PYTHON_BIN
echo PYTHON_LIB=$PYTHON_LIB
echo PYTHON_LIB_NAME=$PYTHON_LIB_NAME
echo PYTHON_LIBDIR=$PYTHON_LIBDIR
echo PYTHON_INCLUDE_DIR=$PYTHON_INCLUDE_DIR
echo XSBCONFIG_LIBDIR=$XSBCONFIG_LIBDIR
echo XSBCONFIG_EMUDIR=$XSBCONFIG_EMUDIR
echo

support1=no
if test -z "$PYTHON_BIN" ; then
   message1="XSBPY:       Python not found"
elif test -z "$PYTHON_LIB" ; then
   message1="XSBPY:       Python shared library (libpython3.so) not found"
elif test -z "$PYTHON_INCLUDE_DIR" ; then
   message1="XSBPY:       Python include directory not found"
else
   support1=yes
fi

support0=no
CFLAGS_backup="${CFLAGS}"
CFLAGS="-I$PYTHON_INCLUDE_DIR ${CFLAGS}"
CPPFLAGS="-I$PYTHON_INCLUDE_DIR ${CPPFLAGS}"
AC_CHECK_HEADERS([Python.h] , [support0=yes], [message0="       XSBPY: Python.h not found"], [])
CFLAGS="${CFLAGS_backup}"

# these are likely useless
ldflags="-l$PYTHON_LIB_NAME -L$PYTHON_LIBDIR"
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

# these are useless
#LDFLAGS="$ldflags"
#echo ccflags=$ccflags
#echo ldflags=$LDFLAGS
#
#support2=no
#AC_SEARCH_LIBS([Py_Initialize], [], [support2=yes], [message2="       XSBPY: Py_Initialize not found"], [])

AC_OUTPUT([ xsb2py_connect_defs.h:xsb2py_connect_defs.in ])

if test "$support0" = "yes" -a "$support1" = "yes" ; then
   support=yes
else
   support=no
fi

(
echo "  XSBPY support (XSB to Python connector):        $support"
if test "$support" = "yes"; then
   cat <<EOT
  XSBPY C flags:      $ccflags
  XSBPY loader flags: $ldflags

EOT
else
    test "$support0" = no && echo "$message0" 
    test "$support1" = no && echo "$message1"
fi
) | tee Installation_summary
