/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../kern/cpufreq_if.m
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
#include "cpufreq_if.h"

struct kobj_method cpufreq_set_method_default = {
	&cpufreq_set_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_set_desc = {
	0, &cpufreq_set_method_default
};

struct kobj_method cpufreq_get_method_default = {
	&cpufreq_get_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_get_desc = {
	0, &cpufreq_get_method_default
};

struct kobj_method cpufreq_levels_method_default = {
	&cpufreq_levels_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_levels_desc = {
	0, &cpufreq_levels_method_default
};

struct kobj_method cpufreq_drv_set_method_default = {
	&cpufreq_drv_set_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_drv_set_desc = {
	0, &cpufreq_drv_set_method_default
};

struct kobj_method cpufreq_drv_get_method_default = {
	&cpufreq_drv_get_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_drv_get_desc = {
	0, &cpufreq_drv_get_method_default
};

struct kobj_method cpufreq_drv_settings_method_default = {
	&cpufreq_drv_settings_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_drv_settings_desc = {
	0, &cpufreq_drv_settings_method_default
};

struct kobj_method cpufreq_drv_type_method_default = {
	&cpufreq_drv_type_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cpufreq_drv_type_desc = {
	0, &cpufreq_drv_type_method_default
};

