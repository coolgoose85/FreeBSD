/*
 * Copyright (c) 2008 Kip Macy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $FreeBSD$
 */
#ifndef XENVAR_H_
#define XENVAR_H_
#define XBOOTUP 0x1
#define XPMAP   0x2
extern int xendebug_flags;
#ifndef NOXENDEBUG
#define XENPRINTF printk
#else
#define XENPRINTF printf
#endif
#include <machine/xen/features.h>

extern	xen_pfn_t *xen_phys_machine;

#if 0
#define TRACE_ENTER XENPRINTF("(file=%s, line=%d) entered %s\n", __FILE__, __LINE__, __FUNCTION__)
#define TRACE_EXIT XENPRINTF("(file=%s, line=%d) exiting %s\n", __FILE__, __LINE__, __FUNCTION__)
#define TRACE_DEBUG(argflags, _f, _a...) \
if (xendebug_flags & argflags) XENPRINTF("(file=%s, line=%d) " _f "\n", __FILE__, __LINE__, ## _a);
#else
#define TRACE_ENTER
#define TRACE_EXIT
#define TRACE_DEBUG(argflags, _f, _a...)
#endif

extern xen_pfn_t *xen_machine_phys;
/* Xen starts physical pages after the 4MB ISA hole -
 * FreeBSD doesn't
 */


#undef ADD_ISA_HOLE /* XXX */

#ifdef ADD_ISA_HOLE
#define ISA_INDEX_OFFSET 1024 
#define ISA_PDR_OFFSET 1
#else
#define ISA_INDEX_OFFSET 0
#define ISA_PDR_OFFSET 0
#endif


#define PFNTOMFN(i) (xen_phys_machine[(i)])
#define MFNTOPFN(i) ((vm_paddr_t)xen_machine_phys[(i)])

#define VTOP(x) ((((uintptr_t)(x))) - KERNBASE)
#define PTOV(x) (((uintptr_t)(x)) + KERNBASE)

#define VTOPFN(x) (VTOP(x) >> PAGE_SHIFT)
#define PFNTOV(x) PTOV((vm_paddr_t)(x)  << PAGE_SHIFT)

#define VTOMFN(va) (vtomach(va) >> PAGE_SHIFT)
#define PFN_UP(x)    (((x) + PAGE_SIZE-1) >> PAGE_SHIFT)

#define phystomach(pa) (((vm_paddr_t)(PFNTOMFN((pa) >> PAGE_SHIFT))) << PAGE_SHIFT)
#define machtophys(ma) (((vm_paddr_t)(MFNTOPFN((ma) >> PAGE_SHIFT))) << PAGE_SHIFT)


void xpq_init(void);

#define BITS_PER_LONG 32
#define NR_CPUS      MAX_VIRT_CPUS

#define BITS_TO_LONGS(bits) \
	(((bits)+BITS_PER_LONG-1)/BITS_PER_LONG)
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]
typedef struct { DECLARE_BITMAP(bits, NR_CPUS); } xen_cpumask_t;

int  xen_create_contiguous_region(vm_page_t pages, int npages);

void  xen_destroy_contiguous_region(void * addr, int npages);

#endif
