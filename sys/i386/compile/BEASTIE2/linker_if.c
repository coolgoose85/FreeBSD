/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../kern/linker_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/linker.h>
#include "linker_if.h"

struct kobj_method linker_lookup_symbol_method_default = {
	&linker_lookup_symbol_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_lookup_symbol_desc = {
	0, &linker_lookup_symbol_method_default
};

struct kobj_method linker_symbol_values_method_default = {
	&linker_symbol_values_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_symbol_values_desc = {
	0, &linker_symbol_values_method_default
};

struct kobj_method linker_search_symbol_method_default = {
	&linker_search_symbol_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_search_symbol_desc = {
	0, &linker_search_symbol_method_default
};

struct kobj_method linker_each_function_name_method_default = {
	&linker_each_function_name_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_each_function_name_desc = {
	0, &linker_each_function_name_method_default
};

struct kobj_method linker_each_function_nameval_method_default = {
	&linker_each_function_nameval_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_each_function_nameval_desc = {
	0, &linker_each_function_nameval_method_default
};

struct kobj_method linker_lookup_set_method_default = {
	&linker_lookup_set_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_lookup_set_desc = {
	0, &linker_lookup_set_method_default
};

struct kobj_method linker_unload_method_default = {
	&linker_unload_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_unload_desc = {
	0, &linker_unload_method_default
};

struct kobj_method linker_ctf_get_method_default = {
	&linker_ctf_get_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_ctf_get_desc = {
	0, &linker_ctf_get_method_default
};

struct kobj_method linker_load_file_method_default = {
	&linker_load_file_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_load_file_desc = {
	0, &linker_load_file_method_default
};

struct kobj_method linker_link_preload_method_default = {
	&linker_link_preload_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_link_preload_desc = {
	0, &linker_link_preload_method_default
};

struct kobj_method linker_link_preload_finish_method_default = {
	&linker_link_preload_finish_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc linker_link_preload_finish_desc = {
	0, &linker_link_preload_finish_method_default
};

