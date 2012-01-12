all:
	gcc -W -Wall -Werror -pedantic -std=gnu99 -lnetfilter_conntrack -lGeoIP qftop.c -lcurses -o _qftop
clean:
	rm -f *.o _qftop
