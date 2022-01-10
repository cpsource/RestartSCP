#!/usr/bin/bash

if [ $# -ne 3 ]; then
    echo "3 command line arguments required"
    echo "  1 - path to file to split"
    echo "  2 - file name to split"
    echo "  3 - off-world target"
    exit 1;
fi

# cleanup old files
rm -f xa* > /dev/null 2>&1
rm files.remaining > /dev/null 2>&1

echo "#define TARG \"$3\"" > restart.h

# we'll softlink in our source file
if [ ! -f $2 ]; then
    ln -s $1/$2 $2
fi
# and calculate its sha256sum
sha256sum $2 > source.sha256sum
# break up source file
split --suffix-length=6 --bytes=10000000 $2
# file a file list that restart will use to restart copies
ls -1 x* > files.restart


# build a test script
echo "#/usr/bin/bash" > tst.sh
for i in x*
do
    echo "./sum.sh `sha256sum $i`" >> tst.sh
done
# make the scrip executable
chmod +x tst.sh
# build restart
make
./restart -i

# and these files too
#echo "tst.sh"         >> files.restart
#echo "sum.sh"         >> files.restart
#echo "all-present.sh" >> files.restart
#echo "rebuild.sh"     >> files.restart

# cleanup softlink
rm $2
# and we are done
exit 0
