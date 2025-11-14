#!/bin/bash

# Params
stego=$1
algo=$2
mode=$3

out="runs/$BASHPID"

enc=""
if [ "$mode" ]
then
    enc="${algo,,}${mode,,}"
    ssl_args="-a $algo -m $mode -pass margarita"
fi

# Setup
mkdir -p $(dirname "$out")
rm -f "$out.png"

# Test
./bin/stego -extract -p "files/lado$stego$enc.input.bmp" -out "$out" -steg "$stego" $ssl_args > /dev/null

# Verify
if cmp -s files/itba.output.png $out.png
then
    echo -e "\t\e[32mPASS\e[0m $stego ${algo:-\b} ${mode:-\b} EXT"
else
    echo -e "\t\e[31mFAIL\e[0m $stego ${algo:-\b} ${mode:-\b} EXT"
    false
fi
