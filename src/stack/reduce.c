/*******************************************************************************
  Copyright(c) 2000 - 2003 Radu Corlan. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  The full GNU General Public License is included in this distribution in the
  file called LICENSE.

  Contact Information: radu@corlan.net
*******************************************************************************/

/*  reduce.c: routines for ccd reduction and image list handling */
/*  $Revision: 1.22 $ */
/*  $Date: 2004/12/04 00:10:43 $ */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

#include "gcx.h"
#include "catalogs.h"
#include "gui.h"
#include "params.h"
#include "reduce.h"
#include "obsdata.h"
#include "misc.h"
#include "sourcesdraw.h"
#include "wcs.h"

int progress_print(char *msg, void *data)
{
	info_printf(msg);
	return 0;
}

/* pixel combination methods
 * they all take an array of pointers to float, and
 * the "sigmas" and "iter" parameters, and return a
 * single float value. Some methods ignore sigmas and/or iter.
 */

/* straight average */
static inline float pix_average(float *dp[], int n, float sigmas, int iter)
{
	int i;
	float m=0.0;
	for (i=0; i<n; i++) {
		m += *dp[i];
	}
	return m / n;
}

/* straight median */
static inline float pix_median(float *dp[], int n, float sigmas, int iter)
{
	int i;
	float pix[COMB_MAX];
// copy pixels to the pix array
	for (i=0; i<n; i++) {
		pix[i] = *dp[i];
	}
	return fmedian(pix, n);
}

/* mean-median; we average pixels within sigmas of the median */
static inline float pix_mmedian(float *dp[], int n, float sigmas, int iter)
{
	int i, k = 0;
	float pix[COMB_MAX];
	float sum = 0.0, sumsq = 0.0;
	float m, s;

	sigmas = sqr(sigmas);
// copy pixels to the pix array
	for (i=0; i<n; i++) {
		sum += *dp[i];
		sumsq += sqr(*dp[i]);
		pix[i] = *dp[i];
	}
	m = sum / (1.0 * n);
	s = sigmas * (sumsq / (1.0 * n) - sqr(m));
	m = fmedian(pix, n);
	sum = 0;
	for (i=0; i<n; i++) {
		if (sqr(*dp[i] - m) < s) {
			sum += *dp[i];
			k++;
		}
	}
	if (k != 0)
		return sum / (1.0 * k);
	else
		return *dp[0];
}


/* kappa-sigma. We start with a robust estimator: the median. Then, we
 * elliminate the pixels that are more than sigmas away from
 * the median. For the remaining pixels, we calculate the
 * mean and variance, and iterate (with the mean replacing the median)
 * until no pixels are elliminated or iter is reached
 */
static inline float pix_ks(float *dp[], int n, float sigmas, int iter)
{
	int i, k = 0, r;
	float pix[COMB_MAX];
	float sum = 0.0, sumsq = 0.0;
	float m, s;

	sigmas = sqr(sigmas);
// copy pixels to the pix array
	for (i=0; i<n; i++) {
		sum += *dp[i];
		sumsq += sqr(*dp[i]);
		pix[i] = *dp[i];
	}
	s = sigmas * (sumsq / (1.0 * n) - sqr(sum / (1.0 * n)));
	m = fmedian(pix, n);

	do {
		iter --;
		sum = 0.0;
		sumsq = 0.0;
		r = k;
		k = 0;
		for (i=0; i<n; i++) {
			if (sqr(*dp[i] - m) < s) {
				sum += *dp[i];
				sumsq += sqr(*dp[i]);
				k++;
			}
		}
		if (k == 0)
			break;
		m = sum / (1.0 * (k));
		s = sigmas * (sumsq / (1.0 * (k)) - sqr(m));
	} while ((iter > 0) && (k != r));
	return m;
}


/* file save functions*/

static int is_zip_name(char *fn)
{
	int len;
	len = strlen(fn);
	if (len < 4)
		return 0;
	if (strcasecmp(fn + len - 3, ".gz") == 0)
		return 1;
	if (strcasecmp(fn + len - 2, ".z") == 0)
		return 1;
	if (strcasecmp(fn + len - 4, ".zip") == 0)
		return 1;
	return 0;
}

/* overwrite the the original file */
static int save_image_file_inplace(struct image_file *imf,
				   int (* progress)(char *msg, void *data), void *data)
{
	char fn[16384];
	int i;
	char msg[256];

	g_return_val_if_fail(imf != NULL, -1);
	g_return_val_if_fail(imf->fr != NULL, -1);

	if (imf->flags & IMG_SKIP)
		return 0;
	snprintf(fn, 16383, "%s", imf->filename);
	if (progress) {
		snprintf(msg, 255, " saving %s\n", fn);
		if ((* progress)(msg, data))
			return -1;
	}
	if (is_zip_name(fn)) {
		for (i = strlen(fn); i > 0; i--)
			if (fn[i] == '.') {
				fn[i] = 0;
				break;
			}
		return write_gz_fits_frame(imf->fr, fn, P_STR(FILE_COMPRESS));
	} else {
		return write_fits_frame(imf->fr, fn);
	}
}


/* outf is a dir name: use the original file names and this dir */
static int save_image_file_to_dir(struct image_file *imf, char *outf,
				   int (* progress)(char *msg, void *data), void *data)
{
	char fn[16384], ifn[16384];
	int i;
	char msg[256];

	g_return_val_if_fail(imf != NULL, -1);
	g_return_val_if_fail(imf->fr != NULL, -1);
	g_return_val_if_fail(outf != NULL, -1);

	if (imf->flags & IMG_SKIP)
		return 0;
	strncpy(ifn, imf->filename, 16383);
	ifn[16383] = 0;
	snprintf(fn, 16383, "%s/%s", outf, basename(ifn));
	if (progress) {
		snprintf(msg, 255, " saving %s\n", fn);
		if ((* progress)(msg, data))
			return -1;
	}
	if (is_zip_name(fn)) {
		for (i = strlen(fn); i > 0; i--)
			if (fn[i] == '.') {
				fn[i] = 0;
				break;
			}
		return write_gz_fits_frame(imf->fr, fn, P_STR(FILE_COMPRESS));
	} else {
		return write_fits_frame(imf->fr, fn);
	}
}

/* outf is a "stud" to a file name; the function adds sequence numbers to it; files
 * are not overwritten, the sequence is just incremented until a new slot is
 * found; sequeces are searched from seq up; seq is updated
 * if seq is NULL, no sequence number is appended, and outf becomes the filename
 * calls progress with a short progress message from time to time. If progress returns
 * a non-zero value, abort remaining operations and return an error (non-zero) */

static int save_image_file_to_stud(struct image_file *imf, char *outf, int *seq,
				   int (* progress)(char *msg, void *data), void *data)
{
	char fn[16384];
	int i;
	char msg[256];

	g_return_val_if_fail(imf != NULL, -1);
	g_return_val_if_fail(imf->fr != NULL, -1);
	g_return_val_if_fail(outf != NULL, -1);

	if (imf->flags & IMG_SKIP)
		return 0;

	if (seq != NULL) {
		strncpy(fn, outf, 16383);
		fn[16383] = 0;
		check_seq_number(fn, seq);
		snprintf(fn, 16383, "%s%03d", outf, *seq);
	} else {
		snprintf(fn, 16383, "%s", outf);
	}

	if (progress) {
		snprintf(msg, 255, " saving %s\n", fn);
		if ((* progress)(msg, data))
			return -1;
	}
	if (is_zip_name(imf->filename)) {
		for (i = strlen(fn); i > 0; i--)
			if (fn[i] == '.') {
				fn[i] = 0;
				break;
			}
		return write_gz_fits_frame(imf->fr, fn, P_STR(FILE_COMPRESS));
	} else {
		return write_fits_frame(imf->fr, fn);
	}
}


/* save the (processed) frames from the image_file_list, using the filename logic
 * described in batch_reduce;
 * call progress with a short progress message from time to time. If progress returns
 * a non-zero value, abort remaining operations and return an error (non-zero) */

int save_image_file(struct image_file *imf, char *outf, int inplace, int *seq,
		    int (* progress)(char *msg, void *data), void *data)
{
	struct stat st;
	int ret;

	d3_printf("save_image_file\n");

	if (inplace) {
		return save_image_file_inplace(imf, progress, data);
	}
	if (outf == NULL || outf[0] == 0) {
		err_printf("need a file or dir name to save reduced files to\n");
		return -1;
	}
	ret = stat(outf, &st);
	if ((!ret) && S_ISDIR(st.st_mode)) {
		return save_image_file_to_dir(imf, outf, progress, data);
	} else {
		return save_image_file_to_stud(imf, outf, seq, progress, data);
	}
	if (progress)
		(* progress)("mock save\n", data);
	return 0;
}

/* call point from main; reduce the frames acording to ccdr.
 * Print progress messages at level 1. If the inplace flag in ccdr->ops is true,
 * the source files are overwritten, else new files will be created according
 * to outf. if outf is a dir name, files will be saved there. If not,
 * it's used as the beginning of a filename.
 * when stacking is requested, intermediate processing steps
 * are never saved */

int batch_reduce_frames(struct image_file_list *imfl, struct ccd_reduce *ccdr,
			char *outf)
{
	int ret, nframes;
	struct image_file *imf;
	struct ccd_frame *fr;
	GList *gl;
	int seq = 1;

	g_return_val_if_fail(imfl != NULL, -1);
	g_return_val_if_fail(ccdr != NULL, -1);

	nframes = g_list_length(imfl->imlist);
	if (!(ccdr->ops & IMG_OP_STACK)) {
		gl = imfl->imlist;
		while (gl != NULL) {
			imf = gl->data;
			gl = g_list_next(gl);
			if (imf->flags & IMG_SKIP)
				continue;
			ret = reduce_frame(imf, ccdr, progress_print, NULL);
			if (ret)
				continue;
			if (ccdr->ops & IMG_OP_INPLACE) {
				save_image_file(imf, outf, 1, NULL, progress_print, NULL);
			} else {
				save_image_file(imf, outf, 0,
						(nframes == 1 ? NULL : &seq),
						progress_print, NULL);
			}
			imf->flags &= ~IMG_SKIP;
			release_frame(imf->fr);
			imf->fr = NULL;
		}
		return 0;
	}
	if (P_INT(CCDRED_STACK_METHOD) == PAR_STACK_METHOD_KAPPA_SIGMA ||
	    P_INT(CCDRED_STACK_METHOD) == PAR_STACK_METHOD_MEAN_MEDIAN ) {
		if (!(ccdr->ops & IMG_OP_BG_ALIGN_MUL))
			ccdr->ops |= IMG_OP_BG_ALIGN_ADD;
	}
	if (reduce_frames(imfl, ccdr, progress_print, NULL))
		return 1;
	if ((fr = stack_frames(imfl, ccdr, progress_print, NULL)) == NULL)
		return 1;
	write_fits_frame(fr, outf);
	return 0;
}


/* call point from main; reduce the frames acording to ccdr.
 * Print progress messages at level 1.
 * returns the result of the stack if a stacking op was specified, or the
 * first frame in the list
 * files are saved if the inplace flag is set */

struct ccd_frame *reduce_frames_load(struct image_file_list *imfl, struct ccd_reduce *ccdr)
{
	int ret, nframes;
	struct image_file *imf;
	struct ccd_frame *fr = NULL;
	GList *gl;

	g_return_val_if_fail(imfl != NULL, NULL);
	g_return_val_if_fail(ccdr != NULL, NULL);

	nframes = g_list_length(imfl->imlist);
	if (!(ccdr->ops & IMG_OP_STACK)) {
		gl = imfl->imlist;
		while (gl != NULL) {
			imf = gl->data;
			gl = g_list_next(gl);
			if (imf->flags & IMG_SKIP)
				continue;
			ret = reduce_frame(imf, ccdr, progress_print, NULL);
			if (ret)
				continue;
			if (ccdr->ops & IMG_OP_INPLACE) {
				save_image_file(imf, NULL, 1, NULL, progress_print, NULL);
			}
			if (fr == NULL) {
				fr = imf->fr;
				get_frame(fr);
			}
		}
		return fr;
	}

	d3_printf("stacking\n");
	if (P_INT(CCDRED_STACK_METHOD) == PAR_STACK_METHOD_KAPPA_SIGMA ||
	    P_INT(CCDRED_STACK_METHOD) == PAR_STACK_METHOD_MEAN_MEDIAN ) {
		if (!(ccdr->ops & IMG_OP_BG_ALIGN_MUL))
			ccdr->ops |= IMG_OP_BG_ALIGN_ADD;
	}
	if (reduce_frames(imfl, ccdr, progress_print, NULL))
		return NULL;
	if ((fr = stack_frames(imfl, ccdr, progress_print, NULL)) == NULL)
		return NULL;
	return fr;
}


/* reduce the frame according to ccdr, up to the stacking step.
 * return 0 for succes or a positive error code
 * call progress with a short progress message from time to time. If progress returns
 * a non-zero value, abort remaining operations and return an error (non-zero) */

int reduce_frame(struct image_file *imf, struct ccd_reduce *ccdr,
		 int (* progress)(char *msg, void *data), void *data)
{
	char msg[256];

	if (ccdr->ops & IMG_OP_BIAS) {
		if (ccdr->bias == NULL) {
			err_printf("no bias image file\n");
			return 1;
		}
		if (!(ccdr->bias->flags & IMG_LOADED)) {
			snprintf(msg, 255, "bias frame: %s\n", ccdr->bias->filename);
			if (progress) {
				if ((*progress)(msg, data))
					return -1;
			}
		}
		if (load_image_file(ccdr->bias))
			return 2;
	}
	if (ccdr->ops & IMG_OP_DARK) {
		if (ccdr->dark == NULL) {
			err_printf("no dark image file\n");
			return 1;
		}
		if (!(ccdr->dark->flags & IMG_LOADED)) {
			snprintf(msg, 255, "dark frame: %s\n", ccdr->dark->filename);
			if (progress) {
				if ((*progress)(msg, data))
					return -1;
			}
		}
		if (load_image_file(ccdr->dark))
			return 2;
	}
	if (ccdr->ops & IMG_OP_FLAT) {
		if (ccdr->flat == NULL) {
			err_printf("no flat image file\n");
			return 1;
		}
		if (!(ccdr->flat->flags & IMG_LOADED)) {
			snprintf(msg, 255, "flat frame: %s\n", ccdr->flat->filename);
			if (progress) {
				if ((*progress)(msg, data))
					return -1;
			}
		}
		if (load_image_file(ccdr->flat))
			return 2;
	}
	if (ccdr->ops & IMG_OP_ALIGN) {
		if (ccdr->alignref == NULL) {
			err_printf("no alignment reference file\n");
			return 1;
		}
		if (!(ccdr->ops & CCDR_ALIGN_STARS)) {
			snprintf(msg, 255, "alignment reference frame: %s\n",
				 ccdr->alignref->filename);
			if (progress) {
				if ((*progress)(msg, data))
					return -1;
			}
			load_alignment_stars(ccdr);
		}
	}

	if (imf->flags & IMG_SKIP)
		return 0;
	if (progress) {
		if ((*progress)(imf->filename, data))
			return -1;
	}
	if (load_image_file(imf)) {
		err_printf("frame will be skipped\n");
		imf->flags |= IMG_SKIP;
		return -1;
	}
	if (progress) {
		if ((*progress)(" loaded", data))
			return -1;
	}
	ccd_reduce_imf(imf, ccdr, progress, data);
	return 0;
}


/* reduce the files in list according to ccdr, up to the stacking step.
 * return 0 for succes or a positive error code
 * call progress with a short progress message from time to time. If progress returns
 * a non-zero value, abort remaining operations and return an error (non-zero) */

int reduce_frames(struct image_file_list *imfl, struct ccd_reduce *ccdr,
		  int (* progress)(char *msg, void *data), void *data)
{
	GList *gl;
	struct image_file *imf;
	char msg[256];
	int ret = 0;

	if (ccdr->ops & IMG_OP_BIAS) {
		if (ccdr->bias == NULL) {
			err_printf("no bias image file\n");
			return 1;
		}
		if (load_image_file(ccdr->bias))
			return 2;
		snprintf(msg, 255, "bias frame: %s\n", ccdr->bias->filename);
		if (progress) {
			if ((*progress)(msg, data))
				return -1;
		}
	}
	if (ccdr->ops & IMG_OP_DARK) {
		if (ccdr->dark == NULL) {
			err_printf("no dark image file\n");
			return 1;
		}
		if (load_image_file(ccdr->dark))
			return 2;
		snprintf(msg, 255, "dark frame: %s\n", ccdr->dark->filename);
		if (progress) {
			if ((*progress)(msg, data))
				return -1;
		}
	}
	if (ccdr->ops & IMG_OP_FLAT) {
		if (ccdr->flat == NULL) {
			err_printf("no flat image file\n");
			return 1;
		}
		if (load_image_file(ccdr->flat))
			return 2;
		snprintf(msg, 255, "flat frame: %s\n", ccdr->flat->filename);
		if (progress) {
			if ((*progress)(msg, data))
				return -1;
		}
	}

	gl = imfl->imlist;
	while (gl != NULL) {
		imf = gl->data;
		gl = g_list_next(gl);
		if (imf->flags & IMG_SKIP)
			continue;
		if (progress) {
			if ((*progress)(imf->filename, data))
				return -1;
		}
		if (load_image_file(imf)) {
			err_printf("frame will be skipped\n");
			imf->flags |= IMG_SKIP;
			continue;
		}
		if (progress) {
			if ((*progress)(" loaded", data))
				return -1;
		}
		ret = ccd_reduce_imf(imf, ccdr, progress, data);
	}
	return ret;
}

/* apply bias, dark, flat, badpix, add, mul and align to a frame (as specified in the ccdr)
 * call progress with a short progress message from time to time. If progress returns
 * a non-zero value, abort remaining operations and return an error (non-zero) */

int ccd_reduce_imf(struct image_file *imf, struct ccd_reduce *ccdr,
		   int (* progress)(char *msg, void *data), void *data)
{
	char lb[81];
	g_return_val_if_fail(imf != NULL, -1);
	g_return_val_if_fail(imf->fr != NULL, -1);

	if (ccdr->ops & IMG_OP_BIAS) {
		g_return_val_if_fail(ccdr->bias->fr != NULL, -1);
		if (progress) {
			if ((*progress)(" bias", data))
				return -1;
		}
		if (!(imf->flags & IMG_OP_BIAS)) {
			sub_frames(imf->fr, ccdr->bias->fr);
			fits_add_history(imf->fr, "'BIAS FRAME SUBTRACTED'");
		} else {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		}
		imf->flags |= IMG_OP_BIAS | IMG_DIRTY;
	}

	if (ccdr->ops & IMG_OP_DARK) {
		g_return_val_if_fail(ccdr->dark->fr != NULL, -1);
		if (progress) {
			if ((*progress)(" dark", data))
				return -1;
		}
		if (!(imf->flags & IMG_OP_DARK)) {
			sub_frames(imf->fr, ccdr->dark->fr);
			fits_add_history(imf->fr, "'BIAS FRAME SUBTRACTED'");
		} else {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		}
		imf->flags |= IMG_OP_DARK | IMG_DIRTY;
	}

	if (ccdr->ops & IMG_OP_FLAT) {
		g_return_val_if_fail(ccdr->flat->fr != NULL, -1);
		if (progress) {
			if ((*progress)(" flat", data))
				return -1;
		}
		if (!(imf->flags & IMG_OP_FLAT)) {
			flat_frame(imf->fr, ccdr->flat->fr);
			fits_add_history(imf->fr, "'FLAT FIELDED'");
		} else {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		}
		imf->flags |= IMG_OP_FLAT | IMG_DIRTY;
	}

	if (ccdr->ops & (IMG_OP_MUL | IMG_OP_ADD)) {
		double m = 1.0, a = 0.0;
		if (progress) {
			if ((*progress)(" mul/add", data))
				return -1;
		}
		if ((ccdr->ops & (IMG_OP_MUL | IMG_OP_ADD))
		    == (imf->flags & (IMG_OP_MUL | IMG_OP_ADD))) {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		} else {
			if ((ccdr->ops & IMG_OP_MUL) && (!(imf->flags & IMG_OP_MUL)))
				m = ccdr->mulv;
			if ((ccdr->ops & IMG_OP_ADD) && (!(imf->flags & IMG_OP_ADD)))
				a = ccdr->addv;
			scale_shift_frame(imf->fr, m, a);
			snprintf(lb, 80, "'ARITHMETIC P <- P * %.3f + %.3f'",
				 m, a);
			fits_add_history(imf->fr, lb);
		}
		imf->flags |= (ccdr->ops & (IMG_OP_MUL | IMG_OP_ADD)) | IMG_DIRTY;
	}

	if (ccdr->ops & (IMG_OP_BG_ALIGN_ADD | IMG_OP_BG_ALIGN_MUL)) {
		if (progress) {
			if (ccdr->ops & (IMG_OP_BG_ALIGN_ADD)) {
				if ((*progress)(" bg_align", data))
					return -1;
			} else {
				if ((*progress)(" bg_align_mul", data))
					return -1;
			}
		}
		if (imf->fr->stats.statsok == 0)
			frame_stats(imf->fr);
		if ((ccdr->ops & (IMG_OP_BG_ALIGN_MUL | IMG_OP_BG_ALIGN_ADD))
		    == (imf->flags & (IMG_OP_BG_ALIGN_MUL | IMG_OP_BG_ALIGN_ADD))) {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		} else {
			if ((ccdr->ops & IMG_OP_BG_ALIGN_MUL) &&
			    imf->fr->stats.median < P_DBL(MIN_BG_SIGMAS) *
				imf->fr->stats.csigma) {
				imf->flags |= IMG_SKIP;
				if (progress) {
					(*progress)(" (too low)", data);
				}
			} else {
				if (!(ccdr->ops & CCDR_BG_VAL_SET)) {
					ccdr->bg = imf->fr->stats.median;
					ccdr->ops |= CCDR_BG_VAL_SET;
				} else if (ccdr->ops & IMG_OP_BG_ALIGN_MUL) {
					scale_shift_frame(imf->fr,
							  ccdr->bg / imf->fr->stats.median, 0);
				} else if (ccdr->ops & IMG_OP_BG_ALIGN_ADD) {
					scale_shift_frame(imf->fr,
							  1.0, ccdr->bg - imf->fr->stats.median);
				}
				imf->flags |= (ccdr->ops & (IMG_OP_BG_ALIGN_MUL
							    | IMG_OP_BG_ALIGN_ADD)) | IMG_DIRTY;
			}
		}
	}

	if (ccdr->ops & (IMG_OP_BLUR)) {
		if (progress) {
			if ((*progress)(" blur", data))
				return -1;
		}
		if (imf->flags & IMG_OP_BLUR) {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		} else {
			float kern[49];
			make_gaussian(ccdr->blurv / 2, 49, kern);
			if (!filter_frame_inplace(imf->fr, kern, 7)) {
				snprintf(lb, 80, "'GAUSSIAN BLUR (FWHM=%.1f)'",
					 ccdr->blurv);
				fits_add_history(imf->fr, lb);
				imf->flags |= IMG_OP_BLUR | IMG_DIRTY;
			}
		}
	}

	if (ccdr->ops & IMG_OP_ALIGN) {
		g_return_val_if_fail(ccdr->align_stars != NULL, -1);
		if (progress) {
			if ((*progress)(" align", data))
				return -1;
		}
		if (!(imf->flags & IMG_OP_ALIGN)) {
			if (!align_imf(imf, ccdr, progress, data)) {
				fits_add_history(imf->fr, "'ALIGNED'");
				imf->flags |= IMG_OP_ALIGN | IMG_DIRTY;
			}
		} else {
			if (progress) {
				if ((*progress)(" (already_done)", data))
					return -1;
			}
		}
	}
	if (progress) {
		if ((*progress)("\n", data))
			return -1;
	}
	noise_to_fits_header(imf->fr, &(imf->fr->exp));
//	d4_printf("\nrdnoise: %.3f\n", imf->fr->exp.rdnoise);
//	d4_printf("scale: %.3f\n", imf->fr->exp.scale);
//	d4_printf("flnoise: %.3f\n", imf->fr->exp.flat_noise);
	return 0;
}

/* the real work of avg-stacking frames
 * return -1 for errors */
static int do_stack_avg(struct image_file_list *imfl, struct ccd_frame *ofr,
			int (* progress)(char *msg, void *data), void *data)
{
	int w, h, i, y, x, n;
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

	// for every image in the list
	while (gl != NULL)
	{
		// get data
		imf = gl->data;
		gl = g_list_next(gl);

		if (imf->flags & IMG_SKIP) continue;

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

	n = i - 1;
	if (i == 0)
	{
		err_printf("no frames to stack\n");
		return -1;
	}

	snprintf(lb, 80, "'AVERAGE STACK %d FRAMES'", i);
	fits_add_history(ofr, lb);
	odp = ofr->dat;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			// calculate pixel average
			*odp = pix_average(dp, n, 0, 1);
			for (i = 0; i < n; i++)
			{
				dp[i]++;
			}

			odp++;
		}

		for (i = 0; i < n; i++)
		{
			dp[i] += rs[i];
		}
	}

	return 0;
}

/* the real work of median-stacking frames
 * return -1 for errors */
static int do_stack_median(struct image_file_list *imfl, struct ccd_frame *ofr,
			   int (* progress)(char *msg, void *data), void *data)
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
	while (gl != NULL) {
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
		if (i >= COMB_MAX)
			break;
	}
	n = i - 1;
	if (i == 0) {
		err_printf("no frames to stack\n");
		return -1;
	}
	snprintf(lb, 80, "'MEDIAN STACK of %d FRAMES'", i);
	fits_add_history(ofr, lb);
	odp = ofr->dat;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*odp = pix_median(dp, n, 0, 1);
			for (i = 0; i < n; i++)
				dp[i]++;
			odp++;
		}
		for (i = 0; i < n; i++) {
			dp[i] += rs[i];
		}
	}
	return 0;
}

/* the real work of k-s-stacking frames
 * return -1 for errors */
static int do_stack_ks(struct image_file_list *imfl, struct ccd_frame *ofr,
			int (* progress)(char *msg, void *data), void *data)
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
	while (gl != NULL) {
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
		snprintf(msg, 63, "kappa-sigma s=%.1f iter=%d (%d frames) ",
			 P_DBL(CCDRED_SIGMAS), P_INT(CCDRED_ITER), i);
		ret = (* progress)(msg, data);
		if (ret) {
			d1_printf("aborted\n");
			return -1;
		}
	}
	t = h / 16;

	snprintf(lb, 80, "'KAPPA-SIGMA STACK (s=%.1f) of %d FRAMES'",
		 P_DBL(CCDRED_SIGMAS), i);
	fits_add_history(ofr, lb);

	odp = ofr->dat;
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
			*odp = pix_ks(dp, n, P_DBL(CCDRED_SIGMAS), P_INT(CCDRED_ITER));
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

/* the real work of mean-median-stacking frames
 * return -1 for errors */
static int do_stack_mm(struct image_file_list *imfl, struct ccd_frame *ofr,
			int (* progress)(char *msg, void *data), void *data)
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
	while (gl != NULL) {
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



/* stack the frames; return a newly created frame, or NULL for an error */
struct ccd_frame * stack_frames(struct image_file_list *imfl, struct ccd_reduce *ccdr,
		 int (* progress)(char *msg, void *data), void *data)
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
/* do the actual stack */
	switch(P_INT(CCDRED_STACK_METHOD)) {
	case PAR_STACK_METHOD_AVERAGE:
		if (do_stack_avg(imfl, ofr, progress, data)) {
			release_frame(ofr);
			return NULL;
		}
		break;
	case PAR_STACK_METHOD_KAPPA_SIGMA:
		eff = 0.9;
		if (do_stack_ks(imfl, ofr, progress, data)) {
			release_frame(ofr);
			return NULL;
		}
		break;
	case PAR_STACK_METHOD_MEDIAN:
		eff = 0.65;
		if (do_stack_median(imfl, ofr, progress, data)) {
			release_frame(ofr);
			return NULL;
		}
		break;
	case PAR_STACK_METHOD_MEAN_MEDIAN:
		eff = 0.85;
		if (do_stack_mm(imfl, ofr, progress, data)) {
			release_frame(ofr);
			return NULL;
		}
		break;
	default:
		err_printf("unknown/unsupported stacking method %d\n",
			   P_INT(CCDRED_STACK_METHOD));
		release_frame(ofr);
		return NULL;
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



/* load the frame specififed in the image file struct into memory and
 * compute it's stats; return a negative error code (and update the

error string)
 * if something goes wrong. Return 0 for success. If the file is already loaded, it's not
 * read again */
int load_image_file(struct image_file *imf)
{
	if (imf->filename == NULL) {
		err_printf("load_image_file: null filename\n");
		return -1;
	}
	if (imf->filename[0] == 0) {
		err_printf("load_image_file: empty filename\n");
		return -1;
	}
	if (imf->flags & IMG_LOADED) {
		if (imf->fr == NULL) {
			err_printf("load_image_file: image flagged as loaded, but fr is NULL\n");
			return -1;
		}
		return 0;
	}
	imf->fr = read_gz_fits_file(imf->filename, P_STR(FILE_UNCOMPRESS),
				    P_INT(FILE_UNSIGNED_FITS));
	if (imf->fr == NULL) {
		err_printf("load_image_file: cannot load %s\n", imf->filename);
		return -1;
	}
	rescan_fits_exp(imf->fr, &(imf->fr->exp));
	if (!imf->fr->stats.statsok)
		frame_stats(imf->fr);
	imf->flags |= IMG_LOADED;
	return 0;
}

/* unload all ccd_frames that are not marked dirty */
void unload_clean_frames(struct image_file_list *imfl)
{
	GList *fl;
	struct image_file *imf;

	fl = imfl->imlist;
	while(fl != NULL) {
		imf = fl->data;
		fl = g_list_next(fl);
		if (!(imf->flags & IMG_DIRTY)) {
			if (imf->fr) {
				d3_printf("unloading\n");
				release_frame(imf->fr);
				imf->fr = NULL;
				imf->flags &= ~IMG_LOADED;
			}
		}
	}
}

static double star_size_flux(double flux, double ref_flux, double fwhm)
{
	double size;
	size = 1.0 * P_INT(DO_MAX_STAR_SZ) + 2.5 * P_DBL(DO_PIXELS_PER_MAG)
		* log10(flux / ref_flux);
	clamp_double(&size, 1.0 * P_INT(DO_MIN_STAR_SZ),
		     1.0 * P_INT(DO_MAX_STAR_SZ));
	return size;
}

/* detect the stars in frame and return them in a list of gui-stars
 * of "simple" type */
static GSList *detect_frame_stars(struct ccd_frame *fr)
{
	struct sources *src;
	struct gui_star *gs;
	GSList *as = NULL;
	double ref_flux = 0.0, ref_fwhm = 0.0;
	int i;

	g_return_val_if_fail(fr != NULL, NULL);

	src = new_sources(P_INT(SD_MAX_STARS));
	if (src == NULL) {
		err_printf("find_stars_cb: cannot create sources\n");
		return NULL;
	}
	extract_stars(fr, NULL, 0, P_DBL(SD_SNR), src);

/* now add to the list */
	if (src->ns != 0) {
		ref_flux = src->s[0].flux;
		ref_fwhm = src->s[0].fwhm;
	}

//	d3_printf("extract returns %d=n", src->ns);

	for (i = 0; i < src->ns; i++) {
		gs = gui_star_new();
		gs->x = src->s[i].x;
		gs->y = src->s[i].y;
		if (src->s[i].datavalid) {
			gs->size = star_size_flux(src->s[i].flux, ref_flux, ref_fwhm);
		} else {
			gs->size = 1.0 * P_INT(DO_DEFAULT_STAR_SZ);
		}
		gs->flags = STAR_TYPE_SIMPLE;
		as = g_slist_prepend(as, gs);
	}
	as = g_slist_reverse(as);
	d3_printf("found %d alignment stars\n", src->ns);
	return as;

}

/* free the alignment star list from the ccdr */
void free_alignment_stars(struct ccd_reduce *ccdr)
{
	GSList *as = NULL;

	as = ccdr->align_stars;
	while (as != NULL) {
		gui_star_release(GUI_STAR(as->data));
		as = g_slist_next(as);
	}
	g_slist_free(ccdr->align_stars);
	ccdr->align_stars = NULL;
	ccdr->ops &= ~CCDR_ALIGN_STARS;
}

/* search the alignment ref frame for alignment stars and build them in the
   align_stars list; return the number of stars found */
int load_alignment_stars(struct ccd_reduce *ccdr)
{
	GSList *as = NULL;
	struct gui_star *gs;

	if (!(ccdr->ops & IMG_OP_ALIGN)) {
		err_printf("no alignment ref frame\n");
		return -1;
	}
	g_return_val_if_fail(ccdr->alignref != NULL, -1);

	if (load_image_file(ccdr->alignref))
		return -1;

	if (ccdr->ops & CCDR_ALIGN_STARS) {
		free_alignment_stars(ccdr);
	}
	ccdr->align_stars = detect_frame_stars(ccdr->alignref->fr);
	if (ccdr->align_stars) {
		as = ccdr->align_stars;
		while (as != NULL) {
			gs = GUI_STAR(as->data);
			gs->flags = STAR_TYPE_ALIGN;
			as = g_slist_next(as);
		}
		ccdr->ops |= CCDR_ALIGN_STARS;
	}

	if (ccdr->alignref->fr) {
		d3_printf("unloading align\n");
		release_frame(ccdr->alignref->fr);
		ccdr->alignref->fr = NULL;
		ccdr->alignref->flags &= ~IMG_LOADED;
	}
	return g_slist_length(ccdr->align_stars);
}


int align_imf(struct image_file *imf, struct ccd_reduce *ccdr,
		   int (* progress)(char *msg, void *data), void *data)
{
	char msg[256];
	int ret;
	GSList *fsl, *sl = NULL;
	GSList *pairs = NULL;
	struct gui_star *gs;
	double dx, dy, ds, dt;

	if (!(ccdr->ops & CCDR_ALIGN_STARS)) {
		ret = load_alignment_stars(ccdr);
		if (progress) {
			snprintf(msg, 255, "Loading alignment stars: %d found\n", ret);
			(* progress)(msg, data);
		}
		if (ret == 0) {
			err_printf("no alignment stars, aborting\n");
			return -1;
		}
	}
	g_return_val_if_fail(ccdr->align_stars != NULL, -1);
	load_image_file(imf);

	fsl = detect_frame_stars(imf->fr);

	if (fsl == NULL) {
		err_printf("no frame stars, aborting\n");
		return -1;
	}

	ret = fastmatch(fsl, ccdr->align_stars);
	if (!ret) {
		if (progress) {
			snprintf(msg, 255, " no match");
			(* progress)(msg, data);
		}
		goto fexit;
	}
	if (progress) {
		snprintf(msg, 255, " %d/%d[%d]", ret, g_slist_length(fsl),
			 g_slist_length(ccdr->align_stars));
		(* progress)(msg, data);
	}

	sl = fsl;
	while (sl != NULL) {
		gs = GUI_STAR(sl->data);
		if ((gs->flags & STAR_HAS_PAIR) && gs->pair != NULL) {
			pairs = g_slist_prepend(pairs, gs);
		}
		sl = g_slist_next(sl);
	}

/*
	pairs_cs_diff(pairs, &dx, &dy, &ds, &dt, 1, 1);

	if (progress) {
		snprintf(msg, 255, " [%.1f, %.1f, %.3f, %.2f]",
			 dx, dy, ds, dt);
		(* progress)(msg, data);
	}
*/
	pairs_cs_diff(pairs, &dx, &dy, &ds, &dt, 0, 0);

	if (progress) {
		snprintf(msg, 255, " [%.1f, %.1f]", dx, dy);
		(* progress)(msg, data);
	}
	shift_frame(imf->fr, -dx, -dy);

fexit:
	sl = fsl;
	while (sl != NULL) {
		gs = GUI_STAR(sl->data);
		gui_star_release(gs);
		sl = g_slist_next(sl);
	}
	g_slist_free(fsl);
	return !ret;
}

/* alloc/free funtions for reduce-related objects */
struct image_file * image_file_new(void)
{
	struct image_file *imf;
	imf = calloc(1, sizeof(struct image_file));
	if (imf == NULL) {
		err_printf("error allocating an image_file\n");
		exit(1);
	}
	imf->ref_count = 1;
	return imf;
}

void image_file_ref(struct image_file *imf)
{
	g_return_if_fail(imf != NULL);
	imf->ref_count ++;
}

void image_file_release(struct image_file *imf)
{
	if (imf == NULL)
		return;
	g_return_if_fail(imf->ref_count >= 1);
	if (imf->ref_count > 1) {
		imf->ref_count--;
		return;
	}
	if (imf->filename)
		free(imf->filename);
	if (imf->fr)
		release_frame(imf->fr);
//	if (imf->item)
//		gtk_widget_unref(imf->item);
	free(imf);
}

struct ccd_reduce * ccd_reduce_new(void)
{
	struct ccd_reduce *ccdr;
	ccdr = calloc(1, sizeof(struct ccd_reduce));
	if (ccdr == NULL) {
		err_printf("error allocating a ccd_reduce\n");
		exit(1);
	}
	ccdr->ref_count = 1;
	return ccdr;
}

void ccd_reduce_ref(struct ccd_reduce *ccdr)
{
	g_return_if_fail(ccdr != NULL);
	ccdr->ref_count ++;
}


void ccd_reduce_release(struct ccd_reduce *ccdr)
{
	if (ccdr == NULL)
		return;
	g_return_if_fail(ccdr->ref_count >= 1);
	if (ccdr->ref_count > 1) {
		ccdr->ref_count--;
		return;
	}
	image_file_release(ccdr->bias);
	image_file_release(ccdr->dark);
	image_file_release(ccdr->flat);
/* TODO: also free the bad pixel map here */
	free(ccdr);
}

struct image_file_list * image_file_list_new(void)
{
	struct image_file_list *imfl;
	imfl = calloc(1, sizeof(struct image_file_list));
	if (imfl == NULL) {
		err_printf("error allocating an image_file_list\n");
		exit(1);
	}
	imfl->ref_count = 1;
	return imfl;
}

void image_file_list_ref(struct image_file_list *imfl)
{
	g_return_if_fail(imfl != NULL);
	imfl->ref_count ++;
}

void image_file_list_release(struct image_file_list *imfl)
{
	if (imfl == NULL)
		return;
	g_return_if_fail(imfl->ref_count >= 1);
	if (imfl->ref_count > 1) {
		imfl->ref_count--;
		return;
	}
	g_list_foreach(imfl->imlist, (GFunc)image_file_release, NULL);
	g_list_free(imfl->imlist);
	free(imfl);
}


