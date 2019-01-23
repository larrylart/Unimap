////////////////////////////////////////////////////////////////////
// File:		sextractor_interpolate.cpp
// Package:		SExtractor
// Author:		E.BERTIN (IAP)
// Contents:	Interpolation of input data.
// Last modify:	26/11/2003
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

//#include	"sextractor_define.h"
//#include	"sextractor_globals.h"
//#include	"sextractor_field.h"
//#include	"sextractor_interpolate.h"

//#include "fits/fitscat.h"
// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		InitInterpolate
// Class:		CSextractor
// Purpose:		Init resources required for data interpolation.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::InitInterpolate( picstruct *field, int xtimeout, int ytimeout )
{
	QMALLOC(field->interp_backup, PIXTYPE, field->width);
	// ytimeout < 0 means we don't need a timeout buffer (it won't be used) 
	if (ytimeout>=0)
	{
		QMALLOC(field->interp_ytimeoutbuf, int, field->width);
		memset(field->interp_ytimeoutbuf, 0, (size_t)(field->width*sizeof(int)));
	}

	field->interp_xtimeout = xtimeout;
	field->interp_ytimeout = ytimeout;

	field->interp_flag = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Interpolate
// Class:		CSextractor
// Purpose:		Interpolate (crudely) input data.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Interpolate( picstruct *field, picstruct *wfield,
								PIXTYPE *data, PIXTYPE *wdata )
{
	PIXTYPE	*backup,*wbackup,
	thresh;
	int		*ytimeout,
	xtimeout,xtimeout0,ytimeout0, allflag, i;

	thresh = wfield->weight_thresh;
	backup = field->interp_backup;
	wbackup = wfield->interp_backup;
	ytimeout = wfield->interp_ytimeoutbuf;
	xtimeout0 = wfield->interp_xtimeout;
	ytimeout0 = wfield->interp_ytimeout;
	// Start as if the previous pixel was already interpolated 
	xtimeout = 0;	
	allflag = field->interp_flag;
	for (i=field->width; i--; ytimeout++)
	{
		// Check if interpolation is needed 
		/// It's a variance map: the bigger the worse 
		if (*wdata>=thresh)	
		{
			// Check if the previous pixel was already interpolated 
			if (!xtimeout)
			{
				if (*ytimeout)
				{
					(*ytimeout)--;
					*wdata = *wbackup;
					if (allflag) *data = *backup;
				}

			} else
			{
				xtimeout--;
				*wdata = *(wdata-1);
				if (allflag) *data = *(data-1);
			}

		} else
		{
			xtimeout = xtimeout0;
			*ytimeout = ytimeout0;
		}

		*(wbackup++) = *(wdata++);
		if (allflag) *(backup++) = *(data++);
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		EndInterpolate
// Class:		CSextractor
// Purpose:		Free memory allocated for data interpolation.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::EndInterpolate( picstruct *field )
{
	free(field->interp_backup);
	free(field->interp_ytimeoutbuf);

	return;
}
