/*	$NetBSD: Locore.c,v 1.7 2000/08/20 07:04:59 tsubai Exp $	*/

/*-
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*-
 * Copyright (C) 2000 Benno Rice.
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
 * THIS SOFTWARE IS PROVIDED BY Benno Rice ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/systm.h>

#include <machine/stdarg.h>

#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofwvar.h>

#include "ofw_if.h"

MALLOC_DEFINE(M_OFWPROP, "openfirm", "Open Firmware properties");

static ihandle_t stdout;

static ofw_def_t	*ofw_def_impl;
static ofw_t		ofw_obj;
static struct ofw_kobj	ofw_kernel_obj;
static struct kobj_ops	ofw_kernel_kops;

/*
 * OFW install routines. Highest priority wins, equal priority also
 * overrides allowing last-set to win.
 */
SET_DECLARE(ofw_set, ofw_def_t);

boolean_t
OF_install(char *name, int prio)
{
	ofw_def_t  **ofwpp, *ofwp;
	static int curr_prio = 0;

	/*
	* Try and locate the OFW kobj corresponding to the name
	*/
	SET_FOREACH(ofwpp, ofw_set) {
		ofwp = *ofwpp;

		if (ofwp->name &&
		    !strcmp(ofwp->name, name) &&
		    prio >= curr_prio) {
			curr_prio = prio;
			ofw_def_impl = ofwp;
			return (TRUE);
		}
	}

	return (FALSE);
}

/* Initialiser */

void
OF_init(void *cookie)
{
	phandle_t chosen;

	ofw_obj = &ofw_kernel_obj;
	/*
	 * Take care of compiling the selected class, and
	 * then statically initialize the OFW object
	 */
	kobj_class_compile_static(ofw_def_impl, &ofw_kernel_kops);
	kobj_init((kobj_t)ofw_obj, ofw_def_impl);

	OFW_INIT(ofw_obj, cookie);

	if ((chosen = OF_finddevice("/chosen")) == -1)
		OF_exit();
	if (OF_getprop(chosen, "stdout", &stdout, sizeof(stdout)) == -1)
		stdout = -1;
}

void
OF_printf(const char *fmt, ...)
{
	va_list	va;
	char buf[1024];

	va_start(va, fmt);
	vsprintf(buf, fmt, va);
	OF_write(stdout, buf, strlen(buf));
	va_end(va);
}

/*
 * Generic functions
 */

/* Test to see if a service exists. */
int
OF_test(const char *name)
{
	return (OFW_TEST(ofw_obj, name));
}

int
OF_interpret(const char *cmd, int nreturns, ...)
{
	va_list ap;
	unsigned long slots[16];
	int i = 0;
	int status;

	status = OFW_INTERPRET(ofw_obj, cmd, nreturns, slots);

	va_start(ap, nreturns);
	while (i < 1 + nreturns)
		*va_arg(ap, cell_t *) = slots[i++];
	va_end(ap);

	return (status);
}

/*
 * Device tree functions
 */

/* Return the next sibling of this node or 0. */
phandle_t
OF_peer(phandle_t node)
{
	return (OFW_PEER(ofw_obj, node));
}

/* Return the first child of this node or 0. */
phandle_t
OF_child(phandle_t node)
{
	return (OFW_CHILD(ofw_obj, node));
}

/* Return the parent of this node or 0. */
phandle_t
OF_parent(phandle_t node)
{
	return (OFW_PARENT(ofw_obj, node));
}

/* Return the package handle that corresponds to an instance handle. */
phandle_t
OF_instance_to_package(ihandle_t instance)
{
	return (OFW_INSTANCE_TO_PACKAGE(ofw_obj, instance));
}

/* Get the length of a property of a package. */
ssize_t
OF_getproplen(phandle_t package, const char *propname)
{
	return (OFW_GETPROPLEN(ofw_obj, package, propname));
}

/* Get the value of a property of a package. */
ssize_t
OF_getprop(phandle_t package, const char *propname, void *buf, size_t buflen)
{
	return (OFW_GETPROP(ofw_obj, package, propname, buf, buflen));
}

/*
 * Resursively search the node and its parent for the given property, working
 * downward from the node to the device tree root.  Returns the value of the
 * first match.
 */
ssize_t
OF_searchprop(phandle_t node, char *propname, void *buf, size_t len)
{
	ssize_t rv;

	for (; node != 0; node = OF_parent(node)) {
		if ((rv = OF_getprop(node, propname, buf, len)) != -1)
			return (rv);
	}
	return (-1);
}

/*
 * Store the value of a property of a package into newly allocated memory
 * (using the M_OFWPROP malloc pool and M_WAITOK). elsz is the size of a
 * single element, the number of elements is return in number.
 */
ssize_t
OF_getprop_alloc(phandle_t package, const char *propname, int elsz, void **buf)
{
	int len;

	*buf = NULL;
	if ((len = OF_getproplen(package, propname)) == -1 ||
	    len % elsz != 0)
		return (-1);

	*buf = malloc(len, M_OFWPROP, M_WAITOK);
	if (OF_getprop(package, propname, *buf, len) == -1) {
		free(*buf, M_OFWPROP);
		*buf = NULL;
		return (-1);
	}
	return (len / elsz);
}

/* Get the next property of a package. */
int
OF_nextprop(phandle_t package, const char *previous, char *buf, size_t size)
{
	return (OFW_NEXTPROP(ofw_obj, package, previous, buf, size));
}

/* Set the value of a property of a package. */
int
OF_setprop(phandle_t package, const char *propname, const void *buf, size_t len)
{
	return (OFW_SETPROP(ofw_obj, package, propname, buf,len));
}

/* Convert a device specifier to a fully qualified pathname. */
ssize_t
OF_canon(const char *device, char *buf, size_t len)
{
	return (OFW_CANON(ofw_obj, device, buf, len));
}

/* Return a package handle for the specified device. */
phandle_t
OF_finddevice(const char *device)
{
	return (OFW_FINDDEVICE(ofw_obj, device));
}

/* Return the fully qualified pathname corresponding to an instance. */
ssize_t
OF_instance_to_path(ihandle_t instance, char *buf, size_t len)
{
	return (OFW_INSTANCE_TO_PATH(ofw_obj, instance, buf, len));
}

/* Return the fully qualified pathname corresponding to a package. */
ssize_t
OF_package_to_path(phandle_t package, char *buf, size_t len)
{
	return (OFW_PACKAGE_TO_PATH(ofw_obj, package, buf, len));
}

/*  Call the method in the scope of a given instance. */
int
OF_call_method(const char *method, ihandle_t instance, int nargs, int nreturns,
    ...)
{
	va_list ap;
	unsigned long args_n_results[12];
	int n, status;

	if (nargs > 6)
		return (-1);
	va_start(ap, nreturns);
	for (n = 0; n < nargs; n++)
		args_n_results[n] = va_arg(ap, unsigned long);

	status = OFW_CALL_METHOD(ofw_obj, instance, method, nargs, nreturns,
	    args_n_results);

	if (status != 0)
		return (status);

	for (; n < nargs + nreturns; n++)
		*va_arg(ap, unsigned long *) = args_n_results[n];
	va_end(ap);
	return (0);
}

/*
 * Device I/O functions
 */

/* Open an instance for a device. */
ihandle_t
OF_open(const char *device)
{
	return (OFW_OPEN(ofw_obj, device));
}

/* Close an instance. */
void
OF_close(ihandle_t instance)
{
	OFW_CLOSE(ofw_obj, instance);
}

/* Read from an instance. */
ssize_t
OF_read(ihandle_t instance, void *addr, size_t len)
{
	return (OFW_READ(ofw_obj, instance, addr, len));
}

/* Write to an instance. */
ssize_t
OF_write(ihandle_t instance, const void *addr, size_t len)
{
	return (OFW_WRITE(ofw_obj, instance, addr, len));
}

/* Seek to a position. */
int
OF_seek(ihandle_t instance, uint64_t pos)
{
	return (OFW_SEEK(ofw_obj, instance, pos));
}

/*
 * Memory functions
 */

/* Claim an area of memory. */
void *
OF_claim(void *virt, size_t size, u_int align)
{
	return (OFW_CLAIM(ofw_obj, virt, size, align));
}

/* Release an area of memory. */
void
OF_release(void *virt, size_t size)
{
	OFW_RELEASE(ofw_obj, virt, size);
}

/*
 * Control transfer functions
 */

/* Suspend and drop back to the Open Firmware interface. */
void
OF_enter()
{
	OFW_ENTER(ofw_obj);
}

/* Shut down and drop back to the Open Firmware interface. */
void
OF_exit()
{
	/* Should not return */
	OFW_EXIT(ofw_obj);

	for (;;)			/* just in case */
		;
}

