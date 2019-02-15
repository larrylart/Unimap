////////////////////////////////////////////////////////////////////
// Part of: Any back-propagation-ANN-oriented software
// Author: E.BERTIN (IAP)
// Contents: Routines for BP-neural network management ("read-only" mode)
// Requirements: The LDACTools
// Last modify: 13/12/2002, 05/03/2010
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

//#include "fits/fitscat.h"
// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		PlayBpann
// Class:		CSextractor
// Purpose:		Single forward pass through the ANN.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::PlayBpann( bpannstruct *bpann, NFLOAT *invec, NFLOAT *outvec )
{
	NFLOAT u, *neuroni=NULL,*neuronj=NULL, *weight=NULL;
	int i=0, j=0, l=0, lp=0, ll=0, lflag=0;

	ll = bpann->nlayers-1;
	memcpy( bpann->neuron[0], invec, bpann->nn[0]*sizeof(float) );
	lflag = bpann->linearoutflag;

	for( lp=0,l=1; lp<ll; l++, lp++ )
	{
		neuronj = bpann->neuron[l];
		weight = bpann->weight[lp];

		for( j=bpann->nn[l]; j--; neuronj++ )
		{	
			// note we don't touch the "bias" neuron (=-1) 
			neuroni = bpann->neuron[lp];
			u = *(weight++)**(neuroni++);

			// The last one is the bias 
			for (i=bpann->nn[lp]; i--;)	u += *(weight++)**(neuroni++);

			if (l == ll)
				*(outvec++)= lflag ? u : SIGMOID(u);
			else
				*neuronj = SIGMOID(u);
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		LoadTabBpann
// Class:		CSextractor
// Purpose:		Load the relevant ANN structure (using the LDACTools).
// Input:		nothing
// Output:		bpannstruct pointer
////////////////////////////////////////////////////////////////////
bpannstruct* CSextractor::LoadTabBpann( tabstruct *tab, char *filename )
{
   bpannstruct* bpann=NULL;
   keystruct* key=NULL;
   char* head=NULL, str[80];
   int l=0;

	// OK, we now allocate memory for the ANN structure itself
	QCALLOC( bpann, bpannstruct, 1 );

	// Load important scalars (which are stored as FITS keywords)
	head = tab->headbuf;
	if( fitsread(head, "BPNLAYER", &bpann->nlayers, H_INT, T_LONG) != RETURN_OK )
		error( EXIT_FAILURE, "*Error*: incorrect BP-ANN header in ", filename );
	if( fitsread(head, "BPLINEAR",&bpann->linearoutflag, H_INT,T_LONG) != RETURN_OK )
		bpann->linearoutflag = 0;

	// Load all vectors!!
	read_keys( tab, NULL, NULL, 0, NULL );
	// Now interpret the result
	if( !(key = name_to_key(tab, "NNEUR_PER_LAYER")) )
		error( EXIT_FAILURE, "*Error*: incorrect BP-ANN header in ", filename );

	bpann->nn = (int*) key->ptr; key->ptr = 0;
	QMALLOC(bpann->neuron, NFLOAT *, bpann->nlayers);
	QMALLOC(bpann->weight, NFLOAT *, bpann->nlayers-1);

	// set all nn layers
	for (l=0; l<bpann->nlayers-1; l++)
	{
		QMALLOC( bpann->neuron[l], NFLOAT, bpann->nn[l]+1 );
		bpann->neuron[l][bpann->nn[l]] = -1.0;
		sprintf( str, "WEIGHT_LAYER%d", l+1 );

		if( !(key = name_to_key(tab, str)) )
			error( EXIT_FAILURE, "*Error*: incorrect BP-ANN header in ", filename );
		bpann->weight[l] = (float*) key->ptr; key->ptr = 0;
	}

	// no bias in this layer 
	QMALLOC( bpann->neuron[l], NFLOAT, bpann->nn[l] ); 

	return bpann;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeBpann
// Class:		CSextractor
// Purpose:		Free all memory modules allocated for a 
//				Back-Propagation ANN structure.*/
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::FreeBpann( bpannstruct *bpann )
{
	int i=0;

	// Loop over the "true" layers
	for( i=0; i<bpann->nlayers-1; i++ )
	{
		free( bpann->neuron[i] );
		free( bpann->weight[i] );
	}
	// Because of the input layer 
	free(bpann->neuron[i]);       

	// Then free pointers of pointers
	free( bpann->neuron );
	free( bpann->weight );
	free( bpann->nn );

	// And finally free the ANN structure itself
	free( bpann );

	return;
}


