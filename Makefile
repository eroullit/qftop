all:
	gcc -W -Wall -Werror -lnetfilter_conntrack qftop.c -o qftop
clean:
	rm -f *.o qftop
