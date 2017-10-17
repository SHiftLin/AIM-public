#ifndef _SEGMENT_H
#define _SEGMENT_H

#define SEG_NULL \
    .word 0, 0;  \
    .byte 0, 0, 0, 0

#define SET_SEG(type, base, limit)           \
    .word ((limit) & 0xffff), ((base) & 0xffff); \
    .byte (((base) >> 16) & 0xff), (0x90 | (type)), (0xC0 | (((limit) >> 16) & 0xf)), (((base) >> 24) & 0xff)

#define CODE_SEG 0x1
#define DATA_SEG 0x2

#endif
