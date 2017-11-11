/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
// vim: expandtab:ts=8:sw=4:softtabstop=4:
///////////////////////////////////////////////////////////////////////////////
//
/// \file       crc32_tablegen.c
/// \brief      Generate crc32_table_le.h and crc32_table_be.h
///
/// Compiling: gcc -std=c99 -o crc32_tablegen crc32_tablegen.c
/// Add -DWORDS_BIGENDIAN to generate big endian table.
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <stdio.h>

#ifdef WORDS_BIGENDIAN
#	include "../../common/bswap.h"
#endif


static uint32_t crc32_table[8][256];


static void
init_crc32_table(void)
{
	static const uint32_t poly32 = UINT32_C(0xEDB88320);

	for (size_t s = 0; s < 8; ++s) {
		for (size_t b = 0; b < 256; ++b) {
			uint32_t r = s == 0 ? b : crc32_table[s - 1][b];

			for (size_t i = 0; i < 8; ++i) {
				if (r & 1)
					r = (r >> 1) ^ poly32;
				else
					r >>= 1;
			}

			crc32_table[s][b] = r;
		}
	}

#ifdef WORDS_BIGENDIAN
	for (size_t s = 0; s < 8; ++s)
		for (size_t b = 0; b < 256; ++b)
			crc32_table[s][b] = bswap_32(crc32_table[s][b]);
#endif

	return;
}


static void
print_crc32_table(void)
{
	printf("/* This file has been automatically generated by "
			"crc32_tablegen.c. */\n\n"
			"const uint32_t lzma_crc32_table[8][256] = {\n\t{");

	for (size_t s = 0; s < 8; ++s) {
		for (size_t b = 0; b < 256; ++b) {
			if ((b % 4) == 0)
				printf("\n\t\t");

			printf("0x%08" PRIX32, crc32_table[s][b]);

			if (b != 255)
				printf(",%s", (b+1) % 4 == 0 ? "" : " ");
		}

		if (s == 7)
			printf("\n\t}\n};\n");
		else
			printf("\n\t}, {");
	}

	return;
}


int
main(void)
{
	init_crc32_table();
	print_crc32_table();
	return 0;
}