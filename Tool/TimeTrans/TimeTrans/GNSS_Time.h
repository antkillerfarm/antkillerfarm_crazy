#ifndef _GNSS_TIME_H_
#define _GNSS_TIME_H_

typedef unsigned char		int8u;		// unsigned 8 bit integer
typedef signed char			int8s;		// signed 8bit integer
typedef unsigned short		int16u;		// unsigned 16 bit integer
typedef signed   short		int16s;		// signed 16 bit integer
typedef unsigned int		int32u;		// unsigned 32 bit integer
typedef signed   int		int32s;		// signed 32 bit integer
typedef unsigned long long	int64u;		// unsigned 64 bit integer
typedef long long			int64s;
typedef float				float32;	// 32 bit float
typedef double				float64;	// 64 bit float

#ifndef TRUE
	#define TRUE (1)
#endif

#ifndef FALSE
	#define FALSE (0)
#endif

#define SEC_OF_MIN	60
#define SEC_OF_HOUR	3600
#define SEC_OF_DAY	86400
#define SEC_OF_WEEK	604800

#define GPS_UTC_DELTA		16
#define BD2_UTC_DELTA		2
#define GPS_BD2_TOW_DELTA	14

#define GPS_WN_OFFSET		1024

#define JD_20060101 2453736.5 // Julian Day of Jan. 1th, 2006
#define JD_19800106 2444244.5 // Julian Day of Jan. 6th, 1980

typedef struct
{
	float64 sec;
	int16u year; 
	int8u mon;
	int8u day;
	int8u hour;
	int8u min;
} UTCTime, *pUTCTime;

typedef struct {
	float64 A1; // first order team of polynomial
	float64 A0; // constant term of polynomial
	float64 tot; // reference time for UTC data
	int8u WNt; // UTC reference week number
	int8s delta_tls; // delta time due to leap seconds
	int8u WNlsf; // week number at the end of which the leap second becomes effective
	int8u DN; // day number at the end of which the leap second becomes effective
	int8s delta_tlsf; // future or recent past value of delta time due to leap seconds
	bool valid; // whether UTC parameter is valid
	bool bakable;
} GPSUTCParameter, *pGPSUTCParameter; // broadcast UTC parameter

typedef struct {
	float64 A0; // constant term of polynomial
	float64 A1; // first order team of polynomial
	int8s delta_tls; // delta time due to leap seconds
	int8u WNlsf; // week number at the end of which the leap second becomes effective
	int8u DN; // day number at the end of which the leap second becomes effective
	int8s delta_tlsf; // future or recent past value of delta time due to leap seconds
	bool valid; // whether UTC parameter is valid
	bool bakable;
} BD2UTCParameter, *pBD2UTCParameter; // broadcast UTC parameter

#ifdef __cplusplus
extern "C"
{
#endif

	void ConvertJulianToUTC(float64 julian_day, float64 time_of_day, pUTCTime p_time);
	int32s GPSConvertUTC(const pGPSUTCParameter p_gps_utc_param, int32u wn, float64 tow, int32u wn_tow_state, pUTCTime p_utc_time, int8u turn);
	int32s BD2ConvertUTC(const pBD2UTCParameter p_bd2_utc_param, int32u wn, float64 tow, int32u wn_tow_state, pUTCTime p_utc_time);

#ifdef __cplusplus
}
#endif

#endif
