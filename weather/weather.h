
#ifndef _WEATHER_H
#define _WEATHER_H

///////////////////////
// weather record
typedef struct
{
	wxChar site_id[100];
	wxChar cond_icon[50];
	wxChar cond_text[100];

	double datetime;

	// weather station location
	double station_latitude;
	double station_longitude;
	double station_altitude;

	// weather data
	double temperature;
	double temperature_var;

	double humidity;
	double windchill;
	double dew_point;
	double heat_index;
	// wind, variation, direction
	double wind;
	int wind_dir;
	wxChar wind_dir_str[10];
	int wind_var;

	double pressure;
	int pressure_var;
	// Conditions code - aka Scattered Clouds
	int conditions;

	double uv;
	int clouds;
	int clouds_low;
	int clouds_high;
	double visibility;

} DefWeatherRecord;

////////////////////////////
// weather forecast record
typedef struct
{
	int id;

	time_t epoch;
	time_t fetch_time;

	// strings
//	char conditions[100];
//	char icon[100];
	wxChar skyicon[100];

	//////////
	// new records
	int time;
	double temperature;
	double dew;
	double humidity;
	double wind_speed;
	double wind_dir;
	double uv;

	wxChar cond_icon[50];
	wxChar cond_text[50];

	double precip;
	double clouds;

} DefWeatherForecastBaseRecord;

///////////////////////////
// weather forecast record
typedef struct
{
	time_t epoch;
	time_t fetch_time;

	double temperature_night;
	double temperature_day;

	wxChar cond_icon_day[50];
	wxChar cond_text_day[50];

	wxChar cond_icon_night[50];
	wxChar cond_text_night[50];

	// a set of day overall values - maybe remove ?
	DefWeatherForecastBaseRecord basic;
	// day's records
	DefWeatherForecastBaseRecord vectDetail[8];
	int nDetail;

} DefWeatherForecastRecord;

////////////////////////////
// seeing prediction record
typedef struct
{
	time_t time;

	// color - for every var
	unsigned char red[12];
	unsigned char green[12];
	unsigned char blue[12];

	unsigned char high_clouds;
	unsigned char mid_clouds;
	unsigned char low_clouds;

	unsigned char seeing_id_1;
	unsigned char seeing_id_2;
//	unsigned char seeing_id_3;

	double seeing_arcsec;
	double jet_stream;
	unsigned char rel_hum_2m;

	double bad_layers_top;
	double bad_layers_bot;
	double bad_layers_k;	

} DefSeeingPredictRec;

// seeing prediction record
typedef struct
{
	time_t date;

	DefSeeingPredictRec hour[24];

} DefSeeingPredictDayRec;

// external classes
class CConfigMain;
//class CUnimapOnline;

// class:	CWeather
///////////////////////////////////////////////////////
class CWeather
{
// methods
public:
    CWeather( );
	~CWeather();

//	void SetConfig( CConfigMain* pMainConfig );
	int GetCurrentWeatherData( double lat, double lon, int nSrcId );
	int GetWeatherData_AtDateTime( double lat, double lon, double obs_time, int nSrcId );
	int GetWeatherData_Forecast( double lat, double lon, int nSrcId=0 );
	int GetWeatherData_DailyForecast( int day, double lat, double lon, int nSrcId=0 );

	int GetSeeingPrediction( double lat, double lon, int nSrcId=0 );

	int IsDailyForecast_ToUpdate( int day );
	int IsForecast_ToUpdate( );
	int IsSeeing_ToUpdate( double lat, double lon, int type=0 );

	int GetBestSeeingPredictEngine( double lat, double lon );
		
// data
public:

	// current data weather record
	DefWeatherRecord m_rCurrent;
	// historical weather record
	DefWeatherRecord m_vectPastRec[101];
	int m_nPastRec;
	// future/forecast weather record
	DefWeatherForecastRecord m_vectForecastRec[10];
	int m_nForecastRec;

	// time of last forecast fetch - to avoid loadind all the time
	time_t m_nForecastFetchTime;
	time_t m_nSeeingFetchTime;

	// exceptions
	int m_nFirstForecastRec;

	///////////////////
	// seeing prediction definition
	DefSeeingPredictDayRec m_vectSeeingPredict[10];
	int m_nSeeingPredict;
	// seeing prediction image
	wxString m_strSeeingPredictImg;

	// data fetch status vars
	int m_nFetchError;
	wxString m_strFetchError;
};

#endif
