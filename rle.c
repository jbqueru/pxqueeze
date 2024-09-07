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
#include "rle.h"

static void allocate_rle_buffers(
		unsigned int ** const lengths,
		unsigned int ** const values,
		unsigned int const buffer_size);

static void resize_rle_buffers(
		unsigned int ** const lengths,
		unsigned int ** const values,
		unsigned int const buffer_size);

void rle_find_runs(
		unsigned int const ** const output_lengths,
		unsigned int const ** const output_values,
		unsigned int * const output_size,
		unsigned int const * const input_data,
		unsigned int const input_size,
		unsigned int const max_run_length) {

	printf("Looking for RLE runs from %u symbols\n", input_size);

	unsigned int read_offset = 0;
	unsigned int write_offset = 0;
	unsigned int current_length;
	unsigned int current_value;

	unsigned int * lengths;
	unsigned int * values;

	// Allocate buffers at maximum theoretical size
	allocate_rle_buffers(&lengths, &values, input_size);

	// Core algorithm
	while (read_offset < input_size) {
		// Start a new run
		current_value = input_data[read_offset++];
		current_length = 1;
		// Add to that run
		while (read_offset < input_size
					&& input_data[read_offset] == current_value
					&& current_length < max_run_length) {
			read_offset++;
			current_length++;
		}
		// Store the run
		lengths[write_offset] = current_length;
		values[write_offset] = current_value;
		write_offset++;
	}

	printf("Found %u RLE runs\n", write_offset);

	// Resize buffers to actual usage
	resize_rle_buffers(&lengths, &values, write_offset);

	// Store return values
	*output_lengths = lengths;
	*output_values = values;
	*output_size = write_offset;
}

void rle_naive_process_runs(
		unsigned int const * const rle_lengths,
		unsigned int const * const rle_values,
		unsigned int const size) {

	unsigned int symbols_huffman_size;
	unsigned int * symbols_huffman_table;
	unsigned int num_symbols;

	symbols_huffman_table = generate_huffman_table(&symbols_huffman_size, &num_symbols, rle_values, 1, size);

	printf("%u symbols, Huffman table %u, %u distinct values\n", num_symbols, symbols_huffman_size, num_symbols + symbols_huffman_size);

	unsigned int bits = 0;
	while(num_symbols + symbols_huffman_size > (1 << bits)) bits++;

	printf("%u bits per value node address\n", bits);
	printf("total Huffman value table size %u bits\n", 3 + bits * (2 * symbols_huffman_size + 1));

	char** symbol_codes;

	symbol_codes = generate_huffman_codes(symbols_huffman_table, num_symbols);

	for (int i = 0; i < num_symbols; i++) {
		if (symbol_codes[i]) {
//			printf("Symbol %u has code %s\n", i, symbol_codes[i]);
		}
	}

	unsigned int lengths_huffman_size;
	unsigned int * lengths_huffman_table;
	unsigned int num_lengths;

	lengths_huffman_table = generate_huffman_table(&lengths_huffman_size, &num_lengths, rle_lengths, 1, size);

	bits = 0;
	while(num_lengths + lengths_huffman_size > (1 << bits)) bits++;

	printf("%u bits per length node address\n", bits);
	printf("total Huffman length table size %u bits\n", 3 + bits * (2 * lengths_huffman_size + 1));

	char** length_codes;

	length_codes = generate_huffman_codes(lengths_huffman_table, num_lengths);

	for (unsigned int i = 0; i < num_lengths; i++) {
		if (length_codes[i]) {
//			printf("length %u has code %s\n", i, length_codes[i]);
		}
	}
	unsigned int output_bits = 0;

	for (unsigned i = 0; i < size; i++) {
		output_bits += (unsigned int)strlen(length_codes[rle_lengths[i]])
					+ (unsigned int)strlen(symbol_codes[rle_values[i]]);
	}
	printf("Total output size %u bits (= %u bytes)\n", output_bits, (output_bits + 7) / 8);

}

static void allocate_rle_buffers(
		unsigned int ** const lengths,
		unsigned int ** const values,
		unsigned int const buffer_size) {
	*lengths = malloc(buffer_size * sizeof(unsigned int));

	if (!*lengths) {
		fprintf(stderr, "%s:%d Could not allocate %lu bytes for RLE lengths\n",
					__FILE__,
					__LINE__,
					buffer_size * sizeof(unsigned int));
		exit(1);
	}

	*values = malloc(buffer_size * sizeof(unsigned int));

	if (!*values) {
		fprintf(stderr, "%s:%d Could not allocate %lu bytes for RLE values\n",
					__FILE__,
					__LINE__,
					buffer_size * sizeof(unsigned int));
		exit(1);
	}

}

static void resize_rle_buffers(
		unsigned int ** const lengths,
		unsigned int ** const values,
		unsigned int const buffer_size) {
	*lengths = realloc(*lengths, buffer_size * sizeof(unsigned int));

	if (!*lengths) {
		fprintf(stderr, "%s:%d Could not re-allocate %lu bytes for RLE lengths\n",
					__FILE__,
					__LINE__,
					buffer_size * sizeof(unsigned int));
		exit(1);
	}

	*values = realloc(*values, buffer_size * sizeof(unsigned int));

	if (!*values) {
		fprintf(stderr, __FILE__"%s:%d Could not re-allocate %lu bytes for RLE values\n",
					__FILE__,
					__LINE__,
					buffer_size * sizeof(unsigned int));
		exit(1);
	}
}
