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

void find_rle_runs(
	unsigned int ** const output_lengths,
	unsigned int ** const output_values,
	unsigned int * const output_size,
	unsigned int const * const input_data,
	unsigned int const input_size,
	unsigned int const max_run_length);

void process_rle_runs(
	unsigned int * rle_lengths,
	unsigned int * rle_values,
	unsigned int size);
