#ifndef AFUNCS_H_INCLUDED
#define AFUNCS_H_INCLUDED

#ifndef DPT
#define DPT struct dpt

DPT
   {
   double x, y;
   };
#endif /* #ifndef DPT */

#ifndef AU_IN_KM
#define AU_IN_KM 1.49597870691e+8
#endif

#ifndef AU_IN_METERS
#define AU_IN_METERS (AU_IN_KM * 1000.)
#endif

#ifndef SPEED_OF_LIGHT
#define SPEED_OF_LIGHT 299792.458
#endif

#ifndef AU_PER_DAY
#define AU_PER_DAY (86400. * SPEED_OF_LIGHT / AU_IN_KM)
#endif

extern int Sol_nutation( const double t, double *d_lon, double *d_obliq );
extern double mean_obliquity( const double t_cen );
extern void alt_az_to_ra_dec( double alt, double az, double *hr_ang, double *dec, const double lat );

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

void make_var_desig( char  *buff, int var_no);
int decipher_var_desig( const char  *desig);
int setup_precession( double  *matrix, double t1,
                               double t2);    /* precess.c */
int setup_ecliptic_precession( double  *matrix,
                    const double t1, const double t2);
int precess_vector( const double  *matrix,
                                      const double  *v1,
                                      double  *v2);    /* precess.c */
int deprecess_vector( const double  *matrix,
                                      const double  *v1,
                                      double  *v2);    /* precess.c */
int precess_ra_dec( const double  *matrix,
                        double  *p_out,
                        const double  *p_in, int backward);
void rotate_vector( double  *v, const double angle,
                                          const int axis);
void polar3_to_cartesian( double *vect, const double lon,
                                          const double lat);
void set_identity_matrix( double  *matrix);
void invert_orthonormal_matrix( double  *matrix);
void spin_matrix( double *v1, double *v2, const double angle);
void pre_spin_matrix( double *v1, double *v2, const double angle);
double td_minus_ut( const double jd);                /* delta_t.c */
double td_minus_utc( const double jd_utc);           /* delta_t.c */
void reset_td_minus_dt_string( const char *string);  /* delta_t.c */

double green_sidereal_time( double t);              /* alt_az.c */
double acose( const double arg);                    /* alt_az.c */
double asine( const double arg);                    /* alt_az.c */
void full_ra_dec_to_alt_az( const DPT  *ra_dec,
                DPT  *alt_az,
                DPT  *loc_epoch, const DPT  *latlon,
                const double jd_utc, double  *hr_ang);
void full_alt_az_to_ra_dec( DPT  *ra_dec,
                              const DPT  *alt_az,
                              const double jd_utc, const DPT  *latlon);
void ra_dec_to_galactic( const double ra, const double dec,
                   double  *glat, double  *glon);
const double *  j2000_to_galactic_matrix( void);
void galactic_to_ra_dec( const double glat, double glon,
                    double  *ra, double  *dec);
void ra_dec_to_supergalactic( const double ra, const double dec,
                   double  *glat, double  *glon);
const double * j2000_to_supergalactic_matrix( void);
void supergalactic_to_ra_dec( const double glat, double glon,
                    double  *ra, double  *dec);
void precess_pt( DPT  *opt, const DPT  *ipt,
                       double from_time, double to_time);
int get_comet_file( const char *cd_path,
                 const double year, const double mag_limit);
int extract_periodic_name( const char *istr, char *ostr);
double refraction( const double observed_alt);
double reverse_refraction( const double true_alt);
double saasta_refraction( const double observed_alt,
         const double pressure_mb, const double temp_kelvin,
         const double relative_humidity);
double reverse_saasta_refraction( const double true_alt,
         const double pressure_mb, const double temp_kelvin,
         const double relative_humidity);
double integrated_refraction( const double latitude,
                  const double observed_alt, const double wavelength_microns,
                  const double height_in_meters, const double rel_humid_pct,
                  const double temp_kelvins, const double pressure_mb);
double reverse_integrated_refraction( const double latitude,
                  const double refracted_alt, const double wavelength_microns,
                  const double height_in_meters, const double rel_humid_pct,
                  const double temp_kelvins, const double pressure_mb);

int  calc_dist_and_posn_ang( const double *p1, const double *p2,
                                       double *dist, double *posn_ang);
void  reverse_dist_and_posn_ang( double *to, const double *from,
                                 const double dist, const double posn_ang);

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */
#endif  /* #ifndef AFUNCS_H_INCLUDED */
