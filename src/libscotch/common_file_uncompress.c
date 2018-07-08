/* Copyright 2008,2010,2015,2018 IPB, Universite de Bordeaux, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : common_file_uncompress.c                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles compressed streams  **/
/**                for uncompression.                      **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 11 mar 2008     **/
/**                                 to   : 15 may 2008     **/
/**                # Version 5.1  : from : 27 jun 2010     **/
/**                                 to     27 jun 2010     **/
/**                # Version 6.0  : from : 27 apr 2015     **/
/**                                 to     08 jul 2018     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_FILE
#define COMMON_FILE_UNCOMPRESS

#ifndef COMMON_NOMODULE
#include "module.h"
#endif /* COMMON_NOMODULE */
#include "common.h"
#include "common_file.h"
#include "common_file_compress.h"
#ifdef COMMON_FILE_COMPRESS_BZ2
#include "bzlib.h"
#endif /* COMMON_FILE_COMPRESS_BZ2 */
#ifdef COMMON_FILE_COMPRESS_GZ
#include "zlib.h"
#endif /* COMMON_FILE_COMPRESS_GZ */
#ifdef COMMON_FILE_COMPRESS_LZMA
#include "lzma.h"
#endif /* COMMON_FILE_COMPRESS_LZMA */

/*
**  The static definitions.
*/

static FileCompressTab      filetab[] = {
#ifdef COMMON_FILE_COMPRESS_BZ2
                                          { ".bz2",  FILECOMPRESSTYPEBZ2,    },
#else /* COMMON_FILE_COMPRESS_BZ2 */
                                          { ".bz2",  FILECOMPRESSTYPENOTIMPL },
#endif /* COMMON_FILE_COMPRESS_BZ */
#ifdef COMMON_FILE_COMPRESS_GZ
                                          { ".gz",   FILECOMPRESSTYPEGZ,     },
#else /* COMMON_FILE_COMPRESS_GZ */
                                          { ".gz",   FILECOMPRESSTYPENOTIMPL },
#endif /* COMMON_FILE_COMPRESS_GZ */
#ifdef COMMON_FILE_COMPRESS_LZMA
                                          { ".lzma", FILECOMPRESSTYPELZMA    },
                                          { ".xz",   FILECOMPRESSTYPELZMA    },
#else /* COMMON_FILE_COMPRESS_LZMA */
                                          { ".lzma", FILECOMPRESSTYPENOTIMPL },
                                          { ".xz",   FILECOMPRESSTYPENOTIMPL },
#endif /* COMMON_FILE_COMPRESS_LZMA */
                                          { NULL,    FILECOMPRESSTYPENOTIMPL } };

/*********************************/
/*                               */
/* Basic routines for filenames. */
/*                               */
/*********************************/

/* This routine searches the given file name
** for relevant extensions and returns the
** corresponding code if it is the case.
** It returns:
** - FILECOMPRESSTYPENONE     : no recognized file extension.
** - FILECOMPRESSTYPENOTIMPL  : compression algorithm not implemented.
** - FILECOMPRESSTYPExxxx     : implemented compression algorithm.
*/

int
fileUncompressType (
const char * const          nameptr)              /*+ Name string +*/
{
  int                 namelen;
  int                 i;

  namelen = strlen (nameptr);
  for (i = 0; filetab[i].name != NULL; i ++) {
    int                 extnlen;                  /* Name of extension string */

    extnlen = strlen (filetab[i].name);
    if ((namelen >= extnlen) && (strncmp (filetab[i].name, nameptr + (namelen - extnlen), extnlen) == 0))
      return (filetab[i].type);
  }

  return (FILECOMPRESSTYPENONE);
}

/* This routine creates a thread to uncompress the
** given stream according to the given (un)compression
** algorithm.
** If threads are available, uncompression will be
** performed by an auxiliary thread. Else, a child process
** will be fork()'ed, and after completion this process
** will remain a zombie until the main process terminates.
** It returns:
** - !NULL  : stream holding uncompressed data.
** - NULL   : on error.
*/

static
void *                                            /* (void *) to comply to the Posix pthread API */
fileUncompress2 (
FileCompressData * const  dataptr)
{
  switch (dataptr->typeval) {
#ifdef COMMON_FILE_COMPRESS_BZ2
    case FILECOMPRESSTYPEBZ2 :
      fileUncompressBz2 (dataptr);
      break;
#endif /* COMMON_FILE_COMPRESS_BZ2 */
#ifdef COMMON_FILE_COMPRESS_GZ
    case FILECOMPRESSTYPEGZ :
      fileUncompressGz (dataptr);
      break;
#endif /* COMMON_FILE_COMPRESS_GZ */
#ifdef COMMON_FILE_COMPRESS_LZMA
    case FILECOMPRESSTYPELZMA :
      fileUncompressLzma (dataptr);
      break;
#endif /* COMMON_FILE_COMPRESS_LZMA */
    default :
      errorPrint ("fileUncompress2: method not implemented");
  }

  close   (dataptr->innerfd);                     /* Close writer's end */
  memFree (dataptr);                              /* Free buffers       */

  return ((void *) 0);                            /* Don't care anyway */
}

FILE *
fileUncompress (
FILE * const                stream,               /*+ Compressed stream         +*/
const int                   typeval)              /*+ (Un)compression algorithm +*/
{
  int                 filetab[2];
  FILE *              readptr;
  FileCompressData *  dataptr;
#ifdef COMMON_PTHREAD_FILE
  pthread_t           thrdval;
#endif /* COMMON_PTHREAD_FILE */

  if (typeval <= FILECOMPRESSTYPENONE)            /* If uncompressed stream, return original stream pointer */
    return (stream);

  if (pipe (filetab) != 0) {
    errorPrint ("fileUncompress: cannot create pipe");
    return (NULL);
  }

  if ((readptr = fdopen (filetab[0], "r")) == NULL) {
    errorPrint ("fileUncompress: cannot create stream");
    close  (filetab[0]);
    close  (filetab[1]);
    return (NULL);
  }

  if ((dataptr = memAlloc (sizeof (FileCompressData) + FILECOMPRESSDATASIZE)) == NULL) {
    errorPrint ("fileUncompress: out of memory");
    fclose (readptr);
    close  (filetab[1]);
    return (NULL);
  }

  dataptr->typeval     = typeval;                 /* Fill structure to be passed to uncompression thread/process */
  dataptr->innerfd     = filetab[1];
  dataptr->outerstream = stream;

#ifdef COMMON_PTHREAD_FILE
  if (pthread_create (&thrdval, NULL, (void * (*) (void *)) fileUncompress2, (void *) dataptr) != 0) { /* If could not create thread */
    errorPrint ("fileUncompress: cannot create thread");
    memFree (dataptr);
    fclose  (readptr);
    close   (filetab[1]);
    return  (NULL);
  }
  pthread_detach (thrdval);                       /* Detach thread so that it will end up gracefully by itself */
#else /* COMMON_PTHREAD_FILE */
  switch (fork ()) {
    case -1 :                                     /* Error */
      errorPrint ("fileUncompress: cannot create child process");
      memFree (dataptr);
      fclose  (readptr);
      close   (filetab[1]);
      return  (NULL);
    case 0 :                                      /* We are the son process    */
      fclose (readptr);                           /* Close reader pipe stream  */
      fileUncompress2 (dataptr);                  /* Perform uncompression     */
      exit (0);                                   /* Exit gracefully           */
    default :                                     /* We are the father process */
      close (filetab[1]);                         /* Close the writer pipe end */
  }
#endif /* COMMON_PTHREAD_FILE */

  return (readptr);
}

/* This routine uncompresses a stream compressed
** in the bzip2 format.
** It returns:
** - void  : in all cases. Uncompression stops
**           immediately in case of error.
*/

#ifdef COMMON_FILE_COMPRESS_BZ2
static
void
fileUncompressBz2 (
FileCompressData * const  dataptr)
{
  BZFILE *            decoptr;
  int                 bytenbr;
  int                 flagval;

  if (FILECOMPRESSDATASIZE < (BZ_MAX_UNUSED)) {
    errorPrint ("fileUncompressBz2: cannot start decompression (1)");
    return;
  }
  if ((decoptr = BZ2_bzReadOpen (&flagval, dataptr->outerstream, 0, 0, NULL, 0)) == NULL) {
    errorPrint ("fileUncompressBz2: cannot start decompression (2)");
    BZ2_bzReadClose (&flagval, decoptr);
    return;
  }

  while ((bytenbr = BZ2_bzRead (&flagval, decoptr, &dataptr->datatab, FILECOMPRESSDATASIZE), flagval) >= BZ_OK) { /* If BZ_OK or BZ_STREAM_END */
    if (write (dataptr->innerfd, &dataptr->datatab, bytenbr) != bytenbr) {
      errorPrint ("fileUncompressBz2: cannot write");
      flagval = BZ_STREAM_END;                    /* Avoid other error message */
      break;
    }
    if (flagval == BZ_STREAM_END) {               /* If end of compressed stream */
      void *              byunptr;
      int                 byunnbr;

      BZ2_bzReadGetUnused (&flagval, decoptr, &byunptr, &byunnbr); /* Get remaining chars in stream    */
      if ((byunnbr == 0) && (feof (dataptr->outerstream) != 0)) { /* If end of uncompressed stream too */
        flagval = BZ_STREAM_END;
        break;
      }
      memMov (&dataptr->datatab, byunptr, byunnbr);
      BZ2_bzReadClose (&flagval, decoptr);
      if ((decoptr = BZ2_bzReadOpen (&flagval, dataptr->outerstream, 0, 0, &dataptr->datatab, byunnbr)) == NULL) {
        errorPrint ("fileUncompressBz2: cannot start decompression (3)");
        flagval = BZ_STREAM_END;
        break;
      }
    }
  }
  if (flagval != BZ_STREAM_END)
    errorPrint ("fileUncompressBz2: cannot read");

  BZ2_bzReadClose (&flagval, decoptr);
  fclose (dataptr->outerstream);                  /* Do as zlib does */
}
#endif /* COMMON_FILE_COMPRESS_BZ2 */

/* This routine uncompresses a stream compressed
** in the gzip format.
** It returns:
** - void  : in all cases. Uncompression stops
**           immediately in case of error.
*/

#ifdef COMMON_FILE_COMPRESS_GZ
static
void
fileUncompressGz (
FileCompressData * const  dataptr)
{
  gzFile              decoptr;
  int                 bytenbr;

  if ((decoptr = gzdopen (fileno (dataptr->outerstream), "rb")) == NULL) {
    errorPrint ("fileUncompressGz: cannot start decompression");
    return;
  }

  while ((bytenbr = gzread (decoptr, &dataptr->datatab, FILECOMPRESSDATASIZE)) > 0) {
    if (write (dataptr->innerfd, &dataptr->datatab, bytenbr) != bytenbr) {
      errorPrint ("fileUncompressGz: cannot write");
      break;
    }
  }
  if (bytenbr < 0)
    errorPrint ("fileUncompressGz: cannot read");

  gzclose (decoptr);
}
#endif /* COMMON_FILE_COMPRESS_GZ */

/* This routine uncompresses a stream compressed
** in the lzma format.
** It returns:
** - void  : in all cases. Uncompression stops
**           immediately in case of error.
*/

#ifdef COMMON_FILE_COMPRESS_LZMA
static
void
fileUncompressLzma (
FileCompressData * const  dataptr)
{
  lzma_stream         decodat = LZMA_STREAM_INIT; /* Decoder data          */
  lzma_action         deacval;                    /* Decoder action value  */
  lzma_ret            dereval;                    /* Decoder return value  */
  byte *              obuftab;                    /* Decoder output buffer */

  if ((obuftab = memAlloc (FILECOMPRESSDATASIZE)) == NULL) {
    errorPrint ("fileUncompressLzma: out of memory");
    return;
  }

  if (lzma_stream_decoder (&decodat, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK) {
    errorPrint ("fileUncompressLzma: cannot start decompression");
    memFree    (obuftab);
    return;
  }

  deacval           = LZMA_RUN;
  dereval           = LZMA_OK;
  decodat.avail_in  = 0;
  decodat.next_out  = obuftab;
  decodat.avail_out = FILECOMPRESSDATASIZE;
  do {
    if ((decodat.avail_in == 0) && (deacval == LZMA_RUN)) {
      ssize_t             bytenbr;

      bytenbr = fread (&dataptr->datatab, 1, FILECOMPRESSDATASIZE, dataptr->outerstream); /* Read from pipe */
      if (bytenbr < 0) {
        errorPrint ("fileUncompressLzma: cannot read");
        break;
      }
      if (bytenbr == 0)
        deacval = LZMA_FINISH;                    /* If end of stream, request completion of encoding */
      decodat.next_in  = (byte *) &dataptr->datatab;
      decodat.avail_in = bytenbr;
    }

    dereval = lzma_code (&decodat, deacval);

    if ((decodat.avail_out == 0) || (dereval == LZMA_STREAM_END)) { /* Write when output buffer full or end of encoding */
      size_t              obufnbr;

      obufnbr = FILECOMPRESSDATASIZE - decodat.avail_out; /* Compute number of bytes to write */
      if (write (dataptr->innerfd, obuftab, obufnbr) != obufnbr) {
        errorPrint ("fileUncompressLzma: cannot write");
        break;
      }
      decodat.next_out  = obuftab;
      decodat.avail_out = FILECOMPRESSDATASIZE;
    }
  } while (dereval == LZMA_OK);

  lzma_end (&decodat);
  memFree  (obuftab);

  fclose (dataptr->outerstream);                  /* Do as zlib does */
}
#endif /* COMMON_FILE_COMPRESS_LZMA */
