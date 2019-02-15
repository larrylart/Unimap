////////////////////////////////////////////////////////////////////
// File:		sextractor_extract.cpp
// Package:		SExtractor
// Author:		E.BERTIN (IAP)
// Contents:	functions for extraction of connected pixels from
//				a bitmap.
// Last modify:	26/11/2003
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<stdio.h>
#include	<math.h>
#include	<stdlib.h>
#include	<string.h>

//#include	"sextractor_define.h"
//#include	"sextractor_globals.h"
//#include	"sextractor_prefs.h"
//#include	"sextractor_extract.h"
//#include	"sextractor_plist.h"

/*------------------------- Static buffers for lutz() -----------------------*/

//static infostruct	*info, *store;
//static char		*marker;
//static status		*psstack;
//static int		*start, *end, *discan, xmin,ymin,xmax,ymax;

//#include "fits/fitscat.h"
// include local header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		LutzAlloc
// Class:		CSextractor
// Purpose:		Allocate once for all memory space for buffers used 
//				by lutz().
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::LutzAlloc( int width, int height )
{
	int	*discant,
	stacksize, i;

	stacksize = width+1;
	xmin = ymin = 0;
	xmax = width-1;
	ymax = height-1;
	QMALLOC(info, infostruct, stacksize);
	QMALLOC(store, infostruct, stacksize);
	QMALLOC(marker, char, stacksize);
	QMALLOC(psstack, status, stacksize);
	QMALLOC(start, int, stacksize);
	QMALLOC(end, int, stacksize);
	QMALLOC(discan, int, stacksize);
	discant = discan;
	for (i=stacksize; i--;) *(discant++) = -1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		LutzFree
// Class:		CSextractor
// Purpose:		Free once for all memory space for buffers used by lutz().
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::LutzFree( )
{
	free(discan);
	free(info);
	free(store);
	free(marker);
	free(psstack);
	free(start);
	free(end);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Lutz
// Class:		CSextractor
// Purpose:		C implementation of R.K LUTZ' algorithm for the extraction 
//				of 8-connected pi-xels in an image
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CSextractor::Lutz( objliststruct *objlistroot, int nroot, 
						objstruct *objparent, objliststruct *objlist )
{
	static infostruct	curpixinfo,initinfo;
	objstruct			*obj, *objroot;
	pliststruct			*plist,*pixel, *plistin, *plistint;

	char				newmarker;
	int					cn, co, luflag, objnb, pstop, xl,xl2,yl,
						out, minarea, stx,sty,enx,eny, step, nobjm = NOBJ,
						inewsymbol, *iscan;
	short				trunflag;
	PIXTYPE				thresh;
	status				cs, ps;

	out = RETURN_OK;

	minarea = prefs.deb_maxarea;
	plistint = plistin = objlistroot->plist;
	objroot = &objlistroot->obj[nroot];
	stx = objparent->xmin;
	sty = objparent->ymin;
	enx = objparent->xmax;
	eny = objparent->ymax;
	thresh = objlist->dthresh;
	initinfo.pixnb = 0;
	initinfo.flag = 0;
	initinfo.firstpix = initinfo.lastpix = -1;
	cn = 0;
	iscan = objroot->submap + (sty-objroot->suby)*objroot->subw + (stx-objroot->subx);
	// As we only analyse a fraction of the map, a step occurs between lines 
	step = objroot->subw - (++enx-stx);
	eny++;

	// Allocate memory to store object data 
	free(objlist->obj);
	if( !(obj=objlist->obj=(objstruct *)malloc(nobjm*sizeof(objstruct))) )
	{
		out = RETURN_FATAL_ERROR;
		// To avoid gcc -Wall warnings 
		plist = NULL;			
		goto exit_lutz;
	}

	// Allocate memory for the pixel list 
	free(objlist->plist);
	if( !(objlist->plist = (pliststruct *)malloc((eny-sty)*(enx-stx)*plistsize)) )
	{
		out = RETURN_FATAL_ERROR;
		// To avoid gcc -Wall warnings 
		plist = NULL;			
		goto exit_lutz;
	}

	pixel = plist = objlist->plist;

	/////////////////////////////////////////////
	for (xl=stx; xl<=enx; xl++) marker[xl] = 0;

	objnb = objlist->nobj = 0;
	co = pstop = 0;
	curpixinfo.pixnb = 1;

	for (yl=sty; yl<=eny; yl++, iscan += step)
	{
		ps = COMPLETE;
		cs = NONOBJECT;
		trunflag =  (yl==0 || yl==ymax) ? OBJ_TRUNC : 0;
		if (yl==eny) iscan = discan;

		for (xl=stx; xl<=enx; xl++)
		{
			newmarker = marker[xl];
			marker[xl] = 0;
			if ((inewsymbol = (xl!=enx)?*(iscan++):-1) < 0)
				luflag = 0;
			else
			{
				curpixinfo.flag = trunflag;
				plistint = plistin+inewsymbol;
				luflag = (PLISTPIX(plistint, cdvalue) > thresh?1:0);
			}

			if (luflag)
			{
				if (xl==0 || xl==xmax) curpixinfo.flag |= OBJ_TRUNC;
				memcpy(pixel, plistint, (size_t)plistsize);
				PLIST(pixel, nextpix) = -1;
				curpixinfo.lastpix = curpixinfo.firstpix = cn;
				cn += plistsize;
				pixel += plistsize;
				// Start Segment 
				if (cs != OBJECT)		
				{
					cs = OBJECT;
					if (ps == OBJECT)
					{
						if (start[co] == UNKNOWN)
						{
							marker[xl] = 'S';
							start[co] = xl;

						} else 
							marker[xl] = 's';

					} else
					{
						psstack[pstop++] = ps;
						marker[xl] = 'S';
						start[++co] = xl;
						ps = COMPLETE;
						info[co] = initinfo;
					}
				}
			}

			///////////////////////////////////
			// Process New Marker
			if (newmarker)
			{
				if (newmarker == 'S')
				{
					psstack[pstop++] = ps;
					if (cs == NONOBJECT)
					{
						psstack[pstop++] = COMPLETE;
						info[++co] = store[xl];
						start[co] = UNKNOWN;

					} else
						Update( &info[co],&store[xl], plist );

					ps = OBJECT;

				} else if (newmarker == 's')
				{
					if ((cs == OBJECT) && (ps == COMPLETE))
					{
						pstop--;
						xl2 = start[co];
						Update( &info[co-1],&info[co], plist );
						if (start[--co] == UNKNOWN)
							start[co] = xl2;
						else
							marker[xl2] = 's';
					}

					ps = OBJECT;

				} else if (newmarker == 'f')
					ps = INCOMPLETE;
				else if (newmarker == 'F')
				{
					ps = psstack[--pstop];
					if ((cs == NONOBJECT) && (ps == COMPLETE))
					{
						if (start[co] == UNKNOWN)
						{
							if ((int)info[co].pixnb >= minarea)
							{
								if (objlist->nobj>=nobjm)
									if (!(obj = objlist->obj = (objstruct *)realloc(obj, (nobjm+=nobjm/2)* sizeof(objstruct))))
									{
										out = RETURN_FATAL_ERROR;
										goto exit_lutz;
									}

								LutzSort(&info[co], objlist);
							}

						} else
						{
							marker[end[co]] = 'F';
							store[start[co]] = info[co];
						}

						co--;
						ps = psstack[--pstop];
					}
				}
			}
  
			////////////////////////////////////////
			if (luflag)
				Update(&info[co],&curpixinfo, plist);
			else
			{
				// End Segment
				if (cs == OBJECT)
				{
					cs = NONOBJECT;
					if (ps != COMPLETE)
					{
						marker[xl] = 'f';
						end[co] = xl;

					} else
					{
						ps = psstack[--pstop];
						marker[xl] = 'F';
						store[start[co]] = info[co];
						co--;
					}
				}
			}
			///////////////////////////////////////////
		}
	}

	// exit label - to replace ??? (larry)
	exit_lutz:

	if (objlist->nobj && out == RETURN_OK)
	{
		if (!(objlist->obj=(objstruct *)realloc(obj, objlist->nobj*sizeof(objstruct))))
			error(EXIT_FAILURE,"problem with mem. realloc. in lutz()","");

	} else
	{
		free(obj);
		objlist->obj = NULL;
	}

	if (cn && out == RETURN_OK)
	{
		if (!(objlist->plist=(pliststruct *)realloc(plist,cn)))
			error(EXIT_FAILURE,"problem with mem. realloc. in lutz()","");

	} else
	{
		free(objlist->plist);
		objlist->plist = NULL;
	}

	return  out;
}

////////////////////////////////////////////////////////////////////
// Method:		LutzSort
// Class:		CSextractor
// Purpose:		Build the object structure.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::LutzSort( infostruct *info, objliststruct *objlist )
{
	objstruct  *obj = objlist->obj+objlist->nobj;

	memset(obj, 0, (size_t)sizeof(objstruct));
	obj->firstpix = info->firstpix;
	obj->lastpix = info->lastpix;
	obj->flag = info->flag;
	objlist->npix += info->pixnb;

	PreAnalyse(objlist->nobj, objlist, ANALYSE_FAST);

	objlist->nobj++;

	return;
}

