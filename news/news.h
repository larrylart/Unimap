
#ifndef _NEWS_H
#define _NEWS_H

// system includes
#include <vector>

// new source types
enum
{
	NEWS_SOURCE_ALL = 0,
	NEWS_SOURCE_ESA,
	NEWS_SOURCE_NASA,
	NEWS_SOURCE_SCIENCEDAILY
};

///////////////////////
// news headline record
typedef struct
{
	wxString title;
	wxString desc;
	wxString image;
	wxString link;
	int source;

	// date/time when the article was published
	time_t datetime;

} DefNewsHeadlineRecord;

// external classes
//class CNewsEsa;

// used namespaces
using namespace std;

// class:	CNews
///////////////////////////////////////////////////////
class CNews
{
// methods
public:
	CNews( );
	~CNews( );

	// get latest headline
	int Get_Headlines( int nType=0 );

	static bool DescCmpByNewsDatetime( DefNewsHeadlineRecord a, DefNewsHeadlineRecord b );

// data
public:
	vector<DefNewsHeadlineRecord> m_vectHeadlines;

	wxString m_vectNewsSources[10];
};


#endif
