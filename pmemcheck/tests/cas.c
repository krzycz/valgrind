/*
 * Persistent memory checker.
 * Copyright (c) 2014-2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, or (at your option) any later version, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */
#include "common.h"
#include <stdint.h>

#define _mm_clflush(addr)\
        asm volatile("clflush %0" : "+m" (*(volatile char *)addr));
#define _mm_clflushopt(addr)\
        asm volatile(".byte 0x66; clflush %0" : "+m" (*(volatile char *)addr));
#define _mm_clwb(addr)\
        asm volatile(".byte 0x66, 0x0f, 0xae, 0x30" : "+m" (*(volatile char *)addr));
#define _mm_sfence()\
        asm volatile(".byte 0x0f, 0xae, 0xf8");

#define FILE_SIZE (16 * 1024 * 1024)

int main ( void )
{
    int32_t tmp = 0;

    /* make, map and register a temporary file */
    void *base = make_map_tmpfile(FILE_SIZE);

    int8_t *i8p = base;
    int16_t *i16p = (int16_t *)((uintptr_t)base + 8);
    int32_t *i32p = (int32_t *)((uintptr_t)base + 16);
    int64_t *i64p = (int64_t *)((uintptr_t)base + 24);

    __sync_bool_compare_and_swap(i8p, *i8p, 1);
    __sync_bool_compare_and_swap(i16p, *i16p + 1, 1);
    __sync_bool_compare_and_swap(i32p, *i32p + 1, 1);
    __sync_bool_compare_and_swap(i64p, *i64p, 1);

    _mm_clflushopt(i8p);
    _mm_clflushopt(i16p);
    _mm_clflushopt(i32p);
    _mm_clflushopt(i64p);
    _mm_sfence();

    __sync_bool_compare_and_swap(&tmp, 0, 1);

    return 0;
}
