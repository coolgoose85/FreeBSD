/*	$NetBSD: ehcivar.h,v 1.19 2005/04/29 15:04:29 augustss Exp $	*/
/*	$FreeBSD$	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

typedef struct ehci_soft_qtd {
	ehci_qtd_t qtd;
	struct ehci_soft_qtd *nextqtd; /* mirrors nextqtd in TD */
	ehci_physaddr_t physaddr;
	usbd_xfer_handle xfer;
	LIST_ENTRY(ehci_soft_qtd) hnext;
	u_int16_t len;
} ehci_soft_qtd_t;
#define EHCI_SQTD_SIZE ((sizeof (struct ehci_soft_qtd) + EHCI_QTD_ALIGN - 1) / EHCI_QTD_ALIGN * EHCI_QTD_ALIGN)
#define EHCI_SQTD_CHUNK (EHCI_PAGE_SIZE / EHCI_SQTD_SIZE)

typedef struct ehci_soft_qh {
	ehci_qh_t qh;
	struct ehci_soft_qh *next;
	struct ehci_soft_qh *prev;
	struct ehci_soft_qtd *sqtd;
	struct ehci_soft_qtd *inactivesqtd;
	ehci_physaddr_t physaddr;
	int islot;		/* Interrupt list slot. */
} ehci_soft_qh_t;
#define EHCI_SQH_SIZE ((sizeof (struct ehci_soft_qh) + EHCI_QH_ALIGN - 1) / EHCI_QH_ALIGN * EHCI_QH_ALIGN)
#define EHCI_SQH_CHUNK (EHCI_PAGE_SIZE / EHCI_SQH_SIZE)

typedef struct ehci_soft_itd {
	ehci_itd_t itd;
	union {
		struct {
			/* soft_itds links in a periodic frame*/
			struct ehci_soft_itd *next;
			struct ehci_soft_itd *prev;
		} frame_list;
		/* circular list of free itds */
		LIST_ENTRY(ehci_soft_itd) free_list;
	} u;
	struct ehci_soft_itd *xfer_next; /* Next soft_itd in xfer */
	ehci_physaddr_t physaddr;
	usb_dma_t dma;
	int offs;
	int slot;
	struct timeval t; /* store free time */
} ehci_soft_itd_t;
#define EHCI_ITD_SIZE ((sizeof(struct ehci_soft_itd) + EHCI_QH_ALIGN - 1) / EHCI_ITD_ALIGN * EHCI_ITD_ALIGN)
#define EHCI_ITD_CHUNK (EHCI_PAGE_SIZE / EHCI_ITD_SIZE)

struct ehci_xfer {
	struct usbd_xfer xfer;
	struct usb_task	abort_task;
	LIST_ENTRY(ehci_xfer) inext; /* list of active xfers */
	ehci_soft_qtd_t *sqtdstart;
	ehci_soft_qtd_t *sqtdend;
	ehci_soft_itd_t *itdstart;
	ehci_soft_itd_t *itdend;
	u_int isoc_len;
	u_int32_t ehci_xfer_flags;
#ifdef DIAGNOSTIC
	int isdone;
#endif
};
#define EHCI_XFER_ABORTING	0x0001	/* xfer is aborting. */
#define EHCI_XFER_ABORTWAIT	0x0002	/* abort completion is being awaited. */

#define EXFER(xfer) ((struct ehci_xfer *)(xfer))

/*
 * Information about an entry in the interrupt list.
 */
struct ehci_soft_islot {
	ehci_soft_qh_t *sqh;		/* Queue Head. */
};

#define EHCI_FRAMELIST_MAXCOUNT	1024
#define EHCI_IPOLLRATES		8	/* Poll rates (1ms, 2, 4, 8 ... 128) */
#define EHCI_INTRQHS		((1 << EHCI_IPOLLRATES) - 1)
#define EHCI_MAX_POLLRATE	(1 << (EHCI_IPOLLRATES - 1))
#define EHCI_IQHIDX(lev, pos)	\
    ((((pos) & ((1 << (lev)) - 1)) | (1 << (lev))) - 1)
#define EHCI_ILEV_IVAL(lev)	(1 << (lev))

#define EHCI_HASH_SIZE 128
#define EHCI_COMPANION_MAX 8

#define	EHCI_FREE_LIST_INTERVAL	100

#define EHCI_SCFLG_DONEINIT	0x0001	/* ehci_init() has been called. */
#define EHCI_SCFLG_LOSTINTRBUG	0x0002	/* workaround for VIA / ATI chipsets */
#define EHCI_SCFLG_SETMODE	0x0004	/* set bridge mode again after init (Marvell) */
#define EHCI_SCFLG_FORCESPEED	0x0008	/* force speed (Marvell) */
#define EHCI_SCFLG_NORESTERM	0x0010	/* don't terminate reset sequence (Marvell) */
#define	EHCI_SCFLG_BIGEDESC	0x0020	/* big-endian byte order descriptors */
#define	EHCI_SCFLG_BIGEMMIO	0x0040	/* big-endian byte order MMIO */
#define	EHCI_SCFLG_TT		0x0080	/* transaction translator present */

typedef struct ehci_softc {
	struct usbd_bus sc_bus;		/* base device */
	int sc_flags;
	bus_space_tag_t iot;
	bus_space_handle_t ioh;
	bus_size_t sc_size;
	void *ih;

	struct resource *io_res;
	struct resource *irq_res;
	u_int sc_offs;			/* offset to operational regs */

	char sc_vendor[32];		/* vendor string for root hub */
	int sc_id_vendor;		/* vendor ID for root hub */

	u_int32_t sc_cmd;		/* shadow of cmd reg during suspend */

	u_int sc_ncomp;
	u_int sc_npcomp;
	struct usbd_bus *sc_comps[EHCI_COMPANION_MAX];

	usb_dma_t sc_fldma;
	ehci_link_t *sc_flist;
	u_int sc_flsize;

	struct ehci_soft_islot sc_islots[EHCI_INTRQHS];

	/* jcmm - an array matching sc_flist, but with software pointers,
	 * not hardware address pointers
	 */
	struct ehci_soft_itd **sc_softitds;

	LIST_HEAD(, ehci_xfer) sc_intrhead;

	ehci_soft_qh_t *sc_freeqhs;
	ehci_soft_qtd_t *sc_freeqtds;
	LIST_HEAD(sc_freeitds, ehci_soft_itd) sc_freeitds;

	int sc_noport;
	u_int8_t sc_addr;		/* device address */
	u_int8_t sc_conf;		/* device configuration */
	usbd_xfer_handle sc_intrxfer;
	char sc_isreset;
#ifdef USB_USE_SOFTINTR
	char sc_softwake;
#endif /* USB_USE_SOFTINTR */

	u_int32_t sc_eintrs;
	ehci_soft_qh_t *sc_async_head;

	STAILQ_HEAD(, usbd_xfer) sc_free_xfers; /* free xfers */

	struct lock sc_doorbell_lock;

	struct callout sc_tmo_intrlist;

	char sc_dying;
} ehci_softc_t;

#define EREAD1(sc, a) bus_space_read_1((sc)->iot, (sc)->ioh, (a))
#define EREAD2(sc, a) bus_space_read_2((sc)->iot, (sc)->ioh, (a))
#define EREAD4(sc, a) bus_space_read_4((sc)->iot, (sc)->ioh, (a))
#define EWRITE1(sc, a, x) bus_space_write_1((sc)->iot, (sc)->ioh, (a), (x))
#define EWRITE2(sc, a, x) bus_space_write_2((sc)->iot, (sc)->ioh, (a), (x))
#define EWRITE4(sc, a, x) bus_space_write_4((sc)->iot, (sc)->ioh, (a), (x))
#define EOREAD1(sc, a) bus_space_read_1((sc)->iot, (sc)->ioh, (sc)->sc_offs+(a))
#define EOREAD2(sc, a) bus_space_read_2((sc)->iot, (sc)->ioh, (sc)->sc_offs+(a))
#define EOREAD4(sc, a) bus_space_read_4((sc)->iot, (sc)->ioh, (sc)->sc_offs+(a))
#define EOWRITE1(sc, a, x) bus_space_write_1((sc)->iot, (sc)->ioh, (sc)->sc_offs+(a), (x))
#define EOWRITE2(sc, a, x) bus_space_write_2((sc)->iot, (sc)->ioh, (sc)->sc_offs+(a), (x))
#define EOWRITE4(sc, a, x) bus_space_write_4((sc)->iot, (sc)->ioh, (sc)->sc_offs+(a), (x))

#ifdef USB_EHCI_BIG_ENDIAN_DESC
/*
 * Handle byte order conversion between host and ``host controller''.
 * Typically the latter is little-endian but some controllers require
 * big-endian in which case we may need to manually swap.
 */
static __inline uint32_t
htohc32(const struct ehci_softc *sc, const uint32_t v)
{
	return sc->sc_flags & EHCI_SCFLG_BIGEDESC ? htobe32(v) : htole32(v);
}

static __inline uint16_t
htohc16(const struct ehci_softc *sc, const uint16_t v)
{
	return sc->sc_flags & EHCI_SCFLG_BIGEDESC ? htobe16(v) : htole16(v);
}

static __inline uint32_t
hc32toh(const struct ehci_softc *sc, const uint32_t v)
{
	return sc->sc_flags & EHCI_SCFLG_BIGEDESC ? be32toh(v) : le32toh(v);
}

static __inline uint16_t
hc16toh(const struct ehci_softc *sc, const uint16_t v)
{
	return sc->sc_flags & EHCI_SCFLG_BIGEDESC ? be16toh(v) : le16toh(v);
}
#else
/*
 * Normal little-endian only conversion routines.
 */
static __inline uint32_t
htohc32(const struct ehci_softc *sc, const uint32_t v)
{
	return htole32(v);
}

static __inline uint16_t
htohc16(const struct ehci_softc *sc, const uint16_t v)
{
	return htole16(v);
}

static __inline uint32_t
hc32toh(const struct ehci_softc *sc, const uint32_t v)
{
	return le32toh(v);
}

static __inline uint16_t
hc16toh(const struct ehci_softc *sc, const uint16_t v)
{
	return le16toh(v);
}
#endif

usbd_status	ehci_reset(ehci_softc_t *);
usbd_status	ehci_init(ehci_softc_t *);
int		ehci_intr(void *);
int		ehci_detach(ehci_softc_t *, int);
void		ehci_power(int state, void *priv);
void		ehci_shutdown(void *v);

#define MS_TO_TICKS(ms) ((ms) * hz / 1000)

void		ehci_dump_regs(ehci_softc_t *);
void		ehci_dump_sqtds(ehci_softc_t *, ehci_soft_qtd_t *);
void		ehci_dump_qtd(ehci_softc_t *, ehci_qtd_t *);
void		ehci_dump_sqtd(ehci_softc_t *, ehci_soft_qtd_t *);
void		ehci_dump_sqh(ehci_softc_t *, ehci_soft_qh_t *);
void		ehci_dump_itd(ehci_softc_t *, struct ehci_soft_itd *);
void		ehci_dump_sitd(ehci_softc_t *, struct ehci_soft_itd *);
void		ehci_dump_exfer(struct ehci_xfer *);
