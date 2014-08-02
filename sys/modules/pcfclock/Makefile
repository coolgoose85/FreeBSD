# $FreeBSD: src/sys/modules/pcfclock/Makefile,v 1.3 2000/05/27 01:14:08 peter Exp $

.PATH:	${.CURDIR}/../../dev/ppbus

KMOD=	pcfclock
SRCS=	pcfclock.c opt_pcfclock.h
SRCS+=	bus_if.h device_if.h ppbus_if.h

.include <bsd.kmod.mk>
