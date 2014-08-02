/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/ata/ata_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/callout.h>
#include <sys/sema.h>
#include <sys/taskqueue.h>
#include <vm/uma.h>
#include <machine/bus.h>
#include <sys/ata.h>
#include <dev/ata/ata-all.h>
#include "ata_if.h"


static int ata_null_locking(device_t dev, int mode)
{
    struct ata_channel *ch = device_get_softc(dev);

    return ch->unit;
}

struct kobj_method ata_locking_method_default = {
	&ata_locking_desc, (kobjop_t) ata_null_locking
};

struct kobjop_desc ata_locking_desc = {
	0, &ata_locking_method_default
};


static void ata_null_setmode(device_t parent, device_t dev)
{
    struct ata_device *atadev = device_get_softc(dev);

    atadev->mode = ata_limit_mode(dev, atadev->mode, ATA_PIO_MAX);
}

struct kobj_method ata_setmode_method_default = {
	&ata_setmode_desc, (kobjop_t) ata_null_setmode
};

struct kobjop_desc ata_setmode_desc = {
	0, &ata_setmode_method_default
};

struct kobj_method ata_reset_method_default = {
	&ata_reset_desc, (kobjop_t) ata_generic_reset
};

struct kobjop_desc ata_reset_desc = {
	0, &ata_reset_method_default
};

struct kobj_method ata_reinit_method_default = {
	&ata_reinit_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ata_reinit_desc = {
	0, &ata_reinit_method_default
};

