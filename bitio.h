/***************************************************************************
 *          Lempel, Ziv Encoding and Decoding
 *
 *   File    : bitio.h
 *   Original authors : David Costa and Pietro De Rosa
 *   Modified for CP/M by Kevin Boone
 ***************************************************************************/

#ifndef BITIO_H 
#define BITIO_H 

#define BIT_IO_W 0
#define BIT_IO_R 1

struct bitFILE;

#ifdef CPM
int             bIO_feof ();

int             bIO_ferror ();
struct bitFILE *bIO_open ();
void            bIO_close ();
int             bIO_write ();
int             bIO_read ();

#else

/* Returns non-zero if we have read to th end of the file */
int             bIO_feof (struct bitFILE *bitF);

/* Returns non-zero if the error flag has been set */
int             bIO_ferror (struct bitFILE *bitF);

/* Opens the file in one of the BIO_ modes */
struct bitFILE *bIO_open (const char *path, int mode);

/* Closes the file */
void            bIO_close (struct bitFILE *bitF);

/* Write nbit bits from location info */
int             bIO_write (struct bitFILE *bitF, void *info, int nbit);

/* Read nbit bits from location info; buffer size is info_s */
int             bIO_read (struct bitFILE *bitF, void *info, 
                             int info_s, int nbit);
#endif

#endif
