#!/bin/bash
wget -c  http://downloads.sourceforge.net/project/udt/udt/4.11/udt.sdk.4.11.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fudt%2F&ts=1448451490&use_mirror=ncu
tar -xf udt.sdk.4.11.tar.gz
make udt4/
sudo cp udt4/src/*.h /usr/include/
sudo cp udt4/src/udt /usr/include/
sudo cp udt4/src/libudt.so /usr/lib/libudt.so

