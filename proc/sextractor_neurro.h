////////////////////////////////////////////////////////////////////
// File:		neurro.h
// Package:		neurro.c
//
// Author:		E.BERTIN, Institut d'Astrophysique de Paris.
// Purpose:		global definitions.
// Created:		30/03/95
// Updated:
////////////////////////////////////////////////////////////////////

#ifndef _SEXTRACTOR_NEURRO_H
#define _SEXTRACTOR_NEURRO_H

///////////////////////////////////////////
// Neural Network parameters 
// max. number of hidden+i/o layers 
#define		LAYERS		3	
#define		CONNEX		LAYERS-1
// maximum number of neurons/layer 
#define		NEURONS		10	

// structures 
typedef	struct
{
	int	layersnb;
	int	nn[LAYERS];
	double	inbias[NEURONS];
	double	inscale[NEURONS];
	double	outbias[NEURONS];
	double	outscale[NEURONS];
	double	ni[NEURONS];
	double	no[NEURONS];
	double	n[LAYERS][NEURONS];
	double	w[CONNEX][NEURONS][NEURONS];
	double	b[CONNEX][NEURONS];

} brainstruct;

// globals 

//extern double	f(double);

#endif
