/************************************************************/
/**                                                        **/
/**   NAME       : mord.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a mesh orderer.                 **/
/**                This module contains the data declara-  **/
/**                tions for the main routine.             **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 15 nov 2002     **/
/**                                 to   : 22 oct 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   4             /* Number of files in list                */
#define C_FILEARGNBR                3             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph input file name */
#define C_filenameordout            C_fileTab[1].name /* Ordering output file name    */
#define C_filenamelogout            C_fileTab[2].name /* Log file name                */
#define C_filenamemapout            C_fileTab[3].name /* Separator mapping file name  */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file */
#define C_filepntrordout            C_fileTab[1].pntr /* Ordering output file    */
#define C_filepntrlogout            C_fileTab[2].pntr /* Log file                */
#define C_filepntrmapout            C_fileTab[3].pntr /* Separator mapping file  */

/*+ Process flags. +*/

#define C_FLAGNONE                  0x0000        /* No flags                  */
#define C_FLAGMAPOUT                0x0001        /* Output mapping data       */
#define C_FLAGVERBSTR               0x0002        /* Output strategy string    */
#define C_FLAGVERBTIM               0x0004        /* Output timing information */
