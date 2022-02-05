/*==========================================================================

bitio.c

Implementation of a file that can be addressed in terms of specific
sequences of bits, rather than bytes. Used by lz77dec.c and lz77enc.c.

See bitio.h for details.
 
Original authors : David Costa and Pietro De Rosa
Modified for CP/M by Kevin Boone
  
==========================================================================*/

#include "compat.h"
#include <stdio.h>
#ifndef CPM
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#else
extern int errno;
#endif
#include "bitio.h"
#include "lz77.h"

/* Buffer size for bit file operations. */
#define BIO_BUFSIZ 4096

/*==========================================================================

struct bitFile
  
==========================================================================*/

struct bitFILE
  {
  FILE *file;     /* file to (from) write (read) */
  int mode;       /* the mode (READ or WRITE) */
  int bytepos;    /* actual byte's position in the buffer */
  int bitpos;     /* last bit's position in the byte */
  int read;       /* # of bytes read from the file and stored in buffer */
  unsigned char *buffer; 
  };

/*==========================================================================
bIO_feof
==========================================================================*/
int bIO_feof (bitF)
struct bitFILE *bitF;
  {
  if (feof (bitF->file) && bitF->bytepos == bitF->read)
    return 1;
  return 0;
  }

/*==========================================================================
bIO_ferror
==========================================================================*/
int bIO_ferror (bitF)
struct bitFILE *bitF;
  {
  return (ferror (bitF->file));
  }

/*==========================================================================
write_buffer
Flushes the buffer
==========================================================================*/
static void write_buffer (bitF)
struct bitFILE *bitF;
  {
  int ret;

  ret = fwrite (bitF->buffer, 1, bitF->bytepos, bitF->file);
  if (ret != bitF->bytepos)
     {
     /* TODO - signal a write error */
     return;
     }
  /* clear the buffer */
  bitF->bytepos = 0;
  bitF->bitpos = 0;
  _memset(bitF->buffer, 0, BIO_BUFSIZ);
  }

/*==========================================================================
read_buffer
Fills the buffer
==========================================================================*/
static void read_buffer (bitF)
struct bitFILE *bitF; 
  {
  /* read data */
  bitF->read = fread (bitF->buffer, 1, BIO_BUFSIZ, bitF->file);
  if(bitF->read < BIO_BUFSIZ && ferror (bitF->file))
     {
     /* TODO - signal error */
     return;
     }
  bitF->bytepos = 0;
  bitF->bitpos = 0;
  }

/*==========================================================================
bIO_open
TODO -- better error reporting
==========================================================================*/
struct bitFILE* bIO_open (path, mode)
CONST char *path;
int mode; 
  {
  struct bitFILE *bitF;

  /* errors handler */
  if(mode!=BIT_IO_W && mode!=BIT_IO_R)
    {
#ifdef DEBUG
    fputs ("bIO: invalid mode\n", stderr);
#endif
    return NULL;
    }
  if(path == NULL)
    {
#ifdef DEBUG
    fputs ("bIO: null path\n", stderr);
#endif
    return NULL;
    }

  /* initialize structure (calloc zeros memory) */
  bitF = (struct bitFILE*) calloc (1, sizeof (struct bitFILE));
  bitF->mode = mode;
  bitF->bytepos = 0;
  bitF->bitpos = 0;
  bitF->buffer = (unsigned char*) calloc (BIO_BUFSIZ, sizeof(unsigned char));
  _memset (bitF->buffer, 0, BIO_BUFSIZ);

  if(bitF->mode == BIT_IO_R)
    {
    if((bitF->file = fopen (path, "r")) == NULL)
      {
#ifdef DEBUG
      fputs ("bIO: fopen for read failed\n", stderr);
#endif
      errno = LZERR_OREAD;
      return NULL;
      }
            
    /* Fill the buffer for the first time */
    read_buffer(bitF);
    }
  else if((bitF->file = fopen (path, "w")) == NULL)
    {
#ifdef DEBUG
    fputs ("bIO: fopen for write failed\n", stderr);
#endif
    errno = LZERR_OWRITE;
    return NULL;
    }

  return bitF;
  }

/*==========================================================================
bIO_close
==========================================================================*/
void bIO_close (bitF)
struct bitFILE *bitF; 
  {
  if(bitF == NULL || bitF->file == NULL)
    {
#ifdef DEBUG
    fputs ("bIO: closing file that is already closed\n", stderr);
#endif
    return;
    }

  /* Write the unwritten bytes into the file */
  if(bitF->mode == BIT_IO_W)
    {
    if (bitF->bitpos > 0)
      (bitF->bytepos)++;
    write_buffer (bitF);
    }

  fclose(bitF->file);
  free(bitF->buffer);
  free(bitF);
  }

/*==========================================================================
bIO_write
==========================================================================*/
int bIO_write (bitF, info, nbit)
struct bitFILE *bitF; 
void *info; 
int nbit; 
  {
  int i;
  unsigned char *t;
  int byte_pos = 0, bit_pos = 0; 
  unsigned char mask; 

  if (bitF == NULL || bitF->file == NULL 
       || bitF->mode != BIT_IO_W || info == NULL || nbit < 0)
    return -1;

  for (i=0; i < nbit; i++)
    {
    /* Get bit to write */
    mask = 1 << bit_pos;
        
    t = (unsigned char *)info + byte_pos;

    if((*(unsigned char *)(t) & mask) != 0)
      bitF->buffer[bitF->bytepos] |= (1 << bitF->bitpos);
        
    /* update info to write variables */
    byte_pos = (bit_pos < 7) ? byte_pos : (byte_pos + 1);
    bit_pos = (bit_pos < 7) ? (bit_pos + 1) : 0;
        
    /* update bitF structure */
    bitF->bytepos = (bitF->bitpos < 7)? bitF->bytepos : (bitF->bytepos + 1);
    bitF->bitpos = (bitF->bitpos <7)? (bitF->bitpos + 1) : 0;
        
    /* Flush buffer to file if full */
    if (bitF->bytepos == BIO_BUFSIZ)
      write_buffer(bitF);
    /* Stop writing on error. TODO -- signal error properly */
    if(bIO_ferror(bitF) != 0)
      break;
    }

  return i;
  }

/*==========================================================================
bIO_read
==========================================================================*/
int bIO_read (bitF, info, info_s, nbit)
struct bitFILE *bitF; 
void *info; 
int info_s; 
int nbit; 
  {
  int i;
  unsigned char *t;
  int byte_pos = 0, bit_pos = 0; 
  unsigned char mask; 

  if (bitF == NULL || bitF->file == NULL || 
      bitF->mode != BIT_IO_R || info == NULL || info_s <= 0 || nbit < 0)
    return -1;

  /* clear the info buffer */
  _memset (info, 0, info_s);
    
  for (i = 0; i < nbit && (bIO_feof(bitF) != 1); i++)
    {
    mask = 1 << bitF->bitpos;
        
    /* If it is a one set it, otherwise do nothing */
    t = (unsigned char *)info + byte_pos;
    if ((bitF->buffer[bitF->bytepos] & mask) != 0)
      *(unsigned char *)(t) |= (1 << bit_pos);
        
    byte_pos = (bit_pos < 7) ? byte_pos : (byte_pos + 1);
    bit_pos = (bit_pos < 7) ? (bit_pos + 1) : 0;
        
    bitF->bytepos = (bitF->bitpos < 7) ? bitF->bytepos : (bitF->bytepos + 1);
    bitF->bitpos = (bitF->bitpos < 7) ? (bitF->bitpos + 1) : 0;

    if (bitF->bytepos == BIO_BUFSIZ)
      read_buffer (bitF);

    if(bIO_ferror (bitF) != 0)
      break;
    }
  return i;
  }

