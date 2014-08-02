/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/uart/uart_if.m
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
#include <sys/mutex.h>
#include <sys/bus.h>
#include <machine/bus.h>
#include <dev/uart/uart.h>
#include <dev/uart/uart_bus.h>
#include "uart_if.h"

struct kobj_method uart_attach_method_default = {
	&uart_attach_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_attach_desc = {
	0, &uart_attach_method_default
};

struct kobj_method uart_detach_method_default = {
	&uart_detach_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_detach_desc = {
	0, &uart_detach_method_default
};

struct kobj_method uart_flush_method_default = {
	&uart_flush_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_flush_desc = {
	0, &uart_flush_method_default
};

struct kobj_method uart_getsig_method_default = {
	&uart_getsig_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_getsig_desc = {
	0, &uart_getsig_method_default
};

struct kobj_method uart_ioctl_method_default = {
	&uart_ioctl_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_ioctl_desc = {
	0, &uart_ioctl_method_default
};

struct kobj_method uart_ipend_method_default = {
	&uart_ipend_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_ipend_desc = {
	0, &uart_ipend_method_default
};

struct kobj_method uart_param_method_default = {
	&uart_param_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_param_desc = {
	0, &uart_param_method_default
};

struct kobj_method uart_probe_method_default = {
	&uart_probe_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_probe_desc = {
	0, &uart_probe_method_default
};

struct kobj_method uart_receive_method_default = {
	&uart_receive_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_receive_desc = {
	0, &uart_receive_method_default
};

struct kobj_method uart_setsig_method_default = {
	&uart_setsig_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_setsig_desc = {
	0, &uart_setsig_method_default
};

struct kobj_method uart_transmit_method_default = {
	&uart_transmit_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc uart_transmit_desc = {
	0, &uart_transmit_method_default
};

