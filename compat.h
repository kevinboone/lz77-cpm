/*===========================================================================

  compat.h

  Some functions missing or broken in the Aztec CP/M C library
  NOTE: these have names that differ from the standard C library, by
  the addition of an underscore. The intention is that, even when 
  building on Linux, these non-standard implementations get used,
  so they can be tested. 

  Copyright (c)2022 Kevin Boone, GPL v3.0

===========================================================================*/
#ifndef __COMPAT_H
#define __COMPAT_H

#ifdef CPM
#define CONST
#else
/* Aztec C does not support "const" */
#define CONST const
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifndef CPM
//#define DEBUG
#endif

#ifdef CPM
int _memcmp ();
void *_memmove ();
void *_memcpy ();
void *_memset ();
char *_strdup ();
int _getopt ();
#else
int _memcmp (CONST unsigned char *s1, CONST unsigned char *s2, unsigned int n);
void *_memmove (void *dest, CONST void *src, unsigned int n);
int puterr ();
void *_memcpy (void *dest, CONST void *src, unsigned int n);
void *_memset (void *s, int c, unsigned int n);
char *_strdup (CONST char *s);
int _getopt (int argc, char **argv, char *optstring);
#endif

#ifdef CPM
void _perror ();
#else
void _perror (CONST char *s);
#endif

#endif /* compat.h */

