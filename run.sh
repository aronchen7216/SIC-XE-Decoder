#!/bin/bash

gcc PA2.c -o testfiles/disassem
./testfiles/disassem testfiles/$1/test.obj testfiles/$1/test.sym
cat obj_struct.txt
