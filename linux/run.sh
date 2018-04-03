#!/usr/bin/env bash

cd linux/
make
make -f Makefile.checker
make clean