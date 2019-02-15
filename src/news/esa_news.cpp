////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/datetime.h>

// local headers
#include "../util/units.h"
#include "../util/func.h"
#include "../util/webfunc.h"
#include "news.h"

// main header
#include "esa_news.h"

////////////////////////////////////////////////////////////////////
int CNewsEsa::Get_Headlines( CNews* pNews )
{
	wxXmlDocument xml;
//	wxString strPage="";
	wxString strUrl;
	wxString strTitle=wxT(""), strDescription=wxT(""), strLink=wxT(""), strPubDateTime=wxT("");
	int nElem=0, bFound=0;

	// build url
	strUrl = wxT( "http://www.esa.int/rss/TopNews.xml" );
	// get page
	if( !GetWebXmlDoc( strUrl, xml ) ) return( 0 );

	/////////////
	// parse XML
	// start processing the XML file
	wxString strTmp = xml.GetRoot()->GetName();
	if( xml.GetRoot()->GetName() != wxT("rss") )
		return( 0 );

	wxXmlNode *base = xml.GetRoot()->GetChildren();
	// next child :: see why this ???
	base = base->GetNext();

	strTmp = base->GetName();
	if( base->GetName() != wxT("channel") )
		return( 0 );

	wxXmlNode *child = base->GetChildren();
	while( child  )
	{
		// get item
		if( child->GetName() == wxT("item") )
		{
			wxXmlNode* child2 = child->GetChildren();
			// look for headlines
			while( child2 )
			{
				// :: title
				if( child2->GetName() == wxT("title") )
				{
					strTitle = child2->GetNodeContent();
					if( strTitle == wxT("") ) 
						bFound = 0;
					else
					{
						bFound = 1;

						DefNewsHeadlineRecord elem;
						elem.title = strTitle;
						elem.source = NEWS_SOURCE_ESA;
						nElem =  pNews->m_vectHeadlines.size();
						pNews->m_vectHeadlines.push_back( elem );
					}
				}
				// :: description
				else if( bFound && child2->GetName() == wxT("description") )
				{
					pNews->m_vectHeadlines[nElem].desc = child2->GetNodeContent();
				}
				// :: link
				else if( bFound && child2->GetName() == wxT("link") )
				{
					pNews->m_vectHeadlines[nElem].link = child2->GetNodeContent();
				}
				// :: publication date/time
				else if( bFound && child2->GetName() == wxT("pubDate") )
				{
					strPubDateTime = child2->GetNodeContent();
					// read in format "%a, %d %b %Y %H:%M:%S %z"
					wxDateTime news_time;
					
					if( news_time.ParseFormat( strPubDateTime, wxT("%a, %d %b %Y %H:%M:%S") ) )
					{
						news_time.MakeFromTimezone(wxDateTime::TZ::GMT1);
						pNews->m_vectHeadlines[nElem].datetime = news_time.GetTicks();
					}
				}

				// get next child
				child2 = child2->GetNext();
			}
		}
		// next child
		child = child->GetNext();
	}

	return( 1 );
}
