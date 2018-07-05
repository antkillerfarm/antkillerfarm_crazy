/*
 * @file  types.h
 *
 * @brief	contain type definition
 * @date	Dec. 10th, 2008
 *
 * 
 * Copyright (C) TechToTop, Inc.
 *
 * MODIFICATIONS:
 *		Dec. 10th, 2008
 *			-content: create this file
 *
 */

#ifndef _TYPES_H_
#define _TYPES_H_

/* basic data type redefinition */
//typedef unsigned char		bool;		// boolean
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
#endif // end of _TYPES_H_

