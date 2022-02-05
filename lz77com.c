/*==========================================================================

lz77.c

Lempel, Ziv 1977 Encoding and Decoding
 
Original authors : David Costa and Pietro De Rosa
Modified for CP/M by Kevin Boone
  
This file contains the compression algorithm.

See lz77com.c for an outline of the algorithm.
 
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


/*==========================================================================
struct token
Each token is composed of a backward offset, the length of the match,
and the next character in the lookahead.
Offset is in the range 0 - SB_SIZE, length in the range 0 - LA_SIZE
==========================================================================*/
struct token
  {
  int off, len;
  char next;
  };

/*==========================================================================
match
find the longest match and create a token
Parameters   : tree - binary search tree
               root - index of the root
               window - pointer to the buffer
               la - starting index of the lookahead
               la_size - actual lookahead size
The token parameters are returned in *off, *len, and *next
==========================================================================*/
static void match (tree, root, window, la, la_size, off, len, next)
struct node *tree; int root; unsigned char *window; 
int la; int la_size; int *off; int *len; char *next;
  {
  struct token t;
  int _off, _len;
 
  /* find the longest match */
  tr_find (tree, root, window, la, la_size, &_off, &_len);
  
  /* create the token */
  t.off = _off;
  t.len = _len;
  t.next = window [la + _len];

  *len = t.len;
  *off = t.off;
  *next = t.next;
  }

/*==========================================================================
 writecode
 write the token to the output file
==========================================================================*/
static void writecode(t, out, la_size, sb_size)
struct token *t; struct bitFILE *out; int la_size; int sb_size;
{

    bIO_write(out, &t->off, SB_BITS);
    bIO_write(out, &t->len, LA_BITS);
    bIO_write(out, &t->next, 8);
}

/*==========================================================================
compress 
==========================================================================*/
int compress (file, out)
FILE *file; 
struct bitFILE *out;
  {
  int i, root = -1;
  int eof;
  struct node *tree;
  struct token t;
  unsigned char *window;
  int la_size, sb_size = 0;    /* actual lookahead and search buffer size */
  int buff_size;
  int sb_index = 0, la_index = 0;
  int window_size;
  int orig_la_size, orig_sb_size;
  int magic = MAGIC;
  
  /* Set window parameters */
  /* (KB) Note that we have to pass these things as parameters to just about
     every function when encoding -- although this is slow -- because 
     the file won't be an integer number of lookahead blocks. */
  orig_la_size = DEF_LA_SIZE;
  orig_sb_size = DEF_SB_SIZE; 
  window_size = (orig_sb_size * N) + orig_la_size;
  
  window = calloc (window_size, sizeof (unsigned char));
  
  tree = tr_create (orig_sb_size);

  /* Write header */
  bIO_write (out, &magic, 16);

  /* Note that we only need to store the bottom 16 bits, since the
      parameters are small numbers. On CP/M, of course, 16 bits will
      be the whole integer. */
  bIO_write (out, &orig_sb_size, 16);
  bIO_write (out, &orig_la_size, 16);
  
  /* fill the lookahead with the first window_size bytes,  or until EOF */
  buff_size = fread (window, 1, window_size, file);
  if (ferror (file)) 
    return LZERR_READ;

  eof = feof (file); /* We could be at EOF already */
  
  /* If we have less in the buffer than the default lookahead size,
       reduce the lookahead size */
  la_size = (buff_size > orig_la_size) ? orig_la_size : buff_size;
  
  while (buff_size > 0)
    {
    /* Find the longest match of the lookahead in the tree*/
    match (tree, root, window, la_index, la_size, &t.off, &t.len, &t.next);
    
    /* Write the token in the output file */
    /* TODO -- this can fail; handle errors */
    writecode (&t, out, orig_la_size, orig_sb_size);
    
    /* read as many bytes as matched in the previuos iteration */
    for (i = 0; i < t.len + 1; i++)
      {
      /* If search buffer's length is max, the oldest node is 
           removed from the tree */
      if (sb_size == orig_sb_size)
        {
	tr_delete (tree, &root, window, sb_index, orig_sb_size);
	sb_index++;
	} 
      else
	sb_size++;
	
      /* Insert a new node in the tree */
      tr_insert (tree, &root, window, la_index, la_size, orig_sb_size);
      la_index++;
	
      if (eof == 0)
        {
	/* scroll backward the buffer when it is almost full */
	if (sb_index == orig_sb_size * (N - 1))
          {
          _memmove (window, &(window[sb_index]), sb_size+la_size);
		
	  /* update the node's offset when the buffer is scrolled */
	  tr_update (tree, sb_index, orig_sb_size);
		
          sb_index = 0;
          la_index = sb_size;
		
          /* read from file */
          buff_size += fread (&(window [sb_size + la_size]), 1, 
              window_size - (sb_size + la_size), file);
          if (ferror(file)) 
            return LZERR_READ;
	  eof = feof(file);
	  }
	}
	
      buff_size--;
      /* case where we hit EOF before filling lookahead */
      la_size = (buff_size > orig_la_size) ? orig_la_size: buff_size;
      }
    }
  
  tr_destroy (tree);
  free (window);
  return 0;
  }


