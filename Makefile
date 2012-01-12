all:
	gcc -W -Wall -Werror -pedantic -std=gnu99 -lnetfilter_conntrack -lGeoIP qftop.c -lcurses -o _qftop
clean:
	rm -f *.o _qftop
install:
	@install _qftop qftop /usr/sbin
	@setcap cap_net_admin=eip /usr/sbin/_qftop