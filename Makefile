COMM_MAKE = 1
COMM_ECHO = 1
include common.mk

CFLAGS += -g -fno-strict-aliasing -Wall -export-dynamic \
	-Wall -pipe  -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 -std=c++11

LINKS += -g -L./lib -L./objs/st -lrabbitstreamer -ldl -lstdc++ -lst

LINKS += -Wl,-rpath=./objs/st

LINKS += -L./third_party/googletest/build/lib -lgtestd -lgtest_maind
#LINKS += -Wl,-rpath=./third_party/googletest/build/lib

LINKS += -lpthread

THIRD_PARTY_MD5 = third_party/md5/md5.o

PROGS = rabbitstreamer superpeer_server tracker_server rabbitstreamertests

all:$(PROGS)

rabbitstreamer: librabbitstreamer.a

librabbitstreamer.a: $(OBJS)
	$(ARSTATICLIB) 

superpeer_server: app/sp/superpeer_server.o
	$(BUILDEXE) 

tracker_server: app/tracker/tracker_server.o
	$(BUILDEXE)

rabbitstreamertests: $(TESTSOBJS)
	$(BUILDEXE)

clean:
	$(CLEAN) *.o $(PROGS)
	rm -rf lib
	rm -rf objs
	rm $(OBJS)

