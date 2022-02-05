/*==========================================================================
  
  lz77 compress/decompress utility for Linux and CP/M

  main.c

  This is the command-line interface for lz77(.com). This interface
  provides both compression and decompression features, but cannot
  write directly to the console.

  Kevin Boone, Feburary 2022
  
  Based on an original implementation by David Costa and Pietro De Rosa
  
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
#include "getopt.h"
#include "bitio.h"
#include "lz77.h"
#include "error.h"

/*==========================================================================

  pager_put 

  This is just a stub implementation because, although lz77 can call it
    in principle, the conditions to do so should never be met. There
    is a real implementation in lz77cat's maincat.c

==========================================================================*/
BOOL pager_put (c)
int c;
  {
  return FALSE;
  }

/*==========================================================================

  main

==========================================================================*/
int main (argc, argv)
int argc; char *argv[]; 
  {
  CONST int M_ENCODE = 1, M_DECODE = 0;
  int opt;
  int ret = 1;

  FILE *file = NULL;
  struct bitFILE *bit_file = NULL;

  int mode = -1;
  char *fn_in = NULL, *fn_out = NULL;
    
  while ((opt = _getopt(argc, argv, "CDI:O:H")) != -1)
    {
    switch(opt)
      {
      case 'C':       /* compression mode */
        mode = M_ENCODE;
        break;
                
      case 'D':       /* decompression mode */
        mode = M_DECODE;
        break;
                
      case 'I':       /* input file name */
        if (fn_in != NULL)
          {
          show_error ("Too many input files", 0);
          goto done;
          }
        fn_in = _strdup (optarg);
        break;
                
      case 'O':       /* output file name */
        if (fn_out != NULL)
          {
          show_error ("Too many output files", 0);
          goto done;
          }
        fn_out = _strdup (optarg);
        break;
                
      case 'H':   
        puts ("Usage: lz77 <options>");
        puts ("  /C : compress");
        puts ("  /D : decompress");
        puts ("  /I {file} : input file");
        puts ("  /O {file} : output file");
        puts ("  /H : show this");
	goto done;
        break;
      }
    }
    
    /* Check command line args */
  if (fn_in == NULL)
    {
    show_error ("No input file", 0); 
    goto done;
    }

  if (fn_out == NULL)
    {
    show_error ("No output file", 0); 
    goto done;
    }

  /* Args OK; see if we should compress or decompress. */ 
  if (mode == M_ENCODE)
    {
    if ((file = fopen(fn_in, "r")) == NULL)
      {
      show_error (fn_in, errno);
      goto done;
      }
    if ((bit_file = bIO_open(fn_out, BIT_IO_W)) == NULL) 
      {
      show_error (fn_out, errno);
      goto done;
      }
    ret = compress (file, bit_file);
    if (ret != 0) show_error ("Encode", ret);
    }
  else if (mode == M_DECODE)
    {
    if ((bit_file = bIO_open (fn_in, BIT_IO_R)) == NULL) 
      {
      show_error (fn_in, errno);
      goto done;
      }
    if ((file = fopen (fn_out, "w")) == NULL)
      {
      show_error (fn_out, errno);
      goto done;
      }
    ret = decompress (bit_file, file);
    if (ret != 0) show_error ("Decode", ret);
    }
  else
    {
    show_error ("/C or /D must be given\n", 0);
    goto done;
    }
    
done: 
  if (file != NULL)
      fclose (file);

  if (bit_file  != NULL)
      bIO_close (bit_file);

  if (fn_in) free (fn_in);
  if (fn_out) free (fn_out);
  exit (ret);
  }


