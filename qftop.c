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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack_tcp.h>

static int dump_cb(enum nf_conntrack_msg_type type,
		   struct nf_conntrack *ct,
		   void *data)
{
	char buf[1024];
	unsigned int op_type = NFCT_O_DEFAULT;
	unsigned int op_flags = NFCT_OF_SHOW_LAYER3 | NFCT_OF_ID;
	
	nfct_snprintf(buf, sizeof(buf), ct, NFCT_T_UNKNOWN, op_type, op_flags);
	printf("%s\n", buf);

	return NFCT_CB_CONTINUE;
}

int main(int argc, char** argv)
{
	struct nfct_handle *cth = NULL;
	uint32_t af = AF_INET;
	int res = 0;
	
	assert(argc);
	assert(argv);

	cth = nfct_open(CONNTRACK, 0);
	
	if (!cth)
		perror("Can't open handler");

	res = nfct_callback_register(cth, NFCT_T_ALL, dump_cb, NULL);
	
	if (res) {
		printf("nfct_callback_register failed %s\n", strerror(errno));
		goto out;
	}

	res = nfct_query(cth, NFCT_Q_DUMP, &af);

	if (res) {
		printf("conntrack query failed %s\n", strerror(errno));
	}
out:
	nfct_close(cth);

	return(!!res);
}