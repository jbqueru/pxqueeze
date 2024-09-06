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

unsigned int * generate_huffman_table(unsigned int * const output_size,
			unsigned int const * const input,
			unsigned int const input_size) {
	// Count distinct symbols, which is the number of leaves in the tree
	unsigned int distinct_symbols = 0;
	for (unsigned int i = 0; i < input_size; i++) {
		if (input[i] > 0) {
			distinct_symbols++;
		}
	}

	printf("Generating Huffman table for symbol range 0 to %u (%u distinct)\n",
				input_size - 1,
				distinct_symbols);

/* The size numbers are wrong
	unsigned int symbol_bits = 0;
	while(size > 1U << symbol_bits) {
		symbol_bits++;
	}

	unsigned int node_bits = 0;
	while(distinct_symbols > 1U << node_bits) {
		node_bits++;
	}

	printf("Dense Huffman table will have %u entries of %u bits each (%u total)\n",
				distinct_symbols - 1, symbol_bits + 1,
				(distinct_symbols - 1) * (symbol_bits + 1));

	printf("Sparse Huffman table will have %u entries of %u bits each,\n",
				distinct_symbols - 1, node_bits + 1);
	printf("  plus a dictionary of %u entries of %u bits each (%u total)\n",
				distinct_symbols, symbol_bits,
				(distinct_symbols - 1) * (node_bits + 1) + distinct_symbols * symbol_bits);
*/

	unsigned int* values = malloc(distinct_symbols * sizeof(unsigned int));
	if (!values) {
		fprintf(stderr, __FILE__":%d Could not allocate %lu bytes for Huffman values\n",
					__LINE__,
					distinct_symbols * sizeof(unsigned int));
		exit(1);
	}

	unsigned int* weights = malloc(distinct_symbols * sizeof(unsigned int));
	if (!weights) {
		fprintf(stderr, __FILE__":%d Could not allocate %lu bytes for Huffman weights\n",
					__LINE__,
					distinct_symbols * sizeof(unsigned int));
		exit(1);
	}

	// populate the table of values / weights with leaf values
	unsigned int w = 0;
	for (int i = 0; i < input_size; i++) {
		if (input[i] > 0) {
			values[w] = i;
			weights[w] = input[i];
			w++;
		}
	}

	// sort the table (I know, bubble sort isn't fast)
	for (int j = 0; j < distinct_symbols - 1; j++) {
		for (int i = 0; i < distinct_symbols - 1; i++) {
			if (weights[i] > weights[i + 1]) {
				unsigned int t;
				t = weights[i];
				weights[i] = weights[i + 1];
				weights[i + 1] = t;
				t = values[i];
				values[i] = values[i + 1];
				values[i + 1] = t;
			}
		}
	}

//	for (int i = 0; i < distinct_symbols; i++) {
//		printf("Initial weight %u value %u\n", weights[i], values[i]);
//	}

	printf("Creating Huffman table with %u entries\n", 2 * (distinct_symbols - 1));
	unsigned int* huffman = malloc(2 * (distinct_symbols - 1) * sizeof(unsigned int));
	if (!huffman) {
		fprintf(stderr, __FILE__":%d Could not allocate %lu bytes for Huffman table\n",
					__LINE__,
					2 * (distinct_symbols - 1) * sizeof(unsigned int));
		exit(1);
	}

	unsigned int next_node = input_size + distinct_symbols - 2;

	for (unsigned int j = 0; j < distinct_symbols - 1; j++) {
//		printf("Creating node %u from %u and %u, weight %u\n",
//					input_size + distinct_symbols - j - 2,
//					values[j],
//					values[j + 1],
//					weights[j] + weights[j + 1]);
		huffman[2 * (distinct_symbols - 2 - j)] = values[j];
		huffman[2 * (distinct_symbols - 2 - j) + 1] = values[j + 1];
		values[j + 1] = input_size + distinct_symbols - j - 2;
		weights[j + 1] += weights[j];
		for (int i = j; i < distinct_symbols - 1; i++) {
			if (weights[i] > weights[i + 1]) {
				unsigned int t;
				t = weights[i];
				weights[i] = weights[i + 1];
				weights[i + 1] = t;
				t = values[i];
				values[i] = values[i + 1];
				values[i + 1] = t;
			}
		}
	}

	free(values);
	free(weights);

	for (int i = 0; i < distinct_symbols - 1; i++) {
//		printf("Huffman node %u: children %u and %u\n",
//					i + input_size,
//					huffman[2 * i],
//					huffman [2 * i + 1]);
	}
	printf("Completed Huffman table with %u entries\n", 2 * (distinct_symbols - 1));
	*output_size = 2 * (distinct_symbols - 1);
	return huffman;
}

static void codes_inner(unsigned int const * const huffman_table,
			unsigned int const num_symbols,
			unsigned int node,
			char * prefix) {
	printf("Processing node %u with prefix [%s]\n", node + num_symbols, prefix);
	char * prefix1 = strdup(prefix);
	prefix1 = realloc(prefix1, strlen(prefix1) + 2);
	strcat(prefix1, "0");
	if (huffman_table[2 * node] >= num_symbols) {
		codes_inner(huffman_table, num_symbols, huffman_table[2 * node] - num_symbols, prefix1);
	} else {
		printf("Symbol %u has code [%s]\n", huffman_table[2 * node], prefix1);
	}
	prefix = realloc(prefix, strlen(prefix) + 2);
	strcat(prefix, "1");
	if (huffman_table[2 * node + 1] >= num_symbols) {
		codes_inner(huffman_table, num_symbols, huffman_table[2 * node + 1] - num_symbols, prefix);
	} else {
		printf("Symbol %u has code [%s]\n", huffman_table[2 * node + 1], prefix);
	}
}

void generate_huffman_codes(unsigned int const * const huffman_table,
			unsigned int const num_symbols) {
	codes_inner(huffman_table, num_symbols, 0, strdup(""));
}
