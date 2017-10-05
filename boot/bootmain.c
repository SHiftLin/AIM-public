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

#include <sys/types.h>
#include <aim/boot.h>
#include <elf.h>
#include <asm.h>

#define ELF_BUF_SIZE 512

void *elf_buf[ELF_BUF_SIZE];

void waitdisk(void)
{
	while ((inb(0x1F7) & 0xC0) != 0x40);
}

void readseg(void *buf, uint32_t cnt, uint32_t lba)
{
	waitdisk();
	outb(0x1F2, 1);
	outb(0x1F3, lba);
	outb(0x1F4, lba >> 8);
	outb(0x1F5, lba >> 16);
	outb(0x1F6, (lba >> 24) | 0xE0);
	outb(0x1F7, 0x20);

	waitdisk();
	insb(0x1F0, buf, cnt);
}

uint32_t get_kernel_base()
{
	uint32_t base = 0;
	int i = 0;
	for (i = 0; i < 4; i++)
		base |= (mbr[446 + 8 + i] << (i * 8));
	return base;
}

__noreturn void bootmain(void)
{
	uint32_t base = get_kernel_base();
	void (*entry)(void);
	elf_hdr *elf = (elf_hdr *)elf_buf;

	readseg(elf, ELF_BUF_SIZE, base);
	Elf32_Half cnt = elf->e_phnum, i = 0;
	elf_phdr *ph = (elf_phdr *)elf->e_phoff;
	for (i = 0; i < cnt; i++, ph++)
	{
		void *pa = (void *)ph->p_vaddr;
		readseg(pa, ph->p_filesz, base + ph->p_offset);
	}
	entry = (void (*)(void))elf->e_entry;
	entry();
	while (1)
		;
}
