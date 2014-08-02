# $FreeBSD: src/sys/modules/sym/Makefile,v 1.2 2000/12/19 12:27:53 alex Exp $

.PATH: ${.CURDIR}/../../dev/sym

KMOD=	sym
SRCS=	sym_hipd.c opt_sym.h opt_cam.h opt_scsi.h
SRCS+=	device_if.h bus_if.h pci_if.h

.include <bsd.kmod.mk>
