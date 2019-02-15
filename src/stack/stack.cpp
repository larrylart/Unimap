

////////////////////////////////////////////////////////////////////
// Method:	StackAverage
// Class:	CImageGroup
// Purpose:	avg-stacking frames
// Input:	nothing
// Output:	return -1 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackAverage( )
{
	int w, h, i, y, x, n;
	float *dp[COMB_MAX];
	float *odp;
	int rs[COMB_MAX];
	GList *gl;
	struct image_file *imf;
	char lb[81];

	// get reference frame width and height
	w = ofr->w;
	h = ofr->h;

	gl = imfl->imlist;
	i=0;

	// for every image in the list
	while( gl != NULL )
	{
		// get data
		imf = gl->data;
		gl = g_list_next(gl);

		if( imf->flags & IMG_SKIP ) continue;

		// check if image size is different
		if( (imf->fr->w < w) || (imf->fr->h < h) )
		{
			err_printf("bad frame size\n");
			continue;
		}

		dp[i] = imf->fr->dat;
		rs[i] = imf->fr->w - w;
		i++;

		if (i >= COMB_MAX) break;
	}

	// set n = last one ?
	n = i - 1;
	if (i == 0)
	{
		err_printf("no frames to stack\n");
		return -1;
	}

	snprintf(lb, 80, "'AVERAGE STACK %d FRAMES'", i);
	fits_add_history(ofr, lb);
	odp = ofr->dat;

	// :: for all pizels on X
	for( y = 0; y < h; y++ )
	{
		// :: and for all pixels on Y
		for( x = 0; x < w; x++ )
		{
			// calculate pixel average for current pixel in all frames
			*odp = pix_average( dp, n, 0, 1 );

			// for each frame in my array
			for( i = 0; i < n; i++ )
			{
				// go to the next pixel
				dp[i]++;
			}

			// increment the destination(stack image result) pixel
			odp++;
		}

		for (i = 0; i < n; i++)
		{
			dp[i] += rs[i];
		}
	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	StackMedian
// Class:	CImageGroup
// Purpose:	the real work of median-stacking frames
// Input:	nothing
// Output:	return -1 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackMedian( )
{
	int w, h, i, y, x, n;
	float *dp[COMB_MAX];
	float *odp;
	int rs[COMB_MAX];
	GList *gl;
	struct image_file *imf;
	char lb[80];

	w = ofr->w;
	h = ofr->h;

	gl = imfl->imlist;
	i=0;

	// for all images in the stack+
	while( gl != NULL )
	{
		imf = gl->data;
		gl = g_list_next( gl );
		if( imf->flags & IMG_SKIP ) continue;

		if( (imf->fr->w < w) || (imf->fr->h < h) )
		{
			err_printf("bad frame size\n");
			continue;
		}

		dp[i] = imf->fr->dat;
		rs[i] = imf->fr->w - w;
		i++;

		if (i >= COMB_MAX) break;
	}

	// set n to the last frame
	n = i - 1;
	if( i == 0 )
	{
		err_printf("no frames to stack\n");
		return -1;
	}

	snprintf(lb, 80, "'MEDIAN STACK of %d FRAMES'", i);
	fits_add_history(ofr, lb);
	odp = ofr->dat;

	for( y = 0; y < h; y++ )
	{
		for( x = 0; x < w; x++ )
		{
			// do median average calc
			*odp = pix_median( dp, n, 0, 1 );

			for( i = 0; i < n; i++ )
			{
				dp[i]++;
			}

			odp++;
		}

		for( i = 0; i < n; i++ )
		{
			dp[i] += rs[i];
		}
	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	StackMeanMedian
// Class:	CImageGroup
// Purpose:	the real work of mean-median-stacking frames
// Input:	nothing
// Output:	return -1 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackMeanMedian( )
{
	int w, h, i, y, x, n, t;
	float *dp[COMB_MAX];
	float *odp;
	int rs[COMB_MAX];
	GList *gl;
	struct image_file *imf;
	char lb[81];

	w = ofr->w;
	h = ofr->h;

	gl = imfl->imlist;
	i=0;
	while (gl != NULL)
	{
		imf = gl->data;
		gl = g_list_next(gl);
		if (imf->flags & IMG_SKIP) continue;
		// check ne frame size ??
		if( (imf->fr->w < w) || (imf->fr->h < h) )
		{
			err_printf("bad frame size\n");
			continue;
		}
		dp[i] = imf->fr->dat;
		rs[i] = imf->fr->w - w;
		i++;
		if (i >= COMB_MAX) {
			d1_printf("reached stacking limit\n");
			break;
		}
	}
	n = i - 1;
	if (i == 0) {
		err_printf("no frames to stack\n");
		return -1;
	}
	if (progress) {
		int ret;
		char msg[64];
		snprintf(msg, 63, "mean-median s=%.1f (%d frames) ", P_DBL(CCDRED_SIGMAS), i);
		ret = (* progress)(msg, data);
		if (ret) {
			d1_printf("aborted\n");
			return -1;
		}
	}
	snprintf(lb, 80, "'MEAN_MEDIAN STACK (s=%.1f) of %d FRAMES'",
		 P_DBL(CCDRED_SIGMAS), i);
	fits_add_history(ofr, lb);

	odp = ofr->dat;
	t = h / 16;
	for (y = 0; y < h; y++) {
		if (t-- == 0 && progress) {
			int ret;
			ret = (* progress)(".", data);
			if (ret) {
				d1_printf("aborted\n");
					return -1;
			}
			t = h / 16;
		}
		for (x = 0; x < w; x++) {
			*odp = pix_mmedian(dp, n, P_DBL(CCDRED_SIGMAS), P_INT(CCDRED_ITER));
			for (i = 0; i < n; i++)
				dp[i]++;
			odp++;
		}
		for (i = 0; i < n; i++) {
			dp[i] += rs[i];
		}
	}
	if (progress) {
		(* progress)("\n", data);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////
// Method:	StackKappaSigma
// Class:	CImageGroup
// Purpose:	the real work of k-s-stacking frames
// Input:	nothing
// Output:	return -1 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackKappaSigma( )
{
	int w, h, i, y, x, n, t;
	float *dp[STACK_MAX_NO_OF_FRAMES];
	float *odp;
	int rs[STACK_MAX_NO_OF_FRAMES];
	GList *gl;
	struct image_file *imf;
	char lb[81];

	w = ofr->w;
	h = ofr->h;

	gl = imfl->imlist;
	i=0;

	while (gl != NULL)
	{
		imf = gl->data;
		gl = g_list_next(gl);
		if (imf->flags & IMG_SKIP)
			continue;
		if ((imf->fr->w < w) || (imf->fr->h < h)) {
			err_printf("bad frame size\n");
			continue;
		}
		dp[i] = imf->fr->dat;
		rs[i] = imf->fr->w - w;
		i++;
		// check for max ?
		if( i >= STACK_MAX_NO_OF_FRAMES )
		{
			d1_printf("reached stacking limit\n");
			break;
		}
	}

	n = i - 1;
	if (i == 0)
	{
		err_printf("no frames to stack\n");
		return -1;
	}

	// debug :: show porogress ??
	if (progress)
	{
		int ret;
		char msg[64];
		snprintf(msg, 63, "kappa-sigma s=%.1f iter=%d (%d frames) ",
			 P_DBL(CCDRED_SIGMAS), P_INT(CCDRED_ITER), i);
		ret = (* progress)(msg, data);
		if (ret)
		{
			d1_printf("aborted\n");
			return -1;
		}
	}
	t = h / 16;

	snprintf(lb, 80, "'KAPPA-SIGMA STACK (s=%.1f) of %d FRAMES'",
		 P_DBL(CCDRED_SIGMAS), i);
	fits_add_history(ofr, lb);

	odp = ofr->dat;
	for( y = 0; y < h; y++ )
	{
		if( t-- == 0 && progress )
		{
			int ret;
			ret = (* progress)(".", data);
			if( ret )
			{
				d1_printf("aborted\n");
					return -1;
			}
			t = h / 16;
		}

		for( x = 0; x < w; x++ )
		{
			// stack pixel with kapp sigma function
			*odp = pix_ks(dp, n, P_DBL(CCDRED_SIGMAS), P_INT(CCDRED_ITER));
			// skip to next pixel
			for( i = 0; i < n; i++ )
			{
				dp[i]++;
			}
			// go to next destination pixel
			odp++;
		}
		// ????
		for( i = 0; i < n; i++ )
		{
			dp[i] += rs[i];
		}
	}

	// show progress ????
	if( progress )
	{
		(* progress)("\n", data);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////
// Method:	StackImages
// Class:	CImageGroup
// Purpose:	stack the frames/images from my stack list
// Input:	stack method type ??
// Output:	return -1 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackImages( )
{
	GList *gl;
	struct image_file *imf;
	struct ccd_frame *ofr = NULL;
	int nf = 0;
	char msg[256];
	double b = 0.0, rn = 0.0, fln = 0.0, sc = 0.0, eff = 1.0;

	int ow = 0, oh = 0;

	/* calculate output w/h */
	gl = imfl->imlist;
	while (gl != NULL) {
		imf = gl->data;
		gl = g_list_next(gl);
		if (imf->flags & IMG_SKIP)
			continue;
		if (ow == 0 || imf->fr->w < ow)
			ow = imf->fr->w;
		if (oh == 0 || imf->fr->h < oh)
			oh = imf->fr->h;
		nf ++;
		b += imf->fr->exp.bias;
		rn += sqr(imf->fr->exp.rdnoise);
		fln += imf->fr->exp.flat_noise;
		sc += imf->fr->exp.scale;
	}
	if (nf == 0) {
		err_printf("No frames to stack\n");
		return NULL;
	}

	/* create output frame */
	gl = imfl->imlist;
	while (gl != NULL) {
		imf = gl->data;
		gl = g_list_next(gl);
		if (imf->flags & IMG_SKIP)
			continue;
		ofr = clone_frame(imf->fr);
		if (ofr == NULL) {
			err_printf("Cannot create output frame\n");
			return NULL;
		}
		crop_frame(ofr, 0, 0, ow, oh);
		break;
	}
	if (progress) {
		snprintf(msg, 255, "Frame stack: output size %d x %d\n", ow, oh);
		(*progress)(msg, data);
	}

	///////////////////////////////////////
	// :: do the actual stack by frame type
	switch( m_nImageStackMethod )
	{
		// :: do stack by average
		case IMAGE_STACK_METHOD_AVERAGE:
		{
			if( do_stack_avg(imfl, ofr, progress, data) )
			{
				release_frame(ofr);
				return NULL;
			}
			break;
		}
		// :: do stack by method median
		case IMAGE_STACK_METHOD_MEDIAN:
		{
			eff = 0.65;
			if( do_stack_median(imfl, ofr, progress, data) )
			{
				release_frame(ofr);
				return NULL;
			}
			break;
		}
		// :: do stack by method mean median
		case IMAGE_STACK_METHOD_MEAN_MEDIAN:
		{
			eff = 0.85;
			if( do_stack_mm(imfl, ofr, progress, data) )
			{
				release_frame(ofr);
				return NULL;
			}
			break;
		}
		// :: do stack by method kappa-sigma
		case IMAGE_STACK_METHOD_KAPPA_SIGMA:
		{
			eff = 0.9;
			if( do_stack_ks(imfl, ofr, progress, data) )
			{
				release_frame(ofr);
				return NULL;
			}
			break;
		}
		// :: default - do nothing
		default: break;
	}

	ofr->exp.rdnoise = sqrt(rn) / nf / eff;
	ofr->exp.flat_noise = fln / nf;
	ofr->exp.scale = sc;
	ofr->exp.bias = b / nf;

	if (ofr->exp.bias != 0.0)
		scale_shift_frame(ofr, 1.0, -ofr->exp.bias);

	noise_to_fits_header(ofr, &(ofr->exp));
	strcpy(ofr->name, "stack_result");
	return ofr;
}

