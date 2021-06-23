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
echo PYTHON_INCLUDE_DIR=$PYTHON_INCLUDE_DIR
echo XSBCONFIG_LIBDIR=$XSBCONFIG_LIBDIR
echo


# xsbincludedir="/home/canderse-adm/dev/XSBs/XSBsrc/XSB/emu"
# pythondir="/home/canderse-adm/.conda/envs/embedding-env-gpu"
xsbincludedir="/usr/local/flora/Flora-2/XSB/bin/xsb"
pythondir="/root/anaconda3/envs/aidaenv"
pythonincludedir=$pythondir/include/python3.7m
pythonlibdir=$pythondir/lib
pythonlib=python3.7m

ldflags="-l$pythonlib -L$pythonlibdir"
runflags="-Wl,-rpath=$pythonlibdir"
ccflags="-I$xsbincludedir -I$pythonincludedir $ldflags $runflags"

generated_files="ld_pymod.P xsbpym.H"
AC_SUBST(ccflags)
AC_SUBST(ldflags)
AC_SUBST(runflags)

AC_SUBST(PYTHON_INCLUDE_DIR)
AC_SUBST(PYTHON_LIB)
AC_SUBST(PYTHON_BIN)
AC_SUBST(XSBCONFIG_LIBDIR)

LIBS="$ldflags"
echo ccflags=$ccflags
echo ldflags=$LIBS

AC_SEARCH_LIBS([Py_Initialize], [], [], [AC_MSG_ERROR([This package needs python.], [1])], [])
echo 'Py_Initialize found'

AC_OUTPUT([$generated_files])
chmod a-w $generated_files

