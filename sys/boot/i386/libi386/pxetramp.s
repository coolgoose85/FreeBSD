#
# Copyright (c) 2000 Peter Wemm
# All rights reserved.
#
# Redistribution and use in source and binary forms are freely
# permitted provided that the above copyright notice and this
# paragraph and the following disclaimer are duplicated in all
# such forms.
#
# This software is provided "AS IS" and without any express or
# implied warranties, including, without limitation, the implied
# warranties of merchantability and fitness for a particular
# purpose.
#
# $FreeBSD: src/sys/boot/i386/libi386/pxetramp.s,v 1.2 2000/04/26 07:38:40 ps Exp $

# ph33r this

		.globl  __bangpxeentry, __bangpxeseg, __bangpxeoff
		.globl  __pxenventry, __pxenvseg, __pxenvoff

		.code16
		.p2align 4,0x90
__bangpxeentry:
		push    %dx			# seg:data
		push    %ax			# off:data
		push    %bx			# int16 func
		.byte   0x9a			# far call
__bangpxeoff:	.word   0x0000			# offset
__bangpxeseg:	.word   0x0000			# segment
		add	$6, %sp			# restore stack
		.byte	0xcb			# to vm86int
#
__pxenventry:
		.byte   0x9a			# far call
__pxenvoff:	.word   0x0000			# offset
__pxenvseg:	.word   0x0000			# segment
		.byte	0xcb			# to vm86int
