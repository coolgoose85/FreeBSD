/*-
 * Copyright (c) 2004 Bernd Walter <ticso@FreeBSD.org>
 *
 * $URL: https://devel.bwct.de/svn/projects/ubser/ubser.c $
 * $Date: 2009/03/02 05:37:05 $
 * $Author: thompsa $
 * $Rev: 1127 $
 */

/*-
 * Copyright (c) 2001-2002, Shunsuke Akiyama <akiyama@jp.FreeBSD.org>.
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
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

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*
 * BWCT serial adapter driver
 */

#include <dev/usb/usb.h>
#include <dev/usb/usb_mfunc.h>
#include <dev/usb/usb_error.h>
#include <dev/usb/usb_cdc.h>
#include <dev/usb/usb_defs.h>

#define	USB_DEBUG_VAR ubser_debug

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_debug.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_request.h>
#include <dev/usb/usb_lookup.h>
#include <dev/usb/usb_util.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_device.h>

#include <dev/usb/serial/usb_serial.h>

#define	UBSER_UNIT_MAX	32

/* Vendor Interface Requests */
#define	VENDOR_GET_NUMSER		0x01
#define	VENDOR_SET_BREAK		0x02
#define	VENDOR_CLEAR_BREAK		0x03

#if USB_DEBUG
static int ubser_debug = 0;

SYSCTL_NODE(_hw_usb2, OID_AUTO, ubser, CTLFLAG_RW, 0, "USB ubser");
SYSCTL_INT(_hw_usb2_ubser, OID_AUTO, debug, CTLFLAG_RW,
    &ubser_debug, 0, "ubser debug level");
#endif

enum {
	UBSER_BULK_DT_WR,
	UBSER_BULK_DT_RD,
	UBSER_N_TRANSFER,
};

struct ubser_softc {
	struct usb2_com_super_softc sc_super_ucom;
	struct usb2_com_softc sc_ucom[UBSER_UNIT_MAX];

	struct usb2_xfer *sc_xfer[UBSER_N_TRANSFER];
	struct usb2_device *sc_udev;
	struct mtx sc_mtx;

	uint16_t sc_tx_size;

	uint8_t	sc_numser;
	uint8_t	sc_iface_no;
	uint8_t	sc_iface_index;
	uint8_t	sc_curr_tx_unit;
	uint8_t	sc_name[16];
};

/* prototypes */

static device_probe_t ubser_probe;
static device_attach_t ubser_attach;
static device_detach_t ubser_detach;

static usb2_callback_t ubser_write_callback;
static usb2_callback_t ubser_read_callback;

static int	ubser_pre_param(struct usb2_com_softc *, struct termios *);
static void	ubser_cfg_set_break(struct usb2_com_softc *, uint8_t);
static void	ubser_cfg_get_status(struct usb2_com_softc *, uint8_t *,
		    uint8_t *);
static void	ubser_start_read(struct usb2_com_softc *);
static void	ubser_stop_read(struct usb2_com_softc *);
static void	ubser_start_write(struct usb2_com_softc *);
static void	ubser_stop_write(struct usb2_com_softc *);

static const struct usb2_config ubser_config[UBSER_N_TRANSFER] = {

	[UBSER_BULK_DT_WR] = {
		.type = UE_BULK,
		.endpoint = UE_ADDR_ANY,
		.direction = UE_DIR_OUT,
		.mh.bufsize = 0,	/* use wMaxPacketSize */
		.mh.flags = {.pipe_bof = 1,.force_short_xfer = 1,},
		.mh.callback = &ubser_write_callback,
	},

	[UBSER_BULK_DT_RD] = {
		.type = UE_BULK,
		.endpoint = UE_ADDR_ANY,
		.direction = UE_DIR_IN,
		.mh.bufsize = 0,	/* use wMaxPacketSize */
		.mh.flags = {.pipe_bof = 1,.short_xfer_ok = 1,},
		.mh.callback = &ubser_read_callback,
	},
};

static const struct usb2_com_callback ubser_callback = {
	.usb2_com_cfg_set_break = &ubser_cfg_set_break,
	.usb2_com_cfg_get_status = &ubser_cfg_get_status,
	.usb2_com_pre_param = &ubser_pre_param,
	.usb2_com_start_read = &ubser_start_read,
	.usb2_com_stop_read = &ubser_stop_read,
	.usb2_com_start_write = &ubser_start_write,
	.usb2_com_stop_write = &ubser_stop_write,
};

static device_method_t ubser_methods[] = {
	DEVMETHOD(device_probe, ubser_probe),
	DEVMETHOD(device_attach, ubser_attach),
	DEVMETHOD(device_detach, ubser_detach),
	{0, 0}
};

static devclass_t ubser_devclass;

static driver_t ubser_driver = {
	.name = "ubser",
	.methods = ubser_methods,
	.size = sizeof(struct ubser_softc),
};

DRIVER_MODULE(ubser, uhub, ubser_driver, ubser_devclass, NULL, 0);
MODULE_DEPEND(ubser, ucom, 1, 1, 1);
MODULE_DEPEND(ubser, usb, 1, 1, 1);

static int
ubser_probe(device_t dev)
{
	struct usb2_attach_arg *uaa = device_get_ivars(dev);

	if (uaa->usb2_mode != USB_MODE_HOST) {
		return (ENXIO);
	}
	/* check if this is a BWCT vendor specific ubser interface */
	if ((strcmp(uaa->device->manufacturer, "BWCT") == 0) &&
	    (uaa->info.bInterfaceClass == 0xff) &&
	    (uaa->info.bInterfaceSubClass == 0x00))
		return (0);

	return (ENXIO);
}

static int
ubser_attach(device_t dev)
{
	struct usb2_attach_arg *uaa = device_get_ivars(dev);
	struct ubser_softc *sc = device_get_softc(dev);
	struct usb2_device_request req;
	uint8_t n;
	int error;

	device_set_usb2_desc(dev);
	mtx_init(&sc->sc_mtx, "ubser", NULL, MTX_DEF);

	snprintf(sc->sc_name, sizeof(sc->sc_name), "%s",
	    device_get_nameunit(dev));

	sc->sc_iface_no = uaa->info.bIfaceNum;
	sc->sc_iface_index = uaa->info.bIfaceIndex;
	sc->sc_udev = uaa->device;

	/* get number of serials */
	req.bmRequestType = UT_READ_VENDOR_INTERFACE;
	req.bRequest = VENDOR_GET_NUMSER;
	USETW(req.wValue, 0);
	req.wIndex[0] = sc->sc_iface_no;
	req.wIndex[1] = 0;
	USETW(req.wLength, 1);
	error = usb2_do_request_flags
	    (uaa->device, &Giant, &req, &sc->sc_numser,
	    0, NULL, USB_DEFAULT_TIMEOUT);

	if (error || (sc->sc_numser == 0)) {
		device_printf(dev, "failed to get number "
		    "of serial ports: %s\n",
		    usb2_errstr(error));
		goto detach;
	}
	if (sc->sc_numser > UBSER_UNIT_MAX)
		sc->sc_numser = UBSER_UNIT_MAX;

	device_printf(dev, "found %i serials\n", sc->sc_numser);

	error = usb2_transfer_setup(uaa->device, &sc->sc_iface_index,
	    sc->sc_xfer, ubser_config, UBSER_N_TRANSFER, sc, &sc->sc_mtx);
	if (error) {
		goto detach;
	}
	sc->sc_tx_size = sc->sc_xfer[UBSER_BULK_DT_WR]->max_data_length;

	if (sc->sc_tx_size == 0) {
		DPRINTFN(0, "invalid tx_size!\n");
		goto detach;
	}
	/* initialize port numbers */

	for (n = 0; n < sc->sc_numser; n++) {
		sc->sc_ucom[n].sc_portno = n;
	}

	error = usb2_com_attach(&sc->sc_super_ucom, sc->sc_ucom,
	    sc->sc_numser, sc, &ubser_callback, &sc->sc_mtx);
	if (error) {
		goto detach;
	}

	mtx_lock(&sc->sc_mtx);
	usb2_transfer_set_stall(sc->sc_xfer[UBSER_BULK_DT_WR]);
	usb2_transfer_set_stall(sc->sc_xfer[UBSER_BULK_DT_RD]);
	usb2_transfer_start(sc->sc_xfer[UBSER_BULK_DT_RD]);
	mtx_unlock(&sc->sc_mtx);

	return (0);			/* success */

detach:
	ubser_detach(dev);
	return (ENXIO);			/* failure */
}

static int
ubser_detach(device_t dev)
{
	struct ubser_softc *sc = device_get_softc(dev);

	DPRINTF("\n");

	usb2_com_detach(&sc->sc_super_ucom, sc->sc_ucom, sc->sc_numser);
	usb2_transfer_unsetup(sc->sc_xfer, UBSER_N_TRANSFER);
	mtx_destroy(&sc->sc_mtx);

	return (0);
}

static int
ubser_pre_param(struct usb2_com_softc *ucom, struct termios *t)
{
	DPRINTF("\n");

	/*
	 * The firmware on our devices can only do 8n1@9600bps
	 * without handshake.
	 * We refuse to accept other configurations.
	 */

	/* ensure 9600bps */
	switch (t->c_ospeed) {
	case 9600:
		break;
	default:
		return (EINVAL);
	}

	/* 2 stop bits not possible */
	if (t->c_cflag & CSTOPB)
		return (EINVAL);

	/* XXX parity handling not possible with current firmware */
	if (t->c_cflag & PARENB)
		return (EINVAL);

	/* we can only do 8 data bits */
	switch (t->c_cflag & CSIZE) {
	case CS8:
		break;
	default:
		return (EINVAL);
	}

	/* we can't do any kind of hardware handshaking */
	if ((t->c_cflag &
	    (CRTS_IFLOW | CDTR_IFLOW | CDSR_OFLOW | CCAR_OFLOW)) != 0)
		return (EINVAL);

	/*
	 * XXX xon/xoff not supported by the firmware!
	 * This is handled within FreeBSD only and may overflow buffers
	 * because of delayed reaction due to device buffering.
	 */

	return (0);
}

static __inline void
ubser_inc_tx_unit(struct ubser_softc *sc)
{
	sc->sc_curr_tx_unit++;
	if (sc->sc_curr_tx_unit >= sc->sc_numser) {
		sc->sc_curr_tx_unit = 0;
	}
}

static void
ubser_write_callback(struct usb2_xfer *xfer)
{
	struct ubser_softc *sc = xfer->priv_sc;
	uint8_t buf[1];
	uint8_t first_unit = sc->sc_curr_tx_unit;
	uint32_t actlen;

	switch (USB_GET_STATE(xfer)) {
	case USB_ST_SETUP:
	case USB_ST_TRANSFERRED:
tr_setup:
		do {
			if (usb2_com_get_data(sc->sc_ucom + sc->sc_curr_tx_unit,
			    xfer->frbuffers, 1, sc->sc_tx_size - 1,
			    &actlen)) {

				buf[0] = sc->sc_curr_tx_unit;

				usb2_copy_in(xfer->frbuffers, 0, buf, 1);

				xfer->frlengths[0] = actlen + 1;
				usb2_start_hardware(xfer);

				ubser_inc_tx_unit(sc);	/* round robin */

				break;
			}
			ubser_inc_tx_unit(sc);

		} while (sc->sc_curr_tx_unit != first_unit);

		return;

	default:			/* Error */
		if (xfer->error != USB_ERR_CANCELLED) {
			/* try to clear stall first */
			xfer->flags.stall_pipe = 1;
			goto tr_setup;
		}
		return;

	}
}

static void
ubser_read_callback(struct usb2_xfer *xfer)
{
	struct ubser_softc *sc = xfer->priv_sc;
	uint8_t buf[1];

	switch (USB_GET_STATE(xfer)) {
	case USB_ST_TRANSFERRED:
		if (xfer->actlen < 1) {
			DPRINTF("invalid actlen=0!\n");
			goto tr_setup;
		}
		usb2_copy_out(xfer->frbuffers, 0, buf, 1);

		if (buf[0] >= sc->sc_numser) {
			DPRINTF("invalid serial number!\n");
			goto tr_setup;
		}
		usb2_com_put_data(sc->sc_ucom + buf[0],
		    xfer->frbuffers, 1, xfer->actlen - 1);

	case USB_ST_SETUP:
tr_setup:
		xfer->frlengths[0] = xfer->max_data_length;
		usb2_start_hardware(xfer);
		return;

	default:			/* Error */
		if (xfer->error != USB_ERR_CANCELLED) {
			/* try to clear stall first */
			xfer->flags.stall_pipe = 1;
			goto tr_setup;
		}
		return;

	}
}

static void
ubser_cfg_set_break(struct usb2_com_softc *ucom, uint8_t onoff)
{
	struct ubser_softc *sc = ucom->sc_parent;
	uint8_t x = ucom->sc_portno;
	struct usb2_device_request req;
	usb2_error_t err;

	if (onoff) {

		req.bmRequestType = UT_READ_VENDOR_INTERFACE;
		req.bRequest = VENDOR_SET_BREAK;
		req.wValue[0] = x;
		req.wValue[1] = 0;
		req.wIndex[0] = sc->sc_iface_no;
		req.wIndex[1] = 0;
		USETW(req.wLength, 0);

		err = usb2_com_cfg_do_request(sc->sc_udev, ucom, 
		    &req, NULL, 0, 1000);
		if (err) {
			DPRINTFN(0, "send break failed, error=%s\n",
			    usb2_errstr(err));
		}
	}
}

static void
ubser_cfg_get_status(struct usb2_com_softc *ucom, uint8_t *lsr, uint8_t *msr)
{
	/* fake status bits */
	*lsr = 0;
	*msr = SER_DCD;
}

static void
ubser_start_read(struct usb2_com_softc *ucom)
{
	struct ubser_softc *sc = ucom->sc_parent;

	usb2_transfer_start(sc->sc_xfer[UBSER_BULK_DT_RD]);
}

static void
ubser_stop_read(struct usb2_com_softc *ucom)
{
	struct ubser_softc *sc = ucom->sc_parent;

	usb2_transfer_stop(sc->sc_xfer[UBSER_BULK_DT_RD]);
}

static void
ubser_start_write(struct usb2_com_softc *ucom)
{
	struct ubser_softc *sc = ucom->sc_parent;

	usb2_transfer_start(sc->sc_xfer[UBSER_BULK_DT_WR]);
}

static void
ubser_stop_write(struct usb2_com_softc *ucom)
{
	struct ubser_softc *sc = ucom->sc_parent;

	usb2_transfer_stop(sc->sc_xfer[UBSER_BULK_DT_WR]);
}
