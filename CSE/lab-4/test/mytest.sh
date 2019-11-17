#!/bin/bash

cd ..
BASE=$(pwd)
DIR=./yfs1
$BASE/start.sh

cd $DIR
mkdir orig  # original directory for symlink

ORIG_FILE="$(pwd)/orig"

echo "TEST start"
for ((i = 1; i < 5; i++))
do
    # echo $(pwd)
    ln -s ${ORIG_FILE} hosts >/dev/null 2>&1  # test symlink
    diff ${ORIG_FILE} hosts >/dev/null 2>&1
    if [ $? -ne 0 ];
    then
        echo "failed test"
        exit
    fi

    cp -r ${ORIG_FILE} hosts_copy >/dev/null 2>&1  # test copy
    ln -s hosts_copy testhostslink >/dev/null 2>&1
    diff testhostslink hosts_copy >/dev/null 2>&1
    if [ $? -ne 0 ];
    then
        echo "failed test"
        exit
    fi

    rm -rf hosts_copy >/dev/null 2>&1  
    touch hosts_copy >/dev/null 2>&1  # test remove and create again
    diff testhostslink hosts_copy >/dev/null 2>&1
    if [ $? -ne 0 ];
    then 
        echo "failed test"
        exit
    fi
    cd hosts
done

echo "Passed TEST"
$BASE/stop.sh >/dev/null 2>&1
