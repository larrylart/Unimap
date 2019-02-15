# include <cv.h>
# include <highgui.h>
# include <math.h>
# include <stdio.h>
int
main (int argc, char ** argv)
{
	char text [16];
	int i, hist_size = 256, sch = 0;
	float range_0[] = {0, 256};
	float * ranges [] = {range_0};

	double tmp, dist = 0;
	IplImage * src_img1 = 0, * src_img2 = 0, * dst_img1 [4] = (0, 0, 0, 0), * dst_img2 [4] = (0, 0, 0, 0);
	CvHistogram * hist1, * hist2;
	CvFont font;
	CvSize text_size;
	// (1) to load the two images. If you are not equal the number of channels, end
	if( argc> = 3 )
	{
		src_img1 = cvLoadImage (argv [1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		src_img2 = cvLoadImage (argv [2], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	}

	if( src_img1 == 0 | | src_img2 == 0 )
		return -1;
	if( src_img1->nChannels! = src_img2->nChannels )
		return -1;

	// (2) the number of input channels of the image area images of
	sch = src_img1-> nChannels;
	for( i = 0; i <sch; i++ )
	{
		dst_img1 [i] = cvCreateImage (cvSize (src_img1-> width, src_img1-> height), src_img1-> depth, 1);
		dst_img2 [i] = cvCreateImage (cvSize (src_img2-> width, src_img2-> height), src_img2-> depth, 1);
	}

	// (3) to secure the structure histogram
	hist1 = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
	hist2 = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );

	// (4) In the case of multi-channel input images, each image into a channel
	if( sch == 1 )
	{
		cvCopy (src_img1, dst_img1 [0], NULL);
		cvCopy (src_img2, dst_img2 [0], NULL);

	} else
	{
		cvSplit (src_img1, dst_img1 [0], dst_img1 [1], dst_img1 [2], dst_img1 [3]);
		cvSplit (src_img2, dst_img2 [0], dst_img2 [1], dst_img2 [2], dst_img2 [3]);
	}

	// (5) histogram calculations, the normalized distance call
	for( i = 0; i <sch; i++ )
	{
		cvCalcHist( &dst_img1 [i], hist1, 0, NULL );
		cvCalcHist( &dst_img2 [i], hist2, 0, NULL );
		cvNormalizeHist( hist1, 10000 );
		cvNormalizeHist( hist2, 10000 );
		tmp = cvCompareHist( hist1, hist2, CV_COMP_BHATTACHARYYA );
		dist + = tmp * tmp;
	}
	dist = sqrt( dist );

	// (6) sought to distance drawn characters in the picture as
	snprintf (text, 16, "Distance =% .3 f", dist);
	cvInitFont (& font, CV_FONT_HERSHEY_SIMPLEX, 2.0, 2.0, 0, 4, 8);
	cvPutText (src_img2, text, cvPoint (10, src_img2-> height - 10), & font, cvScalarAll (0));
	cvGetTextSize ( "Input1", & font, & text_size, 0);
	cvPutText (src_img1, "Input1", cvPoint (10, text_size.height), & font, cvScalarAll (0));
	cvPutText (src_img2, "Input2", cvPoint (10, text_size.height), & font, cvScalarAll (0));

	// (7) show images of the two, ending when a key is pressed
	cvNamedWindow ( "Image1", CV_WINDOW_AUTOSIZE);
	cvNamedWindow ( "Image2", CV_WINDOW_AUTOSIZE);
	cvShowImage ( "Image1", src_img1);
	cvShowImage ( "Image2", src_img2);
	cvWaitKey (0);
	cvDestroyWindow ( "Image1");
	cvDestroyWindow ( "Image2");

	for (i = 0; i <src_img1-> nChannels; i + +)
	{
		cvReleaseImage( &dst_img1[i] );
		cvReleaseImage( &dst_img2[i] );
	}
	cvReleaseImage( &src_img1 );
	cvReleaseImage( &src_img2 );
	cvReleaseHist( &hist1 );
	cvReleaseHist( &hist2 );

	return( 0 );
}
