/* __LICENSE_HEADER_BEGIN__ */

/*
 * Copyright (C) 2012	Emmanuel Roullit <emmanuel.roullit@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 *
 */

/* __LICENSE_HEADER_END__ */

/*
 * Debian: apt-get install libnetfilter-conntrack3 libnetfilter-conntrack-dev
 *
 * Start conntrack (if not yet running):
 *   iptables -A INPUT -p tcp -m state --state ESTABLISHED -j ACCEPT
 *   iptables -A OUTPUT -p tcp -m state --state NEW,ESTABLISHED -j ACCEPT
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include <GeoIP.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack_tcp.h>
#include <curses.h>

struct show_data
{
	WINDOW * screen;
	GeoIP *country_db;
};

static WINDOW * screen_init()
{
	WINDOW * screen = initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(screen, TRUE);
	refresh();
	wrefresh(screen);

	return screen;
}

static int dump_cb(enum nf_conntrack_msg_type type,
	struct nf_conntrack *ct,
	void *data)
{
	struct show_data *show = data;
	struct in_addr src_ip, dst_ip;
	const char *src_country = NULL, *dst_country = NULL;

	assert(data);

	type = type;

	src_ip.s_addr = nfct_get_attr_u32(ct, ATTR_IPV4_SRC);
	dst_ip.s_addr = nfct_get_attr_u32(ct, ATTR_IPV4_DST);

	src_country = GeoIP_country_name_by_ipnum(show->country_db, src_ip.s_addr);
	dst_country = GeoIP_country_name_by_ipnum(show->country_db, dst_ip.s_addr);

	wprintw(show->screen, "src: %s:%u ", inet_ntoa(src_ip), nfct_get_attr_u16(ct, ATTR_PORT_SRC));
	wprintw(show->screen, "(%s) ", src_country ? src_country : "N/A");
	wprintw(show->screen, "dst: %s:%u ", inet_ntoa(dst_ip), nfct_get_attr_u16(ct, ATTR_PORT_DST));
	wprintw(show->screen, "(%s) ", dst_country ? dst_country : "N/A");

	wprintw(show->screen, "\n");

	wrefresh(show->screen);
	refresh();

	return NFCT_CB_CONTINUE;
}

int main(int argc, char** argv)
{
	struct show_data show;
	struct nfct_handle *cth = NULL;
	uint32_t af = AF_INET;
	int res = 0;

	assert(argc);
	assert(argv);

	memset(&show, 0, sizeof(show));

	show.screen = screen_init();

	if (!show.screen)
		perror("Can't init screen");

	show.country_db = GeoIP_new(GEOIP_STANDARD);

	if (!show.country_db)
		perror("Can't open GeoIP database");

	cth = nfct_open(CONNTRACK, 0);

	if (!cth)
		perror("Can't open handler");

	res = nfct_callback_register(cth, NFCT_T_ALL, dump_cb, &show);

	if (res) {
		printf("nfct_callback_register failed %s\n", strerror(errno));
		goto out;
	}

	res = nfct_query(cth, NFCT_Q_DUMP, &af);

	if (res) {
		printf("conntrack query failed %s\n", strerror(errno));
	}

	getchar();

out:
	nfct_close(cth);
	GeoIP_delete(show.country_db);
	endwin();

	return(!!res);
}