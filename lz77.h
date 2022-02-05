/*==========================================================================

lz77.h

Lempel, Ziv 1977 Encoding and Decoding

==========================================================================*/

#ifndef _LZ77_H
#define _lZ77_H

/* Error codes (more to come) */
#define LZERR_READ     1000
#define LZERR_WRITE    1001 
/* Bad file format */
#define LZERR_F_FORMAT 1002
#define LZERR_OREAD    1003
#define LZERR_OWRITE   1004
/* Reading a file that was written with different compression params */
#define LZERR_PARAMS   1005

/* Compression/decompression parameters */
/* Lookahead size and search buffer size, in bytes */
#define DEF_LA_SIZE 15 
#define DEF_SB_SIZE 1023
/* Number of bits needed to store an offset into the 
     LA and SB. These are the base-2 logs of the sizes. These are
     precomputed to save time; in this implementation, the LA and
     SB sizes cannot be changed at runtime. */
#define LA_BITS	4
#define SB_BITS 10
#define N 3

/* Magic number stored at the start of the compressed file, to help
     with idenfification */
#define MAGIC 0x4399

#ifdef CPM
int compress ();
int decompress ();
int pager_put ();
#else
/* compress() and decompress() return 0 on success, or one of the
   LZERR_ codes otherwise */ 
/* Compress the open FILE* in to the bit file out */
int compress (FILE *in, struct bitFILE *out);

/* Decompress to the open FILE* out from the bit file in */
int decompress (struct bitFILE *in, FILE *out);

/* Put a character to the display (only implemented in lz77cat) 
   Return 1 if the user wants stop (e.g., hit ctrl-c on CP/M) */
int pager_put (int c);
#endif

#endif
