#!/bin/bash
set -x

if [ ! -f "objs" ]; then
	echo "mkdir objs"
	mkdir objs
fi
#set the library path for libst.so
echo "set the library path for libst.so"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:objs/st
echo $LD_LIBRARY_PATH

THIRTY_PARTY_FOLDER=thirty_party
RS_OBJS=objs

#build st-1.9
echo "build st-1.9t";
_ST_MAKE=linux-debug && _ST_EXTRA_CFLAGS="-DMD_HAVE_EPOLL"

rm -rf ${RS_OBJS}/st-1.9 && cd ${RS_OBJS} &&
unzip -q ../${THIRTY_PARTY_FOLDER}/st-1.9.zip && cd st-1.9 && chmod +w * &&
make ${_ST_MAKE} EXTRA_CFLAGS="${_ST_EXTRA_CFLAGS}" &&
cd .. && rm -rf st && ln -sf st-1.9/obj st && cd ..

# check status
ret=$?; if [[ $ret -ne 0 ]]; then echo "build st-1.9 failed, ret=$ret"; exit $ret; fi


#make clean
make

ln -s objs/st/libst.so.1.9 libst.so.1
