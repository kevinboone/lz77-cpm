/*==========
 *
 * lz77 compress/decompress utility for Linux and CP/M
 * Kevin Boone, Feburary 2022
 *
 * Based on an original implementation by David Costa and Pietro De Rosa
 *
 * ========*/


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
#include "error.h"

/*===========
 * show_error 
 * ==========*/
void show_error (msg, err)
CONST char *msg;
int err;
  {
  int e = err;
  fputs (NAME, stderr);
  fputs (": ", stderr);
  fputs ( msg, stderr);
  if (err)
    {
    fputs (": ", stderr);
    if (err > 1000)
      {
      switch (err)
        {
        case LZERR_READ:
          fputs ("Read error", stderr);
          break;
        case LZERR_WRITE:
          fputs ("Write error", stderr);
          break;
        case LZERR_OREAD:
          fputs ("Can't open for read", stderr);
          break;
        case LZERR_OWRITE:
          fputs ("Can't open for write", stderr);
          break;
        case LZERR_F_FORMAT:
          fputs ("Bad file format", stderr);
          break;
        }  
      }
#ifndef CPM
    /* TODO -- CPM */
    else
      fputs (strerror (e), stderr);
#endif
    }

  fputs ("\n", stderr);
  }


