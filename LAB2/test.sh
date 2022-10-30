#!/bin/bash

make
for i in {1..100}
do
	taskset -c 2 ./part1
done
