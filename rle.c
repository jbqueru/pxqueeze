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

void find_rle_runs(
			unsigned int ** const output_lengths,
			unsigned int ** const output_values,
			unsigned int * const output_size,
			unsigned int const * const input_data,
			unsigned int const input_size,
			unsigned int const max_run_length) {
	unsigned int read_offset = 0;
	unsigned int write_offset = 0;
	unsigned int current_value;
	unsigned int current_length;

	printf("Looking for RLE runs from %u symbols\n", input_size);

	unsigned int * lengths = malloc(input_size * sizeof(unsigned int));

	if (!lengths) {
		fprintf(stderr, __FILE__":%d Could not allocate %lu bytes for RLE lengths\n",
					__LINE__,
					input_size * sizeof(unsigned int));
		exit(1);
	}

	unsigned int * values = malloc(input_size * sizeof(unsigned int));

	if (!lengths) {
		fprintf(stderr, __FILE__":%d Could not allocate %lu bytes for RLE values\n",
					__LINE__,
					input_size * sizeof(unsigned int));
		exit(1);
	}

	while (read_offset < input_size) {
		current_value = input_data[read_offset++];
		current_length = 1;
		while (read_offset < input_size
					&& input_data[read_offset] == current_value
					&& current_length < max_run_length) {
			read_offset++;
			current_length++;
		}
		lengths[write_offset] = current_length;
		values[write_offset] = current_value;
		write_offset++;
	}

	lengths = realloc(lengths, write_offset * sizeof(unsigned int));

	if (!lengths) {
		fprintf(stderr, __FILE__":%d Could not re-allocate %lu bytes for RLE lengths\n",
					__LINE__,
					write_offset * sizeof(unsigned int));
		exit(1);
	}

	values = realloc(values, write_offset * sizeof(unsigned int));

	if (!values) {
		fprintf(stderr, __FILE__":%d Could not re-allocate %lu bytes for RLE values\n",
					__LINE__,
					write_offset * sizeof(unsigned int));
		exit(1);
	}

	*output_lengths = lengths;
	*output_values = values;
	*output_size = write_offset;
	printf("Found %u RLE runs\n", write_offset);
}

void process_rle_runs(unsigned int * rle_lengths,
				unsigned int * rle_values,
				unsigned int size) {
	unsigned int symbols_huffman_size;
	unsigned int * symbols_huffman_table;
	unsigned int num_symbols;

	symbols_huffman_table = generate_huffman_table(&symbols_huffman_size, &num_symbols, rle_values, 1, size);

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

	lengths_huffman_table = generate_huffman_table(&lengths_huffman_size, &num_lengths, rle_lengths, 1, size);

	char** length_codes;

	length_codes = generate_huffman_codes(lengths_huffman_table, num_lengths);

	for (unsigned int i = 0; i < num_lengths; i++) {
		if (length_codes[i]) {
			printf("length %u has code %s\n", i, length_codes[i]);
		}
	}
	unsigned int output_bits = 0;

	for (unsigned i = 0; i < size; i++) {
/*		printf("Run length %u symbol %u stored in %u+%u bits\n",
					input[i], input[i+1],
					(unsigned int)strlen(length_codes[input[i]]),
					(unsigned int)strlen(symbol_codes[input[i+1]]));*/
		output_bits += (unsigned int)strlen(length_codes[rle_lengths[i]])
					+ (unsigned int)strlen(symbol_codes[rle_values[i]]);
	}
	printf("Total output size %u bits (= %u bytes)\n", output_bits, (output_bits + 7) / 8);

}
