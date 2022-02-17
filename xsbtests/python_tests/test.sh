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
../pygentest.sh $python test_cmd_query.py
../pygentest.sh $python test_interrupts.py
echo "Expect slight diffs in test_comps between (...) and (...), due to set creation in Python"
../pygentest.sh $python test_comps.py

#========== the following are non-regression tests ==========

python3.8 test_iterations.py
