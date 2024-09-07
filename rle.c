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

/*
* Helper function: allocate two arrays of the same size, exit in case of error
*/
static void _allocate_arrays(
		unsigned int ** const lengthArrayP,
		unsigned int ** const symbolArrayP,
		unsigned int const array_size);

/*
* Helper function: resize two arrays to the same size, exit in case of error
*/
static void _resize_arrays(
		unsigned int ** const lengthArrayP,
		unsigned int ** const symbolArrayP,
		unsigned int const array_size);

void rle_find_runs(
		unsigned int const ** const outLengthP,
		unsigned int const ** const outSymbolP,
		unsigned int * const outSize,
		unsigned int const * const inData,
		unsigned int const inSize,
		unsigned int const inMaxRunLength) {

	printf("Looking for RLE runs from %u symbols\n", inSize);

	unsigned int read_offset = 0;
	unsigned int write_offset = 0;
	unsigned int current_length;
	unsigned int current_symbol;

	unsigned int * lengths;
	unsigned int * symbols;

	// Allocate buffers at maximum theoretical size
	_allocate_arrays(&lengths, &symbols, inSize);

	// Core algorithm
	while (read_offset < inSize) {
		// Start a new run
		current_symbol = inData[read_offset++];
		current_length = 1;
		// Add to that run
		while (read_offset < inSize
					&& inData[read_offset] == current_symbol
					&& current_length < inMaxRunLength) {
			read_offset++;
			current_length++;
		}
		// Store the run
		lengths[write_offset] = current_length;
		symbols[write_offset] = current_symbol;
		write_offset++;
	}

	printf("Found %u RLE runs\n", write_offset);

	// Resize buffers to actual usage
	_resize_arrays(&lengths, &symbols, write_offset);

	// Store return values
	*outLengthP = lengths;
	*outSymbolP = symbols;
	*outSize = write_offset;
}

void rle_try_strategies(
		unsigned int const * const inLengthP,
		unsigned int const * const inSymbolP,
		unsigned int const inSize) {

	unsigned int * buffer;
	unsigned int huffman_bit_width;

	unsigned int const * huffman_table;
	unsigned int huffman_size;
	unsigned int huffman_start;
	char** huffman_codes;
	unsigned int huffman_stream_length;

	printf("Trying RLE strategy: single table\n");

	buffer = malloc(2 * inSize * sizeof(unsigned int));
	memcpy(buffer, inLengthP, inSize * sizeof(unsigned int));
	memcpy(buffer + inSize, inSymbolP, inSize * sizeof(unsigned int));

	generate_huffman_table(
			&huffman_table,
			&huffman_size,
			&huffman_start,
			buffer,
			1,
			2 * inSize);

	huffman_codes = generate_huffman_codes(huffman_table, huffman_start);

	huffman_bit_width = 0;
	while (huffman_start + huffman_size > (1 << huffman_bit_width)) {
		huffman_bit_width++;
	}

	huffman_stream_length = 3 + (2 * huffman_size + 1) * huffman_bit_width;
	for (unsigned i = 0; i < 2 * inSize; i++) {
		huffman_stream_length += (unsigned int)strlen(huffman_codes[buffer[i]]);
	}

	printf("With single table: %u bits of Huffman data (= %u bytes)\n",
			huffman_stream_length,
			(huffman_stream_length + 7) / 8);

	free(buffer);
	free((void*)huffman_table);
}

void rle_naive_process_runs(
		unsigned int const * const rle_lengths,
		unsigned int const * const rle_values,
		unsigned int const size) {

	unsigned int symbols_huffman_size;
	unsigned int const * symbols_huffman_table;
	unsigned int num_symbols;

	generate_huffman_table(&symbols_huffman_table, &symbols_huffman_size, &num_symbols, rle_values, 1, size);

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
	unsigned int const * lengths_huffman_table;
	unsigned int num_lengths;

	generate_huffman_table(&lengths_huffman_table, &lengths_huffman_size, &num_lengths, rle_lengths, 1, size);

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

static void _allocate_arrays(
		unsigned int ** const lengthArrayP,
		unsigned int ** const symbolArrayP,
		unsigned int const array_size) {

	// Allocate first array
	*lengthArrayP = malloc(array_size * sizeof(unsigned int));

	// Check that allocation was successful, exit if not
	if (!*lengthArrayP) {
		fprintf(stderr, "%s:%d Could not allocate %lu bytes for RLE lengths\n",
					__FILE__,
					__LINE__,
					array_size * sizeof(unsigned int));
		exit(1);
	}

	// Allocate second array
	*symbolArrayP = malloc(array_size * sizeof(unsigned int));

	// Check that allocation was successful, exit if not
	if (!*symbolArrayP) {
		fprintf(stderr, "%s:%d Could not allocate %lu bytes for RLE symbols\n",
					__FILE__,
					__LINE__,
					array_size * sizeof(unsigned int));
		exit(1);
	}

}

static void _resize_arrays(
		unsigned int ** const lengthArrayP,
		unsigned int ** const symbolArrayP,
		unsigned int const array_size) {

	// Resize first array
	*lengthArrayP = realloc(*lengthArrayP, array_size * sizeof(unsigned int));

	// Check that resize was successful, exit if not
	if (!*lengthArrayP) {
		fprintf(stderr, "%s:%d Could not re-allocate %lu bytes for RLE lengths\n",
					__FILE__,
					__LINE__,
					array_size * sizeof(unsigned int));
		exit(1);
	}

	// Resize second array
	*symbolArrayP = realloc(*symbolArrayP, array_size * sizeof(unsigned int));

	// Check that resize was successful, exit if not
	if (!*symbolArrayP) {
		fprintf(stderr, __FILE__"%s:%d Could not re-allocate %lu bytes for RLE values\n",
					__FILE__,
					__LINE__,
					array_size * sizeof(unsigned int));
		exit(1);
	}
}
