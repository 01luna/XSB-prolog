#! /bin/sh

echo "-------------------------------------------------------"
echo "--- Running nonmt_tests/test.sh                     ---"
echo "-------------------------------------------------------"

XEMU=$1
options=$2

#--------------------------------------------------
../gentest.sh "$XEMU $options" test_tc "test".
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_iso_basic "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_iso_mult_visit "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_iso_hash "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_iso_undef "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_iso_attr "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_lazy "test."
#-------------------------------------------------- yet more inc tests with abolish_table_call_single + gc
#../gentest.sh "$XEMU $options" inc_atc_single_gc_deps "test".  (doesn't work yet)
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_introspection "test."
#-------------------------------------------------- yet more tests of invalidation
../gentest.sh "$XEMU -l $options" test_invalidate "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_goal_interrupt "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_trip "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_trip_mem "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_trip_susp_reset "test."
#--------------------------------------------------
../gentest.sh "$XEMU -l $options" test_maxans "test."
#--------------------------------------------------
