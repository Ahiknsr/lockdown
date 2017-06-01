#!/bin/sh
gcc test.c -o test
gcc execute.c -o execute
#need to compile using static flag since we block the process from opening new files
gcc source.c -o source -static
./execute source < infile > outfile
rm test execute source
