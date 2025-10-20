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

if [ -n "$algo" ] && [ -n "$mode" ] && [ -n "$pass" ]; then
    echo =========================================================
    ./bin/stego -embed -p $img -in $msg -out out.bmp -stego $stg -alg $algo -mode $mode -pass $pass
    echo =========================================================
    ./bin/stego -extract -p out.bmp -out extracted -stego $stg -alg $algo -mode $mode -pass $pass
else
    echo =========================================================
    ./bin/stego -embed -p $img -in $msg -out out.bmp -stego $stg
    echo =========================================================
    ./bin/stego -extract -p out.bmp -out extracted -stego $stg
fi

echo
diff $msg extracted$extension
