/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/acpica/acpi_if.m
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
#include <contrib/dev/acpica/acpi.h>
#include "acpi_if.h"


static char *
acpi_generic_id_probe(device_t bus, device_t dev, char **ids)
{
	return (NULL);
}

struct kobj_method acpi_id_probe_method_default = {
	&acpi_id_probe_desc, (kobjop_t) acpi_generic_id_probe
};

struct kobjop_desc acpi_id_probe_desc = {
	0, &acpi_id_probe_method_default
};

struct kobj_method acpi_evaluate_object_method_default = {
	&acpi_evaluate_object_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_evaluate_object_desc = {
	0, &acpi_evaluate_object_method_default
};

struct kobj_method acpi_pwr_for_sleep_method_default = {
	&acpi_pwr_for_sleep_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_pwr_for_sleep_desc = {
	0, &acpi_pwr_for_sleep_method_default
};

struct kobj_method acpi_scan_children_method_default = {
	&acpi_scan_children_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_scan_children_desc = {
	0, &acpi_scan_children_method_default
};

struct kobj_method acpi_get_features_method_default = {
	&acpi_get_features_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_get_features_desc = {
	0, &acpi_get_features_method_default
};

struct kobj_method acpi_ec_read_method_default = {
	&acpi_ec_read_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_ec_read_desc = {
	0, &acpi_ec_read_method_default
};

struct kobj_method acpi_ec_write_method_default = {
	&acpi_ec_write_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_ec_write_desc = {
	0, &acpi_ec_write_method_default
};

struct kobj_method acpi_batt_get_info_method_default = {
	&acpi_batt_get_info_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_batt_get_info_desc = {
	0, &acpi_batt_get_info_method_default
};

struct kobj_method acpi_batt_get_status_method_default = {
	&acpi_batt_get_status_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc acpi_batt_get_status_desc = {
	0, &acpi_batt_get_status_method_default
};

