////////////////////////////////////////////////////////////////////*
// Author: E.BERTIN (IAP)
// Contents: run only version of the neural network.
// Last modify: 26/11/2003, 05/03/2012
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

// system
#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

//brainstruct	*brain;

//#include "fits/fitscat.h"
// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		NeurInit
// Class:		CSextractor
// Purpose:		Initialization of the "brain".
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::NeurInit( )
{
	QMALLOC( brain, brainstruct, 1 );
	// larry: init all to zero
	memset( brain, 0, sizeof(brainstruct) );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		NeurClose
// Class:		CSextractor
// Purpose:		Close the "brain".
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::NeurClose( )
{
	free( brain );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		NeurResp
// Class:		CSextractor
// Purpose:		Neural network response to an input vector.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::NeurResp( double *input, double *output )
{
	int	i=0, j=0, l=0, lastlay = brain->layersnb-1;
	double	neursum=0.0;

	for( i=0; i<brain->nn[0]; i++ )
		brain->n[0][i] = input[i]*brain->inscale[i] + brain->inbias[i];

	for( l=0; l<lastlay; l++ )
	{
		for( j=0; j<brain->nn[l+1]; j++ )
		{
			neursum = brain->b[l][j];
			for (i=0; i<brain->nn[l]; i++)
				neursum += brain->w[l][i][j] * brain->n[l][i];
			brain->n[l+1][j] = FSigmoid(neursum);
		}
	}

	for( i=0; i<brain->nn[lastlay]; i++ )
	{
		output[i] = (brain->n[lastlay][i] - brain->outbias[i]) / brain->outscale[i];
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FSigmoid
// Class:		CSextractor
// Purpose:		Sigmoid function for a neural network.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
double CSextractor::FSigmoid( double x )
{
	return( 1.0 / (1.0 + exp(-x)) );
}

/*
////////////////////////////////////////////////////////////////////
// Method:		GetNnw
// Class:		CSextractor
// Purpose:		Read the NNW table that contains the weights.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::GetNnw()
{
	FILE *infile;
	int i=0, j=0, k=0, step=0;
	char str[MAXCHAR], *sstr=NULL, *line=NULL;

	if( (infile = fopen(prefs.nnw_name, "r")) == NULL )
		error( EXIT_FAILURE, "*ERROR*: can't read ", prefs.nnw_name );

	fgets( str, MAXCHAR, infile );
	if( strncmp(str, "NNW", 3) )
		error( EXIT_FAILURE, prefs.nnw_name, " is NOT a NNW table!" );

	step = 1;
	i = j = 0;			
	while( fgets(str, MAXCHAR, infile) )
	{
		sstr = &str[(int)strspn(str," \t")];
		// if not empty line
		if( sstr[0] != (char) '#' && sstr[0] != (char) '\n' )
		{
			line = sstr;
			switch(step)
			{
				// definition layout of neurons and layerss
				case 1:
				{
					// layersnb=3 number of definitions .. layer types...
					brain->layersnb = atoi(strtok(sstr, " \t\n"));
					for( i=0; i<brain->layersnb; i++ )
						brain->nn[i] = atoi(strtok(line, " \t\n"));
					step++;
					break;
				}
				// set input bias ?
				case 2:
				{
					// for every neuron on input layer
					for( i=0; i<brain->nn[0]; i++ )
					{
						brain->inbias[i] = atof(strtok(line, " \t\n"));
						line = NULL;
					}
					step++;
					break;
				}
				// set input scale ?
				case 3:	
				{
					// for every neuron on input layer
					for( i=0; i<brain->nn[0]; i++ )
					{
						brain->inscale[i] = atof(strtok(line, " \t\n"));
						line = NULL;
					}
					i = j = 0;
					step++;
					break;
				}
				// set network core here ?
				case 4:
				{
					// j = number of core layers ?? nn[1]=10
					if( j == brain->nn[i+1] )
					{
						j = 0;
						i++;
					}
					// so long i < 2 - last layer i=1 is a special connex 
					// - maybe i =1 the back propagation lary?
					if( i < brain->layersnb-1 )
					{
						// i=0 main core connexions, i=1 last core connexions
						// for every neuron in layer
						for( k=0; k<brain->nn[i]; k++ )
						{
							brain->w[i][k][j] = atof(strtok(line, " \t\n"));
							line = NULL;
						}
						// last value is bias ?
						brain->b[i][j] = atof(strtok(line, " \t\n"));
						// next layer
						j++;
						break;

					} else
						step++;
				}
				// set output bias
				case 5:	
				{
					for( i=0; i<brain->nn[brain->layersnb-1]; i++ )
					{
						brain->outbias[i] = atof(strtok(line, " \t\n"));
						line = NULL;
					}
					step++;
					break;
				}
				// set output scale
				case 6:	
				{
					for( i=0; i<brain->nn[brain->layersnb-1]; i++ )
					{
						brain->outscale[i] = atof(strtok(line, " \t\n"));
						line = NULL;
					}
					step++;
					break;
				}
				// all default
				default: error( EXIT_FAILURE, "*Error*: inconsistency in ", prefs.nnw_name );
			}

      }
    }

	fclose(infile);

	return;
}
*/
