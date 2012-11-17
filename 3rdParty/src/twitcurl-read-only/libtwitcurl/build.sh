#!/bin/sh
make all
ar rvs $PWD/../../x86_64/lib/libtwitcurl.a *.o
cp $PWD/lib/libcurl.lib $PWD/../../x86_64/lib/libcurl.a
