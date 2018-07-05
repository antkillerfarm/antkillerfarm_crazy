#ifndef _CRC16_H_
#define _CRC16_H_

#ifdef __cplusplus
extern "C"
{
#endif

	void CRC16(int8u _data,int8u* _crc_h,int8u* _crc_l);

	int16u Block_Crc16( int8u *pByte, int32u size ) ;

#ifdef __cplusplus
}
#endif

#endif

