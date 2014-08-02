/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/ppbus/ppbus_if.m
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
#include <dev/ppbus/ppbconf.h>
#include "ppbus_if.h"

struct kobj_method ppbus_io_method_default = {
	&ppbus_io_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_io_desc = {
	0, &ppbus_io_method_default
};

struct kobj_method ppbus_exec_microseq_method_default = {
	&ppbus_exec_microseq_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_exec_microseq_desc = {
	0, &ppbus_exec_microseq_method_default
};

struct kobj_method ppbus_reset_epp_method_default = {
	&ppbus_reset_epp_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_reset_epp_desc = {
	0, &ppbus_reset_epp_method_default
};

struct kobj_method ppbus_setmode_method_default = {
	&ppbus_setmode_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_setmode_desc = {
	0, &ppbus_setmode_method_default
};

struct kobj_method ppbus_ecp_sync_method_default = {
	&ppbus_ecp_sync_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_ecp_sync_desc = {
	0, &ppbus_ecp_sync_method_default
};

struct kobj_method ppbus_read_method_default = {
	&ppbus_read_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_read_desc = {
	0, &ppbus_read_method_default
};

struct kobj_method ppbus_write_method_default = {
	&ppbus_write_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc ppbus_write_desc = {
	0, &ppbus_write_method_default
};

