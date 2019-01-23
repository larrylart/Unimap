
#ifndef _NASA_NEWS_H
#define _NASA_NEWS_H

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/string.h>
#include <wx/xml/xml.h>

// external classes
class CNews;

// class:	CNewsNasa
///////////////////////////////////////////////////////
class CNewsNasa
{
public:
	CNewsNasa( ){};
	~CNewsNasa( ){};

	// get latest headline
	int Get_Headlines( CNews* pNews );

};


#endif

