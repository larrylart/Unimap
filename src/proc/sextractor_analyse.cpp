////////////////////////////////////////////////////////////////////
// Purpose:	analyse(), endobject()...: measurements on detections.
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

// local headers in C
/*
extern "C"
{

	#include	"sextractor_define.h"
	#include	"sextractor_globals.h"
	#include	"sextractor_prefs.h"
	#include	"fits/fitscat.h"
	#include	"sextractor_back.h"
	#include	"sextractor_check.h"
	#include	"sextractor_assoc.h"
	#include	"sextractor_astrom.h"
	#include	"sextractor_growth.h"
	#include	"sextractor_image.h"
	#include	"sextractor_psf.h"
	#include	"sextractor_retina.h"
	#include	"sextractor_som.h"
	#include	"sextractor_winpos.h"

//	static obj2struct	*obj2 = &outobj2;
}

	#include	"sextractor_plist.h"
	#include	"sextractor_flag.h"
	#include	"sextractor_photom.h"
*/

//extern "C"
//{
//	#include "fits/fitscat.h"
//}
// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		Analyse
// Class:		CSextractor
// Purpose:		analyze my image ???
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::Analyse(picstruct *field, picstruct *dfield, int objnb,
													objliststruct *objlist)
{
	objstruct	*obj = objlist->obj+objnb;

	// Do photometry on the detection image if no other image available 
	obj->number = ++thecat.ndetect;
	obj->bkg = (float) Back( field, (int)(obj->mx+0.5), (int)(obj->my+0.5));
	obj->dbkg = 0.0;

	if (prefs.pback_type == LOCAL)
		LocalBack(field, obj);
	else
		obj->sigbkg = field->backsig;

	ExamineIso( field, dfield, obj, objlist->plist );


////////////////////////////////////////////////////////////////////
// Put here your calls to custom functions related to isophotal measurements.
// Ex:
//
//	compute_myparams(obj); 
//
////////////////////////////////////////////////////////////////////

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ExamineIso
// Class:		CSextractor
// Purpose:		compute some isophotal parameters IN THE MEASUREMENT 
//				image.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::ExamineIso( picstruct *field, picstruct *dfield, 
								objstruct *obj, pliststruct *pixel )
{
	checkstruct		*check;
	pliststruct		*pixt;
	int				i,j,k,h, photoflag,area,errflag, cleanflag,
					pospeakflag, profflag, minarea, gainflag;
	double			tv,sigtv, ngamma,
					esum, emx2,emy2,emxy, err,gain,backnoise2,dbacknoise2,
					xm,ym, x,y,var,var2, profflux,proffluxvar;
	float			*heap,*heapt,*heapj,*heapk, swap;
	PIXTYPE			pix, cdpix, tpix, peak,cdpeak, thresh,dthresh;
	static PIXTYPE	threshs[NISO];

	if (!dfield) dfield = field;

	// Prepare computation of positional error
	esum = emx2 = emy2 = emxy = 0.0;
	if ((errflag=FLAG(obj.poserr_mx2)))
	{
		dbacknoise2 = dfield->backsig*dfield->backsig;
		xm = obj->mx;
		ym = obj->my;

	// to avoid gcc -Wall warnings
	} else
		xm = ym = dbacknoise2 = 0.0;	

	pospeakflag = FLAG(obj.peakx);
	profflag = FLAG(obj.flux_prof);
	gain = prefs.gain;
	ngamma = field->ngamma;
	photoflag = (prefs.detect_type==PHOTO);
	gainflag = PLISTEXIST(var) && prefs.weightgain_flag;

	h = minarea = prefs.ext_minarea;

	// Prepare selection of the heap selection for CLEANing 
	if ((cleanflag = prefs.clean_flag))
	{
		if (obj->fdnpix < minarea)
		{
			obj->mthresh = 0.0;
			cleanflag = 0;
			// to avoid gcc -Wall warnings 
			heapt = heap = NULL;

		} else
		{
			QMALLOC(heap, float, minarea);
			heapt = heap;
		}

	} else
	{
		obj->mthresh = 0.0;
		// to avoid gcc -Wall warnings 
		heapt = heap = NULL;
	}


	// Measure essential isophotal parameters in the measurement image...
	tv = sigtv = profflux = proffluxvar = 0.0;
	var = backnoise2 = field->backsig*field->backsig;
	peak = (float) -BIG;
	cdpeak = (float) -BIG;
	thresh = field->thresh;
	dthresh = dfield->dthresh;
	area = 0;
	for (pixt=pixel+obj->firstpix; pixt>=pixel; pixt=pixel+PLIST(pixt,nextpix))
	{
		pix = PLIST(pixt,value);
		if( pix > peak ) peak = pix;

		cdpix=PLISTPIX(pixt,cdvalue);
		if (pospeakflag && cdpix>cdpeak)
		{
			cdpeak=cdpix;
			obj->peakx =  PLIST(pixt,x) + 1;
			obj->peaky =  PLIST(pixt,y) + 1;
		}

		if (PLISTEXIST(var)) var = PLISTPIX(pixt, var);
		if (photoflag)
		{
			pix = (float) exp(pix/ngamma);
			var2 = pix*pix*var;

		} else
			var2 = var;

		if (gainflag && pix>0.0 && gain>0.0) var2 += pix/gain*var/backnoise2;

		sigtv += var2;

		if (profflag && cdpix>0.0)
		{
			profflux += cdpix*pix;
			proffluxvar += cdpix*var2;
		}

		if (pix>thresh) area++;
		tv += pix;
		if (errflag)
		{
			err = dbacknoise2;
			if (gain>0.0 && cdpix>0.0) err += cdpix/gain;
			x = PLIST(pixt,x) - xm;
			y = PLIST(pixt,y) - ym;
			esum += err;
			emx2 += err*x*x;
			emy2 += err*y*y;
			emxy += err*x*y;
		}

		// Find the minareath pixel in decreasing intensity for CLEANing
		if (cleanflag)
		{
			tpix = PLISTPIX(pixt, cdvalue) - (PLISTEXIST(dthresh) ? PLISTPIX(pixt, dthresh):dthresh);
			if (h>0)
				*(heapt++) = (float)tpix;
			else if (h)
			{
				if ((float)tpix>*heap)
				{
					*heap = (float)tpix;
					for (j=0; (k=(j+1)<<1)<=minarea; j=k)
					{
						heapk = heap+k;
						heapj = heap+j;
						if (k != minarea && *(heapk-1) > *heapk)
						{
							heapk++;
							k++;
						}
						if (*heapj <= *(--heapk)) break;
						swap = *heapk;
						*heapk = *heapj;
						*heapj = swap;
					}
				}

			} else
				Hmedian(heap, minarea);
			h--;
		}
	}

	// Flagging from the flag-map 
	if (PLISTEXIST(flag)) GetFlags(obj, pixel);

	if( cleanflag )
	{
		obj->mthresh = *heap;
		free(heap);
	}

	if( profflag )
	{
		obj->flux_prof = obj->fdflux>0.0? (float)(profflux/obj->fdflux) : 0.0;
		obj->fluxerr_prof = obj->fdflux>0.0? (float)(proffluxvar/obj->fdflux):0.0;
	}

	if( errflag )
	{
		double	flux2;

		flux2 = obj->fdflux*obj->fdflux;
		// Estimation of the error ellipse moments: we re-use previous variables 
		emx2 /= flux2;	/* variance of xm */
		emy2 /= flux2;	/* variance of ym */
		emxy /= flux2;	/* covariance */

		// Handle fully correlated profiles (which cause a singularity...) 
		esum *= 0.08333/flux2;
		if (obj->singuflag && (emx2*emy2-emxy*emxy) < esum*esum)
		{
			emx2 += esum;
			emy2 += esum;
		}

		obj->poserr_mx2 = emx2;
		obj->poserr_my2 = emy2;
		obj->poserr_mxy = emxy;
	}
 
	if (photoflag)
	{
		tv = ngamma*(tv-obj->fdnpix*exp(obj->dbkg/ngamma));
		sigtv /= ngamma*ngamma;

	} else
	{
		tv -= obj->fdnpix*obj->dbkg;
		if (!gainflag && gain > 0.0 && tv>0.0) sigtv += tv/gain;
	}

	obj->npix = area;
	obj->flux = tv;
	obj->fluxerr = sigtv;
	obj->peak = peak;

	obj->thresh -= obj->dbkg;
	obj->peak -= obj->dbkg;

	// Initialize isophotal thresholds so as to sample optimally the full profile
	if (FLAG(obj.iso[0]))
	{
		int	*iso;
		PIXTYPE	*thresht;

		memset(obj->iso, 0, NISO*sizeof(int));
		if (prefs.detect_type == PHOTO)
		{
			for (i=0; i<NISO; i++)
				threshs[i] = obj->thresh + (obj->peak-obj->thresh)*i/NISO;
		} else
		{
			if (obj->peak>0.0 && obj->thresh>0.0)
			{
				for (i=0; i<NISO; i++)
					threshs[i] = obj->thresh*pow( (double) obj->peak/obj->thresh, (double)i/NISO );
			} else
			{
				for (i=0; i<NISO; i++) threshs[i] = 0.0;
			}
		}

		for (pixt=pixel+obj->firstpix; pixt>=pixel; pixt=pixel+PLIST(pixt,nextpix))
			for (i=NISO,iso=obj->iso,thresht=threshs; i-- && PLIST(pixt,value)>*(thresht++);)
				(*(iso++))++;
	}

	// Put objects in "segmentation check-image" 
	if ((check = prefs.check[CHECK_SEGMENTATION]))
		for (pixt=pixel+obj->firstpix; pixt>=pixel; pixt=pixel+PLIST(pixt,nextpix))
			((ULONG *)check->pix)[check->width*PLIST(pixt,y)+PLIST(pixt,x)] = (ULONG)obj->number;

	if ((check = prefs.check[CHECK_OBJECTS]))
		for (pixt=pixel+obj->firstpix; pixt>=pixel; pixt=pixel+PLIST(pixt,nextpix))
			((PIXTYPE *)check->pix)[check->width*PLIST(pixt,y)+PLIST(pixt,x)] = PLIST(pixt,value);

	// Compute the FWHM of the object 
	if (FLAG(obj.fwhm))
	{
		PIXTYPE	thresh0;

		thresh0 = obj->peak/5.0;
		if (thresh0<obj->thresh) thresh0 = obj->thresh;
		if (thresh0>0.0)
		{
			double	mx,my, s,sx,sy,sxx,sxy, dx,dy,d2, lpix,pix, b, inverr2, sat,
			dbkg, d, bmax;

			mx = obj->mx;
			my = obj->my;
			dbkg = obj->dbkg;
			sat = (double)(prefs.satur_level - obj->bkg);
			s = sx = sy = sxx = sxy = 0.0;
			for (pixt=pixel+obj->firstpix;pixt>=pixel;pixt=pixel+PLIST(pixt,nextpix))
			{
				pix = PLIST(pixt,value)-dbkg;
				if (pix>thresh0 && pix<sat)
				{
					dx = PLIST(pixt,x) - mx;
					dy = PLIST(pixt,y) - my;
					lpix = log(pix);
					inverr2 = pix*pix;
					s += inverr2;
					d2 = dx*dx+dy*dy;
					sx += d2*inverr2;
					sxx += d2*d2*inverr2;
					sy += lpix*inverr2;
					sxy += lpix*d2*inverr2;
				}        
			}

			d = s*sxx-sx*sx;
			if (fabs(d)>0.0)
			{
				b = -(s*sxy-sx*sy)/d;
				// to have FWHM # 6 sigma 
				if (b<(bmax = 1/(13*obj->a*obj->b))) b = bmax;
				obj->fwhm = (float)(1.6651/sqrt(b));
				// correction for undersampling effects (established from simulations) 
				if (obj->fwhm>0.0) obj->fwhm -= 1/(4*obj->fwhm);

			} else
				obj->fwhm = 0.0;

		} else
			obj->fwhm = 0.0;
	}

	return;
}
