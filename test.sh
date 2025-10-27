#!/bin/bash

fails=0

for stego in LSB1 LSB4 LSBI
do
    ./test-embed.sh $stego || ((fails++))
    ./test-extract.sh $stego || ((fails++))
done

if [ $fails -ne 0 ]
then
    false
fi
