#!/bin/bash

stego=$1
enc=$2

rm -f test.bmp
./bin/stego -embed -in files/itba.output.png -p files/lado.input.bmp -out test.bmp -steg $stego > /dev/null

if cmp -s files/lado$stego$enc.input.bmp test.bmp
then
    echo -e "\t\e[32mPASS\e[0m $stego EMB"
else
    echo -e "\t\e[31mFAIL\e[0m $stego EMB"
    false
fi
