////////////////////////////////////////////////////////////////////
// File:		sextractor_field.cpp
// Package:		SExtractor
// Author:		E.BERTIN (IAP)
// Contents:	Handling of field structures.
// Last modify:	29/06/2006
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/image.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

/*
#define NOCTYPES

extern "C" 
{
	#include	"sextractor_define.h"
	#include	"sextractor_globals.h"
	#include	"sextractor_prefs.h"
	#include	"fits/fitscat.h"
	#include	"sextractor_assoc.h"
	#include	"sextractor_astrom.h"
	#include	"sextractor_back.h"
	#include	"sextractor_field.h"
	#include	"sextractor_filter.h"
	#include	"sextractor_interpolate.h"
}

#undef NOCTYPES
*/
//#include "fits/fitscat.h"

// include local header
#include "../image/astroimage.h"

#define NOCTYPES
// include main header
#include "sextractor.h"
#undef NOCTYPES

////////////////////////////////////////////////////////////////////
// Method:		NewField
// Class:		CSextractor
// Purpose:		create a bew field for picstruct
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
picstruct* CSextractor::NewField( CAstroImage* pImg, int flags )
{	
	picstruct	*field;
	OFF_T	mefpos = 0;		
	int		margin;

	// First allocate memory for the new field (and nullify pointers)
	QCALLOC(field, picstruct, 1);
	field->mefpos = mefpos;
	field->flags = flags;
	strcpy(field->filename, pImg->m_strImageName.ToAscii() );
 
	// A short, "relative" version of the filename 
	if (!(field->rfilename = strrchr(field->filename, '/')))
		field->rfilename = field->filename;
	else
		field->rfilename++;

	//////////////////////////////
	// Check the image exists and read important info (image size, etc...)
//	readimagehead(field);
	ReadAstroImageParam( pImg, field );
	///////////////////////////////////

	field->compress_buf = NULL;

	// Check the astrometric system and do the setup of the astrometric stuff
	if (prefs.world_flag && (flags & (MEASURE_FIELD|DETECT_FIELD)))
		InitAstrom(field);
	else
		field->pixscale=prefs.pixel_scale;

	// Background ... something about ???
	if (flags & (DETECT_FIELD|MEASURE_FIELD|WEIGHT_FIELD|VAR_FIELD|RMS_FIELD))
	{
		field->ngamma = prefs.mag_gamma/log(10.0);

		field->backw = prefs.backsize[0]<field->width ? prefs.backsize[0] : field->width;
		field->backh = prefs.backsize[1]<field->height ? prefs.backsize[1] : field->height;
		field->nbackp = field->backw * field->backh;

		if((field->nbackx = (field->width-1)/field->backw + 1) < 1)
			field->nbackx = 1;

		if((field->nbacky = (field->height-1)/field->backh + 1) < 1)
			field->nbacky = 1;

		field->nback = field->nbackx * field->nbacky;
		field->nbackfx = field->nbackx>1 ? prefs.backfsize[0] : 1;
		field->nbackfy = field->nbacky>1 ? prefs.backfsize[1] : 1;

		// Set the back_type flag if absolute background is selected 
		if(((flags & DETECT_FIELD) && prefs.back_type[0]==BACK_ABSOLUTE)
			|| ((flags & MEASURE_FIELD) && prefs.back_type[1]==BACK_ABSOLUTE))
		{
			field->back_type = BACK_ABSOLUTE;
		}
	}

	// Add a comfortable margin for local background estimates
	margin = (prefs.pback_type == LOCAL)? prefs.pback_size + prefs.mem_bufsize/4 : 0;

	field->stripheight = prefs.mem_bufsize + margin;
	if(field->stripheight>field->height) field->stripheight = field->height;

	// Compute the image buffer size 
	// Basically, only one margin line is sufficient... 
	field->stripmargin = 1 + margin;
	//...but : 
	if (prefs.filter_flag)
	{
		// If filtering is on, one should consider the height of the conv. mask
		int margin;

		if(field->stripheight < thefilter->convh) 
			field->stripheight = thefilter->convh;

		if(field->stripmargin < (margin = (thefilter->convh-1)/2))
			field->stripmargin = margin;
	}

	return field;
}

////////////////////////////////////////////////////////////////////
// Method:		InheritField
// Class:		CSextractor
// Purpose:		Make a copy of a field structure, e.g. for interpolation
//				purposes.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
picstruct* CSextractor::InheritField(picstruct *infield, int flags )
{
	picstruct	*field;

	// First allocate memory for the new field (and nullify pointers)
	QCALLOC(field, picstruct, 1);

	// Copy what is important and reset the remaining
	*field = *infield;
	field->flags = flags;
	CopyAstrom(infield, field);
	QMEMCPY(infield->fitshead, field->fitshead, char, infield->fitsheadsize);
	field->interp_flag = 0;
	field->assoc = NULL;
	field->strip = NULL;
	field->fstrip = NULL;
	field->reffield = infield;
	field->compress_buf = NULL;
	field->compress_type = ICOMPRESS_NONE;
	field->file = NULL;

	return field;
}

////////////////////////////////////////////////////////////////////
// Method:		EndField
// Class:		CSextractor
// Purpose:		Free and close everything related to a field structure.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::EndField( picstruct *field )
{
	if (field->file) fclose(field->file);

	//  free(field->fitshead);
	free(field->strip);
	free(field->fstrip);
	free(field->compress_buf);

	if (field->astrom) EndAstrom(field);
	if (field->interp_flag) EndInterpolate(field);

	EndBack(field);
	free(field);

	return;
}

