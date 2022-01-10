#!/bin/bash
# $2 file
# $1 sha256sum
SUM=`sha256sum $2 | awk '{print $1}'`
if [ $SUM != $1 ]; then
    echo "fail"
    exit 1
fi
exit 0
