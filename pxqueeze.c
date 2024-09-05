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

unsigned char tga[192044];

unsigned int pix_xy[320][200];

unsigned int pix[64000];

unsigned int pixmod[64000];

unsigned int pixback[64000];

unsigned char output[64000];

unsigned char lzoutput[64000 * 6];

int tgablue(int x, int y) {
	return tga[18 + (x + y * 320) * 3];
}

int tgagreen(int x, int y) {
	return tga[18 + (x + y * 320) * 3 + 1];
}

int tgared(int x, int y) {
	return tga[18 + (x + y * 320) * 3 + 2];
}

void move_to_front() {
	int maxpix = 0;
	for (int i = 0; i < 64000; i++) {
		if (pix[i] > maxpix) {
			maxpix = pix[i];
		}
	}
	printf("Max pixel value: %d\n", maxpix);

	int* values = malloc((maxpix + 1) * sizeof(int));
	for (int v = 0; v <= maxpix ; v++) {
		values[v] = v;
	}

	for (int i = 0; i < 64000; i++) {
		int c = 0;
		for (int v = 0 ; v <= maxpix ; v++) {
			if (pix[i] == values[v]) {
				if (v > 0) {
					values[0] = values[v];
					values[v] = c;
				}
				pixmod[i] = v;
				break;
			}
			int cc = values[v];
			values[v] = c;
			c = cc;
		}
	}
}

void back_from_front() {
	int maxpix = 0;
	for (int i = 0; i < 64000; i++) {
		if (pixmod[i] > maxpix) {
			maxpix = pixmod[i];
		}
	}
	printf("Max offset value: %d\n", maxpix);

	int* values = malloc((maxpix + 1) * sizeof(int));
	for (int v = 0; v <= maxpix ; v++) {
		values[v] = v;
	}

	for (int i = 0; i < 64000; i++) {
		pixback[i] = values[pixmod[i]];
		if (pixmod[i] > 0) {
			int c = values[pixmod[i]];
			for (int v = pixmod[i] - 1; v >= 0; v--) {
				values[v + 1] = values[v];
			}
			values[0] = c;
		}
	}
}

void generate_huffman_table(unsigned int* input, unsigned int size) {
	unsigned int distinct_symbols = 0;
	for (unsigned int i = 0; i < size; i++) {
		if (input[i] > 0) {
			distinct_symbols++;
		}
	}

	unsigned int symbol_bits = 0;
	while(size > 1U << symbol_bits) {
		symbol_bits++;
	}

	unsigned int node_bits = 0;
	while(distinct_symbols > 1U << node_bits) {
		node_bits++;
	}

	printf("Generate Huffman table with %u symbols (%u distinct)\n", size, distinct_symbols);

/* These numbers are wrong
	printf("Dense Huffman table will have %u entries of %u bits each (%u total)\n",
				distinct_symbols - 1, symbol_bits + 1,
				(distinct_symbols - 1) * (symbol_bits + 1));

	printf("Sparse Huffman table will have %u entries of %u bits each,\n",
				distinct_symbols - 1, node_bits + 1);
	printf("  plus a dictionary of %u entries of %u bits each (%u total)\n",
				distinct_symbols, symbol_bits,
				(distinct_symbols - 1) * (node_bits + 1) + distinct_symbols * symbol_bits);
*/

	unsigned int* values = calloc(distinct_symbols, sizeof(unsigned int));
	unsigned int* weights = calloc(distinct_symbols, sizeof(unsigned int));

	unsigned int w = 0;
	for (int i = 0; i < size; i++) {
		if (input[i] > 0) {
			values[w] = i;
			weights[w] = input[i];
			w++;
		}
	}

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
	for (int i = 0; i < distinct_symbols; i++) {
		printf("Initial weight %u value %u\n", weights[i], values[i]);
	}

	printf("initializing Huffman table with %u entries\n", 2 * (distinct_symbols - 1));
	unsigned int* huffman = calloc(2 * (distinct_symbols - 1), sizeof(unsigned int));
	unsigned int next_node = size + distinct_symbols - 2;

	for (unsigned int j = 0; j < distinct_symbols - 1; j++) {
		printf("Creating node %u from %u and %u\n", size + distinct_symbols - j - 2, values[j], values[j + 1]);
		huffman[2 * (distinct_symbols - 2 - j)] = values[j];
		huffman[2 * (distinct_symbols - 2 - j) + 1] = values[j + 1];
		values[j + 1] = size + distinct_symbols - j - 2;
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
		for (int i = j + 1; i < distinct_symbols; i++) {
			printf("weight %u value %u\n", weights[i], values[i]);
		}
	}

	free(values);
	free(weights);
	free(huffman);
}

unsigned int find_rle_runs(unsigned int* output,
			unsigned int* input,
			unsigned int size) {
	unsigned int read_offset = 0;
	unsigned int write_offset = 0;
	unsigned int current_value;
	unsigned int current_length;

	while (read_offset < size) {
		current_value = input[read_offset++];
		current_length = 1;
		while (read_offset < size && input[read_offset] == current_value) {
			read_offset++;
			current_length++;
		}
//		printf("found run, %u instances of symbol %u\n", current_length, current_value);
		output[write_offset++] = current_length;
		output[write_offset++] = current_value;
	}
//	printf("total runs: %u\n", write_offset / 2);
	return write_offset;
}

void process_rle_runs(unsigned int* input, unsigned int size) {
	unsigned int num_symbols = 0;
	for (unsigned int i = 1; i < size ; i+= 2) {
		if (input[i] > num_symbols) {
			num_symbols = input[i];
		}
	}
	num_symbols++;
//	printf("Num symbols %u\n", num_symbols);
	unsigned int* symbol_frequencies = calloc(num_symbols, sizeof(unsigned int));

	for (unsigned int i = 1; i < size ; i+= 2) {
		symbol_frequencies[input[i]]++;
	}

	generate_huffman_table(symbol_frequencies, num_symbols);
	free(symbol_frequencies);
}

void main() {
	FILE* inputfile = fopen("out/gfx/jbq.tga", "rb");
	fread(tga, 1, 192044, inputfile);
	fclose(inputfile);

	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			pix_xy[x][y] = (tgared(x, y) + tgagreen(x, y) + tgablue(x, y)) * 8 / 766;
		}
	}

	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			pix[x + 320 * y] = pix_xy[x][y];
		}
	}

	unsigned int * rle_output = malloc(64000 * sizeof(unsigned int));

	unsigned int num_runs = find_rle_runs(rle_output, pix, 64000);

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
