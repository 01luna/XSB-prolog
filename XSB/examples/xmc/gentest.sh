#! /bin/sh

## File:      gentest.sh
## Author:    Baoqiu Cui (Changed from XSB's testsuites)

## $1 is expected to be "XMC executable + command line options"
## $2 is expected to be the file name of the benchmark
## $3 is expected to be command to run
## $4 is expected to be the name of the output file

XMC=$1
FILE=$2
CMD=$3
OUTFILE=$4

DIR=`pwd`
BASEDIR=`basename $DIR`

echo "--------------------------------------------------------------------"
echo "Testing $BASEDIR/$FILE"

$XMC << EOF
[$FILE].
tell(${OUTFILE}).
$CMD
told.
EOF

#-----------------------
# print out differences.
#-----------------------
