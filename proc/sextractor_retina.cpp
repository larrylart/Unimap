////////////////////////////////////////////////////////////////////
// File:		sextractor_retina.cpp
// Package:		SExtractor
// Author:		E.BERTIN (IAP)
// Contents:	functions dealing with retinal analysis of the data.
// Last modify:	15/02/2005, 03/01/2009
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

/*
#include	"sextractor_define.h"
#include	"sextractor_globals.h"
#include	"fits/fitscat.h"
#include	"sextractor_bpro.h"
#include	"sextractor_image.h"
#include	"sextractor_retina.h"
*/

//#include "fits/fitscat.h"
// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		ReadRetina
// Class:		CSextractor
// Purpose:		Return the response of the retina at a given image 
//				position.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
float CSextractor::ReadRetina( picstruct *field, retistruct *retina, 
													float x, float y )
{
   float        *pix, resp, norm;
   int          i, ix,iy;

  ix = (int)(x+0.499999);
  iy = (int)(y+0.499999);
  if (ix>=0 && ix<field->width && iy>=field->ymin && iy<field->ymax)
    norm = field->strip[ix+(iy%field->stripheight)*field->width];
  else
    norm = retina->minnorm;
  if (norm<retina->minnorm)
    norm = retina->minnorm;
/* Copy the right pixels to the retina */
  pix = retina->pix;
  CopyImage(field, pix, retina->width, retina->height, ix,iy);
  for (i=retina->npix; i--;)
    *(pix++) /= norm;
  *pix = -2.5*log10(norm/retina->minnorm);
  PlayBpann(retina->bpann, retina->pix, &resp);

	return resp;
}

////////////////////////////////////////////////////////////////////
// Method:		GetRetina
// Class:		CSextractor
// Purpose:		Read an ANN retina file.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
retistruct* CSextractor::GetRetina( char *filename )
{

#define	FILTEST(x) \
        if (x != RETURN_OK) \
	  error(EXIT_FAILURE, "*Error*: RETINA header in ", filename)

   retistruct	*retina;
   catstruct	*fcat;
   tabstruct	*ftab;
   int		ival;

  QMALLOC(retina, retistruct, 1);
/* We first map the catalog */
  if (!(fcat = read_cat(filename)))
    error(EXIT_FAILURE, "*Error*: retina file not found: ", filename);
/* Test if the requested table is present */
  if (!(ftab = name_to_tab(fcat, "BP-ANN", 0)))
    error(EXIT_FAILURE, "*Error*: no BP-ANN info found in ", filename);
  FILTEST(fitsread(ftab->headbuf, "BPTYPE  ", gstr,H_STRING,T_STRING));
  if (strcmp(gstr, "RETINA_2D"))
    error(EXIT_FAILURE, "*Error*: not a suitable retina in ", filename);
  FILTEST(fitsread(ftab->headbuf, "RENAXIS ", &ival ,H_INT, T_LONG));
  if (ival != 2) 
    error(EXIT_FAILURE, "*Error*: not a 2D retina in ", filename);
  FILTEST(fitsread(ftab->headbuf, "RENAXIS1", &retina->width ,H_INT, T_LONG));
  FILTEST(fitsread(ftab->headbuf, "RENAXIS2", &retina->height ,H_INT, T_LONG));
  retina->npix = retina->width*retina->height;
  FILTEST(fitsread(ftab->headbuf, "RENORM  ",&retina->minnorm,H_FLOAT,T_FLOAT));
  retina->bpann = LoadTabBpann(ftab, filename);
  QMALLOC(retina->pix, float, retina->bpann->nn[0]);

  close_cat(fcat);
  free_cat(&fcat,1);

	return( retina );
}

////////////////////////////////////////////////////////////////////
// Method:		EndRetina
// Class:		CSextractor
// Purpose:		Free a retina structure.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::EndRetina( retistruct *retina )
{
	free(retina->pix);
	FreeBpann(retina->bpann);
	free(retina);

	return;
}

