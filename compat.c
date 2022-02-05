/*===========================================================================

  compat.c

  See compat.h for summary.

  Copyright (c)2022 Kevin Boone, GPLv3.0

===========================================================================*/
#include "compat.h"
#include <ctype.h>
#include <stdio.h>


#ifndef CPM
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#else
extern int errno;
#endif

/*
  memcpy
*/
void *_memcpy (dest, src, n)
void *dest;
CONST void *src;
unsigned int n;
  {
  register int i;
  register unsigned int _n = n;
  for (i = 0; i < _n; i++)
    ((char *)(dest))[i] = ((char *)(src))[i];
  return dest;
  }

/*
  memset
*/
void *_memset (s, c, n)
void *s;
int c;
unsigned int n;
  {
  register unsigned int i;
  for (i = 0; i < n; i++)
    ((char *)(s))[i] = c;
  return s;
  }

int _memcmp (s1, s2, n)
CONST unsigned char *s1;
CONST unsigned char *s2;
unsigned int n;
  {
  unsigned int i;
  for (i = 0; i < n; i++)
    {
    unsigned char c1 = *(unsigned char *)s1;
    unsigned char c2 = *(unsigned char *)s2;
    if (c1 != c2) return c1 - c2;
    s1++;
    s2++;
    }
  return 0;
  }

/*
  memmove
*/
void *_memmove (dest, src, n)
void *dest;
CONST void *src;
unsigned int n;
  {
  char *d = (char*)dest;
  char *s = (char*)src;
  if (s < d) 
    {
    s += n;
    d += n;
    while (n--)
      *--d = *--s;
    } 
  else
    {
    while (n--)
      *d++ = *s++;
    }

  return dest;
  }


/*
  strdup
*/
char *_strdup (s)
CONST char *s;
  {
  int l = strlen (s);
  char *ret = malloc (l + 1);
  strcpy (ret, s);
  return ret; 
  }

/*
 * perror
 */
void _perror (s)
CONST char *s;
  {
  /* TODO */
  fputs (s, stderr); 
  }

#ifdef CPM
int puterr (c)
int c;
  {
  return aputc (c, stderr);
  }
#endif

