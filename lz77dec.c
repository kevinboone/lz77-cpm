/*==========================================================================

lz77.c

Lempel, Ziv 1977 Encoding and Decoding
 
Original authors : David Costa and Pietro De Rosa
Modified for CP/M by Kevin Boone
  
This file contains the decompression algorithm.
 
LZ77 is a lossless data compression algorithms published by Abraham Lempel and
Jacob Ziv in 1977. It is a dictionary coder that maintains a sliding window
during compression. The sliding window is divided in two parts: the search
buffer (ie., the dictionary, which is compressed data) and the lookahead,
which is uncompressed data.
 
The LZ77 algorithm achieves compression by addressing byte sequences from the
dictionary instead of the original data. All data will be coded in the same
form (called a "token"): the address of the compressed content, the sequence
length, and the first  deviating byte.  The window is contained in a fixed
size buffer.

For example, the DEF_LA_SIZE=15 and DEF_SB_SIZE=4095, we need
four bits for the length, and one byte for the next deviating byte.
So the token looks like this:

0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         offset        |lenght |   next char   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

The match between the search buffer and the lookahead is made by a binary
tree, implemented in tree.c 

Because the tokens need to be stored with as little space as possible, they
are written and read in a bit-addressable file, implemented in bitio.c
   
The original (Costa/De Rosa) inplementations of the functions in this module
make use of C structs, and the ability to pass structs in an out of functions
by block copying.  So far as I know, no C compiler for CP/M supports this
feature and, anyway, the extra indirect addressing adds considerably to the
execution time. I've essentially optimized away everything that made this code
comprehensible, to make it (just) fast enough to work on a 4MHz Z80. 

==========================================================================*/

#include "compat.h" 
#include <stdio.h>
#ifndef CPM
#include <stdlib.h>
#include <string.h>
#endif
#include "bitio.h"
#include "tree.h"
#include "lz77.h"

/* Precompute the size of the token, which would otherwise get 
   calulcated thousands of times. */
static int TOKBITS = SB_BITS + LA_BITS + 8;

/*==========================================================================
readcode 
read the token from the compressed file, into *off, *len, *next
==========================================================================*/
static void readcode (file, off, len, next)
struct bitFILE *file; int *off; int *len; char *next;
  {
  int _off, _len;
  char _next;

  int ret = 0;
  ret += bIO_read (file, &_off, sizeof (_off), SB_BITS);
  ret += bIO_read (file, &_len, sizeof (_len), LA_BITS);
  ret += bIO_read (file, &_next, sizeof (_next), 8);

  /* check for EOF or ERR */	
  if (ret < TOKBITS)
    {
    /* TOOD handle this error more elegantly */
    if (bIO_ferror (file) != 0)
      {
      _perror ("Error reading bits.\n");
      exit(-1);
      }
    /* EOF */
    _off = -1;
    }

  *off = _off;
  *len = _len;
  *next = _next;
  }

/*==========================================================================
decompress
==========================================================================*/
int decompress (file, out)
struct bitFILE *file; 
FILE *out; 
  {
  int _off, _len;
  char _next;
  int back = 0, off;
  unsigned char *buffer;
  int sb_size, la_size, window_size;
  int ws_minus_1;
  int magic = 0;
  BOOL stop = FALSE;
  
  bIO_read (file, &magic, sizeof(sb_size), 16);
  /* Check magic number in file */

  if (magic != MAGIC) return LZERR_F_FORMAT;

  /* read header */
  bIO_read (file, &sb_size, sizeof(sb_size), 16);
  bIO_read (file, &la_size, sizeof(la_size), 16);

  if (sb_size != DEF_SB_SIZE || la_size != DEF_LA_SIZE)
    return LZERR_PARAMS;
  
  window_size = (sb_size * N) + la_size;
  ws_minus_1 = window_size - 1;
  
  buffer = (unsigned char*)calloc (window_size, sizeof (unsigned char));
  
  while (!stop)
    {
    /* Read the code from the input file */
    readcode (file, &_off, &_len, &_next);

    if (_off == -1)
      break;
      
    if (back + _len > ws_minus_1)
      {
      _memcpy (buffer, &(buffer [back - DEF_SB_SIZE]), DEF_SB_SIZE);
      back = DEF_SB_SIZE;
      }
      
    /* reconstruct the original byte*/
    while (_len > 0)
      {
      char c;
      off = back - _off;
      c = buffer[off];
      buffer[back] = c;
	  
      /* write the byte in the output file*/
      if (out)
        putc (c, out);
      else
        stop = pager_put (c);
	  
      back++;
      _len--;
      }
    buffer[back] = _next;
      
    if (out)
      putc (buffer[back], out);
    else
      stop = pager_put (_next);
      
    back++;
    }
  free (buffer);
  return 0;
  }

