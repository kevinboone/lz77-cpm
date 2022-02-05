/*===========================================================================

  getopt.h

  Copyright (c)2021 Kevin Boone, GPL v3.0

===========================================================================*/
#ifndef __GETOPT_H
#define __GETOPT_H

extern char *optarg;
extern int optind, opterr, optopt, optpos, optreset;

/*
int getopt (int argc, char **argv, char *optstring)
*/
int getopt();

#endif /* getopt.h */

