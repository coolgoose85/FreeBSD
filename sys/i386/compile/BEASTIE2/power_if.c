/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/pccard/power_if.m
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
#include "power_if.h"

struct kobj_method power_enable_socket_method_default = {
	&power_enable_socket_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc power_enable_socket_desc = {
	0, &power_enable_socket_method_default
};

struct kobj_method power_disable_socket_method_default = {
	&power_disable_socket_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc power_disable_socket_desc = {
	0, &power_disable_socket_method_default
};

