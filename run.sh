#!/bin/bash
make DEBUG=1

./bin/stego -embed -in $2 -p $1 -out out.bmp
./bin/stego -extract -in out.bmp -p $1 -out extracted.txt

diff $2 extracted.txt
