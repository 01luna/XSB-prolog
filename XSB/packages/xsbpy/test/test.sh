
../../../bin/xsb -e "[xsbpy],test_xsbpy,halt. " >& /dev/null

grep '!!!' test_xsbpy_out.txt

