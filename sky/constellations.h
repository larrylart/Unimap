
#ifndef _SKY_CONSTELLATIONS_H
#define _SKY_CONSTELLATIONS_H

//////////////////////
// define sky-culture
#define SKY_CULTURE_WESTERN		0
#define SKY_CULTURE_CHINESE		1
#define SKY_CULTURE_EGYPTIAN	2
#define SKY_CULTURE_INUIT		3
#define SKY_CULTURE_KOREAN		4
#define SKY_CULTURE_LAKOTA		5
#define SKY_CULTURE_MAORI		6
#define SKY_CULTURE_NAVAJO		7
#define SKY_CULTURE_NORSE		8
#define SKY_CULTURE_POLYNESIAN	9
#define SKY_CULTURE_TUPI		10
// define line types
#define CONSTELLATION_WESTERN_HARAY	0
#define CONSTELLATION_WESTERN_IAU	1

/////////////////////////////////////////////////
// constelation common name structure
typedef struct
{
	// pointer to name - to be alocated
	wxChar*	name;
	// ra/dec - coordinates
	double ra;
	double dec;

} ConstCommonName;

/////////////////////////////////////////////////
// catalog constelation lines/name structure
////////////
// constelation star
typedef struct
{
	// name of the star
	wxChar*	name;
	// coord
	double ra;
	double dec;
	// start point
	char nRefPoint;

} ConstellationStar;
// const list structure
typedef struct
{
	// 2d image coord
	double ra1;
	double dec1;
	double ra2;
	double dec2;

} ConstellationLine;
// main constelation struct
typedef struct
{
	int id;
	// pointer to name - to be alocated
	wxChar*	name;
	wxChar*	short_name;
	wxChar*	artwork_file;
	long	x1, y1, x2, y2, x3, y3;
	double	ra1, dec1, ra2, dec2, ra3, dec3;

	// member stars of the constellation
	ConstellationStar* vectStar;
	// number of stars in the constelation
	unsigned int nStar;

	// struct as lines
	ConstellationLine* vectLine[2];
	// number of lines
	unsigned int nLineCount[2];

} ConstellationStructure;

/////////////////////////////////////////////////
// image constelation lines/name structure
////////////
// constelation star
typedef struct
{
	// 2d image coord
	long x1;
	long y1;
	long x2;
	long y2;

} ImgConstellationLine;
// const list structure
typedef struct
{
	// pointer to name - to be alocated
	wxChar*	name;
	// id 
	int		id;
	// member stars of the constellation
	ImgConstellationLine* vectLine;
	// number of stars in the constelation
	unsigned int nLine;
	// center
	int center_x;
	int center_y;
	// min max
	int min_x, min_y;
	int max_x, max_y;
	// score to use for sorting
	double score;

} ImgConstellationStructure;

// external classes
class CAstroImage;
class CUnimapWorker;
class CSky;

// class: CSkyConstellations
////////////////////////////////////////////////////////////////////
class CSkyConstellations
{
public:
	CSkyConstellations( CSky* pSky );
	~CSkyConstellations();

	long GetConstlInImage( CAstroImage* pAstroImage, int nCulture, int nVersion, int* vectCnstl, int& nCnstl );
	long FindConstellation( CAstroImage* pAstroImage );
	int GetConstellationNoById( int nId );
	long LoadConstellations( int nCulture );
	// old temp left version - to load old format
	long LoadConstellationsA( const wxChar* strFile );
	void UnloadConstellations( );

	// dup handlers - to retire in cleanup
	long LoadConstNames( const wxChar* strFile );
	void UnLoadConstNames( );

// public data
public:
	// ref pointers
	CSky*	m_pSky;
	CUnimapWorker*	m_pUnimapWorker;

	// types
	int m_nCulture;
	int m_nVersion;
	int m_bHasArtwork;

	// constelation sstructures
	ConstellationStructure*	m_vectConstellations;
	unsigned long	m_nConstellations;

	// Const common names - just another - redundant list 
	//... maybe I should clear this
	ConstCommonName* m_vectConstName;
	unsigned long m_nConstName;

};

#endif
