#! /bin/sh

echo "-------------------------------------------------------"
echo "--- Running python_tests/test.sh                    ---"
echo "-------------------------------------------------------"

python=$1

#if test "$valgrind" = "true"; then
#    echo "valgrind = $valgrind"
#fi

u=`uname`;
#echo "uname for this system is $u";

../pygentest.sh $python px_callbacks.py
