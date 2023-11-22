#!/bin/bash

gcc PA3.c -o testfiles/disassem
./testfiles/disassem testfiles/$1/test.obj testfiles/$1/test.sym
cat out.lst
# diff out.lst testfiles/$1/out.lst
