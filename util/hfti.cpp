/* hfti.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include <math.h>

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;

/* Subroutine */ 
int hfti_(real* a, integer* mda, integer* m, integer* n,
			real* b, integer* mdb, integer* nb, real* tau, 
			integer* krank, real* rnorm, real* h, real* g, integer* ip)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;
    real r__1;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    extern doublereal diff_();
    static real hmax;
    static integer lmax, i, j, k, l, ldiag;
    static doublereal dzero;
    static real szero;
    extern /* Subroutine */ int h12_();
    static integer jb, ii, jj;
    static doublereal sm;
    static real factor;
    static integer ip1, kp1;
    static real sm1, tmp;

/*     c.l.lawson and r.j.hanson, jet propulsion laboratory, 1973 jun 12 
*/
/*     to appear in 'solving least squares problems', prentice-hall, 1974 
*/
/* 	   solve least squares problem using algorithm, hfti. */

    /* Parameter adjustments */
    --ip;
    --g;
    --h;
    --rnorm;
    b_dim1 = *mdb;
    b_offset = b_dim1 + 1;
    b -= b_offset;
    a_dim1 = *mda;
    a_offset = a_dim1 + 1;
    a -= a_offset;

    /* Function Body */
    szero = (float)0.;
    dzero = 0.;
    factor = (float).001;

    k = 0;
    ldiag = min(*m,*n);
    if (ldiag <= 0) {
	goto L270;
    }
    i__1 = ldiag;
    for (j = 1; j <= i__1; ++j) {
	if (j == 1) {
	    goto L20;
	}

/*     update squared column lengths and find lmax */
/*    .. */
	lmax = j;
	i__2 = *n;
	for (l = j; l <= i__2; ++l) {
/* Computing 2nd power */
	    r__1 = a[j - 1 + l * a_dim1];
	    h[l] -= r__1 * r__1;
	    if (h[l] > h[lmax]) {
		lmax = l;
	    }
/* L10: */
	}
	r__1 = hmax + factor * h[lmax];
	if (diff_(&r__1, &hmax) <= (float)0.) {
	    goto L20;
	} else {
	    goto L50;
	}

/*     compute squared column lengths and find lmax */
/*    .. */
L20:
	lmax = j;
	i__2 = *n;
	for (l = j; l <= i__2; ++l) {
	    h[l] = (float)0.;
	    i__3 = *m;
	    for (i = j; i <= i__3; ++i) {
/* L30: */
/* Computing 2nd power */
		r__1 = a[i + l * a_dim1];
		h[l] += r__1 * r__1;
	    }
	    if (h[l] > h[lmax]) {
		lmax = l;
	    }
/* L40: */
	}
	hmax = h[lmax];
/*    .. */
/*     lmax has been determined */

/*     do column interchanges if needed. */
/*    .. */
L50:
	ip[j] = lmax;
	if (ip[j] == j) {
	    goto L70;
	}
	i__2 = *m;
	for (i = 1; i <= i__2; ++i) {
	    tmp = a[i + j * a_dim1];
	    a[i + j * a_dim1] = a[i + lmax * a_dim1];
/* L60: */
	    a[i + lmax * a_dim1] = tmp;
	}
	h[lmax] = h[j];

/*     compute the j-th transformation and apply it to a and b. */
/*    .. */
L70:
	i__2 = j + 1;
	i__3 = *n - j;
	h12_(&c__1, &j, &i__2, m, &a[j * a_dim1 + 1], &c__1, &h[j], &a[(j + 1)
		 * a_dim1 + 1], &c__1, mda, &i__3);
/* L80: */
	i__2 = j + 1;
	h12_(&c__2, &j, &i__2, m, &a[j * a_dim1 + 1], &c__1, &h[j], &b[
		b_offset], &c__1, mdb, nb);
    }

/*     determine the pseudorank, k, using the tolerance, tau. */
/*    .. */
    i__2 = ldiag;
    for (j = 1; j <= i__2; ++j) {
	if ((r__1 = a[j + j * a_dim1], dabs(r__1)) <= *tau) {
	    goto L100;
	}
/* L90: */
    }
    k = ldiag;
    goto L110;
L100:
    k = j - 1;
L110:
    kp1 = k + 1;

/*     compute the norms of the residual vectors. */

    if (*nb <= 0) {
	goto L140;
    }
    i__2 = *nb;
    for (jb = 1; jb <= i__2; ++jb) {
	tmp = szero;
	if (kp1 > *m) {
	    goto L130;
	}
	i__1 = *m;
	for (i = kp1; i <= i__1; ++i) {
/* L120: */
/* Computing 2nd power */
	    r__1 = b[i + jb * b_dim1];
	    tmp += r__1 * r__1;
	}
L130:
	rnorm[jb] = sqrt(tmp);
    }
L140:
/* 					    special for pseudorank = 0 */
    if (k > 0) {
	goto L160;
    }
    if (*nb <= 0) {
	goto L270;
    }
    i__2 = *nb;
    for (jb = 1; jb <= i__2; ++jb) {
	i__1 = *n;
	for (i = 1; i <= i__1; ++i) {
/* L150: */
	    b[i + jb * b_dim1] = szero;
	}
    }
    goto L270;

/*     if the pseudorank is less than n compute householder */
/*     decomposition of first k rows. */
/*    .. */
L160:
    if (k == *n) {
	goto L180;
    }
    i__1 = k;
    for (ii = 1; ii <= i__1; ++ii) {
	i = kp1 - ii;
/* L170: */
	i__2 = i - 1;
	h12_(&c__1, &i, &kp1, n, &a[i + a_dim1], mda, &g[i], &a[a_offset], 
		mda, &c__1, &i__2);
    }
L180:


    if (*nb <= 0) {
	goto L270;
    }
    i__2 = *nb;
    for (jb = 1; jb <= i__2; ++jb) {

/*     solve the k by k triangular system. */
/*    .. */
	i__1 = k;
	for (l = 1; l <= i__1; ++l) {
	    sm = dzero;
	    i = kp1 - l;
	    if (i == k) {
		goto L200;
	    }
	    ip1 = i + 1;
	    i__3 = k;
	    for (j = ip1; j <= i__3; ++j) {
/* L190: */
		sm += a[i + j * a_dim1] * (doublereal) b[j + jb * b_dim1];
	    }
L200:
	    sm1 = sm;
/* L210: */
	    b[i + jb * b_dim1] = (b[i + jb * b_dim1] - sm1) / a[i + i * 
		    a_dim1];
	}

/*     complete computation of solution vector. */
/*    .. */
	if (k == *n) {
	    goto L240;
	}
	i__1 = *n;
	for (j = kp1; j <= i__1; ++j) {
/* L220: */
	    b[j + jb * b_dim1] = szero;
	}
	i__1 = k;
	for (i = 1; i <= i__1; ++i) {
/* L230: */
	    h12_(&c__2, &i, &kp1, n, &a[i + a_dim1], mda, &g[i], &b[jb * 
		    b_dim1 + 1], &c__1, mdb, &c__1);
	}

/*      re-order the solution vector to compensate for the */
/*      column interchanges. */
/*    .. */
L240:
	i__1 = ldiag;
	for (jj = 1; jj <= i__1; ++jj) {
	    j = ldiag + 1 - jj;
	    if (ip[j] == j) {
		goto L250;
	    }
	    l = ip[j];
	    tmp = b[l + jb * b_dim1];
	    b[l + jb * b_dim1] = b[j + jb * b_dim1];
	    b[j + jb * b_dim1] = tmp;
L250:
	    ;
	}
/* L260: */
    }
/*    .. */
/*     the solution vectors, x, are now */
/*     in the first  n  rows of the array b(,). */

L270:
    *krank = k;
    return 0;
} /* hfti_ */

