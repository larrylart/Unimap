////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _STARMATCH_H
#define _STARMATCH_H

// some defines
#define CMDBUFLENGTH					255
#define AT_MATCH_HALTSIGMA_CPP			1.0e-12
#define DEFAULT_MAX_NO_OF_USED_STARS	10000

// predefined type
//typedef TRANS;

#include "../match/misc.h"

// external classes 
//class CLogger;
class CConfigMain;
class CUnimapWorker;

// class: CStarMatch
class CStarMatch
{
// public methods
public:
	CStarMatch( );
	~CStarMatch( );

	void DefaultParam( );
	int Init( );
	void CloseMatch( );
	void SetConfig( CConfigMain* pConfig );

	int ReadStarList( StarDef* vectStar, int nStar, 
						struct s_star **list , int& num_stars, int nMatchFlag );
	struct s_star* CloneStarList( struct s_star *vectStar );

	int Cmp( StarDef* vectStarA, int nStarA, StarDef* vectStarB, 
										int nStarB, int nRecalc );

	int CalcInv( StarDef* vectStarA, int nStarA, StarDef* vectStarB, 
															int nStarB );
	int ApplyObjInv( StarDef& star );
	int CalcPointTrans( double& x, double& y );
	int CalcInvPointTrans( double& x, double& y );
	int CalcWindowTrans( double& x1, double& y1, double& x2, double& y2,
						double& x3, double& y3, double& x4, double& y4 );
	int CalcInvWindowTrans( double& x1, double& y1, double& x2, double& y2,
								double& x3, double& y3, double& x4, double& y4 );

	void ResetCopyIds( int numA, struct s_star *star_list_A, 
							 struct s_star *star_list_A_copy );
	int ResetACoords( int numA, struct s_star *post_list_A, 
								struct s_star *pre_list_A );
	int PrepareToRecalc( char *outfile,  int *num_matched_A, 
						struct s_star **matched_list_A, int *num_matched_B,
						struct s_star **matched_list_B, struct s_star *star_list_A_copy,
							 TRANS *trans );
	void PrintTrans( TRANS *trans );

	void ResetTrans( TRANS* pTrans );
	void CopyTrans( TRANS* pSrcTrans, TRANS* pDstTrans );
	void SetTransType( int nType );

	//////////////////////
	// utils
	void FreeStarArray( s_star *first );
	void FreeStarList( s_star *list );

	// new import from atp match
	int RecalcTrans( int numA, struct s_star *listA, int numB,
					struct s_star *listB, int max_iter, double halt_sigma, TRANS *trans );
	s_star* ListToArray( int num_stars, struct s_star *list );
	int SetStar( s_star *star, double x, double y, double mag );

// public data
public:
//	CLogger*		m_pLogger;
	CUnimapWorker*	m_pUnimapWorker;

	// larry's added data
	int m_nMaxMatch;
	wxString m_strFoundStar;

	// main data block
	int max_iter;
	int trans_order;
	// in triangle-space coords 
	double triangle_radius;  
	// in units of list B 
	double match_radius;        
	double scale;                          
	double min_scale;                          
	double max_scale;                          
	double halt_sigma;
	// used in MEDTF calcs 
	double medsigclip;         
	// guessed shift in X dir 
	double xshift;
	// guessed shift in y dir 
	double yshift;                         
	int nobj;
	// if 1, only find TRANS 
	int transonly;        
	// if 1, calc TRANS again 
	int recalc;                     
	// number of matching pairs 
	int num_matches;          
	// calculate MEDTF stats? 
	int medtf_flag;               
	// use given input TRANS? 
	int intrans;                         
	// use identity as TRANS? 
	int identity;                            
	// trans file ... larry :: i dont know if I need this
	char intransfile[CMDBUFLENGTH + 1];
	char outfile[CMDBUFLENGTH + 1];
	// transformation
	TRANS *m_pTrans;
	TRANS *m_pLastTrans;
	TRANS *m_pBestTrans;
	TRANS *m_pInvTrans;

	// ported from atpMatch to get rid of these files
	s_star *star_array_J, *star_array_K;
	int num_stars_J, num_stars_K;

	// the big counter
	int nUniqueIdCount;
};

#endif
