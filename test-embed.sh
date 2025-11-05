#!/bin/bash

# Params
stego=$1
algo=$2
mode=$3

out="out"

if [ "$mode" ]
then
    enc="${algo,,}${mode,,}"
    ssl_args="-a $algo -m $mode -pass margarita"
fi

# Setup
mkdir -p $(dirname "$out")
rm -f "$out.bmp"

# Test
./bin/stego -embed -in files/itba.output.png -p files/lado.input.bmp -out "$out.bmp" -steg "$stego" $ssl_args > /dev/null

# Verify
if cmp -s "files/lado$stego$enc.input.bmp" "$out.bmp"
then
    echo -e "\t\e[32mPASS\e[0m $stego ${algo:-\b} ${mode:-\b} EMB"
else
    echo -e "\t\e[31mFAIL\e[0m $stego ${algo:-\b} ${mode:-\b} EMB"
    false
fi
