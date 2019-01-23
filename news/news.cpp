
// system headers
#include <math.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

// local headers
#include "esa_news.h"
#include "nasa_news.h"
#include "sciencedaily_news.h"

// main header
#include "news.h"

////////////////////////////////////////////////////////////////////
CNews::CNews()
{
	m_vectHeadlines.clear();

	m_vectNewsSources[NEWS_SOURCE_ALL] = wxT("All");
	m_vectNewsSources[NEWS_SOURCE_ESA] = wxT("ESA");
	m_vectNewsSources[NEWS_SOURCE_NASA] = wxT("NASA");
	m_vectNewsSources[NEWS_SOURCE_SCIENCEDAILY] = wxT("Science Daily");
}

////////////////////////////////////////////////////////////////////
CNews::~CNews()
{
	// todo:tofix: ceal here ?? gives error
	m_vectHeadlines.clear();
}

////////////////////////////////////////////////////////////////////
int CNews::Get_Headlines( int nType )
{ 
	m_vectHeadlines.clear();

	// :: ESA = 1
	if( nType == 0 || nType == 1 )
	{
		CNewsEsa src;
		src.Get_Headlines( this );
	}
	// :: NASA = 2
	if( nType == 0 || nType == 2 )
	{
		CNewsNasa src;
		src.Get_Headlines( this );
	}
	// :: ScienceDaily = 3
	if( nType == 0 || nType == 3 )
	{
		CNewsScienceDaily src;
		src.Get_Headlines( this );
	}

	// sort news by date desc
	std::sort( m_vectHeadlines.begin(), m_vectHeadlines.end(), DescCmpByNewsDatetime );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
bool CNews::DescCmpByNewsDatetime( DefNewsHeadlineRecord a, DefNewsHeadlineRecord b )
{ 
	return( a.datetime > b.datetime ); 
}
