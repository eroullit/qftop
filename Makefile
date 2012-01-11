all:
	gcc -W -Wall -Werror -lnetfilter_conntrack -lGeoIP qftop.c -o qftop
clean:
	rm -f *.o qftop
