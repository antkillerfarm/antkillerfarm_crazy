/*
* @file    strlib.c
*
* @brief   contain string library routines
* @date    Dec.29th, 2008
*
*
* Copyright (C) TechToTop, Inc.
*
* MODIFICATIONS:
*		Dec. 29th, 2008
*			-content: create this file
*
*		Apr. 2nd, 2009
*			-content: correct zero fill bug when number is negitive
*
*/
#include "types.h"
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define LITTLE_ENDIAN
#define inline __inline
#define finite _finite
#define isnan _isnan

/* internal macro */
#define MAX_BUF_LEN 2048
#define MAX_SUBSTRING_LEN 256

#define PRT_PRE_PERCENT	0x1 // '%' in format string
#define PRT_FILL_ZERO	0x2 // fill '0'
#define PRT_LEFT_FILL	0x4 // right align, left fill
#define PRT_SIGNED		0x8 // number is negitive, with '-'
#define PRT_STRING_OUT	0x10 // output string
#define PRT_PREC_FLAG	0x20 // precesion flag
#define PRT_LONG_FLAG	0x40 // 64bit long int flag

/* internal data structure */
#ifdef BIG_ENDIAN
typedef union 
{
	float64 value;

	struct 
	{
		int32u sign      : 1;
		int32u exponent  : 11;
		int32u fraction0 : 4;
		int32u fraction1 : 16;
		int32u fraction2 : 16;
		int32u fraction3 : 16;
	} number;

	struct 
	{
		int32u sign      : 1;
		int32u exponent  : 11;
		int32u quiet     : 1;
		int32u function0 : 3;
		int32u function1 : 16;
		int32u function2 : 16;
		int32u function3 : 16;
	} nan;

	struct 
	{
		int32u msw;
		int32u lsw;
	} parts;

	int32s aslong[2];
} ieee_double_shape_type;
#endif

#ifdef LITTLE_ENDIAN
typedef union 
{
	float64 value;

	struct 
	{
#ifdef __SMALL_BITFIELDS
		int32u fraction3 : 16;
		int32u fraction2 : 16;
		int32u fraction1 : 16;
		int32u fraction0 : 4;
#else
		int32u fraction1 : 32;
		int32u fraction0 : 20;
#endif
		int32u exponent  : 11;
		int32u sign      : 1;
	} number;

	struct 
	{
#ifdef __SMALL_BITFIELDS
		int32u function3 : 16;
		int32u function2 : 16;
		int32u function1 : 16;
		int32u function0 : 3;
#else
		int32u function1 : 32;
		int32u function0 : 19;
#endif
		int32u quiet     : 1;
		int32u exponent  : 11;
		int32u sign      : 1;
	} nan;

	struct 
	{
		int32u lsw;
		int32u msw;
	} parts;

	int32s aslong[2];
} ieee_double_shape_type;
#endif


/* internal function */
static inline int32u fw_vsprintf(int8u *buf, int32u buf_len, const int8u *format, va_list arg);
static inline int32u format_integer(int8u *str, int32s value, int8u type, int32u width, int32u flags);
static inline int32u format_long_integer(int8u *str, int64s value, int8u type, int32u width, int32u flags);
static inline int32u format_float(int8u *str, float64 value, int32u width, int32u prec, int32u flags);
static inline int32u format_string(int8u *str, int8u *value, int32u width, int32u flags);
static void format_fill(int8u *p_src, int8u *p_des, int32u width, int32u len, int32u fill_flag);


/*
* @function    fw_sprintf
*
* @brief   Firmware user-define sprintf function 
* @date    Dec. 29th, 2008
*/
int32u fw_sprintf(int8u *buffer, const int8u *format, ...)
{
	int32u r;

	va_list marker;
	va_start(marker, format);

	r = fw_vsprintf(buffer, MAX_BUF_LEN, format, marker);

	va_end(marker);

	buffer[r] = '\0';
	return r;
}


/*
* @function    fw_vsprintf
*
* @brief   Firmware user-define sprintf function 
* @date    Feb. 24th, 2009
*/
static inline int32u fw_vsprintf(int8u *buf, int32u buf_len, const int8u *format, va_list arg)
{
	float64 ft_val;
	int32u pre_flag = 0;
	int32u width = 0;
	int32u prec = 6;// default decimal precesion is 6 bits
	int32s int_val;
	int64s long_val;
	int32u res = 0, len;
	int8u *p = (int8u *)format;
	int8u *str_val;
	
	while ((*p != '\0') && (res <= buf_len)) {
		if (pre_flag == 0) { // search '%' or '\' in format string
			if (*p == '%') {
				pre_flag = PRT_PRE_PERCENT;
				width = 0;
			}
			else {
				*(buf++) = *p;
				res++;
			}
		}
		else { // scan format string
			switch (*p) {
				case '0':
					pre_flag |= PRT_FILL_ZERO;
					
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					do {
						width = width*10 + (*p)-'0';
						p++;
					} while (*p>='0' && *p<='9');
					pre_flag |= PRT_LEFT_FILL;
					p--; // go back to last position in format string
					break;
				case 'l':
					pre_flag |= PRT_LONG_FLAG;
					break;
				case 'd':
				case 'x':
				case 'X':
				case 'c':
				case 'u':
					if (pre_flag & PRT_LONG_FLAG)
					{
						long_val = va_arg(arg, int64s);
						len = format_long_integer(buf, long_val, *p, width, pre_flag);
					}
					else
					{
						int_val = va_arg(arg, int32s);
						len = format_integer(buf, int_val, *p, width, pre_flag);
					}
					buf += len;
					res += len;
					pre_flag = 0;
					break;
					
				case 'f':
				case 'F':
					ft_val = va_arg(arg, float64);
					len = format_float(buf, ft_val, width, prec, pre_flag);
					buf += len;
					res += len;
					pre_flag = 0;
					break;
				
				case 's':
					str_val = va_arg(arg, int8u*);
					len = format_string(buf, str_val, width, pre_flag);
					buf += len;
					res += len;
					pre_flag = 0;
					break;

				case '.':
					p++;
					pre_flag |= PRT_PREC_FLAG;
					prec = 0;
					do {
						prec = prec*10 + (*p)-'0';
						p++;
					} while (*p>='0' && *p<='9');
					p--; // go back to last position in format string
					break;
					
				default:
					return 0;
			}
		}

		p++;
	}

	return res;
}


/*
* @function    format_integer
*
* @brief   Format integer to string 
* @date    Feb. 24th, 2009
*/
static const int8u hex_ch_uppercase[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
	'A', 'B', 'C', 'D', 'E', 'F'};
static const int8u hex_ch_lowercase[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
	'a', 'b', 'c', 'd', 'e', 'f'};

static inline int32u format_integer(int8u *str, int32s value, int8u type, int32u width, int32u flags)
{
	int32u res = 0, hex_val;
	int8u *p_src, *p_des;
	int8u buf[16];
	int8u num;
	
	p_src = buf;
	p_des = str;
	
	switch (type) {
		case 'd':
			/* get sign */
			if (value < 0) {
				value = -value;
				flags |= PRT_SIGNED;
				res++;
			}

			/* get number */
			do {
				num = value % 10;
				value /= 10;
				*p_src++ = num + '0';
				res++;
			} while (value > 0);
			break;

		case 'u':
			hex_val = (int32u)value;

			/* get number */
			do {
				num = hex_val % 10;
				hex_val /= 10;
				*p_src++ = num + '0';
				res++;
			} while (hex_val > 0);
			break;

		case 'x':
			hex_val = (int32u)value;
			do {
				num = hex_val % 16;
				hex_val /= 16;
				*p_src++ = hex_ch_lowercase[num];
				res++;
			} while (hex_val > 0);
			break;

		case 'X':
			hex_val = (int32u)value;
			do {
				num = hex_val % 16;
				hex_val /= 16;
				*p_src++ = hex_ch_uppercase[num];
				res++;
			} while (hex_val > 0);
			break;

		case 'c':
			num = (int8u)value;
			*p_src++ = num;
			res++;
			break;

		default:
			return 0;
	}
	
	/* process left fill & copy src->des */
	format_fill(p_src, p_des, width, res, flags);

	return (res < width) ? width : res;
}

static inline int32u format_long_integer(int8u *str, int64s value, int8u type, int32u width, int32u flags)
{
	int32u res = 0;
	int64u hex_val;
	int8u *p_src, *p_des;
	int8u buf[64];

	int8u num;
	
	p_src = buf;
	p_des = str;
	
	switch (type) {
		case 'd':
			/* get sign */
			if (value < 0) {
				value = -value;
				flags |= PRT_SIGNED;
				res++;
			}

			/* get number */
			do {
				num = value % 10;
				value /= 10;
				*p_src++ = num + '0';
				res++;
			} while (value > 0);
			break;

		case 'u':
			hex_val = (int64u)value;

			/* get number */
			do {
				num = hex_val % 10;
				hex_val /= 10;
				*p_src++ = num + '0';
				res++;
			} while (hex_val > 0);
			break;

		case 'x':
			hex_val = (int64u)value;
			do {
				num = hex_val % 16;
				hex_val /= 16;
				*p_src++ = hex_ch_lowercase[num];
				res++;
			} while (hex_val > 0);
			break;

		case 'X':
			hex_val = (int64u)value;
			do {
				num = hex_val % 16;
				hex_val /= 16;
				*p_src++ = hex_ch_uppercase[num];
				res++;
			} while (hex_val > 0);
			break;

		default:
			return 0;
	}
	
	/* process left fill & copy src->des */
	format_fill(p_src, p_des, width, res, flags);

	return (res < width) ? width : res;
}

/*
* @function    format_float
*
* @brief   Format float to string 
*          Note: only support 10e9 level float number!!!
* @date    Feb. 25th, 2009
*/
static inline int32u format_float(int8u *str, float64 value, int32u width, int32u prec, int32u flags)
{
	ieee_double_shape_type *ieee_fp;
	float64 dec_part, factor;
	int32u int_part, integer;
	int32u res = 0, i;
	int8u *p_src, *p_des;
	int8u buf[64];
	int8u num;

	if ((flags & PRT_PREC_FLAG) == 0)
	{
		prec = 6; // default decimal precesion is 6 bits
	}
	ieee_fp = (ieee_double_shape_type *)&value;
	p_src = buf;
	p_des = str;
	if ((ieee_fp->parts.lsw !=0 || ieee_fp->parts.msw !=0) && ieee_fp->number.exponent == 0)//exponent too small, FCP can not handle
	{
		*p_des++ = 'T';
		*p_des++ = 'O';
		*p_des++ = 'S';
		return 3;
	}
	if (finite(value)) { // value is finite
		/* get sign */
		if (ieee_fp->number.sign == 1) {
			ieee_fp->number.sign = 0;
			flags |= PRT_SIGNED;
			res++;
		}
		if (value > 2147483647)//2^31 - 1, too large
		{
			*p_des++ = 'T';
			*p_des++ = 'O';
			*p_des++ = 'L';
			return 3;
		}
		factor = 1.0;
		for (i=0; i<prec; i++) {
			factor *= 10.0;
		}

		value = value + 0.5/factor;
		int_part = (int32s)value;
		dec_part = value - int_part;

		/* get decimal part */
		if (prec != 0) {
			for (i = 0; i < prec; i++) {
				dec_part = dec_part * 10;
				integer = (int32u)(dec_part);
				dec_part = dec_part - integer;

				*(p_src + prec - 1 - i)= integer + '0';
				res++;
			}

			p_src += prec;
			*p_src++ = '.';
			res++;
		}

		/* get integer part */
		do {
			num = int_part % 10;
			int_part /= 10;
			*p_src++ = num + '0';
			res++;
		} while (int_part > 0);

		/* process fill & copy src->des */
		format_fill(p_src, p_des, width, res, flags);

		return (res < width) ? width : res;
	}
	else {
		if (isnan(value)) { // value is not a number
			*p_des++ = 'N';
			*p_des++ = 'A';
			*p_des++ = 'N';
		}
		else { // value is infinite
			*p_des++ = 'I';
			*p_des++ = 'N';
			*p_des++ = 'F';
		}

		return 3;
	}
}

/*
* @function    format_string
*
* @brief   Format string to string 
* @date    Feb. 24th, 2009
*/
static inline int32u format_string(int8u *str, int8u *value, int32u width, int32u flags)
{
	int32u res = 0;
	int8u *p = value;

	while (*p != 0 && res < MAX_SUBSTRING_LEN)
	{
		p++;
		res++;
	}
	flags |= PRT_STRING_OUT;
	/* process left fill & copy src->des */
	format_fill(value, str, width, res, flags);

	return (res < width) ? width : res;
}

/*
* @function    format_fill
*
* @brief   Format fill char to string 
* @date    Feb. 26th, 2009
*/
static void format_fill(int8u *p_src, int8u *p_des, int32u width, int32u len, int32u fill_flag)
{
	int32s fill_len;
	int32s i;
	int8u fill_ch;
	
	fill_len = width - len;
	
	/* process left fill */
	if ((width > len) && ((fill_flag & PRT_LEFT_FILL) == PRT_LEFT_FILL)) {
		if ((fill_flag & PRT_FILL_ZERO) == PRT_FILL_ZERO) {
			/* process sign first */
			if ((fill_flag & PRT_SIGNED) == PRT_SIGNED) {
				*p_des++ = '-';
				fill_flag &= ~PRT_SIGNED;
			}
			
			fill_ch = '0';
		}
		else {
			fill_ch = ' ';
		}

		for (i = 0; i < fill_len; i++) {
			*p_des++ = fill_ch;
		}
	}

	/* process sign */
	if ((fill_flag & PRT_SIGNED) == PRT_SIGNED) {
		*p_des++ = '-';
	}

	/* copy source to destination */
	for (i = 0; i < (int32s)len; i++) {
		if ((fill_flag & PRT_STRING_OUT) == PRT_STRING_OUT)
		{
			*p_des++ = *p_src++;
		}
		else
		{
			*p_des++ = *--p_src;
		}
	}

	/* process right fill */
	if ((width > len) && ((fill_flag & PRT_LEFT_FILL) == 0)) {
		for (i = 0; i < fill_len; i++) {
			*p_des++ = ' ';
		}
	}
}

#define SCN_PRE_PERCENT	0x1 // '%' in format string
#define SCN_LONG_FLAG	0x2 // for 64 bit integer
#define SCN_HALF_FLAG	0x4 // for 16 bit integer
#define SCN_BYTE_FLAG	0x8 // for 8 bit integer
#define SCN_AFT_PERCENT	0x10 // format string is over
#define SCN_INT_FLAG	0x20 // integer
#define SCN_HEX_FLAG	0x40 // hex
#define SCN_CHAR_FLAG	0x80 // char
#define SCN_STRING_FLAG	0x100 // string
#define SCN_FLOAT_FLAG	0x200 // float
#define SCN_SUPPRESS	0x400 // '*' in format string

/* internal function */
static inline int32u fw_vsscanf(int8u *buf, const int8u *format, va_list arg);
static int8u fw_assign_value(int8u** buf, int8u end, int32u pre_flag, int32u width, va_list* arg);
/*
* @function    fw_sscanf
*
* @brief   Firmware user-define sscanf function 
* @date    Nov. 6th, 2013
*/
int32u fw_sscanf(int8u *buffer, const int8u *format, ...)
{
	int32u r;

	va_list marker;
	va_start(marker, format);

	r = fw_vsscanf(buffer, format, marker);

	va_end(marker);

	return r;
}


/*
* @function    fw_vsscanf
*
* @brief   Firmware user-define sscanf function 
* @date    Nov. 6th, 2013
*/
static inline int32u fw_vsscanf(int8u *buf, const int8u *format, va_list arg)
{
	int32u pre_flag = 0;
	int32u nassigned = 0;/* number of fields assigned */
	int8u *p = (int8u *)format;
	int8u *p_buf = buf;
	int32u width;
	int8u res;

	while ((*p != '\0')) {
		if (pre_flag == 0) { // search '%' or '\' in format string
			if (*p == '%') {
				pre_flag = SCN_PRE_PERCENT;
				width = 0;
			}
			else {
				if (*p != *p_buf)
				{
					return nassigned;
				}
				else
				{
					p++;
					p_buf++;
					continue;
				}
			}
		}
		else { // scan format string
			switch (*p) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					do {
						width = width*10 + (*p)-'0';
						p++;
					} while (*p>='0' && *p<='9');
					p--; // go back to last position in format string
					break;
				case '*':
					pre_flag |= SCN_SUPPRESS;
					break;
				case 'l':
					pre_flag |= SCN_LONG_FLAG;
					break;
				case 'h':
					pre_flag |= SCN_HALF_FLAG;
					break;
				case 'b':
					pre_flag |= SCN_BYTE_FLAG;
					break;
				case 'd':
					pre_flag |= SCN_INT_FLAG;
					break;
				case 'x':
					pre_flag |= SCN_HEX_FLAG;
					break;
				case 'c':
					pre_flag |= SCN_CHAR_FLAG;
					break;
				case 's':
					pre_flag |= SCN_STRING_FLAG;
					break;
				default:
					pre_flag |= SCN_AFT_PERCENT;
			}
		}
		if (pre_flag & SCN_AFT_PERCENT)
		{
			res = fw_assign_value(&p_buf, *p, pre_flag, width, &arg);
			if (res)
			{
				if ((pre_flag & SCN_SUPPRESS) == 0)
				{
					nassigned++;
				}
				pre_flag = 0;
			}
			else
			{
				return nassigned;
			}
		}
		else
		{
			p++;
		}
	}
	if (pre_flag)
	{
		res = fw_assign_value(&p_buf, *p, pre_flag, width, &arg);
		if (res)
		{
			if ((pre_flag & SCN_SUPPRESS) == 0)
			{
				nassigned++;
			}
		}
	}
	return nassigned;
}

static int8u fw_assign_value(int8u** buf, int8u end, int32u pre_flag, int32u width, va_list* arg)
{
	int32u sub_str_len, i;
	int8u* p_buf = *buf;
	int8u res = 0;
	int8u *p;

	if (pre_flag & SCN_STRING_FLAG)
	{
		if ((width > 0) && ((pre_flag & SCN_SUPPRESS) == 0))
		{
			int8u* p_str = va_arg(*arg, int8u*);
			memcpy(p_str, p_buf, width);
			p_str[width] = 0;
		}
		res = 1;
		*buf += width;
	}
	else if (pre_flag & SCN_CHAR_FLAG)
	{
		if ((pre_flag & SCN_SUPPRESS) == 0)
		{
			int8u* p_char = va_arg(*arg, int8u*);
			*p_char = p_buf[0];
		}
		res = 1;
		*buf += 1;
	}
	else
	{
		sub_str_len = 0;

		while ((*(p_buf + sub_str_len) != '\0') && (*(p_buf + sub_str_len) != end))
		{
			sub_str_len++;
		}
		*buf += sub_str_len;
		if (pre_flag & SCN_SUPPRESS)
		{
			return 1;
		}
		if (pre_flag & SCN_INT_FLAG)
		{
			int8s sign = 1;
			int64s value = 0;
			if (p_buf[0] == '-')
			{
				sign = -1;
				p_buf++;
				sub_str_len--;
			}
			else if (p_buf[0] == '+')
			{
				p_buf++;
				sub_str_len--;
			}
			for (i = 0; i < sub_str_len; i++)
			{
				p = p_buf + i;
				if (*p>='0' && *p<='9')
				{
					value = value * 10 + (*p) - '0';
				}
				else
				{
					return 0;
				}
			}
			value = value * sign;

			if (pre_flag & SCN_LONG_FLAG)
			{
				int64s* p_int = va_arg(*arg, int64s*);
				*p_int = value;
			}
			else if (pre_flag & SCN_HALF_FLAG)
			{
				int16s* p_int = va_arg(*arg, int16s*);
				*p_int = (int16s)value;
			}
			else if (pre_flag & SCN_BYTE_FLAG)
			{
				int8s* p_int = va_arg(*arg, int8s*);
				*p_int = (int8s)value;
			}
			else
			{
				int32s* p_int = va_arg(*arg, int32s*);
				*p_int = (int32s)value;
			}
		}
		else if (pre_flag & SCN_HEX_FLAG)
		{
			int64u value = 0;
			for (i = 0; i < sub_str_len; i++)
			{
				p = p_buf + i;
				if (*p >= '0' && *p <= '9')
				{
					value = value * 16 + (*p) - '0';
				}
				else if (*p >= 'a' && *p <= 'f')
				{
					value = value * 16 + (*p) - 'a' + 10;
				}
				else if (*p >= 'A' && *p <= 'F')
				{
					value = value * 16 + (*p) - 'A' + 10;
				}
				else
				{
					return 0;
				}
			}

			if (pre_flag & SCN_LONG_FLAG)
			{
				int64u* p_int = va_arg(*arg, int64u*);
				*p_int = value;
			}
			else if (pre_flag & SCN_HALF_FLAG)
			{
				int16u* p_int = va_arg(*arg, int16u*);
				*p_int = (int16u)value;
			}
			else if (pre_flag & SCN_BYTE_FLAG)
			{
				int8u* p_int = va_arg(*arg, int8u*);
				*p_int = (int8u)value;
			}
			else
			{
				int32u* p_int = va_arg(*arg, int32u*);
				*p_int = (int32u)value;
			}
		}
		res = 1;
	}

	return res;
}
