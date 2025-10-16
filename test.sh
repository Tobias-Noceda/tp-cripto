#!/bin/bash

stego=$1
enc=$2

rm -f test.png
./bin/stego -extract -p files/lado$stego$enc.input.bmp -out test -stego $stego > /dev/null

if cmp -s files/itba.output.png test.png
then
    echo -e "\t\e[32mPASS\e[0m $stego"
else
    echo -e "\t\e[31mFAIL\e[0m $stego"
fi
