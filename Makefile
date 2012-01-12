all:
	gcc -W -Wall -Werror -pedantic -std=gnu99 -lnetfilter_conntrack -lGeoIP qftop.c -lcurses -o qftop
clean:
	rm -f *.o qftop
