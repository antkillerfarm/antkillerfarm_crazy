#ifndef _LIB_TYPES_PRIVATE_
#define _LIB_TYPES_PRIVATE_

/* basic data type redefinition */
typedef unsigned char	    bool;
typedef unsigned int 	    int32u;
typedef signed int	        int32s;
typedef unsigned char	    int8u;
typedef signed char	        int8s;
typedef unsigned short	    int16u;
typedef signed short	    int16s;
typedef unsigned long long	int64u;
typedef signed long long	int64s;
typedef float		        float32;
typedef double		        float64;

typedef void* nptr;

typedef void (*pCallbackFun)(void);

typedef void* (*ThreadThrFxn)(void * , void *) ;

typedef int16u (*pHwTransmit)(int8u  , const int8u * , int16u )  ;

#define THREAD_SUCCESS      (void *) 0
#define THREAD_FAILURE      (void *) -1

typedef union
{
	int8u		_byte[2];
	int16u		_int16u;
	int16s		_int16s;
}union_2byte;

typedef union
{
	int8u	_byte[4];
	int16u	_int16u[2];
	int32u	_int32u;
	int32s		_int32s;
	float32		_float32;
	union_2byte	_union_2byte[2];
}union_4byte;

typedef union{
	int8u	_byte[8];
	int16u	_int16u[4];
	int32u	_int32u[2];
	int64u	_int64u;
	int64s	_int64s;
	float64	_float64;
	union_2byte 	_union_2byte[4];
	union_4byte	_union_4byte[2];
}union_8byte;

/* logical macro */
#ifndef TRUE
	#define TRUE (1)
#endif

#ifndef FALSE
	#define FALSE (0)
#endif

#ifndef NULL
	#define NULL (0)
#endif

#ifndef SUCCESS
    #define SUCCESS (0)
#endif

#ifndef FAILED
    #define FAILED  (-1)
#endif

#endif // end of _TYPES_H_
