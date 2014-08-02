/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/agp/agp_if.m
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
#include "agp_if.h"

struct kobj_method agp_get_aperture_method_default = {
	&agp_get_aperture_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_get_aperture_desc = {
	0, &agp_get_aperture_method_default
};

struct kobj_method agp_set_aperture_method_default = {
	&agp_set_aperture_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_set_aperture_desc = {
	0, &agp_set_aperture_method_default
};

struct kobj_method agp_bind_page_method_default = {
	&agp_bind_page_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_bind_page_desc = {
	0, &agp_bind_page_method_default
};

struct kobj_method agp_unbind_page_method_default = {
	&agp_unbind_page_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_unbind_page_desc = {
	0, &agp_unbind_page_method_default
};

struct kobj_method agp_flush_tlb_method_default = {
	&agp_flush_tlb_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_flush_tlb_desc = {
	0, &agp_flush_tlb_method_default
};

struct kobj_method agp_enable_method_default = {
	&agp_enable_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_enable_desc = {
	0, &agp_enable_method_default
};

struct kobj_method agp_alloc_memory_method_default = {
	&agp_alloc_memory_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_alloc_memory_desc = {
	0, &agp_alloc_memory_method_default
};

struct kobj_method agp_free_memory_method_default = {
	&agp_free_memory_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_free_memory_desc = {
	0, &agp_free_memory_method_default
};

struct kobj_method agp_bind_memory_method_default = {
	&agp_bind_memory_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_bind_memory_desc = {
	0, &agp_bind_memory_method_default
};

struct kobj_method agp_unbind_memory_method_default = {
	&agp_unbind_memory_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc agp_unbind_memory_desc = {
	0, &agp_unbind_memory_method_default
};

