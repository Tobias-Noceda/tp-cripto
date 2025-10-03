#!/bin/bash
gcc -o stego src/writers/* src/readers/* src/stego.c -lssl -lcrypto

./stego -embed -in hello.txt -p mesi.bmp -out out.bmp
