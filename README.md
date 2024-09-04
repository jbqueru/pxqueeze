# pxqueeze

A research project, investigating compression approaches
tuned specifically for retrocomputing graphics.

```
Copyright 2024 Jean-Baptiste "JBQ" "Djaybee" Queru

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```

## Rationale

### What for?

When writing games or demos for 8-bit or 16-bit computers, handling
of graphics rapidly runs into memory constraints.
There usually isn't much RAM available, which is often gobbled up
by high-performance graphics techniques.

On most 8-bit machines, mass storage isn't an option in-flight,
since such storage tends to be tape-based, with low transfer rates,
and which routinely cannot be used in the background.

For games on 16-bit machines, floppies can be relied on between
levels, but loading from a floppy during a level can cause trouble
and is best avoided, such that whole levels are still expected to
fit within RAM.
For demos on 16-bit machines, the technical expectations in megademos
imply that the whole screen fits in RAM and exclude access to the
floppy drive.

Consoles of similar vintages have dedicated graphics capabilities
that tend to require fewer high-performance graphics techniques.
However, they are routinely limited by ROM sizes as well.
For consoles that load graphics into RAM before displaying them
(e.g. Sega's), compression also brings value.

The goal here is to significantly compress graphics so that more of
them can be used from a given game or demo.

### Why?

There exist already a variety of compression utilities. However,
those tend to be unsuitable for one reason or another. Such reasons
might include:
- complex formats that are impractical on vintage machines (e.g.
by requiring lot of RAM or advanced math).
- formats that aren't documented enough to enable an independent
re-implementation.
- formats that are too byte-oriented, which misses compression
opportunities for pixel data that doesn't neatly line up with
bytes.
- formats that were designed a long time ago and aimed for
compression to be practical on machines as they existed at the
time of their creation, and which ignore compression opportunities
that are now practical on more advanced machines.

Building a compression system from scratch allows to better aim
for a balance between constraints:
- Using the power of advanced desktop PCs as of 2024 while keeping
decompression practical on vintage hardware.
- Controlling the entire data format to avoid corner cases that
might arise when writing a decompression routine for an existing
format.
- Tuning compression for the specifics of vintage graphics formats.

### What?

If this project is successful, it will likely contain two sides:
- A linux-based compressor program, or a set of programs, with the
ability to explore various compression parameters and to compress
based on pre-set parameters that might have been pre-determined.
- Decompression routines for various processors and graphics formats.
Possible candidates might include the following families:
  + Motorola 68000
    * Atari ST (initial candidate)
    * Commodore Amiga
    * Sega Mega Drive (a.k.a. Genesis)
  + Zilog Z80
    * Sinclair ZX Spectrum
    * Sega SG-1000
    * MSX
    * Amstrad CPC (likely second candidate)
    * Amstrad PCW
    * Sega Master System (a.k.a. Mk III)
  + MOS 6502
    * Apple II
    * Atari 800
    * BBC Micro
    * Commodore 64
    * Atari Lynx

### How?

Implementation-wise, the compressor is written in C, while the production
decompressors are written in assembly.

Technology-wise, the following are on the table during initial planning:
- Pixel unpacking
- Pixel re-ordering (e.g. Hilbert curve)
- Delta encoding
- Move-to-Front transform
- Burrows-Wheeler transform
- Some LZ77 variant (e.g. LZSS-like)
- Entropy coding (starting with Huffman coding)

Intermediate steps should all rely on entropy coding existing at
the end, and therefore should not make assumptions about bit
formats: the later entropy coding will naturally determine the
most appropriate encoding. This also creates freedom to handle
transparency colors without storing a separate transparency mask,
or to add inline start markers in a Burrows-Wheeler transform.

### Who?

This is meant to be initially implemented by JBQ. Forks will
be welcome, as will contributions if/when the project takes off
and becomes useful to others.

### When?

JBQ intends to use this code as part of future demos, starting
with the ITTOP concept demo meant to be developed in late 2024
or early 2025.

# Credits

# (Un)important things

## Licensing

The code in this project is licensed under the terms of the
**[AGPL, version 3](https://www.gnu.org/licenses/agpl-3.0.en.html)**
or later.

For clarification, the author has no claims on the output of the
code in this project, whether through copyright, patents, trademarks,
or any other such rights.

## Privacy (including GDPR)

By itself, none of the code in this project processes any personal
data in any way. It does not collect, record, organize, structure,
store, adapt, alter, retrieve, consult, use, disclose, transmit,
disseminate, align, combine, restrict, erase, or destroy any
personal data.

By itself, none of the code in this project identifies natural
persons in any way, directly or indirectly. It does not reference
any name, identification number, location data, online
identifier, or any factors related to the physical, psychological,
genetic, mental, economic, cultural or social identity of
any person.

By itself, none of the code in this project evaluates any aspect of
any natural person. It neither analyzes nor predicts performance
at work, economic situation, health, personal preferences,
interests, reliability, behavior, location, and movements.

Should the code in this project be integrated in a larger product
such that the code in this project processes personal data, the
responsibility for that data lies with the integrator of the
larger product.

## Security (including CRA)

The code in this project is meant to be used in the process of
creating demos and/or games for old computers and/or video game
consoles, with the understanding that the security features on
such hardware and the bundled software, if any, are typically
grossly inappropriate by modern standards.
The code in this project should be considered a research project
within that context.
In other words: **don't even think of using this code in a way
that faces the Internet and/or processes arbitrary user data,
that's not what it's meant for.**

None of the code in this project involves any direct or indirect
logical or physical data connection to a device or network.

Also, all of the code in this project is provided under a free
and Open Source license, in a non-commercial manner. It is
developed, maintained, and distributed openly. As of September
2024, no price has been charged for any of the code in this
project, nor have any donations been accepted in connection
with this project. The author has no intention of charging a
price for this code. They also do not intend to accept donations,
but acknowledge that, in extreme situations, donations of
hardware or of access to hardware might facilitate development,
without any intent to make a profit.
