# lz77 for CP/M and Linux

Version 0.1a, February 2022

## What is this?

`lz77` is a file compressor and decompressor that uses the 
Lempel-Ziv 1977 algorithm. It offers only modest compression efficiency
(typically about 50% on plain text), but at least the decompression
is fast enough to be (just about) tractable on a Z80-based CP/M 
system. If built as described, the Linux version and the CP/M version
will be inter-operable (to some extent -- see below).

`lz77cat` uncompresses a file directly to the console, with 
a rudimentary paging facility.

The programs are implemented using Kernighan and Ritchie C, with
a bit of 8080 assembly language, and
capable of being compiled/assembled on a Z80-based CP/M machine using
the Manx Aztec C compiler. This is not a rapid process however and,
in practice, I build the CP/M version on Linux. 

The implementation of the LZ77 algorithm is heavily based on an original
version by David Costa and Pietro De Rosa, which is available here:

https://github.com/cstdvd/lz77

However, I've had to modify it considerably to make it work
with reasonable speed on a CP/M machine. As a result, my version
of the C implementation is much less self-explanatory than the
original. In addition, my implementation has the compression
paramters hard-coded. This is simply to reduce the number of
memory indirections required to run the algorithms -- I have
replaced variables with constants, some of which can be pre-computed.
All the configuration paramters are in `lm77.h`.


## Building for CP/M on Linux

The build system is designed to be used with Parag Patel's Z80 emulator,
a CP/M-ready version of which is available here:

https://github.com/jhallen/cpm

The CP/M version of the Aztec C compiler can be obtained here:

https://www.aztecmuseum.ca/compilers.htm#cpm 

It's free of charge but not, so far as I know, open source.  Only version 1.6d
is known to work. Download this version and edit `prepare_cpm.sh` to indicate
the location of the zipfile. The scripts provided for building for
CP/M are as follows. 

`prepare_cpm.sh`

Unpack the necessary parts of the Aztec compiler; check the source
for the location of the download zipfile.

`unprepare_cpm.sh`

Remove the files installed by `prepare_cpm.sh`

`make_cpm.sh`

Build `lz77cat.com` and `lz77.com` using the CP/M emulator and
Aztec C

`clean_cmp.sh`

Remove all files built by `make_cpm.sh`

## Building for Linux on Linux

The Linux version is designed to be compiled using GCC on Linux.
If built on a little-endian machine (e.g., x86) it will be inter-operable
with the CP/M version. That is, files compressed on one platform will
uncompress on the other. However, although the implementation can
cope with the different data sizes of different platforms, at present
it can't cope with different endian-ness. 

TO build for Linux, it should only be necessary to run `make`.

## Usage

The Linux and CP/M versions both use a CP/M-style command line.
To compress `foo.txt` to `foo.lz7`

    lz77 /C /I foo.txt /O foo.lz7

To decompress:

    lz77 /D /I foo.lz7 /O foo.txt

To view the compressed file on the console:

   lz77cat foo.lz7

Note that the command-line switches must be upper-case on Linux.
On CP/M, the command-line is always presented to the program
in upper-case.

## Limitations

The biggest limitation, of course, is speed. Compressing on CP/M
is painfully slow. Decompressing is slow, too, but not so bad.
For compressing, thought, you'll certainly need to take a coffee
break, if not a sleep.

CP/M has no notion of file size, other than the number of records
occupied on disk. If you compress a file on Linux, copy it to
a CP/M disk (somehow) and uncompress it, you could end up with
junk at the end of the file. This doesn't happen when compressing
with CP/M, because CP/M will automatically pad the non-full disk
block with ctrl-z characters. These will be restored when 
uncompressing. The Linux version doesn't do this, although it
could be modified to do so. The solution to this problem is to
copy files to CP/M using a tool that will round up the file size
to a whole number of records, and insert the padding characters. 

## Memory required on CP/M for compression

The memory requirements for the compression algorithm 
are very approximately

    3 * SB_SIZE + 10 * LA_SIZE

There will also be file management overheads:
1 kB for the bit-addressable file buffer, plus 1 kB for the 
C library's file buffer. With default parameters, this comes to about
6 kB. The program itself takes about 18 kB. With these parameters,
therefore, the program should run in 32 kB RAM (just). With more
memory there is some mileage in increasing the lookahead size and
search buffer size -- it's possible to improve the compression
ration. However, increasing these settings will make both compression
and decompression slower.  

Decompression only requires about 3kB.

