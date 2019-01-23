
#ifndef _LIBASTRO_EARTH_SAT_H
#define _LIBASTRO_EARTH_SAT_H


#define ESAT_MAG        2       /* fake satellite magnitude */

typedef double MAT3x3[3][3];

static int crazyOp (Now *np, Obj *op);
static void esat_prop (Now *np, Obj *op, double *SatX, double *SatY, double
    *SatZ, double *SatVX, double *SatVY, double *SatVZ);
static void GetSatelliteParams (Obj *op);
static void GetSiteParams (Now *np);
static double Kepler (double MeanAnomaly, double Eccentricity);
static void GetSubSatPoint (double SatX, double SatY, double SatZ,
    double T, double *Latitude, double *Longitude, double *Height);
static void GetSatPosition (double EpochTime, double EpochRAAN,
    double EpochArgPerigee, double SemiMajorAxis, double Inclination,
    double Eccentricity, double RAANPrecession, double PerigeePrecession,
    double T, double TrueAnomaly, double *X, double *Y, double *Z,
    double *Radius, double *VX, double *VY, double *VZ);
static void GetSitPosition (double SiteLat, double SiteLong,
    double SiteElevation, double CrntTime, double *SiteX, double *SiteY,
    double *SiteZ, double *SiteVX, double *SiteVY, MAT3x3 SiteMatrix);
static void GetRange (double SiteX, double SiteY, double SiteZ,
    double SiteVX, double SiteVY, double SatX, double SatY, double SatZ,
    double SatVX, double SatVY, double SatVZ, double *Range,
    double *RangeRate);
static void GetTopocentric (double SatX, double SatY, double SatZ,
    double SiteX, double SiteY, double SiteZ, MAT3x3 SiteMatrix, double *X,
    double *Y, double *Z);
static void GetBearings (double SatX, double SatY, double SatZ,
    double SiteX, double SiteY, double SiteZ, MAT3x3 SiteMatrix,
    double *Azimuth, double *Elevation);
static int Eclipsed (double SatX, double SatY, double SatZ,
    double SatRadius, double CrntTime);
static void InitOrbitRoutines (double EpochDay, int AtEod);

#ifdef USE_ORBIT_PROPAGATOR
static void GetPrecession (double SemiMajorAxis, double Eccentricity,
    double Inclination, double *RAANPrecession, double *PerigeePrecession);
#endif /* USE_ORBIT_PROPAGATOR */

/* stuff from orbit */
/* char VersionStr[] = "N3EMO Orbit Simulator  v3.9"; */

#ifdef PI2
#undef PI2
#endif

#define PI2 (PI*2)

#define MinutesPerDay (24*60.0)
#define SecondsPerDay (60*MinutesPerDay)
#define HalfSecond (0.5/SecondsPerDay)
#define EarthRadius 6378.16             /* Kilometers           */
#define C 2.997925e5                    /* Kilometers/Second    */
#define RadiansPerDegree (PI/180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define SQR(x) ((x)*(x))

#define EarthFlat (1/298.25)            /* Earth Flattening Coeff. */
#define SiderealSolar 1.0027379093
#define SidRate (PI2*SiderealSolar/SecondsPerDay)	/* radians/second */
#define GM 398600			/* Kilometers^3/seconds^2 */

#define Epsilon (RadiansPerDegree/3600)     /* 1 arc second */
#define SunRadius 695000
#define SunSemiMajorAxis  149598845.0  	    /* Kilometers 		   */

/*  Keplerian Elements and misc. data for the satellite              */
static double  EpochDay;                   /* time of epoch                 */
static double EpochMeanAnomaly;            /* Mean Anomaly at epoch         */
static long EpochOrbitNum;                 /* Integer orbit # of epoch      */
static double EpochRAAN;                   /* RAAN at epoch                 */
static double epochMeanMotion;             /* Revolutions/day               */
static double OrbitalDecay;                /* Revolutions/day^2             */
static double EpochArgPerigee;             /* argument of perigee at epoch  */
static double Eccentricity;
static double Inclination;

/* Site Parameters */
static double SiteLat,SiteLong,SiteAltitude;


static double SidDay,SidReference;	/* Date and sidereal time	*/

/* Keplerian elements for the sun */
static double SunEpochTime,SunInclination,SunRAAN,SunEccentricity,
       SunArgPerigee,SunMeanAnomaly,SunMeanMotion;

/* values for shadow geometry */
static double SinPenumbra,CosPenumbra;

#endif
