#!/bin/bash
make DEBUG=1

./bin/stego -embed -in hello.txt -p mesi.bmp -out out.bmp
