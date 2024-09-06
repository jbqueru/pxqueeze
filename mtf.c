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

unsigned int pixmod[64000];

unsigned int pixback[64000];

void move_to_front(unsigned int* pix) {
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
