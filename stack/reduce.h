#ifndef _IMGLIST_H_
#define _IMGLIST_H_

/* an image file we may or may not have already loaded into memory; 
   we generally keep a GList of those around */

struct image_file {
	int ref_count;
	int flags;
	char *filename;
	struct ccd_frame *fr;
};

#define IMG_OP_BIAS 0x01 /* image has been bias-substracted */
#define IMG_OP_DARK 0x02 /* image has been dark-substracted */
#define IMG_OP_FLAT 0x04 /* image has been flat-fielded */
#define IMG_OP_BADPIX 0x08 /* image has had bad pixels corrected */
#define IMG_OP_ADD 0x10 /* image has had a constant bias added */
#define IMG_OP_MUL 0x20 /* image has been multiplied by a constant factor */
#define IMG_OP_BLUR 0x2000 /* image has been gaussian-blurred */
#define IMG_OP_ALIGN 0x40 /* image has been aligned */
#define IMG_OP_STACK 0x80 /* image was used for stacking */
#define IMG_OP_INPLACE 0x100 /* save the op's result to the source file */
#define IMG_OP_BG_ALIGN_ADD 0x200 /* align background additively */
#define IMG_OP_BG_ALIGN_MUL 0x400 /* align background multiplicatively */
#define CCDR_BG_VAL_SET 0x800 /* a ccdr flag telling that the bg target value is valid */
#define CCDR_ALIGN_STARS 0x1000 /* this tells us we should have something in align_stars */

#define IMG_LOADED 0x20000 /* image has been loaded into memory */
#define IMG_DIRTY 0x40000 /* image has unsaved changes */
#define IMG_SKIP 0x80000 /* image has been marked to be skipped on processing */

struct image_file * image_file_new(void);
void image_file_ref(struct image_file *imf);
void image_file_release(struct image_file *imf);

/* a ccd reduction data set */
struct ccd_reduce {
	int ref_count;
	int ops;
	double addv; /* a bias we add to the frames */
	double mulv; /* a value we multiply the frames by */
	double blurv; /* gaussian blur fwhm */
	double bg; /* target backgound value */
	struct image_file *bias;
	struct image_file *dark;
	struct image_file *flat;
	struct image_file *alignref;
	struct bad_pix_map *bad_pix_map;
	GSList *align_stars; 	/* a glist of alignment stars (SREF gui_stars) */
};

/* the 'ops' field uses IMG_OP flags */

struct ccd_reduce * ccd_reduce_new(void);
void ccd_reduce_ref(struct ccd_reduce *ccdr);
void ccd_reduce_release(struct ccd_reduce *ccdr);

struct image_file_list {
	int ref_count;
	GList *imlist;
};

struct image_file_list * image_file_list_new(void);
void image_file_list_ref(struct image_file_list *imfl);
void image_file_list_release(struct image_file_list *imfl);

int batch_reduce_frames(struct image_file_list *imfl, struct ccd_reduce *ccdr,
			char *outf);
int reduce_frames(struct image_file_list *imfl, struct ccd_reduce *ccdr, 
		   int (* progress)(char *msg, void *data), void *data);
int ccd_reduce_imf(struct image_file *imf, struct ccd_reduce *ccdr, 
		   int (* progress)(char *msg, void *data), void *data);
int load_image_file(struct image_file *imf);
int reduce_frame(struct image_file *imf, struct ccd_reduce *ccdr,
		 int (* progress)(char *msg, void *data), void *data);
struct ccd_frame * stack_frames(struct image_file_list *imfl, struct ccd_reduce *ccdr,
		 int (* progress)(char *msg, void *data), void *data);
struct ccd_frame *reduce_frames_load(struct image_file_list *imfl, struct ccd_reduce *ccdr);
void unload_clean_frames(struct image_file_list *imfl);
int save_image_file(struct image_file *imf, char *outf, int inplace, int *seq,
		    int (* progress)(char *msg, void *data), void *data);

int load_alignment_stars(struct ccd_reduce *ccdr);
int align_imf(struct image_file *imf, struct ccd_reduce *ccdr, 
	      int (* progress)(char *msg, void *data), void *data);
void free_alignment_stars(struct ccd_reduce *ccdr);

/* from reducegui.h */

void set_imfl_ccdr(gpointer window, struct ccd_reduce *ccdr, 
		   struct image_file_list *imfl);


#endif
