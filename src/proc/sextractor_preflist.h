 /*
 				preflist.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Keywords for the configuration file.
*
*	Last modify:	14/07/2006
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifndef _SEXTRACTOR_PREFLIST_H
#define _SEXTRACTOR_PREFLIST_H

#include "sextractor_key.h"

#ifndef _XML_H_
#include "sextractor_xml.h"
#endif

#ifdef  USE_THREADS
#define THREADS_PREFMAX THREADS_NMAX
#else
#define THREADS_PREFMAX 65535
#endif

/*-------------------------------- initialization ---------------------------*/
// int	idummy;


const char	notokstr[] = {" \t=,;\n\r\""};

#endif
