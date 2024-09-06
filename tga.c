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

unsigned int* read_tga() {
	unsigned char* tga = malloc(192044);
	FILE* inputfile = fopen("out/gfx/jbq.tga", "rb");
	fread(tga, 1, 192044, inputfile);
	fclose(inputfile);

	unsigned int* pixels = malloc(64000 * sizeof(unsigned int));

	for (int i = 0; i < 64000; i++) {
		pixels[i] = (5 * tga[18 + 3 * i + 2] + 9 * tga[18 + 3 * i + 1] + 2 * tga[18 + 3 * i]) / 512;
	}

	return pixels;
}
