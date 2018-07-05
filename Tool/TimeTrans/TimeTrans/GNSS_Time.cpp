#include "StdAfx.h"
#include "GNSS_Time.h"

void ConvertJulianToUTC(float64 julian_day, float64 time_of_day, pUTCTime p_time)
{
	float64 original_time_of_day;
	int32u a, b, c, d, e;

	if(julian_day != 0.0)
	{
		original_time_of_day = time_of_day;
		time_of_day = (int32u)(time_of_day * 100.0 + 0.5) / 100.0;
		if ((original_time_of_day < SEC_OF_DAY) && (time_of_day >= SEC_OF_DAY)) 
		{
			time_of_day -= SEC_OF_DAY;
			julian_day++;
		}

		a = (int32u)(julian_day + 0.5);
		b = a + 1537;
		c = (int32u)((b-122.1) / 365.25);
		d = (int32u)(c * 365.25);
		e = (int32u)((b-d) / 30.6001);

		p_time->day = b - d - (int32u)(e*30.6001);
		p_time->mon = e - 1 - e/14*12;
		p_time->year = c - 4715 - (p_time->mon+7)/10;

		
	}
	else
	{
		original_time_of_day = time_of_day;
		time_of_day = (int32u)(time_of_day * 100.0 + 0.5) / 100.0;
		if ((original_time_of_day < SEC_OF_DAY) && (time_of_day >= SEC_OF_DAY)) 
		{
			time_of_day -= SEC_OF_DAY;
		}

		p_time->day = 0;
		p_time->mon = 0;
		p_time->year = 0;
	}

	time_of_day = time_of_day + 0.05;
	time_of_day = (float64)((int64u)(time_of_day * 10.0)) / 10.0;

	p_time->hour = (int32u)(time_of_day / SEC_OF_HOUR);
	if (p_time->hour == 24) 
	{
		p_time->hour = 23;
	}
	p_time->min = (int32u)((time_of_day - p_time->hour*SEC_OF_HOUR) / SEC_OF_MIN);
	if (p_time->min == 60) 
	{
		p_time->min = 59;
	}
	p_time->sec = time_of_day - p_time->hour*SEC_OF_HOUR - p_time->min*SEC_OF_MIN;
}

int32s GPSConvertUTC(const pGPSUTCParameter p_gps_utc_param, int32u wn, float64 tow, int32u wn_tow_state, pUTCTime p_utc_time, int8u turn)
{
	float64 gps_time, gps_wn, delta_utc, delta_t, utc_time_of_day, W, tmp, jd;
	int32u utc_day, integer;
	int32s leap_second;

	leap_second = GPS_UTC_DELTA;

	if(wn_tow_state == 0)
	{
		jd = 0.0;
		utc_time_of_day = 0.0;
	}
	else
	if(wn_tow_state == 1)
	{
		jd = 0.0;
		
		tmp = tow - leap_second;
		
		if (tmp < 0.0) 
		{
			tmp = tmp + SEC_OF_WEEK;
		}
		if (tmp >= SEC_OF_WEEK) 
		{
			tmp = tmp - SEC_OF_WEEK;
		}

		utc_day = (int32u)(tmp / SEC_OF_DAY);
		utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
	}
	else
	{
		
		if ((p_gps_utc_param->valid == FALSE) && ((wn - 668) < 26))
		{
			p_gps_utc_param->valid = TRUE;	

			p_gps_utc_param->A1 = 0;
			p_gps_utc_param->A0 = 0;
			p_gps_utc_param->tot = 405504;
			p_gps_utc_param->WNt = 173;
			p_gps_utc_param->delta_tls = 16;
			p_gps_utc_param->WNlsf = 158;
			p_gps_utc_param->DN = 7;
			p_gps_utc_param->delta_tlsf = 16;
		}
		
		gps_time = (wn + turn * GPS_WN_OFFSET) * SEC_OF_WEEK + tow;
		if (p_gps_utc_param->valid == TRUE) 
		{
			gps_wn = (float64)(wn & 0xff);
			delta_utc = p_gps_utc_param->delta_tls + p_gps_utc_param->A0 
						+ p_gps_utc_param->A1 * ((gps_wn - p_gps_utc_param->WNt) * SEC_OF_WEEK + tow - p_gps_utc_param->tot);

			delta_t = (gps_wn - p_gps_utc_param->WNlsf) * SEC_OF_WEEK + tow - p_gps_utc_param->DN * SEC_OF_DAY;

			if (delta_t < (-6.0*SEC_OF_HOUR)) 
			{
				tmp = gps_time - delta_utc;
				
				utc_day = (int32u)(tmp / SEC_OF_DAY);
				utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
			}
			else if ((delta_t >= (-6.0*SEC_OF_HOUR)) && (delta_t <= (6.0*SEC_OF_HOUR))) 
			{
				tmp = gps_time - delta_utc - 43200.0;
				integer = (int32u)(tmp / SEC_OF_DAY);
				W = tmp - integer * SEC_OF_DAY + 43200.0;

				tmp = SEC_OF_DAY + p_gps_utc_param->delta_tlsf - p_gps_utc_param->delta_tls;
				integer = (int32u)(W / tmp);
				
				utc_time_of_day = W - integer * tmp;
				utc_day = (int32u)((gps_time - utc_time_of_day) / SEC_OF_DAY);
			}
			else 
			{
				delta_utc = delta_utc - p_gps_utc_param->delta_tls + p_gps_utc_param->delta_tlsf;

				tmp = gps_time - delta_utc;
				
				utc_day = (int32u)(tmp / SEC_OF_DAY);
				utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
			}
			if(delta_utc < 0.0)
			{
				leap_second = (int32s)(delta_utc - 0.5);	
			}
			else
			{
				leap_second = (int32s)(delta_utc + 0.5);	
			}
		}
		else 
		{
			delta_utc = leap_second;

			tmp = gps_time - delta_utc;
			utc_day = (int32u)(tmp / SEC_OF_DAY);
			utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
		}

		jd = utc_day + JD_19800106; 
	}

	ConvertJulianToUTC(jd, utc_time_of_day, p_utc_time);
	
	return leap_second;
}

int32s BD2ConvertUTC(const pBD2UTCParameter p_bd2_utc_param, int32u wn, float64 tow, int32u wn_tow_state, pUTCTime p_utc_time)
{
	float64 bd2_time, bd2_wn, delta_utc, delta_t, utc_time_of_day, W, tmp, jd;
	int32u utc_day, integer;
	int32s leap_second;

	leap_second = BD2_UTC_DELTA;
	
	if(wn_tow_state == 0)
	{
		jd = 0.0;
		utc_time_of_day = 0.0;
	}
	else
	if(wn_tow_state == 1)
	{
		jd = 0.0;

		tmp = tow - leap_second;
		
		if (tmp < 0.0) 
		{
			tmp = tmp + SEC_OF_WEEK;
		}
		if (tmp >= SEC_OF_WEEK) 
		{
			tmp = tmp - SEC_OF_WEEK;
		}

		utc_day = (int32u)(tmp / SEC_OF_DAY);
		utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
	}
	else
	{
		
		if ((p_bd2_utc_param->valid == FALSE) && ((wn - 337) < 26))
		{
			p_bd2_utc_param->valid = TRUE;
			
			p_bd2_utc_param->A0 = 0.0;
			p_bd2_utc_param->A1 = 0.0;
			p_bd2_utc_param->delta_tls = 2;
			p_bd2_utc_param->WNlsf = 82;
			p_bd2_utc_param->DN = 6;
			p_bd2_utc_param->delta_tlsf = 2;
		}
		
		bd2_time = wn*SEC_OF_WEEK + tow;	
		
		if (p_bd2_utc_param->valid == TRUE) 
		{
			bd2_wn = (float64)(wn & 0xff);

			delta_utc = p_bd2_utc_param->delta_tls + p_bd2_utc_param->A0 + p_bd2_utc_param->A1 * tow;

			delta_t = (bd2_wn - p_bd2_utc_param->WNlsf) * SEC_OF_WEEK + tow - (p_bd2_utc_param->DN + 1)*SEC_OF_DAY;

			if (delta_t < (-6.0*SEC_OF_HOUR)) 
			{
				tmp = bd2_time - delta_utc;
				utc_day = (int32u)(tmp / SEC_OF_DAY);
				utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
			}
			else 
			if ((delta_t >= (-6.0*SEC_OF_HOUR)) && (delta_t <= (6.0*SEC_OF_HOUR))) 
			{
				tmp = bd2_time - delta_utc - 43200.0;
				integer = (int32u)(tmp / SEC_OF_DAY);
				W = tmp - integer*SEC_OF_DAY + 43200.0;

				tmp = SEC_OF_DAY + p_bd2_utc_param->delta_tlsf - p_bd2_utc_param->delta_tls;
				integer = (int32u)(W / tmp);
				utc_time_of_day = W - integer*tmp;

				utc_day = (int32u)((bd2_time - utc_time_of_day) / SEC_OF_DAY);
			}
			else 
			{
				delta_utc = delta_utc - p_bd2_utc_param->delta_tls + p_bd2_utc_param->delta_tlsf;

				tmp = bd2_time - delta_utc;
				utc_day = (int32u)(tmp / SEC_OF_DAY);
				utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
			}

			if(delta_utc < 0.0)
			{
				leap_second = (int32s)(delta_utc - 0.5);	
			}
			else
			{
				leap_second = (int32s)(delta_utc + 0.5);	
			}
		}
		else 
		{
			delta_utc = leap_second;

			tmp = bd2_time - delta_utc;
			utc_day = (int32u)(tmp / SEC_OF_DAY);
			utc_time_of_day = tmp - utc_day*SEC_OF_DAY;
		}
		
		jd = utc_day + JD_20060101; // Julian day
	}
	
	ConvertJulianToUTC(jd, utc_time_of_day, p_utc_time);

	return leap_second;
}
