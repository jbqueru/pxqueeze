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

As a program about compression, a lot of credits go to a lot of
people, such that this list can't be exhaustive.
- Claude Shannon, who founded information theory in 1948.
- Robert Fano, for his work (with Shannon) on Shannon-Fano coding,
two early approaches for entropy coding from 1948-49.
- David Huffman, for inventing Huffman coding in 1951 while
studying under Fano.
- Abraham Lempel and Jacob Ziv, for publishing LZ77 and LZ78
in 1977-78, which are dictionary compressors at the foundation
of almost all lossless compression techniques to this day.
- Jim Storer and Thomas Szymanski, for showing in 1982 with
LZSS that LZ77 is not alone in the family of sliding-window
dictionary compressors.
- Boris Ryabko for the Move-to-Front algorithm in 1980.
- A variety of people in the late 1980s and early 1990s, for
combining LZ compression with entropy coding, into a variety
of formats.
- Phil Katz, for creating Deflate around 1993, which is the gold
standard of compression, with a few interesting tricks around
Huffman tables.
- Michael Burrows and David Wheeler for creating the eponymous
Burrows-Wheeler transform in 1994 while at DEC SRC.
- Julian Seward for creating bzip2 in 1996, which doesn't use
any LZ stage. Bzip2 shows both the importance of bijective
pre-processing to surface redundancies and repetitions in the
data, and of post-processing of Huffman tables to make them
smaller.
- Igor Pavlov, for some creative approaches around LZ77 offset /
length pairs in LZMA in 1996-98, including the observation that
an LZ matches that overflow the maximum length will continue into
another match with the same offset.
- Lode Vandevenne and Jyrki Alakuijala for creating Zopfli in
2013 while at Google, showing the possibility of spending a lot
of CPU power can improve compression ratios without having to
change actual file formats.

# Research direction

## Pixel preprocessing

### Unpacking

Pixels initially need to be extracted into individual symbols,
because pixels in typical file formats are packed into groups
that can vary from 2 pixels each (Amstrad mode 0) all the way
to at least 16 pixels each (Atari ST).

The working theory here is that having pixels as individual
symbols allows to better process repeating patterns, by allowing
sub-byte alignment of duplicate patterns.

On the other hand, extracting pixels as individual symbols
creates a lot more symbols, literally 8x as many as there
are bytes for a monochrome image. It'll be interesting to
investigate that balance by trying to group pixels such
that a symbol would contain 2 pixels.

### Ordering

Pixels can be re-ordered from memory order to screen order,
for screens where pixels aren't stored linearly (which is
the case e.g. on Sinclair ZX Spectrum or Amstrad CPC).

Furthermore, changing the orders of the pixels (e.g. by
organizing them along a Peano curve or a Hilbert curve)
might keep pixels close in compression order that are close
on the screen, which could help find repetitions that are
closer in compression order, which can be helpful for MTF
algorithms, or with a BWT with small words, or in an LZ
stage with a small sliding window, or when switching between
Huffman tables.

## Data conditioning

While most compressors tend to focus primarily on handling
the direct input stream with a dictionary compressors
followed by entropy coding, bzip2 showed that bijective
conditioning can be very effective. In the case of bzip2,
a Burrows-Wheeler transform followed by a move-to-front
transform eliminates the need for an LZ stage, replacing
it with a plain RLE.

There's room to investigate which types of preprocessing
can produce the best results, and whether the types of
preprocessing need to be specified dynamically.

One type of preprocessing would be delta coding, which
would turn gradients into repeating patters.

The Burrows-Wheeler transform is another such candidate,
it does a great job at transforming repeating patterns
into groups of identical symbols. Because it operates on
blocks, the size of blocks is potentially a parameter to
be tweaked, which might create tradeoffs between ratio
and speed. Whether using variable-size blocks can provide
an advantage over fixed blocks remains to be seen.

Finally, a move-to-front transform also changes the
symbol space in a way that can create a lot of repetitions.
The output of a move-to-front transform tends to have a
compact set of symbols, which can be used in later
stages if it is guaranteed to exist. As an example,
it can later help create canonical Huffman codes.

While delta coding and Burrows-Wheeler transforms are
both bijective, they both involve arithmetic operations
between symbols, and they can therefore be affected by
the order of the input palette, in a way that has an
impact on later stages. This might be more true if they
are used together, though that remains to be verified.

## Repetition elimination

Starting with LZ77, a variety of approaches have been used
to eliminate repetitions through a sliding window.

In plain LZ77, there's a single type of block, which contains
distance and length for a repetition, along with one symbol to
be inserted after the repetition. We can note that, when
length = 0, the distance isn't needed, such that the block
only contains a single literal symbol.

LZSS uses two types of blocks, one for repetitions and one
for literals, where a block of literals can contain multiple
symbols.

Deflate's LZ stage uses two block types, one for repetitions
and one for single symbols. It's more flexible than LZ77 by
being able to code multiple repetitions in a row and multiple
literals in a row, but doesn't code the case of multiple
literals the same way LZSS would.

We can note that RLE (depending on implementations) would look
like LZSS or deflate's LZ stage where the sliding window is
limited to a single byte (i.e. where there's only 1 possible
offset).

In addition to the types from deflate's LZ stage, LZMA's LZ
stage also has blocks that re-use the most recent match
distances: a short repetition copies a single byte from the
most recent distance, while a long repetition can choose
from the 4 most recent matches.

## Entropy coding

Because the last stage is always going to be a form of entropy
coding, earlier stages can count on that fact. They can use
a number of symbols that is not a power of two without worrying
about bit waste, and they can count on multiple pieces of
information getting combined together.

As an example, deflate's LZ stage uses Huffman coding with
a dictionary that contains both literals and repetition lengths,
which is different from the LZSS approach where an explicit
bit distinguishes between literal blocks and repeated blocks.
Deflate's approach better adapts to situations where there's
an imbalance between literals and repeated blocks, where
there's an imbalance between literals, and any combination of
those two.

Huffman coding is quite an obvious starting choice here.

When the set of symbols is compact enough, canonical Huffman
coding might be able to shrink the Huffman tables, under the
right circumstances. Tables of pixel symbols will be compact
enough but not necessarily large enough to justify the work,
while tables of LZ distances are unlikely to be compact
enough. Under appropriate restrictions, tables of LZ lengths
migth qualify.

There might be situations where Huffman might be able to encode
pairs of symbols (or longer sequences if appropriate).
This might bring it closer to the efficiency of arithmetic
coding without incurring the whole cost of non-binary codes.
It should also be able to deal with conditional probabilities,
e.g. LZSS with no limits of the length of literal blocks
will never contain two literal blocks back-to-back, while
LZSS with a limit on that length will only have back-to-back
literal blocks if the length of the first block is the
maximum length.

The overhead of adaptive Huffman coding doesn't seem to be
justified for data that is entirely known at compression
time (i.e. not streaming).

Arithmetic coding, while mathematically optimal, might be
difficult to deploy on older processors, especially 8-bit
processors that lack instructions to multiply or divide.

There also exist a variety of codes that, unlike Huffman,
don't rely on a table. Golomb, exponential Golomb, Elias
delta, Elias gamma, Elias omega, Levenshtein, Even-Rodeh...
Such codes might happen to be smaller than Huffman on
small enough data sets. Those are all tuned for situations
where smaller symbols are more frequent than large ones.

As a note, while fixed Huffman tables do make sense for
cases like that of deflate where the decompressor and
the compressed data are sent separately, they do not help
when the decompressor is sent with the data.

## Streaming

While decoding, some stages can be streamed better than
others, and some stages are more stateful than others.

Entropy codes are stateless and can be decoded in a purely
streamed fashion.

While LZ codes can be streamed, they are significantly
stateful since they require to preserve a copy of the
output's sliding window, which is an issue when that copy
is later post-processed.

A move-to-front transform can easily be streamed while
decoding, but it keeps the ordered symbol list as a state.

The Burrows-Wheeler transform is block oriented, which
means that it is not purely streamable, and it also carries
a state while a block is getting filled or emptied.

Delta coding is easily stremable and carries very little
state.

Palette remapping is streamable and stateless.

Re-ordering the pixels (e.g. along a Hilbert or Peano curve,
and/or in memory order) can be streamed and doesn't carry
much state.

## Sharing data across images

There might be opportunities to share Huffman tables between
images. Also, pre-initializing a sliding window for the
LZ stage could provide significant decompression benefits.

## Tunable parameters

The following parameters are likely tunable, subject to research:
-Whether the pixels are left in framebuffer order, linearized,
or re-ordered along a Hilbert or Peano curve.
-Re-ordering the palette (an exhaustive search might be practical
up to 3 or 4 colors, palettes beyond that are impractical and
would require some heuristics about which operations to try).
-Any combination of pre-processing stages (delta, BWT, MTF).
-The block size of BWT, and whether a variable block size can
be beneficial.
-The set of LZ block types available.
-The choices of LZ blocks when multiple choices are available.
This indirectly includes restrictions on block sizes and size
of the sliding window.
-The choice of entropy coding.

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
