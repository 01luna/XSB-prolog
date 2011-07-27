#! /bin/sh

echo "-------------------------------------------------------"
echo "--- Running incremental_tests/test.sh                     ---"
echo "-------------------------------------------------------"

XEMU=$1
options=$2

#------------------------------------------------- general incremental tests
../gentest.sh "$XEMU $options" incremental "test".
#------------------------------------------------- same as above, but with changed decls.
../gentest.sh "$XEMU $options" incremental1 "test".
#------------------------------------------------- incremental tests with interned tries
../gentest.sh "$XEMU $options" incremental_trie "test".
#------------------------------------------------- incremental tests with trie asserts
../gentest.sh "$XEMU $options" inc_trie_dyn "test".
#------------------------------------------------- inc with interned tries - for storage.P
../gentest.sh "$XEMU $options" inc_trie_alt "test".
#------------------------------------------------- inc with asserted/retracted clauses
../gentest.sh "$XEMU $options" incremental_rule "test".
#------------------------------------------------- inc_rule with initial empty dyn predicate
../gentest.sh "$XEMU $options" incremental_rule_alt "test".
#------------------------------------------------- inc tests with abolish_all_tables
../gentest.sh "$XEMU $options" inc_abol "test".
#------------------------------------------------- inc tests with abolish_table_call
../gentest.sh "$XEMU $options" inc_atc "test".
#------------------------------------------------- inc trans depends
../gentest.sh "$XEMU $options" test_incr_depends "test".
#------------------------------------------------- test incremental <-> opaque
../gentest.sh "$XEMU $options" test_inc_switch "test".
#------------------------------------------------- testing executable incremental dirs.
../gentest.sh "$XEMU $options" test_directives "test".
#------------------------------------------------- testing executable incremental decls.
../gentest.sh "$XEMU $options" test_declarations "test".