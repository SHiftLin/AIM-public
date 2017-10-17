/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <segment.h>

.set CR0_PE,             0x1

.section .entry

.code16
.globl _start
_start:
    cli
    cld
    
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

setA20.1:
    in $0x64, %al
    test $0x2, %al
    jnz setA20.1

    mov $0xd1, %al
    out %al, $0x64

setA20.2:
    in $0x64, %al
    test $0x2, %al
    jnz setA20.2

    mov $0xdf, %al
    out %al, $0x60

    lgdt gdtr
    mov	%cr0, %eax
	or	$CR0_PE, %eax
	mov	%eax, %cr0

    ljmp $(CODE_SEG<<3), $bootasm

.code32
.text

bootasm:
    mov $(DATA_SEG<<3), %ax
    mov %ax, %ds
    mov %ax, %ss

    mov $STACK_TOP, %esp
    call bootmain

.data

gdt:
    SEG_NULL
    SET_SEG(0xa,0x0,0xffffffff)
    SET_SEG(0x2,0x0,0xffffffff)

gdtr:
    .word  0x17
    .long  gdt
