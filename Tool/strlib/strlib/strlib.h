/*
 * @file  strlib.h
 *
 * @brief	contain string library routines
 * @date	Dec.29th, 2008
 *
 * 
 * Copyright (C) TechToTop, Inc.
 *
 * MODIFICATIONS:
 *		Dec. 29th, 2008
 *			-content: create this file
 *
 */

#ifndef _STRLIB_H_
#define _STRLIB_H_


#ifdef __cplusplus
extern "C"
{
#endif

	int32u fw_sprintf(int8u *buffer, const int8u *format, ...);
	int32u fw_sscanf(int8u *buffer, const int8u *format, ...);

#ifdef __cplusplus
}
#endif


#endif // end of _STRLIB_H

