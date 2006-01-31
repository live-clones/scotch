/************************************************************/
/**                                                        **/
/**   NAME       : gmtst.h                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This program computes statistics on     **/
/**                graph mappings.                         **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 17 jul 1996     **/
/**                                 to     23 jul 1996     **/
/**                # Version 3.2  : from : 02 jun 1997     **/
/**                                 to     02 jun 1997     **/
/**                # Version 3.3  : from : 06 jun 1999     **/
/**                                 to     07 jun 1999     **/
/**                # Version 4.0  : from : 12 feb 2004     **/
/**                                 to     29 nov 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   4             /* Number of files in list                */
#define C_FILEARGNBR                4             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph file name        */
#define C_filenametgtinp            C_fileTab[1].name /* Target architecture file name */
#define C_filenamemapinp            C_fileTab[2].name /* Mapping result file name      */
#define C_filenamedatout            C_fileTab[3].name /* Statistics file name          */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file   */
#define C_filepntrtgtinp            C_fileTab[1].pntr /* Target architecture file  */
#define C_filepntrmapinp            C_fileTab[2].pntr /* Mapping result input file */
#define C_filepntrdatout            C_fileTab[3].pntr /* Statistics output file    */

