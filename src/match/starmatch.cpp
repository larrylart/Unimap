////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

//system headers
#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <string.h>
#include <math.h>

// local headers in C
extern "C"
{
	#include "misc.h"
	#include "atpmatch.h"
}

// local headers
#include "../util/func.h"
#include "../sky/star.h"
//#include "../logger/logger.h"
#include "../config/mainconfig.h"
#include "../unimap_worker.h"

// main header
#include "starmatch.h"

////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CStarMatch
// Purpose:		build my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CStarMatch::CStarMatch( )
{
	m_pUnimapWorker = NULL;

	m_pTrans = NULL;
	m_pLastTrans = NULL;
	m_pBestTrans = NULL;
	m_pInvTrans = NULL;

	DefaultParam( );
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CStarMatch
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CStarMatch::~CStarMatch( )
{
	CloseMatch( );

	m_pUnimapWorker = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:		CloseMatch
// Class:		CStarMatch
// Purpose:		destroy my object's bellongings
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::CloseMatch( )
{
	// larry :: dealocate this transport array's
	FreeStarArray( star_array_J );
	FreeStarArray( star_array_K );
	star_array_J = NULL;
	star_array_K = NULL;

	if( m_pTrans ) free( m_pTrans );
	if( m_pBestTrans ) free( m_pBestTrans );
	if( m_pInvTrans ) free( m_pInvTrans );
	if( m_pLastTrans ) free( m_pLastTrans );

	m_pLastTrans =  NULL;
	m_pTrans = NULL;
	m_pBestTrans = NULL;
	m_pInvTrans = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		DefaultParam
// Class:		CStarMatch
// Purpose:		reset params to default values
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::DefaultParam( )
{
	m_nMaxMatch = 0;

	max_iter = AT_MATCH_MAXITER;
	trans_order = AT_TRANS_LINEAR;
	//trans_order = AT_TRANS_QUADRATIC;
	//trans_order = AT_TRANS_CUBIC;

	triangle_radius = AT_TRIANGLE_RADIUS;  
	match_radius = AT_MATCH_RADIUS;       
	scale = -1.0;                          
	min_scale = -1.0;                          
	max_scale = -1.0;                          
	halt_sigma = AT_MATCH_HALTSIGMA_CPP;
	medsigclip = 0.0;                     
	xshift = 0.0;                        
	yshift = 0.0;                        
	nobj = AT_MATCH_NBRIGHT;
	transonly = 0;                           
	recalc = 0;                            
	num_matches = 0;                        
	medtf_flag = 0;                         
	intrans = 0;                           
	identity = 0;
	
	outfile[CMDBUFLENGTH] = '\0';
	intransfile[CMDBUFLENGTH] = '\0';

	sprintf( outfile, "larstar" );

	///////////////////////
	// larry port ... get rid of these files
	star_array_J = (s_star *) shMalloc(DEFAULT_MAX_NO_OF_USED_STARS*sizeof(s_star));
	star_array_K = (s_star *) shMalloc(DEFAULT_MAX_NO_OF_USED_STARS*sizeof(s_star));

	m_strFoundStar = wxT("");
}

////////////////////////////////////////////////////////////////////
// Method:		SetTransType
// Class:		CStarMatch
// Purpose:		set a new trasforma type
// Input:		new type of trans
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::SetTransType( int nType )
{
	trans_order = nType;
	if( m_pTrans ) m_pTrans->order = trans_order;
	if( m_pLastTrans ) m_pLastTrans->order = trans_order;
	if( m_pBestTrans ) m_pBestTrans->order = trans_order;
	if( m_pInvTrans ) m_pInvTrans->order = trans_order;
}

/////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CStarMatch
// Purpose:	set the object with current config values
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CStarMatch::SetConfig( CConfigMain* pConfig )
{
	int nVarInt = 0;
	double nVarFloat = 0;

	//////////////////////////////////////////////////////////////
	// CONFIG :: from matching section
	// get transaction type
	SetTransType( pConfig->m_nMatchTransType+1 );
	// get if to recalculate
	recalc = pConfig->m_bMatchDoRecalc;
	// get max distance
	match_radius = pConfig->m_nMatchMaxDist;
	// triangle fit distance
	triangle_radius = pConfig->m_nMatchTriangFit;
	// triangle scale
	scale = pConfig->m_nMatchTriangScale;
	// max iterations
	max_iter = pConfig->m_nMatchMaxIter;
	// halt sigma
	halt_sigma = pConfig->m_nMatchHaltSigma;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Init
// Class:		CStarMatch
// Purpose:		Initialize object with param
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::Init( )
{	
	int nStatus = 1;


	// We need to check the validity of the arguments.
	// We can only calculate _clipped_ MEDTF statistics if we calculate
	// the regular ones.  So it makes no sense to specify 'medsigclip',
	// but not 'medtf'.
	if ((medtf_flag == 0) && (medsigclip != 0.0))
	{
		m_pUnimapWorker->Log( wxT("WARNING :: medsigclip requires the 'medtf' option"), GUI_LOGGER_ATTENTION_STYLE );
		m_pUnimapWorker->Log( wxT("WARNING :: Setting medtf "), GUI_LOGGER_ATTENTION_STYLE );
		medtf_flag = 1;
	}
	// Impossible to calculation the MEDTF statistics unless we match
	// up stars after finding the TRANS.  So, if the user does want
	// the MEDTF stats, we force the necessary step.
	if ((medtf_flag == 1) && (recalc == 0))
	{
		m_pUnimapWorker->Log( wxT("WARNING: 'medtf' requires the 'recalc' option"), GUI_LOGGER_ATTENTION_STYLE );
		m_pUnimapWorker->Log( wxT("WARNING: Setting recalc "), GUI_LOGGER_ATTENTION_STYLE );
		recalc = 1;
	}
	// Check to make sure that the user did exactly one of the following:
	//    a. did not specify "scale" or "min_scale" or "max_scale"
	//    b. did specify "scale" only
	//    c. did specify "min_scale" and "max_scale", but not "scale"
	//
	// If choice b., then translate the single "scale" value into
	// a pair of "min_scale" and "max_scale" limits.  We'll always
	// pass these limits to the matching procedures.
	if ((scale == -1) && (min_scale == -1) && (max_scale == -1))
	{
		// okay 
		;
	} else if ((scale != -1) && (min_scale == -1) && (max_scale == -1))
	{
		// okay 
		min_scale = scale - (0.01*AT_MATCH_PERCENT*scale);
		max_scale = scale + (0.01*AT_MATCH_PERCENT*scale);

	} else if ((scale == -1) && (min_scale != -1) && (max_scale != -1)) 
	{
		// okay 
		if (min_scale > max_scale)
		{
			m_pUnimapWorker->Log( wxT("ERROR :: min_scale must be smaller than max_scale"), GUI_LOGGER_ATTENTION_STYLE );
			nStatus = 0;
		}

	} else
	{
		// not okay 
		m_pUnimapWorker->Log( wxT("ERROR :: invalid combination of 'scale', 'min_scale', 'max_scale'"), GUI_LOGGER_ATTENTION_STYLE );
		nStatus = 0;
	}

#ifdef DEBUG
	if ((scale == -1) && (min_scale == -1) && (max_scale == -1))
	{
		m_pUnimapWorker->Log( "DEBUG :: no limits set on relative scales for matching.", GUI_LOGGER_ATTENTION_STYLE );

	} else
	{
		m_pUnimapWorker->Log( "DEBUG :: using min_scale %f  max_scale %f", min_scale, max_scale );
	}
#endif
		
	/* Can only specify one of 'identity' and 'intrans' */
	if ((intrans != 0) && (identity != 0))
	{
		m_pUnimapWorker->Log( wxT("ERROR :: cannot specify both 'identity' and an input TRANS file"), GUI_LOGGER_ATTENTION_STYLE );
		nStatus = 0;
	}

	// Makes no sense to specify 'identity' and 'quadratic' or 'cubic'
	if ((identity != 0) && (trans_order != AT_TRANS_LINEAR))
	{
		m_pUnimapWorker->Log( wxT("ERROR :: cannot specify both 'identity' and any order beyond linear"), GUI_LOGGER_ATTENTION_STYLE );
		nStatus = 0;
	}
 
	// Likewise, makes no sense to specify 'intrans=' and 
	// 'quadratic' or 'cubic' (or 'linear', actually, but we have
	// no easy way to check for that mistake :-(
	if ((intrans != 0) && (trans_order != AT_TRANS_LINEAR))
	{
		m_pUnimapWorker->Log( wxT("ERROR :: cannot specify both 'intrans' and any order"), GUI_LOGGER_ATTENTION_STYLE );
		nStatus = 0;
	}

	// There are three possibilities for the initial TRANS values:
	//
	//   1. The user specified "identity", so we start with a linear TRANS
	//      which has no scale change or rotation; it has no translation,
	//      unless the user gave 'xsh' and 'ysh'.
	//
	//   2. The user specified "intrans=file", in which case we read
	//      the initial TRANS values from the given file.
	//
	//   3. The user didn't give us either, so we start with an empty
	//      TRANS and will have to find one ourselves via atFindTrans().
	//
	// In either case 1 or 2, we will end up with a non-zero initial
	// TRANS structure -- which means that we want to use IT to find
	// matches, instead of atFindTrans().  We'll therefore set the 'intrans'
	// flag to 1, to indicate "don't try to find a TRANS, just use
	// the one the user provided".
	if (identity == 1)
	{
		m_pTrans = getIdentityTrans();
		trans_order = m_pTrans->order;

		if (xshift != 0.0)
		{
			m_pTrans->a = xshift;
		}

		if (yshift != 0.0)
		{
			m_pTrans->d = yshift;
		}

		intrans = 1;

	} else if( intrans == 1 )
	{
		if( (m_pTrans = getGuessTrans(intransfile)) == NULL )
		{
			m_pUnimapWorker->Log( wxT("ERROR :: GetGuessTrans fails -- must give up"), GUI_LOGGER_ATTENTION_STYLE );
		}
		trans_order = m_pTrans->order;

	} else
	{
		// this will be an "empty" TRANS; atFindTrans will try to fill it 
		atTransOrderSet( trans_order );
		m_pTrans = atTransNew();
		m_pTrans->order = trans_order;
	}
	// create copy trans
	if( !m_pLastTrans ) 
		m_pLastTrans = atTransNew();
	// create also the best trans
	m_pBestTrans = atTransNew();
	m_pBestTrans->order = trans_order;
	// create the inverse trans
	m_pInvTrans = atTransNew();
	m_pInvTrans->order = trans_order;


#ifdef DEBUG
   m_pUnimapWorker->Log( "DEBUG :: using trans_order %d\n", trans_order );
#endif

	return( nStatus );
}


////////////////////////////////////////////////////////////////////
// Method:		ResetTrans
// Class:		CStarMatch
// Purpose:		reset a trans to default values
// Input:		pointer to trans
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::ResetTrans( TRANS* pTrans )
{
	// what's this ? dosen't make much sense ...
	static int id_number = 0;

	pTrans->id = id_number++;
	pTrans->order = atTransOrderGet();
	pTrans->nr = 0;
	pTrans->nm = 0;
	pTrans->sig = 0.0;
	pTrans->sx = 0.0;
	pTrans->sy = 0.0;
	// don't know why this is so ??? i got it myself from ini phase
	m_pTrans->order = trans_order;

	pTrans->a = 0;
	pTrans->b = 0;
	pTrans->c = 0;
	pTrans->d = 0;
	pTrans->e = 0;
	pTrans->f = 0;
	pTrans->g = 0;
	pTrans->h = 0;
	pTrans->i = 0;
	pTrans->j = 0;
	pTrans->k = 0;
	pTrans->l = 0;
	pTrans->m = 0;
	pTrans->n = 0;
	pTrans->o = 0;
	pTrans->p = 0;
 
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		CopyTrans
// Class:		CStarMatch
// Purpose:		reset a trans to default values
// Input:		pointer to trans
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::CopyTrans( TRANS* pSrcTrans, TRANS* pDstTrans )
{
	pDstTrans->id = pSrcTrans->id;
	pDstTrans->order = pSrcTrans->order;
	pDstTrans->nr = pSrcTrans->nr;
	pDstTrans->nm = pSrcTrans->nm;
	pDstTrans->sig = pSrcTrans->sig;
	pDstTrans->sx = pSrcTrans->sx;
	pDstTrans->sy = pSrcTrans->sy;

	pDstTrans->a = pSrcTrans->a;
	pDstTrans->b = pSrcTrans->b;
	pDstTrans->c = pSrcTrans->c;
	pDstTrans->d = pSrcTrans->d;
	pDstTrans->e = pSrcTrans->e;
	pDstTrans->f = pSrcTrans->f;
	pDstTrans->g = pSrcTrans->g;
	pDstTrans->h = pSrcTrans->h;
	pDstTrans->i = pSrcTrans->i;
	pDstTrans->j = pSrcTrans->j;
	pDstTrans->k = pSrcTrans->k;
	pDstTrans->l = pSrcTrans->l;
	pDstTrans->m = pSrcTrans->m;
	pDstTrans->n = pSrcTrans->n;
	pDstTrans->o = pSrcTrans->o;
	pDstTrans->p = pSrcTrans->p;
 
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeStarList
// Class:		CStarMatch
// Purpose:		Read a start list from external format in match format
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::FreeStarList( s_star *list )
{
	s_star* pStar;
	s_star* pNextStar;

	pStar = list;
	pNextStar = pStar;
	while( pStar && pStar != NULL )
	{
		// first get the next star
		pNextStar = pStar->next;
		// then free alloc for current
		shFree( pStar );
		// then get next in current
		pStar = pNextStar;
	}
	// now that everything is done reset pointer
	list = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:		ReadStarList
// Class:		CStarMatch
// Purpose:		Read a start list from external format in match format
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::ReadStarList( StarDef* vectStar, int nStar, 
							 struct s_star **list , int& num_stars, int nMatchFlag )
{
	struct s_star *headStar, *lastStar, *newStar;
	int num = 0, i = 0;

	headStar = (struct s_star *) NULL;
	lastStar = headStar;

	for( i=0; i<nStar; i++ )
	{
		// we check to include if we have flag for match
		if( nMatchFlag == 0 || vectStar[i].match == 1 )
		{
			// create new star 
			newStar = atStarNew(vectStar[i].x, vectStar[i].y, vectStar[i].mag );
			newStar->id = nUniqueIdCount;
			nUniqueIdCount++;
			// check if i'am at the begining of the list
			if( headStar == NULL )
			{
				headStar = newStar;
				lastStar = newStar;
			} else
			{
				lastStar->next = newStar;
				lastStar = newStar;
			}
			// increment counter
			num++;
		}
	}

	// check if any
	if( num > 0 )
	{
		// set last pointer to null
		lastStar->next = NULL;
	} else
	{
		lastStar = NULL;
	}

	num_stars = num;
	*list = headStar;

	return( 1 );
}



////////////////////////////////////////////////////////////////////
// Method:		CloneStarList
// Class:		CStarMatch
// Purpose:		duplicate a star list
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
struct s_star* CStarMatch::CloneStarList( struct s_star *vectStar )
{
	struct s_star *pStar, *newStar, *firstStar, *prevStar;

	firstStar = (struct s_star *) NULL;
	prevStar = (struct s_star *) NULL;
	newStar = (struct s_star *) NULL;

	pStar = vectStar;
	while( pStar )
	{
		// create new star 
		newStar = atStarNew( pStar->x, pStar->y, pStar->mag );
		newStar->id = pStar->id;
	
		// if first star null set 
		if( firstStar == NULL ) 
		{
			// set current star to be the first 
			firstStar = newStar;
		} else
		{
			// set previous star to point to this one
			prevStar->next = newStar;
		}

		// set also the next element
		prevStar = newStar;

		// got to the next
		pStar = pStar->next;
	}
	// set last to point to null
	if( newStar != NULL ) 
		newStar->next = NULL;

	return( firstStar );
}

////////////////////////////////////////////////////////////////////
// Method:		ApplyObjInv
// Class:		CStarMatch
// Purpose:		apply inverse transform for an abject (star, dso, etc)
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::ApplyObjInv( StarDef& star )
{
/*
	// larry's version of inverse trans
	double nOldX = star.x;
	double nOldY = star.y;
	star.y = (m_pBestTrans->e*(nOldX-m_pBestTrans->a)-m_pBestTrans->b*(nOldY-m_pBestTrans->d))/(m_pBestTrans->e*m_pBestTrans->c-m_pBestTrans->b*m_pBestTrans->f);
	star.x = (m_pBestTrans->f*(nOldX-m_pBestTrans->a)+m_pBestTrans->c*(nOldY-m_pBestTrans->d))/(m_pBestTrans->b*m_pBestTrans->f-m_pBestTrans->e*m_pBestTrans->c);
*/

	s_star cstar;
	s_star *sp;
	double newx=0, newy=0;
   
	cstar.x = star.x;
	cstar.y = star.y;

	sp = &cstar;

    if( calc_trans_coords(sp, m_pInvTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans has failed") );

         return( 0 ); 
	}

	star.x = newx;
	star.y = newy;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CalcPointTrans
// Class:		CStarMatch
// Purpose:		calculate x, y best trans
// Input:		reference to x, y double coord
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::CalcPointTrans( double& x, double& y )
{
	s_star cstar;
	s_star *sp;
	double newx=0, newy=0;
   
	cstar.x = x;
	cstar.y = y;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
	if( calc_trans_coords(sp, m_pLastTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: could not calculate best trans for point") );

         return( 0 ); 
	}

	x = newx;
	y = newy;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CalcInvPointTrans
// Class:		CStarMatch
// Purpose:		calculate x, y best trans
// Input:		reference to x, y double coord
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::CalcInvPointTrans( double& x, double& y )
{
	s_star cstar;
	s_star *sp;
	double newx=0, newy=0;
   
	cstar.x = x;
	cstar.y = y;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
	if( calc_trans_coords(sp, m_pInvTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: could not calculate best trans for point") );

         return( 0 ); 
	}

	x = newx;
	y = newy;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CalcWindowTrans
// Class:		CStarMatch
// Purpose:		calculate window (x1,y1) (x2,y2) inverse
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::CalcWindowTrans( double& x1, double& y1, double& x2, double& y2,
								double& x3, double& y3, double& x4, double& y4 )
{
	s_star cstar;
	s_star *sp;
	double newx=0, newy=0;
   
	cstar.x = x1;
	cstar.y = y1;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pLastTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point1 has failed") );

         return( 0 ); 
	}

	x1 = newx;
	y1 = newy;

	// now point 2
	newx=0;
	newy=0;
	cstar.x = x2;
	cstar.y = y2;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pLastTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point2 has failed") );

         return( 0 ); 
	}

	x2 = newx;
	y2 = newy;

	// now point 3
	newx=0;
	newy=0;
	cstar.x = x3;
	cstar.y = y3;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pLastTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point3 has failed") );

         return( 0 ); 
	}

	x3 = newx;
	y3 = newy;

	// now point 4
	newx=0;
	newy=0;
	cstar.x = x4;
	cstar.y = y4;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pLastTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point4 has failed") );

         return( 0 ); 
	}

	x4 = newx;
	y4 = newy;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CalcInvWindowTrans
// Class:		CStarMatch
// Purpose:		calculate inverse window (x1,y1) (x2,y2) inverse
// Input:		window coords
// Output:		status
////////////////////////////////////////////////////////////////////
int CStarMatch::CalcInvWindowTrans( double& x1, double& y1, double& x2, double& y2,
								double& x3, double& y3, double& x4, double& y4 )
{
	s_star cstar;
	s_star *sp;
	double newx=0, newy=0;
   
	cstar.x = x1;
	cstar.y = y1;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pInvTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point1 has failed") );

         return( 0 ); 
	}

	x1 = newx;
	y1 = newy;

	// now point 2
	newx=0;
	newy=0;
	cstar.x = x2;
	cstar.y = y2;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pInvTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point2 has failed") );

         return( 0 ); 
	}

	x2 = newx;
	y2 = newy;

	// now point 3
	newx=0;
	newy=0;
	cstar.x = x3;
	cstar.y = y3;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pInvTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point3 has failed") );

         return( 0 ); 
	}

	x3 = newx;
	y3 = newy;

	// now point 4
	newx=0;
	newy=0;
	cstar.x = x4;
	cstar.y = y4;

	sp = &cstar;

//    if( calc_trans_coords(sp, m_pBestTrans, &newx, &newy) != SH_SUCCESS )
    if( calc_trans_coords(sp, m_pInvTrans, &newx, &newy) != SH_SUCCESS )
	{
         m_pUnimapWorker->Log( wxT("ERROR :: apply inverse trans for point4 has failed") );

         return( 0 ); 
	}

	x4 = newx;
	y4 = newy;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CalcInv
// Class:		CStarMatch
// Purpose:		compare two list of stars
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::CalcInv( StarDef* vectStarA, int nStarA, StarDef* vectStarB, 
						int nStarB )
{
//	recalc = nRecalc;
	recalc = 1;
//	max_iter = 6;
	// AT_TRANS_QUADRATIC or AT_TRANS_CUBIC
//	trans_order = AT_TRANS_CUBIC;
//	m_pInvTrans->order = AT_TRANS_CUBIC;
	// main block no ini
	wxString strLog;
	int i = 0;
	int nReturnStatus = 1;

	int ret = 0;
	int numA=0, numB=0;
	int num_matched_A=0, num_matched_B=0;
	int numA_copy=0;
	struct s_star *star_list_A = NULL, *star_list_B = NULL;
	struct s_star *star_list_A_copy = NULL;
	struct s_star *matched_list_A = NULL, *matched_list_B = NULL;
	MEDTF *medtf;
	
	num_stars_J = 0;
	num_stars_K = 0;
	nUniqueIdCount = 0;

	// read first list of stars - this will be from the image
	ReadStarList( vectStarA, nStarA, &star_list_A, numA, 1 ); // - this was the orig version
	//ReadStarList( vectStarA, nStarA, &star_list_A, numA, 0 );

	// make a second calculation of the TRANS, using only objects in
	star_list_A_copy = CloneStarList( star_list_A );
	numA_copy = numA;

	// read the second list of stars - this will be from the catalog
	ReadStarList( vectStarB, nStarB, &star_list_B, numB, 1 ); // - this was the orig version
	// ReadStarList( vectStarB, nStarB, &star_list_B, numB, 0 );

	// reset the trans in any case first :: larry
	ResetTrans( m_pInvTrans );

	//////////////////////////////////////////////
	// Now, if the has has not given us an initial TRANS structure, we need
	// to find one ourselves. 
	if( intrans == 0 )
	{
		// and anly now try to find trans
		ret = atFindTrans( numA, star_list_A, numB, star_list_B,
							triangle_radius, nobj, min_scale, max_scale, 
							max_iter, halt_sigma, m_pInvTrans );
		if (ret != SH_SUCCESS)
		{
//			m_pUnimapWorker->Log( "WARNING :: initial call to atFindTrans fails", GUI_LOGGER_ATTENTION_STYLE );
			nReturnStatus = 0;
		}
	}

	// having found (or been given) the TRANS that takes A -> B, let us apply
	atApplyTrans( numA, star_list_A, m_pInvTrans );

	// now match up the two sets of items, and find four subsets:
	atMatchLists( numA, star_list_A, numB, star_list_B, 
					match_radius, outfile, &num_matches, 
					star_array_J, &num_stars_J, star_array_K, &num_stars_K );
	m_pInvTrans->nm = num_matches;

	// Now, we want to improve the initial TRANS, whether it was supplied
	// need to send trans to prepare_to_recalc because it adds sdx,sdy to it
	if( PrepareToRecalc(outfile, &num_matched_A, &matched_list_A, 
		&num_matched_B, &matched_list_B, star_list_A_copy, m_pInvTrans ) != 0)
	{
//		m_pUnimapWorker->Log( "WARNING :: prepare_to_recalc fails", GUI_LOGGER_ATTENTION_STYLE );
		nReturnStatus = 0;
	}
	// okay, now we're ready to call atRecalcTrans, on matched items only 
	if( RecalcTrans(num_matched_A, matched_list_A, num_matched_B, matched_list_B, 
								max_iter, halt_sigma, m_pInvTrans ) != SH_SUCCESS ) 
	{
//		m_pUnimapWorker->Log( "WARNING :: atRecalcTrans fails on matched pairs only", GUI_LOGGER_ATTENTION_STYLE );
		nReturnStatus = 0;

	}

	// At this point, we have a TRANS which is based solely on those items
	if( recalc == 1 )
	{
		// free what was allocated
		FreeStarList( matched_list_A );
		FreeStarList( matched_list_B );

		// re-set coords of all items in star A 
		if( ResetACoords(numA, star_list_A, star_list_A_copy) != 0 )
		{
			m_pUnimapWorker->Log( wxT("WARNING :: reset_A_coords returns with error before recalc"), GUI_LOGGER_ATTENTION_STYLE );
			nReturnStatus = 0;
		}

		// apply the current TRANS (which is probably much better than
		// the initial TRANS) to all items in list A
		atApplyTrans( numA, star_list_A, m_pInvTrans );

		// Match items in list A to those in list B
		atMatchLists( numA, star_list_A, numB, star_list_B, match_radius,
						outfile, &num_matches, 
						star_array_J, &num_stars_J, star_array_K, &num_stars_K );
		m_pInvTrans->nm = num_matches;

		// prepare to call atRecalcTrans one last time 
		// need to send trans to prepare_to_recalc because it adds sdx,sdy 
		if( PrepareToRecalc(outfile, &num_matched_A, &matched_list_A, 
			&num_matched_B, &matched_list_B, star_list_A_copy, m_pInvTrans ) != 0 )
		{
//			m_pUnimapWorker->Log( "WARNING :: prepare_to_recalc fails", GUI_LOGGER_ATTENTION_STYLE );
			nReturnStatus = 0;
		}

		// final call atRecalcTrans, on matched items only 
		if( RecalcTrans(num_matched_A, matched_list_A, num_matched_B, matched_list_B, 
									max_iter, halt_sigma, m_pInvTrans ) != SH_SUCCESS)
		{
//			m_pUnimapWorker->Log( "WARNING :: atRecalcTrans fails on matched pairs only", GUI_LOGGER_ATTENTION_STYLE );
			nReturnStatus = 0;
		}

	}


	///////////////////////////////////////////////////////////
	// Larry :: display the number of matches
	strLog.Printf( wxT("DEBUG :: InvTrans max no match=%d from %d fit=%f"), 
					m_pInvTrans->nm, m_pInvTrans->nr, m_pInvTrans->sig );
	m_pUnimapWorker->Log( strLog );

	// resert star_array J/K
	for( i=0; i<DEFAULT_MAX_NO_OF_USED_STARS; i++ )
	{
		// J
		star_array_J[i].id=-1;
		star_array_J[i].index=-1;
		star_array_J[i].mag = 0;
		star_array_J[i].match_id = -1;
		star_array_J[i].next = NULL;
		star_array_J[i].x = 0;
		star_array_J[i].y = 0;
		// k
		star_array_K[i].id=-1;
		star_array_K[i].index=-1;
		star_array_K[i].mag = 0;
		star_array_K[i].match_id = -1;
		star_array_K[i].next = NULL;
		star_array_K[i].x = 0;
		star_array_K[i].y = 0;

	}
	num_stars_J = 0;
	num_stars_K = 0;

	FreeStarList( matched_list_A );
	FreeStarList( matched_list_B );

	FreeStarList( star_list_A );
	FreeStarList( star_list_A_copy );
	FreeStarList( star_list_B );

	return( nReturnStatus );
}

////////////////////////////////////////////////////////////////////
// Method:		Cmp
// Class:		CStarMatch
// Purpose:		compare two list of stars
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CStarMatch::Cmp( StarDef* vectStarA, int nStarA, StarDef* vectStarB, 
						int nStarB, int nRecalc )
{
//	recalc = nRecalc;
	recalc = 1;
	// main block no ini
	wxString strLog;
	int i = 0;
	int nReturnStatus = 0;

	int ret = 0;
	int numA=0, numB=0;
	int num_matched_A=0, num_matched_B=0;
	int numA_copy=0;
	struct s_star *star_list_A = NULL, *star_list_B = NULL;
	struct s_star *star_list_A_copy = NULL;
	struct s_star *matched_list_A = NULL, *matched_list_B = NULL;
	MEDTF *medtf;
	
	num_stars_J = 0;//nStarA;
	num_stars_K = 0;//nStarB;
	nUniqueIdCount = 0;

	// read first list of stars - this will be from the image
	ReadStarList( vectStarA, nStarA, &star_list_A, numA, 0 );

	// We always (whether given initial TRANS or not) will 
	// make a second calculation of the TRANS, using only objects in
	// the set of matched pairs.  
	// As input to this second calculation, we will need items
	// from list A with their original coordinates.  
	// Therefore, we now create a copy of the stars in set A,
	// so that we can restore the output matched coords 
	// (which have been converted to those in set B) with the original coords.
//	ReadStarList( vectStarA, nStarA, &star_list_A_copy, numA_copy );
	// instead clode with new method
	star_list_A_copy = CloneStarList( star_list_A );
	numA_copy = numA;

	// reset the 'id' field values in the star_list_A_copy so that they
	// match the 'id' field values in their counterparts in star_list_A
//	ResetCopyIds( numA, star_list_A, star_list_A_copy );

	// read the second list of stars - this will be from the catalog
	ReadStarList( vectStarB, nStarB, &star_list_B, numB, 0 );

	if( nRecalc == 1 )
	{
		// copy trans from best trans
		CopyTrans( m_pBestTrans, m_pTrans );

	} else
	{
		// reset the trans in any case first :: larry
		ResetTrans( m_pTrans );
	}

	//////////////////////////////////////////////
	// Now, if the has has not given us an initial TRANS structure, we need
	// to find one ourselves. 
	if( intrans == 0 && nRecalc == 0 )
	{
		// and anly now try to find trans
		ret = atFindTrans( numA, star_list_A, numB, star_list_B,
							triangle_radius, nobj, min_scale, max_scale, 
							max_iter, halt_sigma, m_pTrans );
		if (ret != SH_SUCCESS)
		{
//			m_pUnimapWorker->Log( "WARNING :: initial call to atFindTrans fails", GUI_LOGGER_ATTENTION_STYLE );
		}
	}

#ifdef DEBUG
	m_pUnimapWorker->Log("DEBUG :: using trans_order %d.",trans_order);
	m_pUnimapWorker->Log("DEBUG :: Initial trans structure:");
	PrintTrans( trans );
#endif

	// if the "transonly" flag is set, stop at this point
	if( transonly == 1 )
	{
		PrintTrans( m_pTrans );
		return(0);
	}

	// having found (or been given) the TRANS that takes A -> B, let us apply
	// it to all the elements in A; thus, we'll have two sets of
	// of stars, each in the same coordinate system
	atApplyTrans( numA, star_list_A, m_pTrans );


	// now match up the two sets of items, and find four subsets:
	//
	//     those from list A that do     have matches in list B
	//     those from list B that do     have matches in list A
	//     those from list A that do NOT have matches in list B
	//     those from list B that do NOT have matches in list A
	// 
	// We may use the two sets of matched objects for further processing,
	// so we put the names of the files containing those matched objects
	// into 'matched_file_A' and 'matched_file_B' for easy reference.
	atMatchLists( numA, star_list_A, numB, star_list_B, 
					match_radius, outfile, &num_matches, 
					star_array_J, &num_stars_J, star_array_K, &num_stars_K );
	m_pTrans->nm = num_matches;

	// Okay, there are two possibilities at this point:
	//
	//    1. The user gave us information about the initial TRANS,
	//       either via 'intrans=' or 'identity'.  We have applied
	//       his initial TRANS to the input list A, and looked for
	//       matched pairs.
	//
	//    2. The user didn't give us any information about an initial
	//       TRANS, so we called 'atFindTrans()' to find one.
	//       We have applied this TRANS to input list A, and
	//       looked for matched pairs.
	//
	// Now, we want to improve the initial TRANS, whether it was supplied
	// by user or determined by 'atFindTrans()'.  We do so by applying
	// the initial TRANS to only the matched objects in list A, and then
	// calling 'atRecalcTrans()' on the matched objects only.  This should
	// give an improved TRANS, since it very likely won't be contaminated
	// by any spurious matches.

	// need to send trans to prepare_to_recalc because it adds sdx,sdy to it
	if( PrepareToRecalc(outfile, &num_matched_A, &matched_list_A, 
		&num_matched_B, &matched_list_B, star_list_A_copy, m_pTrans ) != 0)
	{
//		m_pUnimapWorker->Log( "WARNING :: prepare_to_recalc fails", GUI_LOGGER_ATTENTION_STYLE );
	}
	// okay, now we're ready to call atRecalcTrans, on matched items only 
	if( RecalcTrans(num_matched_A, matched_list_A, num_matched_B, matched_list_B, 
		max_iter, halt_sigma, m_pTrans ) != SH_SUCCESS ) 
	{
//		m_pUnimapWorker->Log( "WARNING :: atRecalcTrans fails on matched pairs only", GUI_LOGGER_ATTENTION_STYLE );
	}

#ifdef DEBUG
   m_pUnimapWorker->Log( "DEBUG :: TRANS based on matches only :" );
   PrintTrans( m_pTrans );
#endif

	// At this point, we have a TRANS which is based solely on those items
	// which matched.  If the user wishes, we can improve the TRANS
	// even more by applying the current transformation to ALL items
	// in list A, making a second round of matching pairs, and then
	// using these pairs to calculate a new and better TRANS.
	// The point is that we'll probably end up with more matched pairs
	// if we start with the current TRANS, instead of the initial TRANS.
	if( recalc == 1 )
	{
		// LARRY :: here we free what was allocated
		FreeStarList( matched_list_A );
		FreeStarList( matched_list_B );

		// re-set coords of all items in star A 
		if( ResetACoords(numA, star_list_A, star_list_A_copy) != 0 )
		{
			m_pUnimapWorker->Log( wxT("WARNING :: reset_A_coords returns with error before recalc"), GUI_LOGGER_ATTENTION_STYLE );
		}

		// apply the current TRANS (which is probably much better than
		// the initial TRANS) to all items in list A
		atApplyTrans( numA, star_list_A, m_pTrans );

		// Match items in list A to those in list B
		atMatchLists( numA, star_list_A, numB, star_list_B, match_radius,
						outfile, &num_matches, 
						star_array_J, &num_stars_J, star_array_K, &num_stars_K );
		m_pTrans->nm = num_matches;

#ifdef DEBUG
		m_pUnimapWorker->Log( "DEBUG :: After tuning with recalc, num matches is %d", num_matches );
		PrintTrans( m_pTrans );
#endif

		// prepare to call atRecalcTrans one last time 
		// need to send trans to prepare_to_recalc because it adds sdx,sdy 
		if( PrepareToRecalc(outfile, &num_matched_A, &matched_list_A, 
			&num_matched_B, &matched_list_B, star_list_A_copy, m_pTrans ) != 0 )
		{
//			m_pUnimapWorker->Log( "WARNING :: prepare_to_recalc fails", GUI_LOGGER_ATTENTION_STYLE );
		}

		// final call atRecalcTrans, on matched items only 
		if( RecalcTrans(num_matched_A, matched_list_A, num_matched_B, matched_list_B, 
			max_iter, halt_sigma, m_pTrans ) != SH_SUCCESS)
		{
//			m_pUnimapWorker->Log( "WARNING :: atRecalcTrans fails on matched pairs only", GUI_LOGGER_ATTENTION_STYLE );
		}

#ifdef DEBUG
       m_pUnimapWorker->Log("TRANS based on recalculated matches is:");
       PrintTrans( m_pTrans );
#endif

	}

	// If the user wants summary statistics on the straight translation
	// differences between the lists (without any scale or rotation),
	// we calculate them now.  We take all the pairs of stars which
	// match -- based on their TRANSFORMED coordinates -- and go back
	// to look at the differences between their ORIGINAL coordinates.
	// This only makes sense if a pure translation connects the lists.
	if( medtf_flag )
	{
		if( ResetACoords(num_matched_A, matched_list_A, star_list_A_copy) != 0)
		{
			m_pUnimapWorker->Log( wxT("WARNING :: second call to reset_A_coords returns with error"), GUI_LOGGER_ATTENTION_STYLE );
		}

		medtf = atMedtfNew();
		if( atFindMedtf(num_matched_A, matched_list_A, num_matched_B, 
			matched_list_B, medsigclip, medtf) != SH_SUCCESS)
		{
			m_pUnimapWorker->Log( wxT("WARNING :: atFindMedtf fails"), GUI_LOGGER_ATTENTION_STYLE );

		} else 
		{
			print_medtf(medtf);
		}
		atMedtfDel(medtf);
	}

//	PrintTrans( m_pTrans );

	if( m_pTrans->nm > 0 ) CopyTrans( m_pTrans, m_pLastTrans );

	///////////////////////////////////////////////////////////
	// LARRY'S INSERT BEFORE THE LIST GETS ALTERETED
	// claculate max no matched larry :: >= ?
	if( m_pTrans->nm >= m_nMaxMatch && m_pTrans->nm > 2 )
	{
		m_nMaxMatch = m_pTrans->nm;
		strLog.Printf( wxT("DEBUG :: max no match=%d from %d fit=%f"), 
					m_pTrans->nm, m_pTrans->nr, m_pTrans->sig );
//		m_pUnimapWorker->Log( strLog );

		// copy the best trans
		CopyTrans( m_pTrans, m_pBestTrans );

		// reset cat_no for all stars in the input list A
		for( i=0; i <nStarA; i++ ) 
		{
			vectStarA[i].cat_no = 0;
			vectStarA[i].match = 0;
		}
		// reset match flag for all stars in input list B 
		for( i=0; i <nStarB; i++ ) 
		{
			vectStarB[i].match = 0;
		}

		int cont = 0;

		m_strFoundStar = wxT("");
		// used the matched vector instead of the method just commented
		for( i=0; i<num_stars_J; i++ )
		{
//			// print map from position in vector j to position in vector k
//			sprintf( m_strFoundStar, "%s %d=%d", m_strFoundStar, 
//											star_array_J[i].id, star_array_K[i].id );
//			// set catalog number for image star
//			vectStarA[star_array_J[i].id].sao_no = vectStarB[star_array_K[i].id].sao_no;


			// new version
			int nListAId = star_array_J[i].id;
			int nListBId = star_array_K[i].id-nStarA;

			if( nListAId < 0 || nListBId < 0 )
			{
				FreeStarList( matched_list_A );
				FreeStarList( matched_list_B );

				FreeStarList( star_list_A );
				FreeStarList( star_list_A_copy );
				FreeStarList( star_list_B );

				return( 0 );
			}

			// print map from position in vector j to position in vector k
//			sprintf( m_strFoundStar, "%s %d=%d", m_strFoundStar, 
//											nListAId, nListBId );


			// set catalog number for image star
			vectStarA[nListAId].cat_no = vectStarB[nListBId].cat_no;
			vectStarA[nListAId].cat_type = vectStarB[nListBId].cat_type;
			vectStarA[nListAId].zone_no = vectStarB[nListBId].zone_no;
			vectStarA[nListAId].comp_no = vectStarB[nListBId].comp_no;
			// set ids
			vectStarA[nListAId].cat_id = vectStarB[nListBId].cat_id;

			// also set the vmag to the catalog vmag
			vectStarA[nListAId].vmag = vectStarB[nListBId].mag;
			// and set as a match
			vectStarA[nListAId].match = 1;
			// some other details like ra and dec
			vectStarA[nListAId].ra = vectStarB[nListBId].ra;
			vectStarA[nListAId].dec = vectStarB[nListBId].dec;

			// set match flag in catlog list as well
			vectStarB[nListBId].match = 1;
		}
		
		// set return status to one to mark that i found a new max
		nReturnStatus = 1;

	} else
	{
		m_nMaxMatch = 0;
		//////////////////
		// larry hack :: todo check what else should I reset
		// reset cat_no for all stars in the input list A
		for( i=0; i <nStarA; i++ ) 
		{
			vectStarA[i].cat_no = 0;
			vectStarA[i].match = 0;
		}
		// reset match flag for all stars in input list B 
		for( i=0; i <nStarB; i++ ) 
		{
			vectStarB[i].match = 0;
		}
	}

	// resert star_array J/K
	for( i=0; i<DEFAULT_MAX_NO_OF_USED_STARS; i++ )
	{
		// J
		star_array_J[i].id=-1;
		star_array_J[i].index=-1;
		star_array_J[i].mag = 0;
		star_array_J[i].match_id = -1;
		star_array_J[i].next = NULL;
		star_array_J[i].x = 0;
		star_array_J[i].y = 0;
		// k
		star_array_K[i].id=-1;
		star_array_K[i].index=-1;
		star_array_K[i].mag = 0;
		star_array_K[i].match_id = -1;
		star_array_K[i].next = NULL;
		star_array_K[i].x = 0;
		star_array_K[i].y = 0;

	}
	num_stars_J = 0;
	num_stars_K = 0;

// only init at the begining - not to free - reset instead
//	FreeStarList( star_array_J );
//	FreeStarList( star_array_K );

	FreeStarList( matched_list_A );
	FreeStarList( matched_list_B );

	FreeStarList( star_list_A );
	FreeStarList( star_list_A_copy );
	FreeStarList( star_list_B );

	return( nReturnStatus );
	//return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		ResetCopyIds
// Class:		CStarMatch
// Purpose:		Modify the 'id' field values in the given list (a copy of list A)
//				so that they will match the 'id' values in the corresponding
//				stars of list A.
// Input:		number of stars in list A, original star A list, 
//				copy of star A list
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::ResetCopyIds( int numA, struct s_star *star_list_A, 
							 struct s_star *star_list_A_copy )
{
	int i;
	struct s_star *star, *star_copy;

	star = star_list_A;
	star_copy = star_list_A_copy;

	for (i = 0; i < numA; i++)
	{
		shAssert(star != NULL);
		shAssert(star_copy != NULL);
		star_copy->id = star->id;

		star = star->next;
		star_copy = star_copy->next;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ResetACoords
// Class:		CStarMatch
// Purpose:		Given the number of elements in list 'post_list_A', 
//				and two versions of the stars in list A (after conversion 
//				to coord system of list B, and before conversion), 
//				restore the original coords of stars in the the matched list.
// Input:		lists and counts
// Output:		0 - if all goes well, 1 - otherwise
////////////////////////////////////////////////////////////////////
int CStarMatch::ResetACoords( int numA, struct s_star *post_list_A, 
										struct s_star *pre_list_A )
{
	int post_index;
	int found_it;
	struct s_star *pre_star, *post_star;
	wxString strLog;

	// if handed empty list, do nothing. 
	if( numA == 0 )
	{
		m_pUnimapWorker->Log( wxT("WARNING :: reset_A_coords: handed empty list, will do nothing"), GUI_LOGGER_ATTENTION_STYLE );
		return(0);
	}

	// sanity checks 
	shAssert(post_list_A != NULL);
	shAssert(pre_list_A != NULL);

	for( post_index = 0, post_star = post_list_A; post_index < numA; 
							post_index++, post_star = post_star->next )
	{
		shAssert(post_star != NULL);

		found_it = 0;
		pre_star = pre_list_A;
		while( pre_star != NULL )
		{
			if( pre_star->id == post_star->id )
			{
				post_star->x = pre_star->x;
				post_star->y = pre_star->y;
				found_it = 1;
				break;
			}
			pre_star = pre_star->next;
		}

		if( found_it == 0 )
		{
			// LARRY IMPORTANT !!!! SEE WHY DOESN WORK HERE ??? - it seems that id's are afected somehow
			strLog.Printf( wxT("WARNING :: reset_A_coords: no match for post_star %d?"), post_index );
			m_pUnimapWorker->Log( strLog, GUI_LOGGER_ATTENTION_STYLE );
			shAssert(0);
		}
	}

	return(0);
}

////////////////////////////////////////////////////////////////////
// Method:		PrepareToRecalc
// Class:		CStarMatch
// Purpose:		This function sets us up to call "atRecalcTrans".
//				We have already found (or been given) a TRANS, and
//				used it to match up items from list A and list B.
//				Those matched items are in a pair of files
//				with names based on 'outfile', but with
//				extensions "mtA" and "mtB".  
//				Ex: if 'outfile' is "matched",
//				then the two sets of matched items are in 
//
//					matched.mtA      matched.mtB
//
//				The format of each file is one star per line, 
//				with 4 fields:
//
//					internal_ID     xval   yval     mag
//
//				where the coords (xval, yval) are in system of list B.
//	
//				We are about to use these good, matched items to 
//				find an improved TRANS -- which should take objects
//				from coord system A to coord system B.
//
//				In order to do that, we need to 
//
//				 a. Read in the good, matched items.  The coordinates
//				of objects in list A will have been transformed
//				to their corresponding values in coord system 
//				of list B, so ...
//
//				b. We must then re-set the coords of the items in
//				list A to their original values, so that we can
//				re-calculate a TRANS which takes the coords
//				from system A to system B.
//
//				We also take this opportunity to compare the transformed
//				positions of items in list A against the positions of
//				the matching objects in list B.  We calculate the
//				RMS of the differences in both "x" and "y" directions,
//				and place them into the "sx" and "sy" members of
//				the current TRANS.
// Input:		lists and counts
// Output:		0 - if all goes well, 1 - otherwise
////////////////////////////////////////////////////////////////////
int CStarMatch::PrepareToRecalc( char *outfile,  int *num_matched_A, 
							struct s_star **matched_list_A, int *num_matched_B,
							struct s_star **matched_list_B, 
							struct s_star *star_list_A_copy, TRANS *trans )
{
//	char matched_file_A[CMDBUFLEN + 4];
//	char matched_file_B[CMDBUFLEN + 4];
	double Xrms,Yrms;
	// larry's addition
	char strLog[255];
	int i = 0;
	struct s_star *headStar, *lastStar, *newStar;

	// first do this check
	if( num_stars_J == 0 || num_stars_K == 0 ) return( 1 );
	
//	shAssert(outfile != NULL);

//	sprintf(matched_file_A, "%s.mtA", outfile);
//	if( read_matched_file(matched_file_A, num_matched_A, matched_list_A) != SH_SUCCESS )
//	{
//		sprintf( strLog, "ERROR :: read_matched_file can't read data from file %s", matched_file_A );
//		m_pUnimapWorker->Log( strLog, GUI_LOGGER_ATTENTION_STYLE );
//		return( 1 );
//	}

	// here we copy from vector instead of file
	headStar = (struct s_star *) NULL;
	lastStar = headStar;
	for( i=0; i<num_stars_J; i++ )
	{
		newStar = atStarNew(star_array_J[i].x, star_array_J[i].y, star_array_J[i].mag);
		newStar->id = star_array_J[i].id;

		if( headStar == NULL )
		{
			headStar = newStar;
			lastStar = newStar;
		} else
		{
			lastStar->next = newStar;
			lastStar = newStar;
		}
	}
	*num_matched_A = num_stars_J;
	*matched_list_A = headStar;
	lastStar->next = NULL;


//	sprintf(matched_file_B, "%s.mtB", outfile);
//	if( read_matched_file(matched_file_B, num_matched_B, matched_list_B) != SH_SUCCESS)
//	{
//		sprintf( strLog, "read_matched_file can't read data from file %s", matched_file_B );
//		m_pUnimapWorker->Log( strLog, GUI_LOGGER_ATTENTION_STYLE );
//		return(1);
//	}

	// here we copy from vector instead of file
	headStar = (struct s_star *) NULL;
	lastStar = headStar;
	for( i=0; i<num_stars_K; i++ )
	{
		newStar = atStarNew(star_array_K[i].x, star_array_K[i].y, star_array_K[i].mag);
		newStar->id = star_array_K[i].id;

		if( headStar == NULL )
		{
			headStar = newStar;
			lastStar = newStar;
		} else
		{
			lastStar->next = newStar;
			lastStar = newStar;
		}
	}
	*num_matched_B = num_stars_K;
	*matched_list_B = headStar;
	lastStar->next = NULL;

	// here we find the rms of those stars we read in -- JPB 17/Jan/02 
	if( atCalcRMS(*num_matched_A, *matched_list_A, *num_matched_B,
						*matched_list_B, &Xrms, &Yrms) != SH_SUCCESS )
	{
//		m_pUnimapWorker->Log( "WARNING :: atCalcRMS fails on matched pairs", GUI_LOGGER_ATTENTION_STYLE );
	}
	trans->sx = Xrms;
	trans->sy = Yrms;

	/////////////////////////////////////////////////////////////

	if( ResetACoords( *num_matched_A, *matched_list_A, star_list_A_copy ) != 0 )
	{
//		m_pUnimapWorker->Log( "WARNING :: prepare_to_recalc: reset_A_coords returns with error", GUI_LOGGER_ATTENTION_STYLE );
		return(1);
	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		PrintTrans
// Class:		CStarMatch
// Purpose:		Print the elements of a TRANS structure.
// Input:		TRANS to print out
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::PrintTrans( TRANS *trans )
{
	wxString strLog;

	switch( trans->order )
	{
		// linear transformation 
		case 1:
		{
			strLog.Printf( wxT("LINE TRANS: a=%-15.9f b=%-15.9f c=%-15.9f d=%-15.9f e=%-15.9f f=%-15.9f"),
					trans->a, trans->b, trans->c, trans->d, trans->e, trans->f );
			m_pUnimapWorker->Log( strLog );
			break;
		}
		// quadratic terms 
		case 2:
		{
			strLog.Printf( wxT("QUAD TRANS: a=%-15.9f b=%-15.9f c=%-15.9f d=%-15.9f e=%-15.9f f=%-15.9f "),
							trans->a, trans->b, trans->c, trans->d, trans->e, trans->f);
			strLog.Printf( wxT("%s       g=%-15.9f h=%-15.9f i=%-15.9f j=%-15.9f k=%-15.9f l=%-15.9f"), strLog,
					trans->g, trans->h, trans->i, trans->j, trans->k, trans->l);
			m_pUnimapWorker->Log( strLog );
			break;
		}
		// cubic terms 
		case 3:
		{
			strLog.Printf( wxT("CUBIC TRANS: a=%-15.9f b=%-15.9f c=%-15.9f d=%-15.9f e=%-15.9f f=%-15.9f g=%-15.9f h=%-15.9f"),
						trans->a, trans->b, trans->c, trans->d, trans->e, trans->f, trans->g, trans->h);
			strLog.Printf( wxT("%s       i=%-15.9f j=%-15.9f k=%-15.9f l=%-15.9f m=%-15.9f n=%-15.9f o=%-15.9f p=%-15.9f"), strLog, 
						trans->i, trans->j, trans->k, trans->l, trans->m, trans->n, trans->o, trans->p);
			m_pUnimapWorker->Log( strLog );
			break;
		}

		default:
		{
			strLog.Printf( wxT("ERROR :: print_trans: invalid trans->order %d \n"), trans->order );
			m_pUnimapWorker->Log( strLog, GUI_LOGGER_ATTENTION_STYLE );
			//exit(1);
		}
	}


	// we always print this information about the match at the end 
	// of the line 
	strLog.Printf( wxT(" sig=%-.4e Nr=%d Nm=%d sx=%-.4e sy=%-.4e"), trans->sig, trans->nr, trans->nm, trans->sx, trans->sy);
	m_pUnimapWorker->Log( strLog );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		PrintTrans
// Class:		CStarMatch
// Purpose:		Delete an array of "num" s_star structures.
// Input:		first star in the array to be deleted 
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarMatch::FreeStarArray( s_star *first )
{
   shFree(first);
}

////////////////////////////////////////////////////////////////////
// Method:		RecalcTrans
// Class:		CStarMatch
// Purpose:		Given two lists of stars which ALREADY have been matched,
//				this routine finds a coord transformation which takes coords
//				of stars in list A to those in list B. We can skip all 
//				the matching-triangles business, which makes this _much_ 
//				faster than atFindTrans.
// Input:		number of stars in list A, match this set of objects 
//				with list B, number of stars in list B, match this set
//				of objects with list A, go through at most this many 
//				iterations in the iter_trans() loop., halt the fitting 
//				procedure if the mean residual becomes this small, 
//				place into this TRANS structure's fields, the coeffs 
//				which convert coords of chainA, into coords of chainB 
//				system.
// Output:		SH_SUCCESS - if all goes well, SH_GENERIC_ERROR - if 
//				an error occurs
////////////////////////////////////////////////////////////////////
int CStarMatch::RecalcTrans( int numA, struct s_star *listA, int numB,
								struct s_star *listB, int max_iter,         
								double halt_sigma, TRANS *trans )
{
	wxString strLog;

	int i, nbright, min;
	// number of stars in chain A 
	int num_stars_A;         
	// number of stars in chain B 
	int num_stars_B;      
    // # votes gotten by top pairs of matched stars 
	int *winner_votes;    
    // elem i in this array is index in star array A 
	int *winner_index_A;      
	// which matches elem i in this array, index in star array B 
	int *winner_index_B;      
	int start_pairs=0;
	s_star *star_array_A = NULL;
	s_star *star_array_B = NULL;

	num_stars_A = numA;
	num_stars_B = numB;
	star_array_A = ListToArray(numA, listA);
	star_array_B = ListToArray(numB, listB);

	shAssert(star_array_A != NULL);
	shAssert(star_array_B != NULL);

	switch( trans->order )
	{
		case AT_TRANS_LINEAR:
		{
			start_pairs = AT_MATCH_STARTN_LINEAR;
			break;
		}
		case AT_TRANS_QUADRATIC:
		{
			start_pairs = AT_MATCH_STARTN_QUADRATIC;
			break;
		}
		case AT_TRANS_CUBIC:
		{
			start_pairs = AT_MATCH_STARTN_CUBIC;
			break;
		}
		default:
		{
			strLog.Printf( wxT("atRecalcTrans: invalid pTrans->order %d "), trans->order );
			m_pUnimapWorker->Log( strLog );
			break;
		}
	}

	// here we check to see if each list of stars contains a
	// required minimum number of stars.  If not, we return with
	// an error message, and SH_GENERIC_ERROR.  
	// We set 'nbright' to the minimum of the two list lengths
	min = (num_stars_A < num_stars_B ? num_stars_A : num_stars_B);
	if (min < start_pairs)
	{
//		sprintf( strLog, "ERROR :: RecalcTrans: only %d stars in list(s), require at least %d",	min, start_pairs);
//		m_pUnimapWorker->Log( strLog );

		FreeStarArray(star_array_A);
		FreeStarArray(star_array_B);
		return(SH_GENERIC_ERROR);
	}
	nbright = min;

	// this is a sanity check on the above checks 
	shAssert((nbright >= start_pairs) && (nbright <= min));

#ifdef DEBUG
	printf("here comes star array A\n");
	print_star_array(star_array_A, num_stars_A);
	printf("here comes star array B\n");
	print_star_array(star_array_B, num_stars_B);
#endif

	// We need to create dummy arrays for 'winner_votes', and the
	// 'winner_index' arrays.  We already know that all these stars
	// are good matches, and so we can just create some arrays
	// and fill them with identical numbers.  They aren't used by
	// iter_trans(), anyway.
	winner_votes = (int *) shMalloc(nbright*sizeof(int));
	winner_index_A = (int *) shMalloc(nbright*sizeof(int));
	winner_index_B = (int *) shMalloc(nbright*sizeof(int));

	for (i = 0; i < nbright; i++)
	{
		winner_votes[i] = 100;
		winner_index_A[i] = i;
		winner_index_B[i] = i;
	}

	// next, we take ALL the matched pairs of coodinates, and
	// figure out a transformation of the form
	//       x' = A + Bx + Cx
	//       y' = D + Ex + Fy
	// (i.e. a TRANS structure) which converts the coordinates
	// of objects in list A to those in list B
	if( iter_trans( nbright, star_array_A, num_stars_A, 
					star_array_B, num_stars_B, winner_votes, winner_index_A, 
					winner_index_B, RECALC_YES, max_iter, halt_sigma, trans ) != SH_SUCCESS ) 
	{
		strLog.Printf( wxT("ERROR :: RecalcTrans: iter_trans unable to create a valid TRANS") );
		m_pUnimapWorker->Log( strLog );

		FreeStarArray(star_array_A);
		FreeStarArray(star_array_B);
		return(SH_GENERIC_ERROR);
	}

#ifdef DEBUG
	printf("  after calculating new TRANS structure, here it is\n");
	print_trans(trans);
#endif

	// clean up memory we allocated during the matching process 
	shFree(winner_votes);
	shFree(winner_index_A);
	shFree(winner_index_B);
	FreeStarArray(star_array_A);
	FreeStarArray(star_array_B);

	return(SH_SUCCESS);
}

////////////////////////////////////////////////////////////////////
// Method:		ListToArray
// Class:		CStarMatch
// Purpose:		Create an array of s_star structures, identical to the 
//				given linked list. Just make a copy of each structure.
//				Sure, this is inefficient, but I'm using legacy code ...
// Input:		number of stars in the list, the linked list
// Output:		Return a pointer to the complete, filled array.
////////////////////////////////////////////////////////////////////
s_star* CStarMatch::ListToArray( int num_stars, struct s_star *list )
{
	int i;
	struct s_star *array = NULL;
	struct s_star *ptr;
	struct s_star *star;

	// okay, now we can walk down the CHAIN and create a new s_star
	// for each item on the CHAIN.
	array = (s_star *) shMalloc(num_stars*sizeof(s_star));
	shAssert(array != NULL);
	for (i = 0, ptr = list; i < num_stars; i++, ptr = ptr->next)
	{
		shAssert(ptr != NULL);
		star = &(array[i]);
		shAssert(star != NULL);
		SetStar( star, ptr->x, ptr->y, ptr->mag );
		star->match_id = i;
	}

	return(array);
}

////////////////////////////////////////////////////////////////////
// Method:		SetStar
// Class:		CStarMatch
// Purpose:		Given a pointer to an EXISTING s_star, initialize its values
//				and set x, y, and mag to the given values.
// Input:		pointer to existing s_star structure, star's "X" coordinate,
//				star's "Y" coordinate, star's "mag" coordinate 
// Output:		SH_SUCCESS - if all goes well, SH_GENERIC_ERROR  if not
////////////////////////////////////////////////////////////////////
int CStarMatch::SetStar( s_star *star, double x, double y, double mag )
{
	static int id_number = 0;

	if (star == NULL)
	{
		m_pUnimapWorker->Log( wxT("ERROR :: SetStar methods ahs been given a NULL star") );
		return(SH_GENERIC_ERROR);
	}

	star->id = id_number++;
	star->index = -1;
	star->x = x;
	star->y = y;
	star->mag = mag;
	star->match_id = -1;
	star->next = (s_star *) NULL;

	return(SH_SUCCESS);
}

