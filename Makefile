COMM_MAKE = 1
COMM_ECHO = 1
include common.mk

CFLAGS += -g -fno-strict-aliasing -Wall -export-dynamic \
	-Wall -pipe  -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64

LINKS += -g -L./lib -L./objs/st -lrabbitstreamer -lpthread -ldl -lstdc++ -lst

THIRTY_PARTY_MD5 = thirty_party/md5/md5.o

PROGS = rabbitstreamer superpeer_server tracker_server

all:$(PROGS)

rabbitstreamer: librabbitstreamer.a

librabbitstreamer.a: $(OBJS)
	$(ARSTATICLIB) 

superpeer_server: app/sp/superpeer_server.o
	$(BUILDEXE) 

tracker_server: app/tracker/tracker_server.o
	$(BUILDEXE)

clean:
	$(CLEAN) *.o $(PROGS)
	rm -rf lib
	rm $(OBJS)

