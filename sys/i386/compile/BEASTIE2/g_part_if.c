/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../geom/part/g_part_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/mutex.h>
#include <sys/sbuf.h>
#include <sys/bus.h>
#include <machine/bus.h>
#include <sys/systm.h>
#include <geom/geom.h>
#include <geom/part/g_part.h>
#include "g_part_if.h"


static int
default_precheck(struct g_part_table *t __unused,
    enum g_part_ctl r __unused, struct g_part_parms *p __unused)
{
	return (0);
}

struct kobj_method g_part_add_method_default = {
	&g_part_add_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_add_desc = {
	0, &g_part_add_method_default
};

struct kobj_method g_part_bootcode_method_default = {
	&g_part_bootcode_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_bootcode_desc = {
	0, &g_part_bootcode_method_default
};

struct kobj_method g_part_create_method_default = {
	&g_part_create_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_create_desc = {
	0, &g_part_create_method_default
};

struct kobj_method g_part_destroy_method_default = {
	&g_part_destroy_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_destroy_desc = {
	0, &g_part_destroy_method_default
};

struct kobj_method g_part_devalias_method_default = {
	&g_part_devalias_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_devalias_desc = {
	0, &g_part_devalias_method_default
};

struct kobj_method g_part_dumpconf_method_default = {
	&g_part_dumpconf_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_dumpconf_desc = {
	0, &g_part_dumpconf_method_default
};

struct kobj_method g_part_dumpto_method_default = {
	&g_part_dumpto_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_dumpto_desc = {
	0, &g_part_dumpto_method_default
};

struct kobj_method g_part_modify_method_default = {
	&g_part_modify_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_modify_desc = {
	0, &g_part_modify_method_default
};

struct kobj_method g_part_name_method_default = {
	&g_part_name_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_name_desc = {
	0, &g_part_name_method_default
};

struct kobj_method g_part_precheck_method_default = {
	&g_part_precheck_desc, (kobjop_t) default_precheck
};

struct kobjop_desc g_part_precheck_desc = {
	0, &g_part_precheck_method_default
};

struct kobj_method g_part_probe_method_default = {
	&g_part_probe_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_probe_desc = {
	0, &g_part_probe_method_default
};

struct kobj_method g_part_read_method_default = {
	&g_part_read_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_read_desc = {
	0, &g_part_read_method_default
};

struct kobj_method g_part_setunset_method_default = {
	&g_part_setunset_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_setunset_desc = {
	0, &g_part_setunset_method_default
};

struct kobj_method g_part_type_method_default = {
	&g_part_type_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_type_desc = {
	0, &g_part_type_method_default
};

struct kobj_method g_part_write_method_default = {
	&g_part_write_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc g_part_write_desc = {
	0, &g_part_write_method_default
};

