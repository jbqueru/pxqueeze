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
	unsigned int const ** const output_lengths,
	unsigned int const ** const output_values,
	unsigned int * const output_size,
	unsigned int const * const input_data,
	unsigned int const input_size,
	unsigned int const max_run_length);

void rle_process_runs(
	unsigned int const * const rle_lengths,
	unsigned int const * const rle_values,
	unsigned int const size);

#endif
