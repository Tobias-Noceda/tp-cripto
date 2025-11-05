#!/bin/bash

fails=0

for args in \
    "LSB1" \
    "LSB4" \
    "LSBI" \
    "LSBI 3DES CFB" \
    "LSBI AES256 OFB"
do
    ./test-embed.sh $args || ((fails++)) &
    ./test-extract.sh $args || ((fails++)) &
done

wait

if [ $fails -ne 0 ]
then
    false
fi
