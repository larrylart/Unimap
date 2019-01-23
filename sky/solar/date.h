
#ifndef _SOLAR_DATE_H
#define _SOLAR_DATE_H

#ifdef __cplusplus
extern "C" {
#endif

long  dmy_to_day( const int day, const int month, const long year,
                            const int calendar);
void  day_to_dmy( const long jd, int  *day,
                  int  *month, long  *year, const int calendar);
void  full_ctime( char *buff, double jd, const int format);
const char *  set_month_name( const int month, const char *new_name);
const char *  set_day_of_week_name( const int day_of_week,
                                            const char *new_name);
double  decimal_day_to_dmy( double jd, long *year, int *month,
                                 const int calendar);
double  split_time( double jd, long *year, int *month, int *day,
                                 int *hr, int *min, const int time_format);
double  get_time_from_string( const double initial_jd,
                     const char *str, const int time_format);

#define FULL_CTIME_FORMAT_MASK           0x700
#define FULL_CTIME_FORMAT_SECONDS        0x000
#define FULL_CTIME_FORMAT_HH_MM          0x100
#define FULL_CTIME_FORMAT_HH             0x200
#define FULL_CTIME_FORMAT_DAY            0x300
#define FULL_CTIME_FORMAT_YEAR           0x400
#define FULL_CTIME_FORMAT_JD             0x500
#define FULL_CTIME_FORMAT_MJD            0x600
#define FULL_CTIME_YEAR_FIRST            0x800
#define FULL_CTIME_YEAR_LAST                 0
#define FULL_CTIME_MONTH_DAY            0x1000
#define FULL_CTIME_DAY_MONTH                 0
#define FULL_CTIME_MONTHS_AS_DIGITS     0x2000
#define FULL_CTIME_TIME_ONLY            0x4000
#define FULL_CTIME_LEADING_ZEROES       0x8000
#define FULL_CTIME_TWO_DIGIT_YEAR       0x10000
#define FULL_CTIME_NO_YEAR              0x20000
#define FULL_CTIME_DAY_OF_WEEK_FIRST    0x40000
#define FULL_CTIME_DAY_OF_WEEK_LAST     0x80000

         /* Some convenience combos of the above flags: */
#define FULL_CTIME_YMD           (FULL_CTIME_YEAR_FIRST | FULL_CTIME_MONTH_DAY)
#define FULL_CTIME_YDM           (FULL_CTIME_YEAR_FIRST | FULL_CTIME_DAY_MONTH)
#define FULL_CTIME_DMY           (FULL_CTIME_YEAR_LAST | FULL_CTIME_DAY_MONTH)
#define FULL_CTIME_MDY           (FULL_CTIME_YEAR_LAST | FULL_CTIME_MONTH_DAY)

         /* "date only" = show in days with zero decimal places: */
#define FULL_CTIME_DATE_ONLY   FULL_CTIME_FORMAT_DAY

#define FULL_CTIME_TENTHS_SEC           0x10
#define FULL_CTIME_WHOLE_SECONDS           0
#define FULL_CTIME_HUNDREDTH_SEC        0x20
#define FULL_CTIME_MILLISECS            0x30

/* The calendar is stored in the lowest four bits (nibble),  allowing
for 16 possible calendars;  only 9 are currently assigned,  but there
are plenty of candidates for the remaining seven: */

#define CALENDAR_MASK                  0xf

#define CALENDAR_GREGORIAN             0
#define CALENDAR_JULIAN                1
#define CALENDAR_HEBREW                2
#define CALENDAR_ISLAMIC               3
#define CALENDAR_REVOLUTIONARY         4
#define CALENDAR_PERSIAN               5
#define CALENDAR_CHINESE               7
#define CALENDAR_MODERN_PERSIAN        8

/* The "Julian/Gregorian" calendar uses the Julian system before
   mid-September 1582,  and Gregorian after that.   */

#define CALENDAR_JULIAN_GREGORIAN      6

/* The usual switchover date between the two calendars is JD 2299160.5,   */
/* which corresponds to 15 Oct 1582 (Gregorian) = 5 Oct 1582 (Julian).    */
/* That is to say,  4 October 1582 was followed by 15 October 1582;       */
/* 5 October-14 October 1582 "never happened".                            */

/*           October 1582           */
/*       Su Mo Tu We Th Fr Sa       */
/*           1  2  3  4 15 16       */
/*       17 18 19 20 21 22 23       */
/*       24 25 26 27 28 29 30       */
/*       31                         */

#define GREGORIAN_SWITCHOVER_JD     2299160L

         /* The Chinese calendar routines use a buffer of precomputed data */
         /* indicating when various months occur,  which are considered    */
         /* intercalary,  etc.  This function sets that buffer (see jd.cpp */
         /* for an example of the use of both of the following functions): */
void  set_chinese_calendar_data( void *cdata);
int  get_chinese_intercalary_month( void);

#ifdef __cplusplus
}
#endif

#endif
