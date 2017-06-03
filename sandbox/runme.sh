#!/bin/sh
if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    echo "usage ./runme.sh sourcecode input"
    exit 
fi
if [[ "$1" == *.c ]]
then
    #echo "c file"
    compiler=gcc
elif [[ "$1" == *.cpp ]]
then 
    #echo "c++ file"
    compiler=g++
else
    echo "unknown file"
    echo "invalid file" > outfile
    exit
fi
#echo $compiler
if [ ! -f test ]; 
then
    gcc test.c -o test
fi

if [ ! -f execute ]; 
then
    gcc execute.c -o execute
fi

#need to compile using static flag since we block the process from opening new files
$compiler $1 -o source -static
./execute source < $2 > outfile
rm source
