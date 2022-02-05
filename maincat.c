/*==========================================================================
  
  lz77 compress/decompress utility for Linux and CP/M

  maincat.c

  This is the command-line interface for lz77cat(.com). This interface
    is for decompressing direct to the console

  Kevin Boone, Feburary 2022
  
  Based on an original implementation by David Costa and Pietro De Rosa
  
==========================================================================*/

#include "compat.h" 
#include <stdio.h>
#ifndef CPM
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#else
extern int errno;
#endif
#include "getopt.h"
#include "bitio.h"
#include "lz77.h"
#include "error.h"

#ifndef CPM
struct termios orig_termios;
#else
extern int getchne(); /* In assembly */
#endif

/* Number of lines read so far by the pager */
int lines = 0;

/* Number of lines per page */
int rows = 23;

/*==========================================================================

  gcne
  Get a character without echoing or waiting for a newline. On Linux
  we just use getc(), because the console has already been set up
  appropriately 

==========================================================================*/
int gcne ()
  {
#ifdef CPM
  return getchne();
#else
  return getc (stdin);
#endif
  }

/*==========================================================================

  pager_put 

==========================================================================*/
BOOL pager_put (c)
int c;
  {
  if (c == 26) return TRUE; /* CP/M ctrl-z -- end of text */
  if (c == 10)
    {
#ifdef CPM
    putc (13, stdout);
#endif
    putc (10, stdout);
    lines++;
    if (lines == rows)
      {
      int c;
#ifdef CPM
/*      putc (13, stdout); */
#endif
      /*putc (10, stdout); */
      c = gcne();
      if (c == 3) /* ctrl-c */
        exit(0);
      lines = 0;
      }
    }
  else
    putc (c, stdout);
  return FALSE;
  }


/*==========================================================================

  init_term 

==========================================================================*/
void init_term ()
  {
#ifdef CPM 
#else
  tcgetattr (0, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= (unsigned int) ~(ECHO | ICANON);
  raw.c_cc[VTIME] = 0;
  raw.c_cc[VMIN] = 1;
  tcsetattr (0, TCSAFLUSH, &raw);
#endif
  }

/*==========================================================================

  cleanup_term 

==========================================================================*/
void cleanup_term ()
  {
#ifdef CPM 
#else
  tcsetattr (0, TCSAFLUSH, &orig_termios);
#endif
  }

/*==========================================================================

  main 

==========================================================================*/
int main (argc, argv)
int argc;
char **argv;
  {
  int ret = 0;
  if (argc == 2)
    {
    CONST char* fn_in = argv[1];
    struct bitFILE *bit_file = NULL;
    if ((bit_file = bIO_open (fn_in, BIT_IO_R)) == NULL) 
      {
      show_error (fn_in, errno);
      goto done;
      }
    init_term();
    ret = decompress (bit_file, NULL);
    cleanup_term();
    if (ret != 0) show_error ("Decode", ret);
done:
    if (bit_file  != NULL)
      bIO_close (bit_file);
    }
  else
    {
    fputs ("Usage: lz77cat {compressed file}\n", stderr);
    }
  return ret; 
  }

