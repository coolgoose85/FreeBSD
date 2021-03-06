/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/eisa/eisa_if.m
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
#include <sys/types.h>
#include "eisa_if.h"

struct kobj_method eisa_add_intr_method_default = {
	&eisa_add_intr_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc eisa_add_intr_desc = {
	0, &eisa_add_intr_method_default
};

struct kobj_method eisa_add_iospace_method_default = {
	&eisa_add_iospace_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc eisa_add_iospace_desc = {
	0, &eisa_add_iospace_method_default
};

struct kobj_method eisa_add_mspace_method_default = {
	&eisa_add_mspace_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc eisa_add_mspace_desc = {
	0, &eisa_add_mspace_method_default
};

