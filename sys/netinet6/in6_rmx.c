/*-
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$KAME: in6_rmx.c,v 1.11 2001/07/26 06:53:16 jinmei Exp $
 */

/*-
 * Copyright 1994, 1995 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * This code does two things necessary for the enhanced TCP metrics to
 * function in a useful manner:
 *  1) It marks all non-host routes as `cloning', thus ensuring that
 *     every actual reference to such a route actually gets turned
 *     into a reference to a host route to the specific destination
 *     requested.
 *  2) When such routes lose all their references, it arranges for them
 *     to be deleted in some random collection of circumstances, so that
 *     a large quantity of stale routing data is not kept in kernel memory
 *     indefinitely.  See in6_rtqtimo() below for the exact mechanism.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_route.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/sysctl.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/mbuf.h>
#include <sys/rwlock.h>
#include <sys/syslog.h>
#include <sys/callout.h>
#include <sys/vimage.h>

#include <net/if.h>
#include <net/route.h>
#include <net/vnet.h>

#include <netinet/in.h>
#include <netinet/ip_var.h>
#include <netinet/in_var.h>

#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>

#include <netinet/icmp6.h>
#include <netinet6/nd6.h>
#include <netinet6/vinet6.h>

#include <netinet/tcp.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>

extern int	in6_inithead(void **head, int off);

#define RTPRF_OURS		RTF_PROTO3	/* set on routes we manage */

/*
 * Do what we need to do when inserting a route.
 */
static struct radix_node *
in6_addroute(void *v_arg, void *n_arg, struct radix_node_head *head,
    struct radix_node *treenodes)
{
	struct rtentry *rt = (struct rtentry *)treenodes;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)rt_key(rt);
	struct radix_node *ret;

	RADIX_NODE_HEAD_WLOCK_ASSERT(head);
	if (IN6_IS_ADDR_MULTICAST(&sin6->sin6_addr))
		rt->rt_flags |= RTF_MULTICAST;

	/*
	 * A little bit of help for both IPv6 output and input:
	 *   For local addresses, we make sure that RTF_LOCAL is set,
	 *   with the thought that this might one day be used to speed up
	 *   ip_input().
	 *
	 * We also mark routes to multicast addresses as such, because
	 * it's easy to do and might be useful (but this is much more
	 * dubious since it's so easy to inspect the address).  (This
	 * is done above.)
	 *
	 * XXX
	 * should elaborate the code.
	 */
	if (rt->rt_flags & RTF_HOST) {
		if (IN6_ARE_ADDR_EQUAL(&satosin6(rt->rt_ifa->ifa_addr)
					->sin6_addr,
				       &sin6->sin6_addr)) {
			rt->rt_flags |= RTF_LOCAL;
		}
	}

	if (!rt->rt_rmx.rmx_mtu && rt->rt_ifp)
		rt->rt_rmx.rmx_mtu = IN6_LINKMTU(rt->rt_ifp);

	ret = rn_addroute(v_arg, n_arg, head, treenodes);
	if (ret == NULL) {
		struct rtentry *rt2;
		/*
		 * We are trying to add a net route, but can't.
		 * The following case should be allowed, so we'll make a
		 * special check for this:
		 *	Two IPv6 addresses with the same prefix is assigned
		 *	to a single interrface.
		 *	# ifconfig if0 inet6 3ffe:0501::1 prefix 64 alias (*1)
		 *	# ifconfig if0 inet6 3ffe:0501::2 prefix 64 alias (*2)
		 *	In this case, (*1) and (*2) want to add the same
		 *	net route entry, 3ffe:0501:: -> if0.
		 *	This case should not raise an error.
		 */
		rt2 = rtalloc1((struct sockaddr *)sin6, 0, RTF_RNH_LOCKED);
		if (rt2) {
			if (((rt2->rt_flags & (RTF_HOST|RTF_GATEWAY)) == 0)
			 && rt2->rt_gateway
			 && rt2->rt_gateway->sa_family == AF_LINK
			 && rt2->rt_ifp == rt->rt_ifp) {
				ret = rt2->rt_nodes;
			}
			RTFREE_LOCKED(rt2);
		}
	}
	return (ret);
}

/*
 * This code is the inverse of in6_clsroute: on first reference, if we
 * were managing the route, stop doing so and set the expiration timer
 * back off again.
 */
static struct radix_node *
in6_matroute(void *v_arg, struct radix_node_head *head)
{
	struct radix_node *rn = rn_match(v_arg, head);
	struct rtentry *rt = (struct rtentry *)rn;

	if (rt && rt->rt_refcnt == 0) { /* this is first reference */
		if (rt->rt_flags & RTPRF_OURS) {
			rt->rt_flags &= ~RTPRF_OURS;
			rt->rt_rmx.rmx_expire = 0;
		}
	}
	return rn;
}

SYSCTL_DECL(_net_inet6_ip6);

#ifdef VIMAGE_GLOBALS
static int rtq_reallyold6;
static int rtq_minreallyold6;
static int rtq_toomany6;
#endif

SYSCTL_V_INT(V_NET, vnet_inet6, _net_inet6_ip6, IPV6CTL_RTEXPIRE,
    rtexpire, CTLFLAG_RW, rtq_reallyold6 , 0, "");

SYSCTL_V_INT(V_NET, vnet_inet6, _net_inet6_ip6, IPV6CTL_RTMINEXPIRE,
    rtminexpire, CTLFLAG_RW, rtq_minreallyold6 , 0, "");

SYSCTL_V_INT(V_NET, vnet_inet6, _net_inet6_ip6, IPV6CTL_RTMAXCACHE,
    rtmaxcache, CTLFLAG_RW, rtq_toomany6 , 0, "");



struct rtqk_arg {
	struct radix_node_head *rnh;
	int mode;
	int updating;
	int draining;
	int killed;
	int found;
	time_t nextstop;
};

/*
 * Get rid of old routes.  When draining, this deletes everything, even when
 * the timeout is not expired yet.  When updating, this makes sure that
 * nothing has a timeout longer than the current value of rtq_reallyold6.
 */
static int
in6_rtqkill(struct radix_node *rn, void *rock)
{
	INIT_VNET_INET6(curvnet);
	struct rtqk_arg *ap = rock;
	struct rtentry *rt = (struct rtentry *)rn;
	int err;

	RADIX_NODE_HEAD_WLOCK_ASSERT(ap->rnh);

	if (rt->rt_flags & RTPRF_OURS) {
		ap->found++;

		if (ap->draining || rt->rt_rmx.rmx_expire <= time_uptime) {
			if (rt->rt_refcnt > 0)
				panic("rtqkill route really not free");

			err = rtrequest(RTM_DELETE,
					(struct sockaddr *)rt_key(rt),
					rt->rt_gateway, rt_mask(rt),
					rt->rt_flags|RTF_RNH_LOCKED, 0);
			if (err) {
				log(LOG_WARNING, "in6_rtqkill: error %d", err);
			} else {
				ap->killed++;
			}
		} else {
			if (ap->updating
			   && (rt->rt_rmx.rmx_expire - time_uptime
			       > V_rtq_reallyold6)) {
				rt->rt_rmx.rmx_expire = time_uptime
					+ V_rtq_reallyold6;
			}
			ap->nextstop = lmin(ap->nextstop,
					    rt->rt_rmx.rmx_expire);
		}
	}

	return 0;
}

#define RTQ_TIMEOUT	60*10	/* run no less than once every ten minutes */
#ifdef VIMAGE_GLOBALS
static int rtq_timeout6;
static struct callout rtq_timer6;
#endif

static void
in6_rtqtimo(void *rock)
{
	CURVNET_SET_QUIET((struct vnet *) rock);
	INIT_VNET_NET((struct vnet *) rock);
	INIT_VNET_INET6((struct vnet *) rock);
	struct radix_node_head *rnh = rock;
	struct rtqk_arg arg;
	struct timeval atv;
	static time_t last_adjusted_timeout = 0;

	arg.found = arg.killed = 0;
	arg.rnh = rnh;
	arg.nextstop = time_uptime + V_rtq_timeout6;
	arg.draining = arg.updating = 0;
	RADIX_NODE_HEAD_LOCK(rnh);
	rnh->rnh_walktree(rnh, in6_rtqkill, &arg);
	RADIX_NODE_HEAD_UNLOCK(rnh);

	/*
	 * Attempt to be somewhat dynamic about this:
	 * If there are ``too many'' routes sitting around taking up space,
	 * then crank down the timeout, and see if we can't make some more
	 * go away.  However, we make sure that we will never adjust more
	 * than once in rtq_timeout6 seconds, to keep from cranking down too
	 * hard.
	 */
	if ((arg.found - arg.killed > V_rtq_toomany6)
	   && (time_uptime - last_adjusted_timeout >= V_rtq_timeout6)
	   && V_rtq_reallyold6 > V_rtq_minreallyold6) {
		V_rtq_reallyold6 = 2*V_rtq_reallyold6 / 3;
		if (V_rtq_reallyold6 < V_rtq_minreallyold6) {
			V_rtq_reallyold6 = V_rtq_minreallyold6;
		}

		last_adjusted_timeout = time_uptime;
#ifdef DIAGNOSTIC
		log(LOG_DEBUG, "in6_rtqtimo: adjusted rtq_reallyold6 to %d",
		    V_rtq_reallyold6);
#endif
		arg.found = arg.killed = 0;
		arg.updating = 1;
		RADIX_NODE_HEAD_LOCK(rnh);
		rnh->rnh_walktree(rnh, in6_rtqkill, &arg);
		RADIX_NODE_HEAD_UNLOCK(rnh);
	}

	atv.tv_usec = 0;
	atv.tv_sec = arg.nextstop - time_uptime;
	callout_reset(&V_rtq_timer6, tvtohz(&atv), in6_rtqtimo, rock);
	CURVNET_RESTORE();
}

/*
 * Age old PMTUs.
 */
struct mtuex_arg {
	struct radix_node_head *rnh;
	time_t nextstop;
};
#ifdef VIMAGE_GLOBALS
static struct callout rtq_mtutimer;
#endif

static int
in6_mtuexpire(struct radix_node *rn, void *rock)
{
	struct rtentry *rt = (struct rtentry *)rn;
	struct mtuex_arg *ap = rock;

	/* sanity */
	if (!rt)
		panic("rt == NULL in in6_mtuexpire");

	if (rt->rt_rmx.rmx_expire && !(rt->rt_flags & RTF_PROBEMTU)) {
		if (rt->rt_rmx.rmx_expire <= time_uptime) {
			rt->rt_flags |= RTF_PROBEMTU;
		} else {
			ap->nextstop = lmin(ap->nextstop,
					rt->rt_rmx.rmx_expire);
		}
	}

	return 0;
}

#define	MTUTIMO_DEFAULT	(60*1)

static void
in6_mtutimo(void *rock)
{
	CURVNET_SET_QUIET((struct vnet *) rock);
	INIT_VNET_NET((struct vnet *) rock);
	INIT_VNET_INET6((struct vnet *) rock);
	struct radix_node_head *rnh = rock;
	struct mtuex_arg arg;
	struct timeval atv;

	arg.rnh = rnh;
	arg.nextstop = time_uptime + MTUTIMO_DEFAULT;
	RADIX_NODE_HEAD_LOCK(rnh);
	rnh->rnh_walktree(rnh, in6_mtuexpire, &arg);
	RADIX_NODE_HEAD_UNLOCK(rnh);

	atv.tv_usec = 0;
	atv.tv_sec = arg.nextstop - time_uptime;
	if (atv.tv_sec < 0) {
		printf("invalid mtu expiration time on routing table\n");
		arg.nextstop = time_uptime + 30;	/* last resort */
		atv.tv_sec = 30;
	}
	callout_reset(&V_rtq_mtutimer, tvtohz(&atv), in6_mtutimo, rock);
	CURVNET_RESTORE();
}

#if 0
void
in6_rtqdrain(void)
{
	INIT_VNET_NET(curvnet);
	struct radix_node_head *rnh = V_rt_tables[AF_INET6];
	struct rtqk_arg arg;

	arg.found = arg.killed = 0;
	arg.rnh = rnh;
	arg.nextstop = 0;
	arg.draining = 1;
	arg.updating = 0;
	RADIX_NODE_HEAD_LOCK(rnh);
	rnh->rnh_walktree(rnh, in6_rtqkill, &arg);
	RADIX_NODE_HEAD_UNLOCK(rnh);
}
#endif

/*
 * Initialize our routing tree.
 * XXX MRT When off == 0, we are being called from vfs_export.c
 * so just set up their table and leave. (we know what the correct
 * value should be so just use that).. FIX AFTER RELENG_7 is MFC'd
 * see also comments in in_inithead() vfs_export.c and domain.h
 */
int
in6_inithead(void **head, int off)
{
	INIT_VNET_INET6(curvnet);
	struct radix_node_head *rnh;

	if (!rn_inithead(head, offsetof(struct sockaddr_in6, sin6_addr) << 3))
		return 0;		/* See above */

	if (off == 0)		/* See above */
		return 1;	/* only do the rest for the real thing */

	V_rtq_reallyold6 = 60*60; /* one hour is ``really old'' */
	V_rtq_minreallyold6 = 10; /* never automatically crank down to less */
	V_rtq_toomany6 = 128;	  /* 128 cached routes is ``too many'' */
	V_rtq_timeout6 = RTQ_TIMEOUT;

	rnh = *head;
	rnh->rnh_addaddr = in6_addroute;
	rnh->rnh_matchaddr = in6_matroute;
	callout_init(&V_rtq_timer6, CALLOUT_MPSAFE);
	in6_rtqtimo(rnh);	/* kick off timeout first time */
	callout_init(&V_rtq_mtutimer, CALLOUT_MPSAFE);
	in6_mtutimo(rnh);	/* kick off timeout first time */
	return 1;
}
