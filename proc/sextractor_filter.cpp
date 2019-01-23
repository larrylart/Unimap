////////////////////////////////////////////////////////////////////
// File:		sextractor_filter.cpp
// Package:		SExtractor
// Author:		E.BERTIN (IAP)
// Contents:	functions dealing with on-line filtering of the image
//				(for detection).
// Last modify:	26/11/2003
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

//#include	"sextractor_define.h"
//#include	"sextractor_globals.h"
//#include	"sextractor_prefs.h"
//#include	"fits/fitscat.h"
//#include	"sextractor_bpro.h"
//#include	"sextractor_filter.h"
//#include	"sextractor_image.h"

//#include "fits/fitscat.h"
// include main header
#include "sextractor.h"
#include "../config/detectconfig.h"
#include "../proc/detectfilter.h"


////////////////////////////////////////////////////////////////////
// Method:		Convolve
// Class:		CSextractor
// Purpose:		Convolve a scan line with an array.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Convolve( picstruct *field, PIXTYPE *mscan )
{
   int		mw,mw2,m0,me,m,mx,dmx, y0,dy, sw,sh;
   float	*mask;
   PIXTYPE	*mscane, *s,*s0, *d,*de, mval;

  sw = field->width;
  sh = field->stripheight;
  mw = thefilter->convw;
  mw2 = mw/2;
  mscane = mscan+sw;
  y0 = field->y - (thefilter->convh/2);
  if ((dy = field->ymin-y0) > 0)
    {
    m0 = mw*dy;
    y0 = field->ymin;
    }
  else
    m0 = 0;

  if ((dy = field->ymax - y0) < thefilter->convh)
    me = mw*dy;
  else
    me = mw*thefilter->convh;

  memset(mscan, 0, sw*sizeof(PIXTYPE));
  s0 = NULL;				/* To avoid gcc -Wall warnings */
  mask = thefilter->conv+m0;
  for (m = m0, mx = 0; m<me; m++, mx++)
    {
    if (mx==mw)
      mx = 0;
    if (!mx)
      s0 = field->strip+sw*((y0++)%sh);

    if ((dmx = mx-mw2)>=0)
      {
      s = s0 + dmx;
      d = mscan;
      de = mscane - dmx;
      }
    else
      {
      s = s0;
      d = mscan - dmx;
      de = mscane;
      }

    mval = *(mask++);
    while (d<de)
      *(d++) += mval**(s++);
    }

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		GetConv
// Class:		CSextractor
// Purpose:		Read the convolution mask from a file.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CSextractor::GetConv( char *filename )
{
  FILE		*file;
  char		str[MAXCHAR], *sstr, *null = NULL;
  double	sum, var, pix;
  int		i,j, n, normflag;


/* Open the file which may contain a convolution mask */

  if (!(file = fopen(filename, "r")))
    error(EXIT_FAILURE, "*Error*: cannot open ", filename);

/* Check it is a convolution mask. Otherwise, exit */
  fgets(str,MAXCHAR,file);
  if (strncmp(str,"CONV",4))
    {
    fclose(file);
    return RETURN_ERROR;
    }

  if (strstr(str,"NORM"))
    normflag = strstr(str,"NONORM")?0:1;
  else
    {
    warning("No normalization info found in convolution file (old format?)\n",
	"> => I will assume you want the mask to be normalized...");
    normflag = 1;
    }
/* Allocate memory for storing mask elements */
  QMALLOC(thefilter->conv, float, MAXMASK);

  for (i=0, n=0; fgets(str,MAXCHAR,file);)
    {
    j=0;
    sstr = strtok(str, " \t\n");
    if (sstr && sstr[0]!=(char)'#')
      do
        {
        j++;
        thefilter->conv[i++] = (float)atof(sstr);
        if (i>MAXMASK)
          error(EXIT_FAILURE, "*Error*: Convolution mask too large in ",
		filename);
        }	while ((sstr = strtok(null, " \t\n")));
    if (j>n)
      n = j;
    }

  fclose(file);

  if ((thefilter->convw = n)<1)
    error(EXIT_FAILURE, "*Error*: unappropriate convolution mask width in ",
	filename);
  if (i%n)
    error(EXIT_FAILURE, "*Error*: unappropriate convolution mask line in ",
	filename);

  QREALLOC(thefilter->conv, float, i);

  thefilter->convh = i/n;
  var = 0.0;
  for (j=0, sum=0.0; j<i; j++)
    {
    sum += fabs(pix = thefilter->conv[j]);
    var += pix*pix;
    }

  thefilter->varnorm = (float)(var=sqrt(var));

  if (normflag)
    {
    if (sum == 0.0)
      {
      warning("Zero-sum filter: ", "Normalization switched to variance-mode");
      sum = var;
      }
    for (j=0; j<i; j++)
      thefilter->conv[j] /= (float) sum;
    }

  thefilter->nconv = thefilter->convw*thefilter->convh;

	return RETURN_OK;
}

////////////////////////////////////////////////////////////////////
// Method:		GetConvConf
// Class:		CSextractor
// Purpose:		Read the convolution mask from a file.
// Input:		filter id
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CSextractor::GetConvConf( int nFilterId )
{
	char	strMsg[255];
	double	sum, var, pix;
	int		i, j, nCounter, normflag;
	int		nFilterWidth = 0;
	int		nFilterHeight = 0;

	// set filter name
	sprintf( strMsg, "%s", m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_strFilterName );

	// Check it is a convolution mask. Otherwise, exit 
	if( wxStrncmp( m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_strFilterMask, wxT("CONV"), 4 ) )
    {
		return RETURN_ERROR;
	}

	if( wxStrcmp( m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_strFilterFlag, wxT("NORM") ) == 0 )
	{
		normflag = 1;
	} else if( wxStrcmp( m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_strFilterFlag, wxT("NONORM") ) == 0 )
	{
		normflag = 0;
	} else
	{
		//warning( "No normalization info found in convolution file (old format?)\n",
		//		"> => I will assume you want the mask to be normalized..." );
		normflag = 1;
    }
	// Allocate memory for storing mask elements 
	QMALLOC( thefilter->conv, float, MAXMASK );
	// get filter size
	nFilterWidth = m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_nFilterWidth;
	nFilterHeight = m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_nFilterHeight;

	// load filter data from config
	nCounter = 0;
	for( i=0; i<nFilterWidth; i++ )
	{
		for( j=0; j<nFilterHeight; j++ )
		{
			thefilter->conv[nCounter++] = (float) m_pDetectConfig->m_vectDetectFilter[nFilterId]->m_vectData[i][j];
			if( nCounter>MAXMASK ) 
				error( EXIT_FAILURE, "*Error*: Convolution mask too large in ", strMsg );
		} 
	}

	if( (thefilter->convw = nCounter)<1 )
		error( EXIT_FAILURE, "*Error*: unappropriate convolution mask width in ", strMsg );
	if( nCounter%nFilterWidth )
		error( EXIT_FAILURE, "*Error*: unappropriate convolution mask line in ", strMsg );

	QREALLOC( thefilter->conv, float, nCounter );

	// here use instead width/height
//	thefilter->convh = nCounter/nFilterWidth;
	thefilter->convh = nFilterHeight;
	thefilter->convw = nFilterWidth;

	var = 0.0;
	for (j=0, sum=0.0; j<nCounter; j++)
	{
		sum += fabs(pix = thefilter->conv[j]);
		var += pix*pix;
	}

	thefilter->varnorm = (float)(var=sqrt(var));

	if( normflag )
	{
		if (sum == 0.0)
		{
			warning("Zero-sum filter: ", "Normalization switched to variance-mode");
			sum = var;
		}
		for( j=0; j<nCounter; j++ )
			thefilter->conv[j] /= (float) sum;
	}

	thefilter->nconv = thefilter->convw*thefilter->convh;

	return( RETURN_OK );
}

////////////////////////////////////////////////////////////////////
// Method:		GetFilter
// Class:		CSextractor
// Purpose:		Read either a convolution mask or an ANN file.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::GetFilter( int nFilterId, char *filename )
{
	QCALLOC(thefilter, filterstruct, 1);
// larry :: old option
//	if( GetConv(filename) != RETURN_OK && GetNeurFilter(filename) != RETURN_OK)
//		error(EXIT_FAILURE, "*Error*: not a suitable filter in ", filename);
	if( GetConvConf( nFilterId ) != RETURN_OK && GetNeurFilter(filename) != RETURN_OK)
		error(EXIT_FAILURE, "*Error*: not a suitable filter in ", filename);


	return;
}

////////////////////////////////////////////////////////////////////
// Method:		GetNeurFilter
// Class:		CSextractor
// Purpose:		Read an ANN RETINA-filter file.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CSextractor::GetNeurFilter( char *filename )
{

#define	FILTEST(x) \
	if (x != RETURN_OK) \
		error(EXIT_FAILURE, "*Error*: incorrect filter header in ", filename)

   catstruct	*fcat;
   tabstruct	*ftab;
   int		ival;

	// We first map the catalog 
	if(!(fcat = read_cat(filename)))
		return RETURN_ERROR;
	// Test if the requested table is present */
	if(!(ftab = name_to_tab(fcat, "BP-ANN", 0)))
		error(EXIT_FAILURE, "*Error*: no BP-ANN info found in ", filename);

	FILTEST(fitsread(ftab->headbuf, "BPTYPE  ", gstr,H_STRING,T_STRING));
	if( strcmp(gstr, "RETINA") )
		error( EXIT_FAILURE, "*Error*: not a retina-filter in ", filename );
	FILTEST(fitsread(ftab->headbuf, "RENAXIS ", &ival ,H_INT, T_LONG));
	if( ival != 2 ) 
		error( EXIT_FAILURE, "*Error*: not a 2D retina in ", filename );

	FILTEST(fitsread(ftab->headbuf, "RENAXIS1", &thefilter->convw ,H_INT,T_LONG));
	FILTEST(fitsread(ftab->headbuf, "RENAXIS2", &thefilter->convh ,H_INT,T_LONG));
	thefilter->nconv = thefilter->convw*thefilter->convh;
	QMALLOC(thefilter->conv, float, thefilter->nconv);
	thefilter->bpann = LoadTabBpann(ftab, filename);

	close_cat(fcat);
	free_cat(&fcat,1);

	return RETURN_OK;
}

////////////////////////////////////////////////////////////////////
// Method:		EndFilter
// Class:		CSextractor
// Purpose:		Terminate filtering procedures.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::EndFilter( )
{
	QFREE(thefilter->conv);
	if (thefilter->bpann)
	{
		FreeBpann(thefilter->bpann);
		thefilter->bpann = NULL;
	}

	QFREE(thefilter);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Filter
// Class:		CSextractor
// Purpose:		Switch to the appropriate filtering routine.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Filter( picstruct *field, PIXTYPE *mscan )
{
	if (thefilter->bpann)
		NeurFilter(field, mscan);
	else
		Convolve(field, mscan);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		NeurFilter
// Class:		CSextractor
// Purpose:		Filter a scan line using an artificial retina.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::NeurFilter( picstruct *field, PIXTYPE *mscan )
{
   PIXTYPE	cval;
   float	*pix, resp, sig, threshlow, threshhigh;
   int		i,x, tflag;

  sig = field->backsig;
  if ((tflag = (prefs.nfilter_thresh>0)))
    {
    threshlow = (float) prefs.filter_thresh[0]*field->backsig;
    threshhigh = (prefs.nfilter_thresh<2)?
		BIG : prefs.filter_thresh[1]*field->backsig;
    }
  else
    threshlow = threshhigh = 0.0;	/* To avoid gcc -Wall warnings */

  for (x=0;x<field->width;x++)
    {
    if (tflag)
      {
      cval = PIX(field, x, field->y);
      if (cval<threshlow || cval>threshhigh)
        {
        *(mscan++) = cval;
        continue;
	}
      }
/*-- Copy the surrounding image area to the retina */
    CopyImage(field, thefilter->conv, thefilter->convw, thefilter->convh,
		x, field->y);
    pix = thefilter->conv;
/*-- Apply a transform of the intensity scale */
    for (i=thefilter->nconv; i--; pix++)
      *pix = *pix>0.0?log(1+*pix/sig):-log(1-*pix/sig);

    PlayBpann(thefilter->bpann, thefilter->conv, &resp);

    if (resp>70.0)
      resp = 70.0;
    else if (resp<-70.0)
      resp = -70.0;
/*-- Recover the linear intensity scale */
    *(mscan++) = resp>0.0?sig*(exp(resp)-1):sig*(exp(-resp)-1);
    }

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ConvolveImage
// Class:		CSextractor
// Purpose:		Convolve a vignet with an array.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::ConvolveImage( picstruct *field, double *vig1,
								double *vig2, int width, int height )

  {
   int		mw,mw2,m0,me,m,mx,dmx, y, y0,dy;
   float	*mask;
   double	*mscane, *s,*s0, *vig3, *d,*de, mval, val;

/* If no filtering, just return a copy of the input data */
  if (!thefilter || thefilter->bpann)
    {
    memcpy(vig2, vig1, width*height*sizeof(double));
    return;
    }    
  s0 = NULL;				/* To avoid gcc -Wall warnings */
  mw = thefilter->convw;
  mw2 = mw/2;
  memset(vig2, 0, width*height*sizeof(double));
  vig3 = vig2;
  for (y=0; y<height; y++, vig3+=width)
    {
    mscane = vig3+width;
    y0 = y - (thefilter->convh/2);
    if (y0 < 0)
      {
      m0 = -mw*y0;
      y0 = 0;
      }
    else
      m0 = 0;
    if ((dy = height - y0) < thefilter->convh)
      me = mw*dy;
    else
      me = mw*thefilter->convh;
    mask = thefilter->conv+m0;
    for (m = m0, mx = 0; m<me; m++, mx++)
      {
      if (mx==mw)
        mx = 0;
      if (!mx)
        s0 = vig1+width*(y0++);
      if ((dmx = mx-mw2)>=0)
        {
        s = s0 + dmx;
        d = vig3;
        de = mscane - dmx;
        }
      else
        {
        s = s0;
        d = vig3 - dmx;
        de = mscane;
        }

      mval = *(mask++);
      while (d<de)
        *(d++) += ((val = *(s++))>-BIG? mval*val:0.0);
      }
    }

	return;
}
