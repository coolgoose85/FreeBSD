#$FreeBSD$
.PATH:  ${.CURDIR}/../../dev/ixgbe
KMOD    = ixgbe
SRCS    = device_if.h bus_if.h pci_if.h
SRCS    += ixgbe.c
# Shared source
SRCS    += ixgbe_common.c ixgbe_api.c ixgbe_phy.c ixgbe_82598.c
CFLAGS+= -I${.CURDIR}/../../dev/ixgbe

clean:
	rm -f device_if.h bus_if.h pci_if.h setdef* *_StripErr
	rm -f *.o *.kld *.ko
	rm -f @ machine

.include <bsd.kmod.mk>
