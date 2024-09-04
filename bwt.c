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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int comparein(void const* v1, void const* v2, void* c) {
	return memcmp(*(int*const*)v1, *(int*const*)v2, *(int*)c * sizeof(int));
}

struct offsetvalue {
	int offset;
	int value;
};

int compareov(void const* v1, void const* v2, void* c) {
	struct offsetvalue const* ov1 = (struct offsetvalue const*)v1;
	struct offsetvalue const* ov2 = (struct offsetvalue const*)v2;
	if (ov1 -> value < ov2 -> value) {
		return -1;
	}
	if (ov1 -> value > ov2 -> value) {
		return 1;
	}
	if (ov1 -> offset < ov2 -> offset) {
		return -1;
	}
	if (ov1 -> offset > ov2 -> offset) {
		return 1;
	}
	return 0;
}

void main() {
	unsigned char* input = "ABRACADABRA";
	int inputsize = strlen(input);
	int outputsize = inputsize + 1;

	printf("Input string: %s\n\n", input);

	int* rotations = malloc(2 * outputsize * sizeof(int));
	rotations[outputsize - 1] = rotations[2 * outputsize - 1] = '~';
	for (int i = 0; i < inputsize ; i++) {
		rotations[i] = rotations[outputsize + i] = input[inputsize - 1 - i];
	}

	printf("Prepared rotations buffer: ");
	for (int i = 0; i < 2 * outputsize; i++) {
		printf("%c", rotations[i]);
	}
	printf("\n\n");

	int** sortedrotations = malloc(outputsize * sizeof(int*));
	for (int i = 0; i < outputsize ; i++) {
		sortedrotations[i] = rotations + i;
	}

	printf("Unsorted rotations:\n");
	for (int j = 0; j < outputsize; j++) {
		for (int i = 0; i < outputsize ; i++) {
			printf("%c", sortedrotations[j][i]);
		}
		printf("\n");
	}
	printf("\n");

	qsort_r(sortedrotations, outputsize, sizeof(int*), comparein, &outputsize);

	printf("Sorted rotations:\n");
	for (int j = 0; j < outputsize; j++) {
		for (int i = 0; i < outputsize ; i++) {
			printf("%c", sortedrotations[j][i]);
		}
		printf("\n");
	}
	printf("\n");

	int* output = malloc(outputsize * sizeof(int));
	for(int i = 0; i < outputsize ; i++) {
		output[i] = sortedrotations[i][outputsize - 1];
	}

	printf("Transformed string: ");
	for(int i = 0; i < outputsize ; i++) {
		printf("%c", output[i]);
	}
	printf("\n\n");

	struct offsetvalue * offsetvalues = malloc(outputsize * sizeof(struct offsetvalue));

	for (int i = 0; i < outputsize ; i++) {
		offsetvalues[i].offset = i;
		offsetvalues[i].value = output[i];
	}

	printf("Operations on output matrix, without sorting:\n");
	for (int i = 0; i < outputsize ; i++) {
		printf("Row %d gets %c prepended\n", offsetvalues[i].offset, offsetvalues[i].value);
	}
	printf("\n");

	qsort_r(offsetvalues, outputsize, sizeof(struct offsetvalue), compareov, 0);

	printf("Operations on output matrix, with sorting, in destination order:\n");
	for (int i = 0; i < outputsize ; i++) {
		printf("New row %d comes from %d and gets %c prepended\n", i, offsetvalues[i].offset, offsetvalues[i].value);
	}
	printf("\n");

	int* inverseoffsets = malloc(outputsize * sizeof(int));

	for (int i = 0; i < outputsize ; i++) {
		inverseoffsets[offsetvalues[i].offset] = i;
	}

	printf("Operations on output matrix, with sorting, in source order:\n");
	for (int i = 0; i < outputsize ; i++) {
		printf("Row %d goes to %d and gets %c prepended\n",
					i, inverseoffsets[i], offsetvalues[inverseoffsets[i]].value);
	}
	printf("\n");

	printf("Recovered string: ");
	int targetrotation = outputsize - 1;
	for (int i = 0; i < outputsize - 1; i++) {
		printf("%c", output[targetrotation]);
		targetrotation = inverseoffsets[targetrotation];
	}
	printf("\n\n");
}
