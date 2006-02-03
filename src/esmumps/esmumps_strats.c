/************************************************************/
/**                                                        **/
/**   NAME       : esmumps_strats.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains defalut strategy   **/
/**                building routines for calling Scotch    **/
/**                from MUMPS.                             **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 08 dec 2003     **/
/**                                 to     08 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ESMUMPS

#include "common.h"
#include "esmumps.h"

/**************************************/
/*                                    */
/* This routine creates a strategy    */
/* string from the given parameters.  */
/*                                    */
/**************************************/

/* Meaning of the parameters :
** - procnbr : number of processors.
** - leafsiz : limit size in vertices of the leaf subgraphs.
** - leorval : type of halo ordering : 0 : HAMD; 1 : HAMF.
** - cminval : minimum number of column blocks.
** - cmaxval : maximum number of column blocks.
** - fratval : maximum fill ratio.
** - verbval : verbose flag if > 0.
** - stream  : verbose stream.
** - straptr : pointer to character string of sufficient size
**             to hold the resulting Scotch strategy.
*/

int
esmumps_strat1 (
const INT                   procnbr,
const INT                   leafsiz,
const int                   leorval,
const INT                   cminval,
const INT                   cmaxval,
const double                fratval,
const int                   verbval,
FILE * const                stream,
char * const                straptr)
{
  INT                 levlval;                    /* Nested dissection level */
  INT                 procmax;
  char                hamxval;                    /* Type of halo ordering routine */

  for (levlval = 1, procmax = 1; procmax < procnbr; procmax <<= 1, levlval ++) ; /* Compute log2 of procnbr, + 1 */

  hamxval = (leorval == 0) ? 'h' : 'f';           /* HAMD if 0, HAMF if 1 */

  if (verbval != 0) {
    fprintf (stream, "Scotch strategy:\n- %ld levels of ND are necessary for %ld processors\n",
             (long) levlval,
             (long) procnbr);
    fprintf (stream, "- If compressed (0.7) graph, then perform %ld levels of ND, then switch to HAM(%c)\n",
             (long) levlval,
             (char) hamxval);
    fprintf (stream, "- If uncompressed graph, then perform at least %ld levels of ND, and proceed\n  until graph size less than %ld vertices, then switch to HAM(%c)\n",
             (long) levlval,
             (long) leafsiz,
             (char) hamxval);
    fprintf (stream, "- At the end of HAM(%c), amalgamate if number of columns not greater than %ld,\n  and if either column size is smaller than %ld or fill ratio less than %lf\n",
             (char) hamxval,
             (long) cmaxval,
             (long) cminval,
             (double) fratval);
    fprintf (stream, "- Separators are not split and are ordered in natural order\n");
  }

  sprintf (straptr, "c{rat=0.7,cpr=n{sep=/((levl<%ld)|(vert>%ld))?m{vert=100,low=h{pass=10},asc=f{bal=0.2}}|m{vert=100,low=h{pass=10},asc=f{bal=0.2}};,ole=%c{cmin=%ld,cmax=%ld,frat=%lf},ose=s},unc=n{sep=/(levl<%ld)?(m{vert=100,low=h{pass=10},asc=f{bal=0.2}})|m{vert=100,low=h{pass=10},asc=f{bal=0.2}};,ole=%c{cmin=%ld,cmax=%ld,frat=%lf},ose=s}}",
           (long) levlval,
           (long) leafsiz,
           (char) hamxval,
           (long) cminval,
           (long) cmaxval,
           (double) fratval,
           (long) levlval,
           (char) hamxval,
           (long) cminval,
           (long) cmaxval,
           (double) fratval);

  return (0);
}
