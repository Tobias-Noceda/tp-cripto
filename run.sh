#!/bin/bash
img=$1
msg=$2

make DEBUG=1

extension=${msg##*.}

./bin/stego -embed -p $img -in $msg -out out.bmp
./bin/stego -extract -p out.bmp -out extracted

echo
diff $msg extracted.$extension
