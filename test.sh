#!/bin/bash

pids=()

for args in \
    "LSB1" \
    "LSB1 AES128 CBC" \
    "LSB4" \
    "LSBI" \
    "LSBI 3DES CFB" \
    "LSBI AES256 OFB"
do
    ./test-embed.sh $args &
    pids+=($!)
    ./test-extract.sh $args &
    pids+=($!)
done

fails=0
for pid in "${pids[@]}"
do
    wait $pid || ((fails++))
done

if [ $fails -ne 0 ]
then
    false
fi
