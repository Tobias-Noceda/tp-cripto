#!/bin/bash
img=$1
msg=$2
stg=$3

make clean all DEBUG=1

extension=${msg##*.}

./bin/stego -embed -p $img -in $msg -out out.bmp -stego $stg
./bin/stego -extract -p out.bmp -out extracted -stego $stg

echo
diff $msg extracted.$extension
