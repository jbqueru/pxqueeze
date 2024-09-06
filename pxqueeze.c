/*
 * Copyright 2024 Jean-Baptiste M. "JBQ" "Djaybee" Queru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

// SPDX-License-Identifier: AGPL-3.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.h"
#include "read_tga.h"

unsigned int* find_rle_runs(unsigned int * const output_size,
			unsigned int const * const input_data,
			unsigned int const input_size) {
	unsigned int read_offset = 0;
	unsigned int write_offset = 0;
	unsigned int current_value;
	unsigned int current_length;

	printf("Looking for RLE runs from %u symbols\n", input_size);

	unsigned int * output = malloc(input_size * 2 * sizeof(unsigned int));

	if (!output) {
		fprintf(stderr, __FILE__":%d Could not allocate %lu bytes for RLE runs\n",
					__LINE__,
					input_size * 2 * sizeof(unsigned int));
		exit(1);
	}

	while (read_offset < input_size) {
		current_value = input_data[read_offset++];
		current_length = 1;
		while (read_offset < input_size && input_data[read_offset] == current_value) {
			read_offset++;
			current_length++;
		}
		output[write_offset++] = current_length;
		output[write_offset++] = current_value;
	}
	output = realloc(output, write_offset * sizeof(unsigned int));

	if (!output) {
		fprintf(stderr, __FILE__":%d Could not re-allocate %lu bytes for RLE runs\n",
					__LINE__,
					write_offset * sizeof(unsigned int));
		exit(1);
	}

	*output_size = write_offset;
	printf("Found %u RLE runs\n", write_offset / 2);
	return output;
}

void process_rle_runs(unsigned int* input, unsigned int size) {
	unsigned int symbols_huffman_size;
	unsigned int * symbols_huffman_table;
	unsigned int num_symbols;

	symbols_huffman_table = generate_huffman_table(&symbols_huffman_size, &num_symbols, input + 1, 2, size / 2);

	char** symbol_codes;

	symbol_codes = generate_huffman_codes(symbols_huffman_table, num_symbols);

	for (int i = 0; i < num_symbols; i++) {
		if (symbol_codes[i]) {
			printf("Symbol %u has code %s\n", i, symbol_codes[i]);
		}
	}

	unsigned int lengths_huffman_size;
	unsigned int * lengths_huffman_table;
	unsigned int num_lengths;

	lengths_huffman_table = generate_huffman_table(&lengths_huffman_size, &num_lengths, input, 2, size / 2);

	char** length_codes;

	length_codes = generate_huffman_codes(lengths_huffman_table, num_lengths);

	for (unsigned int i = 0; i < num_lengths; i++) {
		if (length_codes[i]) {
			printf("length %u has code %s\n", i, length_codes[i]);
		}
	}
	unsigned int output_bits = 0;

	for (unsigned i = 0; i < size; i += 2) {
/*		printf("Run length %u symbol %u stored in %u+%u bits\n",
					input[i], input[i+1],
					(unsigned int)strlen(length_codes[input[i]]),
					(unsigned int)strlen(symbol_codes[input[i+1]]));*/
		output_bits += (unsigned int)strlen(length_codes[input[i]])
					+ (unsigned int)strlen(symbol_codes[input[i+1]]);
	}
	printf("Total output size %u bits (= %u bytes)\n", output_bits, (output_bits + 7) / 8);

}

void main() {
	unsigned int * pixels = read_tga();

	unsigned int * rle_output;
	unsigned int num_runs;

	rle_output = find_rle_runs(&num_runs, pixels, 64000);

	process_rle_runs(rle_output, num_runs);

	free(rle_output);


/*
	for (int n = 0; n < 64000; n++) {
		output[n] = pix[n];
	}

	FILE* outputfile = fopen("out/gfx/pixels.bin", "wb");
	fwrite(output, 1, 64000, outputfile);
	fclose(outputfile);

	move_to_front();

	for (int n = 0; n < 64000; n++) {
		output[n] = pixmod[n];
	}

	outputfile = fopen("out/gfx/mtf.bin", "wb");
	fwrite(output, 1, 64000, outputfile);
	fclose(outputfile);

	back_from_front();

	for (int n = 0; n < 64000; n++) {
		output[n] = pixback[n];
	}

	outputfile = fopen("out/gfx/mtfback.bin", "wb");
	fwrite(output, 1, 64000, outputfile);
	fclose(outputfile);
*/

/*	int s = 0;

	for (int target = 0; target < 64000; target++) {
		int beststart = 0;
		int bestlength = 0;
		for (int start = 0; start < target; start++) {
			int length;
			for (length = 0; length < 64000 - target; length++) {
				if (pix[start + length] != pix[target + length]) {
					break;
				}
				if (length >= bestlength) {
					bestlength = length;
					beststart = start;
				}
			}
		}
		if (bestlength == 0) {
//			printf("no match found for %d\n", target);
		} else {
			if (target + bestlength == 64000) {
//				printf("input overflow\n");
				bestlength--;
			}
//			printf("found match of %d pixels at %d for %d\n", bestlength, beststart, target);
		}

		lzoutput[s++] = bestlength >> 8;
		lzoutput[s++] = bestlength % 256;
		lzoutput[s++] = beststart >> 8;
		lzoutput[s++] = beststart % 256;
		lzoutput[s++] = pix[target + bestlength];
		s++;

		target += bestlength;
	}

	FILE* lzfile = fopen("out/gfx/lz.bin", "wb");
	fwrite(lzoutput, 1, s, lzfile);
	fclose(lzfile);
*/
}
