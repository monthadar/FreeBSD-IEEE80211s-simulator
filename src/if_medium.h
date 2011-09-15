/*-
 * Copyright (c) 2010-2011 Monthadar Al Jaberi, TerraNet AB
 * All rights reserved.
 *
 * Copyright (c) 2002-2009 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 *
 * $FreeBSD$
 */

#ifndef _DEV_WTAP_MEDIUM_H
#define _DEV_WTAP_MEDIUM_H

#include "if_wtapvar.h"
#include "wtap_hal/handler.h"

struct packet {
    STAILQ_ENTRY(packet)	pf_list;
    struct mbuf 		*m;
    int				id;
};
typedef STAILQ_HEAD(, packet) md_pkthead;

struct wtap_medium {
    struct mtx 			md_mtx;
    //int				visibility[MAX_NBR_WTAP];
    //struct stailhead 		*headp; 
    //packet_head			pktbuf;
    //STAILQ_HEAD(stailhead, packet) pktbuf;
    //STAILQ_HEAD(stailhead, packet) pktbuf;
	// = STAILQ_HEAD_INITIALIZER(head);
    int				open;		/* 0 means we drop packets, 1 we queue them */
    md_pkthead			md_pktbuf;	/* master queue */
    struct eventhandler 	*tx_handler;
    struct timehandler		*bc_handler;
};

void init_medium(struct wtap_medium *);
void deinit_medium(struct wtap_medium *);
void medium_open(struct wtap_medium *);
void medium_close(struct wtap_medium *);
int medium_transmit(struct wtap_medium *, int id, struct mbuf*);
struct packet *medium_get_next_packet(struct wtap_medium *);

#endif
