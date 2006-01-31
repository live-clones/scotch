/************************************************************/
/**                                                        **/
/**   NAME       : mmk_m3.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the data declara-    **/
/**                tions for the tridimensional finite     **/
/**                element grid mesh building program.     **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 26 sep 2002     **/
/**                                 to   : 26 sep 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   2             /* Number of files in list                */
#define C_FILEARGNBR                1             /* Number of files which can be arguments */

#define C_filenamemshout            C_fileTab[0].name /* Source mesh output file name   */
#define C_filenamegeoout            C_fileTab[1].name /* Geometry mesh output file name */

#define C_filepntrmshout            C_fileTab[0].pntr /* Source mesh output file   */
#define C_filepntrgeoout            C_fileTab[1].pntr /* Geometry mesh output file */

/*+ Process flags. +*/

#define C_FLAGGEOOUT                0x0001        /* Output mesh geometry */

#define C_FLAGDEFAULT               0x0000        /* Default flags */
