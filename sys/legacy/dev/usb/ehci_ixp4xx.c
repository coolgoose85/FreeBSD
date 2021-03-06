/*-
 * Copyright (c) 2008 Sam Leffler.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * IXP435 attachment driver for the USB Enhanced Host Controller.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_bus.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/bus.h>
#include <sys/endian.h>
#include <sys/queue.h>
#include <sys/lockmgr.h>
#include <sys/rman.h>

#include <machine/bus.h>
#include <machine/resource.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdivar.h>
#include <dev/usb/usb_mem.h>

#include <dev/usb/ehcireg.h>
#include <dev/usb/ehcivar.h>

#include <arm/xscale/ixp425/ixp425reg.h>
#include <arm/xscale/ixp425/ixp425var.h>

#define EHCI_VENDORID_IXP4XX	0x42fa05
#define EHCI_HC_DEVSTR		"IXP4XX Integrated USB 2.0 controller"

struct ixp_ehci_softc {
	ehci_softc_t		base;	/* storage for EHCI code */
	bus_space_tag_t		iot;
	bus_space_handle_t	ioh;
	struct bus_space	tag;	/* tag for private bus space ops */
};

static int ehci_ixp_detach(device_t self);

static uint8_t ehci_bs_r_1(void *, bus_space_handle_t, bus_size_t);
static void ehci_bs_w_1(void *, bus_space_handle_t, bus_size_t, u_int8_t);
static uint16_t ehci_bs_r_2(void *, bus_space_handle_t, bus_size_t);
static void ehci_bs_w_2(void *, bus_space_handle_t, bus_size_t, uint16_t);
static uint32_t ehci_bs_r_4(void *, bus_space_handle_t, bus_size_t);
static void ehci_bs_w_4(void *, bus_space_handle_t, bus_size_t, uint32_t);

static int
ehci_ixp_suspend(device_t self)
{
	ehci_softc_t *sc;
	int err;

	err = bus_generic_suspend(self);
	if (err == 0) {
		sc = device_get_softc(self);
		ehci_power(PWR_SUSPEND, sc);
	}
	return err;
}

static int
ehci_ixp_resume(device_t self)
{
	ehci_softc_t *sc = device_get_softc(self);

	ehci_power(PWR_RESUME, sc);
	bus_generic_resume(self);
	return 0;
}

static int
ehci_ixp_shutdown(device_t self)
{
	ehci_softc_t *sc;
	int err;

	err = bus_generic_shutdown(self);
	if (err == 0) {
		sc = device_get_softc(self);
		ehci_shutdown(sc);
	}
	return err;
}

static int
ehci_ixp_probe(device_t self)
{
	device_set_desc(self, EHCI_HC_DEVSTR);
	return BUS_PROBE_DEFAULT;
}

static int
ehci_ixp_attach(device_t self)
{
	struct ixp_ehci_softc *isc = device_get_softc(self);
	ehci_softc_t *sc = &isc->base;
	int err, rid;

	sc->sc_bus.usbrev = USBREV_2_0;

	/* NB: hints fix the memory location and irq */

	rid = 0;
	sc->io_res = bus_alloc_resource_any(self, SYS_RES_MEMORY,
	    &rid, RF_ACTIVE);
	if (sc->io_res == NULL) {
		device_printf(self, "Could not map memory\n");
		return ENXIO;
	}

	/*
	 * Craft special resource for bus space ops that handle
	 * byte-alignment of non-word addresses.  Also, since
	 * we're already intercepting bus space ops we handle
	 * the register window offset that could otherwise be
	 * done with bus_space_subregion.
	 */
	isc->iot = rman_get_bustag(sc->io_res);
	isc->tag.bs_cookie = isc->iot;
	/* read single */
	isc->tag.bs_r_1	= ehci_bs_r_1,
	isc->tag.bs_r_2	= ehci_bs_r_2,
	isc->tag.bs_r_4	= ehci_bs_r_4,
	/* write (single) */
	isc->tag.bs_w_1	= ehci_bs_w_1,
	isc->tag.bs_w_2	= ehci_bs_w_2,
	isc->tag.bs_w_4	= ehci_bs_w_4,

	sc->iot = &isc->tag;
	sc->ioh = rman_get_bushandle(sc->io_res);
	sc->sc_size = IXP435_USB1_SIZE - 0x100;

	rid = 0;
	sc->irq_res = bus_alloc_resource_any(self, SYS_RES_IRQ,
	    &rid, RF_ACTIVE);
	if (sc->irq_res == NULL) {
		device_printf(self, "Could not allocate irq\n");
		ehci_ixp_detach(self);
		return ENXIO;
	}
	sc->sc_bus.bdev = device_add_child(self, "usb", -1);
	if (!sc->sc_bus.bdev) {
		device_printf(self, "Could not add USB device\n");
		ehci_ixp_detach(self);
		return ENOMEM;
	}
	device_set_ivars(sc->sc_bus.bdev, &sc->sc_bus);

	sprintf(sc->sc_vendor, "Intel");
	sc->sc_id_vendor = EHCI_VENDORID_IXP4XX;

	err = bus_setup_intr(self, sc->irq_res, INTR_TYPE_BIO,
	    NULL, (driver_intr_t*)ehci_intr, sc, &sc->ih);
	if (err) {
		device_printf(self, "Could not setup irq, %d\n", err);
		sc->ih = NULL;
		ehci_ixp_detach(self);
		return ENXIO;
	}

	/* There are no companion USB controllers */
	sc->sc_ncomp = 0;

	/* Allocate a parent dma tag for DMA maps */
	err = bus_dma_tag_create(bus_get_dma_tag(self), 1, 0,
	    BUS_SPACE_MAXADDR_32BIT, BUS_SPACE_MAXADDR, NULL, NULL,
	    BUS_SPACE_MAXSIZE_32BIT, USB_DMA_NSEG, BUS_SPACE_MAXSIZE_32BIT, 0,
	    NULL, NULL, &sc->sc_bus.parent_dmatag);
	if (err) {
		device_printf(self, "Could not allocate parent DMA tag (%d)\n",
		    err);
		ehci_ixp_detach(self);
		return ENXIO;
	}

	/* Allocate a dma tag for transfer buffers */
	err = bus_dma_tag_create(sc->sc_bus.parent_dmatag, 1, 0,
	    BUS_SPACE_MAXADDR_32BIT, BUS_SPACE_MAXADDR, NULL, NULL,
	    BUS_SPACE_MAXSIZE_32BIT, USB_DMA_NSEG, BUS_SPACE_MAXSIZE_32BIT, 0,
	    busdma_lock_mutex, &Giant, &sc->sc_bus.buffer_dmatag);
	if (err) {
		device_printf(self, "Could not allocate buffer DMA tag (%d)\n",
		    err);
		ehci_ixp_detach(self);
		return ENXIO;
	}

	/*
	 * Arrange to force Host mode, select big-endian byte alignment,
	 * and arrange to not terminate reset operations (the adapter
	 * will ignore it if we do but might as well save a reg write).
	 * Also, the controller has an embedded Transaction Translator
	 * which means port speed must be read from the Port Status
	 * register following a port enable.
	 */
	sc->sc_flags |= EHCI_SCFLG_TT
		     | EHCI_SCFLG_SETMODE
		     | EHCI_SCFLG_BIGEDESC
		     | EHCI_SCFLG_BIGEMMIO
		     | EHCI_SCFLG_NORESTERM
		     ;
	(void) ehci_reset(sc);

	err = ehci_init(sc);
	if (!err) {
		sc->sc_flags |= EHCI_SCFLG_DONEINIT;
		err = device_probe_and_attach(sc->sc_bus.bdev);
	}

	if (err) {
		device_printf(self, "USB init failed err=%d\n", err);
		ehci_ixp_detach(self);
		return EIO;
	}
	return 0;
}

static int
ehci_ixp_detach(device_t self)
{
	struct ixp_ehci_softc *isc = device_get_softc(self);
	ehci_softc_t *sc = &isc->base;
	int err;

	if (sc->sc_flags & EHCI_SCFLG_DONEINIT) {
		ehci_detach(sc, 0);
		sc->sc_flags &= ~EHCI_SCFLG_DONEINIT;
	}

	/*
	 * Disable interrupts that might have been switched on in ehci_init()
	 */
	if (sc->iot && sc->ioh)
		bus_space_write_4(sc->iot, sc->ioh, EHCI_USBINTR, 0);
	if (sc->sc_bus.parent_dmatag != NULL)
		bus_dma_tag_destroy(sc->sc_bus.parent_dmatag);
	if (sc->sc_bus.buffer_dmatag != NULL)
		bus_dma_tag_destroy(sc->sc_bus.buffer_dmatag);

	if (sc->irq_res && sc->ih) {
		err = bus_teardown_intr(self, sc->irq_res, sc->ih);

		if (err)
			device_printf(self, "Could not tear down irq, %d\n",
			    err);
		sc->ih = NULL;
	}
	if (sc->sc_bus.bdev != NULL) {
		device_delete_child(self, sc->sc_bus.bdev);
		sc->sc_bus.bdev = NULL;
	}
	if (sc->irq_res != NULL) {
		bus_release_resource(self, SYS_RES_IRQ, 0, sc->irq_res);
		sc->irq_res = NULL;
	}
	if (sc->io_res != NULL) {
		bus_release_resource(self, SYS_RES_MEMORY, 0, sc->io_res);
		sc->io_res = NULL;
	}
	sc->iot = 0;
	sc->ioh = 0;
	return 0;
}

/*
 * Bus space accessors for PIO operations.
 */

static uint8_t
ehci_bs_r_1(void *t, bus_space_handle_t h, bus_size_t o)
{
	return bus_space_read_1((bus_space_tag_t) t, h,
	    0x100 + (o &~ 3) + (3 - (o & 3)));
}

static void
ehci_bs_w_1(void *t, bus_space_handle_t h, bus_size_t o, u_int8_t v)
{
	panic("%s", __func__);
}

static uint16_t
ehci_bs_r_2(void *t, bus_space_handle_t h, bus_size_t o)
{
	return bus_space_read_2((bus_space_tag_t) t, h,
	    0x100 + (o &~ 3) + (2 - (o & 3)));
}

static void
ehci_bs_w_2(void *t, bus_space_handle_t h, bus_size_t o, uint16_t v)
{
	panic("%s", __func__);
}

static uint32_t
ehci_bs_r_4(void *t, bus_space_handle_t h, bus_size_t o)
{
	return bus_space_read_4((bus_space_tag_t) t, h, 0x100 + o);
}

static void
ehci_bs_w_4(void *t, bus_space_handle_t h, bus_size_t o, uint32_t v)
{
	bus_space_write_4((bus_space_tag_t) t, h, 0x100 + o, v);
}

static device_method_t ehci_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe, ehci_ixp_probe),
	DEVMETHOD(device_attach, ehci_ixp_attach),
	DEVMETHOD(device_detach, ehci_ixp_detach),
	DEVMETHOD(device_suspend, ehci_ixp_suspend),
	DEVMETHOD(device_resume, ehci_ixp_resume),
	DEVMETHOD(device_shutdown, ehci_ixp_shutdown),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),

	{0, 0}
};

static driver_t ehci_driver = {
	"ehci",
	ehci_methods,
	sizeof(struct ixp_ehci_softc),
};
static devclass_t ehci_devclass;
DRIVER_MODULE(ehci, ixp, ehci_driver, ehci_devclass, 0, 0);
