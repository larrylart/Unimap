
#ifndef _SAT_CODE_OBSERVE_H
#define _SAT_CODE_OBSERVE_H

#define JD2000 2451545.5
#define JD1900 (JD2000 - 36525. - 1.)

void lat_alt_to_parallax( const double lat, const double ht_in_meters,
                    double *rho_cos_phi, double *rho_sin_phi);
void observer_cartesian_coords( const double jd, const double lon,
              const double rho_cos_phi, const double rho_sin_phi,
              double *vect);
void get_satellite_ra_dec_delta( const double *observer_loc,
                                 const double *satellite_loc, double *ra,
                                 double *dec, double *delta);
void epoch_of_date_to_j2000( const double jd, double *ra, double *dec);

#endif

