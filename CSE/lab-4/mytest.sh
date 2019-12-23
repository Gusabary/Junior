#!/bin/bash

while((1))
do
./lock_server 3000 &
t=$!
echo $t
./lock_tester 3000
kill -9 $t
done
