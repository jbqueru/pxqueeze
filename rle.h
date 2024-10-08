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

#ifndef __RLE_H__
#define __RLE_H__

void rle_find_runs(
	unsigned int const ** const outLengthP,
	unsigned int const ** const outSymbolP,
	unsigned int * const outSize,
	unsigned int const * const inData,
	unsigned int const inSize,
	unsigned int const inMaxRunLength);

void rle_flat_table(
	char const ** const outBitStreamP,
	unsigned int const * const inLengthP,
	unsigned int const * const inSymbolP,
	unsigned int const inSize);

/*
 * One Huffman table for lengths, one for values, that's it.
 */
void rle_naive_process_runs(
	unsigned int const * const rle_lengths,
	unsigned int const * const rle_values,
	unsigned int const size);

#endif
