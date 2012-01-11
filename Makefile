all:
	gcc -W -Wall -Werror -lnetfilter_conntrack -lGeoIP qftop.c -lcurses -o qftop
clean:
	rm -f *.o qftop
