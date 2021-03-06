/*	$OpenBSD: locore.S,v 1.18 1998/09/15 10:58:53 pefo Exp $	*/
/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Digital Equipment Corporation and Ralph Campbell.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Copyright (C) 1989 Digital Equipment Corporation.
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies.
 * Digital Equipment Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * from: Header: /sprite/src/kernel/mach/ds3100.md/RCS/loMem.s,
 *	v 1.1 89/07/11 17:55:04 nelson Exp  SPRITE (DECWRL)
 * from: Header: /sprite/src/kernel/mach/ds3100.md/RCS/machAsm.s,
 *	v 9.2 90/01/29 18:00:39 shirriff Exp  SPRITE (DECWRL)
 * from: Header: /sprite/src/kernel/vm/ds3100.md/vmPmaxAsm.s,
 *	v 1.1 89/07/10 14:27:41 nelson Exp  SPRITE (DECWRL)
 *
 *	from: @(#)locore.s	8.5 (Berkeley) 1/4/94
 *	JNPR: swtch.S,v 1.6.2.1 2007/09/10 10:36:50 girish
 * $FreeBSD$
 */

/*
 *	Contains code that is the first executed at boot time plus
 *	assembly language support routines.
 */

#include "opt_cputype.h"
#include <sys/syscall.h>
#include <machine/asm.h>
#include <machine/cpu.h>
#include <machine/cpuregs.h>
#include <machine/regnum.h>
#include <machine/pte.h>

#include "assym.s"

#if defined(ISA_MIPS32)
#undef WITH_64BIT_CP0
#elif defined(ISA_MIPS64)
#define WITH_64BIT_CP0
#elif defined(ISA_MIPS3)
#define WITH_64BIT_CP0
#else
#error "Please write the code for this ISA"
#endif

#ifdef WITH_64BIT_CP0
#define _SLL	dsll
#define	_SRL	dsrl
#define	_MFC0	dmfc0
#define	_MTC0	dmtc0
#define WIRED_SHIFT 34
#define PAGE_SHIFT 34
#else
#define _SLL	sll
#define	_SRL	srl
#define	_MFC0	mfc0
#define	_MTC0	mtc0
#define WIRED_SHIFT 2
#define PAGE_SHIFT 2
#endif
	.set	noreorder			# Noreorder is default style!
#if defined(ISA_MIPS32)
	.set	mips32
#elif defined(ISA_MIPS64)
	.set	mips64
#elif defined(ISA_MIPS3)
	.set	mips3
#endif

#if defined(ISA_MIPS32)
#define	STORE		sw		/* 32 bit mode regsave instruction */
#define	LOAD		lw		/* 32 bit mode regload instruction */
#define	RSIZE		4		/* 32 bit mode register size */
#define	STORE_FP	swc1		/* 32 bit mode fp regsave instruction */
#define	LOAD_FP		lwc1		/* 32 bit mode fp regload instruction */
#define	FP_RSIZE	4		/* 32 bit mode fp register size */
#else
#define	STORE		sd		/* 64 bit mode regsave instruction */
#define	LOAD		ld		/* 64 bit mode regload instruction */
#define	RSIZE		8		/* 64 bit mode register size */
#define	STORE_FP	sdc1		/* 64 bit mode fp regsave instruction */
#define	LOAD_FP		ldc1		/* 64 bit mode fp regload instruction */
#define	FP_RSIZE	8		/* 64 bit mode fp register size */
#endif

/*
 * FREEBSD_DEVELOPERS_FIXME
 * Some MIPS CPU may need delays using nops between executing CP0 Instructions
 */

#if 1
#define	HAZARD_DELAY			nop ; nop ; nop ; nop
#else
#define	HAZARD_DELAY
#endif

#define	SAVE_U_PCB_REG(reg, offs, base) \
	STORE	reg, U_PCB_REGS + (RSIZE * offs) (base)

#define	RESTORE_U_PCB_REG(reg, offs, base) \
	LOAD	reg, U_PCB_REGS + (RSIZE * offs) (base)

#define	SAVE_U_PCB_FPREG(reg, offs, base) \
	STORE_FP reg, U_PCB_FPREGS + (FP_RSIZE * offs) (base)

#define	RESTORE_U_PCB_FPREG(reg, offs, base) \
	LOAD_FP	reg, U_PCB_FPREGS + (FP_RSIZE * offs) (base)

#define	SAVE_U_PCB_FPSR(reg, offs, base) \
	STORE	reg, U_PCB_FPREGS + (FP_RSIZE * offs) (base)

#define	RESTORE_U_PCB_FPSR(reg, offs, base) \
	LOAD	reg, U_PCB_FPREGS + (FP_RSIZE * offs) (base)

#define	SAVE_U_PCB_CONTEXT(reg, offs, base) \
	STORE	reg, U_PCB_CONTEXT + (RSIZE * offs) (base)

#define	RESTORE_U_PCB_CONTEXT(reg, offs, base) \
	LOAD	reg, U_PCB_CONTEXT + (RSIZE * offs) (base)

#define	ITLBNOPFIX	nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;

/*
 * Setup for and return to user.
 */
LEAF(fork_trampoline)
	move	a0,s0
	move	a1,s1
	jal	_C_LABEL(fork_exit)
	move	a2,s2			  #BDSlot

	DO_AST

/*
 * Since interrupts are enabled at this point, we use a1 instead of
 * k0 or k1 to store the PCB pointer.  This is because k0 and k1
 * are not preserved across interrupts.
 */
	GET_CPU_PCPU(a1)
	lw	a1, PC_CURPCB(a1)
1:

	mfc0	v0, COP_0_STATUS_REG	# set exeption level bit.
	or	v0, SR_EXL
	and     v0, ~(SR_INT_ENAB)
	mtc0	v0, COP_0_STATUS_REG	# set exeption level bit.
	nop
	nop
	nop
	nop
	.set	noat
	move	k1, a1	
	RESTORE_U_PCB_REG(t0, MULLO, k1)
	RESTORE_U_PCB_REG(t1, MULHI, k1)
	mtlo	t0
	mthi	t1
	RESTORE_U_PCB_REG(a0, PC, k1)
	RESTORE_U_PCB_REG(AT, AST, k1)
	RESTORE_U_PCB_REG(v0, V0, k1)
	_MTC0	a0, COP_0_EXC_PC	# set return address

/*
 * The use of k1 for storing the PCB pointer must be done only
 * after interrupts are disabled.  Otherwise it will get overwritten
 * by the interrupt code.
 */
	RESTORE_U_PCB_REG(v1, V1, k1)
	RESTORE_U_PCB_REG(a0, A0, k1)
	RESTORE_U_PCB_REG(a1, A1, k1)
	RESTORE_U_PCB_REG(a2, A2, k1)
	RESTORE_U_PCB_REG(a3, A3, k1)
	RESTORE_U_PCB_REG(t0, T0, k1)
	RESTORE_U_PCB_REG(t1, T1, k1)
	RESTORE_U_PCB_REG(t2, T2, k1)
	RESTORE_U_PCB_REG(t3, T3, k1)
	RESTORE_U_PCB_REG(t4, T4, k1)
	RESTORE_U_PCB_REG(t5, T5, k1)
	RESTORE_U_PCB_REG(t6, T6, k1)
	RESTORE_U_PCB_REG(t7, T7, k1)
	RESTORE_U_PCB_REG(s0, S0, k1)
	RESTORE_U_PCB_REG(s1, S1, k1)
	RESTORE_U_PCB_REG(s2, S2, k1)
	RESTORE_U_PCB_REG(s3, S3, k1)
	RESTORE_U_PCB_REG(s4, S4, k1)
	RESTORE_U_PCB_REG(s5, S5, k1)
	RESTORE_U_PCB_REG(s6, S6, k1)
	RESTORE_U_PCB_REG(s7, S7, k1)
	RESTORE_U_PCB_REG(t8, T8, k1)
	RESTORE_U_PCB_REG(t9, T9, k1)
	RESTORE_U_PCB_REG(k0, SR, k1)
	RESTORE_U_PCB_REG(gp, GP, k1)
	RESTORE_U_PCB_REG(s8, S8, k1)
	RESTORE_U_PCB_REG(ra, RA, k1)
	RESTORE_U_PCB_REG(sp, SP, k1)
	mtc0	k0, COP_0_STATUS_REG	# switch to user mode (when eret...)
	HAZARD_DELAY
	sync
	eret
	.set	at
END(fork_trampoline)

/*
 * Update pcb, saving current processor state.
 * Note: this only works if pcbp != curproc's pcb since
 * cpu_switch() will copy over pcb_context.
 *
 *	savectx(struct pcb *pcbp);
 */
LEAF(savectx)
	SAVE_U_PCB_CONTEXT(s0, PREG_S0, a0)
	SAVE_U_PCB_CONTEXT(s1, PREG_S1, a0)
	SAVE_U_PCB_CONTEXT(s2, PREG_S2, a0)
	SAVE_U_PCB_CONTEXT(s3, PREG_S3, a0)
	mfc0	v0, COP_0_STATUS_REG
	SAVE_U_PCB_CONTEXT(s4, PREG_S4, a0)
	SAVE_U_PCB_CONTEXT(s5, PREG_S5, a0)
	SAVE_U_PCB_CONTEXT(s6, PREG_S6, a0)
	SAVE_U_PCB_CONTEXT(s7, PREG_S7, a0)
	SAVE_U_PCB_CONTEXT(sp, PREG_SP, a0)
	SAVE_U_PCB_CONTEXT(s8, PREG_S8, a0)
	SAVE_U_PCB_CONTEXT(ra, PREG_RA, a0)
	SAVE_U_PCB_CONTEXT(v0, PREG_SR, a0)
	SAVE_U_PCB_CONTEXT(gp, PREG_GP, a0)
	/*
	 * FREEBSD_DEVELOPERS_FIXME:
	 * In case there are CPU-specific registers that need
	 * to be saved with the other registers do so here.
	 */
	j	ra
	move	v0, zero
END(savectx)


KSEG0TEXT_START;

NON_LEAF(mips_cpu_throw, STAND_FRAME_SIZE, ra)
	mfc0	t0, COP_0_STATUS_REG		# t0 = saved status register
	nop
	nop
	and     a3, t0, ~(SR_INT_ENAB)
	mtc0	a3, COP_0_STATUS_REG		# Disable all interrupts
	ITLBNOPFIX
	j	mips_sw1			# We're not interested in old 
						# thread's context, so jump 
						# right to action
	nop					# BDSLOT
END(mips_cpu_throw)

/*
 *XXX Fixme:	should be written to new interface that requires lock
 *		storage.  We fake it for now.
 * cpu_switch(struct thread *old, struct thread *new);
 * Find the highest priority process and resume it.
 */
NON_LEAF(cpu_switch, STAND_FRAME_SIZE, ra)
	mfc0	t0, COP_0_STATUS_REG		# t0 = saved status register
	nop
	nop
	and     a3, t0, ~(SR_INT_ENAB)	
	mtc0	a3, COP_0_STATUS_REG		# Disable all interrupts
	ITLBNOPFIX
	beqz	a0, mips_sw1
	move	a3, a0
	lw	a0, TD_PCB(a0)		# load PCB addr of curproc
	SAVE_U_PCB_CONTEXT(sp, PREG_SP, a0)		# save old sp
	subu	sp, sp, STAND_FRAME_SIZE
	sw	ra, STAND_RA_OFFSET(sp)
	.mask	0x80000000, (STAND_RA_OFFSET - STAND_FRAME_SIZE)
	SAVE_U_PCB_CONTEXT(s0, PREG_S0, a0)		# do a 'savectx()'
	SAVE_U_PCB_CONTEXT(s1, PREG_S1, a0)
	SAVE_U_PCB_CONTEXT(s2, PREG_S2, a0)
	SAVE_U_PCB_CONTEXT(s3, PREG_S3, a0)
	SAVE_U_PCB_CONTEXT(s4, PREG_S4, a0)
	SAVE_U_PCB_CONTEXT(s5, PREG_S5, a0)
	SAVE_U_PCB_CONTEXT(s6, PREG_S6, a0)
	SAVE_U_PCB_CONTEXT(s7, PREG_S7, a0)
	SAVE_U_PCB_CONTEXT(s8, PREG_S8, a0)
	SAVE_U_PCB_CONTEXT(ra, PREG_RA, a0)		# save return address
	SAVE_U_PCB_CONTEXT(t0, PREG_SR, a0)		# save status register
	SAVE_U_PCB_CONTEXT(gp, PREG_GP, a0)
	/*
	 * FREEBSD_DEVELOPERS_FIXME:
	 * In case there are CPU-specific registers that need
	 * to be saved with the other registers do so here.
	 */

	sw	a3, TD_LOCK(a0)			# Switchout td_lock 

mips_sw1:
#if defined(SMP) && defined(SCHED_ULE)
	la	t0, _C_LABEL(blocked_lock)
blocked_loop:
	lw	t1, TD_LOCK(a1)
	beq	t0, t1, blocked_loop
	nop
#endif
	move	s7, a1	# Store newthread
/*
 * Switch to new context.
 */
	GET_CPU_PCPU(a3)
	sw	a1, PC_CURTHREAD(a3)
	lw	a2, TD_PCB(a1)
	sw	a2, PC_CURPCB(a3)
	lw	v0, TD_REALKSTACK(a1)
	li	s0, (MIPS_KSEG2_START+VM_KERNEL_ALLOC_OFFSET)		# If Uarea addr is below kseg2,
	bltu	v0, s0, sw2			# no need to insert in TLB.
	lw	a1, TD_UPTE+0(s7)		# t0 = first u. pte
	lw	a2, TD_UPTE+4(s7)		# t1 = 2nd u. pte
	and	s0, v0, PTE_ODDPG
	beq	s0, zero, entry0
	nop

	PANIC_KSEG0("USPACE sat on odd page boundary", t1)

/*
 * Wiredown the USPACE of newproc in TLB entry#0.  Check whether target
 * USPACE is already in another place of TLB before that, and if so
 * invalidate that TLB entry.
 * NOTE: This is hard coded to UPAGES == 2.
 * Also, there should be no TLB faults at this point.
 */
entry0:
	mtc0	v0, COP_0_TLB_HI		# VPN = va
	HAZARD_DELAY
	tlbp					# probe VPN
	HAZARD_DELAY
	mfc0	s0, COP_0_TLB_INDEX
	nop
pgm:
	bltz	s0, entry0set
	li	t1, MIPS_KSEG0_START + 0x0fff0000	# invalidate tlb entry
	sll	s0, PAGE_SHIFT + 1
	addu	t1, s0
	mtc0	t1, COP_0_TLB_HI
	mtc0	zero, COP_0_TLB_LO0
	mtc0	zero, COP_0_TLB_LO1
	HAZARD_DELAY
	tlbwi
	HAZARD_DELAY
	mtc0	v0, COP_0_TLB_HI		# set VPN again
entry0set:
/* SMP!! - Works only for  unshared TLB case - i.e. no v-cpus */
	mtc0	zero, COP_0_TLB_INDEX		# TLB entry #0
#	or	a1, PG_G
	mtc0	a1, COP_0_TLB_LO0		# upte[0]
#	or	a2, PG_G
	mtc0	a2, COP_0_TLB_LO1		# upte[1]
	HAZARD_DELAY
	tlbwi					# set TLB entry #0
	HAZARD_DELAY
/*
 * Now running on new u struct.
 */
sw2:
	la	t1, _C_LABEL(pmap_activate)	# s7 = new proc pointer
	jalr	t1				# s7 = new proc pointer
	move	a0, s7				# BDSLOT
/*
 * Restore registers and return.
 */
	lw	a0, TD_PCB(s7)
	RESTORE_U_PCB_CONTEXT(gp, PREG_GP, a0)
	RESTORE_U_PCB_CONTEXT(v0, PREG_SR, a0)	# restore kernel context
	RESTORE_U_PCB_CONTEXT(ra, PREG_RA, a0)
	RESTORE_U_PCB_CONTEXT(s0, PREG_S0, a0)
	RESTORE_U_PCB_CONTEXT(s1, PREG_S1, a0)
	RESTORE_U_PCB_CONTEXT(s2, PREG_S2, a0)
	RESTORE_U_PCB_CONTEXT(s3, PREG_S3, a0)
	RESTORE_U_PCB_CONTEXT(s4, PREG_S4, a0)
	RESTORE_U_PCB_CONTEXT(s5, PREG_S5, a0)
	RESTORE_U_PCB_CONTEXT(s6, PREG_S6, a0)
	RESTORE_U_PCB_CONTEXT(s7, PREG_S7, a0)
	RESTORE_U_PCB_CONTEXT(sp, PREG_SP, a0)
	RESTORE_U_PCB_CONTEXT(s8, PREG_S8, a0)
	/*
	 * FREEBSD_DEVELOPERS_FIXME:
	 * In case there are CPU-specific registers that need
	 * to be restored with the other registers do so here.
	 */
	mtc0	v0, COP_0_STATUS_REG
	ITLBNOPFIX

	j	ra
	nop
END(cpu_switch)
KSEG0TEXT_END;

/*----------------------------------------------------------------------------
 *
 * MipsSwitchFPState --
 *
 *	Save the current state into 'from' and restore it from 'to'.
 *
 *	MipsSwitchFPState(from, to)
 *		struct thread *from;
 *		struct trapframe *to;
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------------
 */
LEAF(MipsSwitchFPState)
	mfc0	t1, COP_0_STATUS_REG	# Save old SR
	li	t0, SR_COP_1_BIT	# enable the coprocessor
	mtc0	t0, COP_0_STATUS_REG
	ITLBNOPFIX

	beq	a0, zero, 1f		# skip save if NULL pointer
	nop
/*
 * First read out the status register to make sure that all FP operations
 * have completed.
 */
	lw	a0, TD_PCB(a0)			# get pointer to pcb for proc
	cfc1	t0, FPC_CSR			# stall til FP done
	cfc1	t0, FPC_CSR			# now get status
	li	t3, ~SR_COP_1_BIT
	RESTORE_U_PCB_REG(t2, PS, a0)		# get CPU status register
	SAVE_U_PCB_FPSR(t0, FSR_NUM, a0)	# save FP status
	and	t2, t2, t3			# clear COP_1 enable bit
	SAVE_U_PCB_REG(t2, PS, a0)		# save new status register
/*
 * Save the floating point registers.
 */
	SAVE_U_PCB_FPREG($f0, F0_NUM, a0)
	SAVE_U_PCB_FPREG($f1, F1_NUM, a0)
	SAVE_U_PCB_FPREG($f2, F2_NUM, a0)
	SAVE_U_PCB_FPREG($f3, F3_NUM, a0)
	SAVE_U_PCB_FPREG($f4, F4_NUM, a0)
	SAVE_U_PCB_FPREG($f5, F5_NUM, a0)
	SAVE_U_PCB_FPREG($f6, F6_NUM, a0)
	SAVE_U_PCB_FPREG($f7, F7_NUM, a0)
	SAVE_U_PCB_FPREG($f8, F8_NUM, a0)
	SAVE_U_PCB_FPREG($f9, F9_NUM, a0)
	SAVE_U_PCB_FPREG($f10, F10_NUM, a0)
	SAVE_U_PCB_FPREG($f11, F11_NUM, a0)
	SAVE_U_PCB_FPREG($f12, F12_NUM, a0)
	SAVE_U_PCB_FPREG($f13, F13_NUM, a0)
	SAVE_U_PCB_FPREG($f14, F14_NUM, a0)
	SAVE_U_PCB_FPREG($f15, F15_NUM, a0)
	SAVE_U_PCB_FPREG($f16, F16_NUM, a0)
	SAVE_U_PCB_FPREG($f17, F17_NUM, a0)
	SAVE_U_PCB_FPREG($f18, F18_NUM, a0)
	SAVE_U_PCB_FPREG($f19, F19_NUM, a0)
	SAVE_U_PCB_FPREG($f20, F20_NUM, a0)
	SAVE_U_PCB_FPREG($f21, F21_NUM, a0)
	SAVE_U_PCB_FPREG($f22, F22_NUM, a0)
	SAVE_U_PCB_FPREG($f23, F23_NUM, a0)
	SAVE_U_PCB_FPREG($f24, F24_NUM, a0)
	SAVE_U_PCB_FPREG($f25, F25_NUM, a0)
	SAVE_U_PCB_FPREG($f26, F26_NUM, a0)
	SAVE_U_PCB_FPREG($f27, F27_NUM, a0)
	SAVE_U_PCB_FPREG($f28, F28_NUM, a0)
	SAVE_U_PCB_FPREG($f29, F29_NUM, a0)
	SAVE_U_PCB_FPREG($f30, F30_NUM, a0)
	SAVE_U_PCB_FPREG($f31, F31_NUM, a0)

1:
/*
 *  Restore the floating point registers.
 */
	RESTORE_U_PCB_FPSR(t0, FSR_NUM, a1)	# get status register
	RESTORE_U_PCB_FPREG($f0, F0_NUM, a1)
	RESTORE_U_PCB_FPREG($f1, F1_NUM, a1)
	RESTORE_U_PCB_FPREG($f2, F2_NUM, a1)
	RESTORE_U_PCB_FPREG($f3, F3_NUM, a1)
	RESTORE_U_PCB_FPREG($f4, F4_NUM, a1)
	RESTORE_U_PCB_FPREG($f5, F5_NUM, a1)
	RESTORE_U_PCB_FPREG($f6, F6_NUM, a1)
	RESTORE_U_PCB_FPREG($f7, F7_NUM, a1)
	RESTORE_U_PCB_FPREG($f8, F8_NUM, a1)
	RESTORE_U_PCB_FPREG($f9, F9_NUM, a1)
	RESTORE_U_PCB_FPREG($f10, F10_NUM, a1)
	RESTORE_U_PCB_FPREG($f11, F11_NUM, a1)
	RESTORE_U_PCB_FPREG($f12, F12_NUM, a1)
	RESTORE_U_PCB_FPREG($f13, F13_NUM, a1)
	RESTORE_U_PCB_FPREG($f14, F14_NUM, a1)
	RESTORE_U_PCB_FPREG($f15, F15_NUM, a1)
	RESTORE_U_PCB_FPREG($f16, F16_NUM, a1)
	RESTORE_U_PCB_FPREG($f17, F17_NUM, a1)
	RESTORE_U_PCB_FPREG($f18, F18_NUM, a1)
	RESTORE_U_PCB_FPREG($f19, F19_NUM, a1)
	RESTORE_U_PCB_FPREG($f20, F20_NUM, a1)
	RESTORE_U_PCB_FPREG($f21, F21_NUM, a1)
	RESTORE_U_PCB_FPREG($f22, F22_NUM, a1)
	RESTORE_U_PCB_FPREG($f23, F23_NUM, a1)
	RESTORE_U_PCB_FPREG($f24, F24_NUM, a1)
	RESTORE_U_PCB_FPREG($f25, F25_NUM, a1)
	RESTORE_U_PCB_FPREG($f26, F26_NUM, a1)
	RESTORE_U_PCB_FPREG($f27, F27_NUM, a1)
	RESTORE_U_PCB_FPREG($f28, F28_NUM, a1)
	RESTORE_U_PCB_FPREG($f29, F29_NUM, a1)
	RESTORE_U_PCB_FPREG($f30, F30_NUM, a1)
	RESTORE_U_PCB_FPREG($f31, F31_NUM, a1)

	and	t0, t0, ~FPC_EXCEPTION_BITS
	ctc1	t0, FPC_CSR
	nop

	mtc0	t1, COP_0_STATUS_REG		# Restore the status register.
	ITLBNOPFIX
	j	ra
	nop
END(MipsSwitchFPState)

/*----------------------------------------------------------------------------
 *
 * MipsSaveCurFPState --
 *
 *	Save the current floating point coprocessor state.
 *
 *	MipsSaveCurFPState(td)
 *		struct thread *td;
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	machFPCurProcPtr is cleared.
 *
 *----------------------------------------------------------------------------
 */
LEAF(MipsSaveCurFPState)
	lw	a0, TD_PCB(a0)			# get pointer to pcb for thread
	mfc0	t1, COP_0_STATUS_REG		# Disable interrupts and
	li	t0, SR_COP_1_BIT		#  enable the coprocessor
	mtc0	t0, COP_0_STATUS_REG
	ITLBNOPFIX
	GET_CPU_PCPU(a1)
	sw	zero, PC_FPCURTHREAD(a1)	# indicate state has been saved
/*
 * First read out the status register to make sure that all FP operations
 * have completed.
 */
	RESTORE_U_PCB_REG(t2, PS, a0)		# get CPU status register
	li	t3, ~SR_COP_1_BIT
	and	t2, t2, t3			# clear COP_1 enable bit
	cfc1	t0, FPC_CSR			# stall til FP done
	cfc1	t0, FPC_CSR			# now get status
	SAVE_U_PCB_REG(t2, PS, a0)		# save new status register
	SAVE_U_PCB_FPSR(t0, FSR_NUM, a0)	# save FP status
/*
 * Save the floating point registers.
 */
	SAVE_U_PCB_FPREG($f0, F0_NUM, a0)
	SAVE_U_PCB_FPREG($f1, F1_NUM, a0)
	SAVE_U_PCB_FPREG($f2, F2_NUM, a0)
	SAVE_U_PCB_FPREG($f3, F3_NUM, a0)
	SAVE_U_PCB_FPREG($f4, F4_NUM, a0)
	SAVE_U_PCB_FPREG($f5, F5_NUM, a0)
	SAVE_U_PCB_FPREG($f6, F6_NUM, a0)
	SAVE_U_PCB_FPREG($f7, F7_NUM, a0)
	SAVE_U_PCB_FPREG($f8, F8_NUM, a0)
	SAVE_U_PCB_FPREG($f9, F9_NUM, a0)
	SAVE_U_PCB_FPREG($f10, F10_NUM, a0)
	SAVE_U_PCB_FPREG($f11, F11_NUM, a0)
	SAVE_U_PCB_FPREG($f12, F12_NUM, a0)
	SAVE_U_PCB_FPREG($f13, F13_NUM, a0)
	SAVE_U_PCB_FPREG($f14, F14_NUM, a0)
	SAVE_U_PCB_FPREG($f15, F15_NUM, a0)
	SAVE_U_PCB_FPREG($f16, F16_NUM, a0)
	SAVE_U_PCB_FPREG($f17, F17_NUM, a0)
	SAVE_U_PCB_FPREG($f18, F18_NUM, a0)
	SAVE_U_PCB_FPREG($f19, F19_NUM, a0)
	SAVE_U_PCB_FPREG($f20, F20_NUM, a0)
	SAVE_U_PCB_FPREG($f21, F21_NUM, a0)
	SAVE_U_PCB_FPREG($f22, F22_NUM, a0)
	SAVE_U_PCB_FPREG($f23, F23_NUM, a0)
	SAVE_U_PCB_FPREG($f24, F24_NUM, a0)
	SAVE_U_PCB_FPREG($f25, F25_NUM, a0)
	SAVE_U_PCB_FPREG($f26, F26_NUM, a0)
	SAVE_U_PCB_FPREG($f27, F27_NUM, a0)
	SAVE_U_PCB_FPREG($f28, F28_NUM, a0)
	SAVE_U_PCB_FPREG($f29, F29_NUM, a0)
	SAVE_U_PCB_FPREG($f30, F30_NUM, a0)
	SAVE_U_PCB_FPREG($f31, F31_NUM, a0)

	mtc0	t1, COP_0_STATUS_REG		# Restore the status register.
	ITLBNOPFIX
	j	ra
	nop
END(MipsSaveCurFPState)

/*
 * When starting init, call this to configure the process for user
 * mode.  This will be inherited by other processes.
 */
LEAF_NOPROFILE(prepare_usermode)
	j	ra
	nop
END(prepare_usermode)


/*
 * This code is copied the user's stack for returning from signal handlers
 * (see sendsig() and sigreturn()). We have to compute the address
 * of the sigcontext struct for the sigreturn call.
 */
	.globl	_C_LABEL(sigcode)
_C_LABEL(sigcode):
	addu	a0, sp, SIGF_UC			# address of ucontext
	li	v0, SYS_sigreturn
# sigreturn (ucp)
	syscall
	break	0				# just in case sigreturn fails
	.globl	_C_LABEL(esigcode)
_C_LABEL(esigcode):

	.data
	.globl	szsigcode
szsigcode:
	.long	esigcode-sigcode
	.text
