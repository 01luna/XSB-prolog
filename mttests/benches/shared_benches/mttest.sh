#! /bin/sh

XEMU=$1
FILE=$2
NUM=$3
OPT=$4

echo "-------------------------------------------------------"
echo "--- Running benches/tab_benches/mttest.sh           ---"
echo "-------------------------------------------------------"

# XEMU and options must be together in quotes
 ../genbench.sh "$XEMU"  "[prod_cons]." \
		"mt_bench([prod_cons],1000000,$NUM)." "$FILE"
 ../genbench.sh "$XEMU $OPT"  "[trans_clos],readGraph(g512x8)." \
		"mt_benches(g512x8,8,$NUM)." "$FILE"
 ../genbench.sh "$XEMU $OPT"  "[trans_clos],readGraph(g2048x2)." \
		"mt_benches(g2048x2,2048,$NUM)." "$FILE"
 ../genbench.sh "$XEMU $OPT"  "[trans_clos],readGraph(g8192x1)." \
		"mt_benches(g8192x1,8192,$NUM)." "$FILE"
