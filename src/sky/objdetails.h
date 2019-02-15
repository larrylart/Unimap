////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SKYOBJDETAILS_H
#define _SKYOBJDETAILS_H

// local headers
#include "../util/folders.h"

// galaxy distance files details
#define FILE_DSO_DETAILS_DISTANCE_NGC		unMakeAppPath(wxT("data/gl_ngc_fin.txt"))
#define FILE_DSO_DETAILS_DISTANCE_IC		unMakeAppPath(wxT("data/gl_ic_fin.txt"))
#define FILE_DSO_DETAILS_DISTANCE_MESSIER	unMakeAppPath(wxT("data/gl_mes_fin.txt"))

// local includes
#include "star.h"

// local classes
class CUnimapWorker;
class CAstroImage;

class CSkyObjDetails
{
// public methods
public:
	CSkyObjDetails( CAstroImage* pAstroImage, CUnimapWorker* pUnimapWorker );
	~CSkyObjDetails( );
	// load galaxy distances
	int LoadDsoDistances( StarDef* vectObj, unsigned long nObj, StarBasicDetailsDef* vectObjDetails );

// public data
public:
	CAstroImage*	m_pAstroImage;
	CUnimapWorker*	m_pUnimapWorker;

};

#endif
