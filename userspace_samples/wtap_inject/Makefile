CFLAGS = -Wall -O2 -I/usr/src/sys/net80211/
APP = wtap_inject
LOG = debug
SRCS = wtap_inject.c


all:
	$(CC) $(CFLAGS) -o $(APP) $(SRCS)

clean:
	-rm -f *.o $(APP)

