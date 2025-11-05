#!/bin/bash

separator=$1

embed="./bin/stego -embed -in Makefile -p files/lado.input.bmp -out example.bmp"
extract="./bin/stego -extract -p example.bmp -out example"

for stego in LSB1 LSB4 LSBI
do
    echo $embed -steg $stego
    echo $extract -steg $stego
    echo $separator

    for algo in AES128 AES192 AES256 3DES
    do
        for mode in CBC ECB CFB OFB
        do
            echo $embed -steg $stego -a $algo -m $mode -pass example
            echo $extract -steg $stego -a $algo -m $mode -pass example
            echo $separator
        done
    done
done
