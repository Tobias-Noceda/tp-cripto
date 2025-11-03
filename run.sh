#!/bin/bash
img=$1
msg=$2
stg=$3
algo=$4
mode=$5
pass=$6

make clean all DEBUG=1

extension=${msg##*.}
if [ "$extension" = "$msg" ]; then
    extension=""
else 
    extension=".$extension"
fi

ssl=""
if [ -n "$algo" ] && [ -n "$mode" ] && [ -n "$pass" ]; then
    ssl="-a $algo -m $mode -pass $pass"
fi

echo =========================================================
./bin/stego -embed -p $img -in $msg -out out.bmp -steg $stg $ssl
echo =========================================================
./bin/stego -extract -p out.bmp -out extracted -steg $stg $ssl

echo
diff $msg extracted$extension
