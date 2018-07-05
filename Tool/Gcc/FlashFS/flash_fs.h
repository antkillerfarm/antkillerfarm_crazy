#ifndef _FLASH_FS_H_
#define _FLASH_FS_H_

#include "types.h"

#define FL_EARSE_MODE_LONG 0
#define FL_EARSE_MODE_SHORT 1
#define FL_EARSE_MODE FL_EARSE_MODE_LONG

#define FL_POOL_TOTAL_SIZE (64 * 1024)
#define FL_SMALL_POOL_SIZE (16 * 1024)
#define FL_BIG_POOL_SIZE   (48 * 1024)

#define FL_ADDR_SMALL_POOL0 (0)
#define FL_ADDR_SMALL_POOL1 (8 * 1024)
#define FL_ADDR_BIG_POOL0 (16 * 1024)
#define FL_ADDR_BIG_POOL1 (40 * 1024)

#define FL_SMALL_POOL_SUPER_BLOCK_NUM 8
#define FL_SMALL_POOL_SUPER_BLOCK_SIZE (1 * 1024)
#define FL_BIG_POOL_SUPER_BLOCK_NUM 1
#define FL_BIG_POOL_SUPER_BLOCK_SIZE (24 * 1024)

#define FL_SMALL_POOL 0
#define FL_BIG_POOL 1

#define FL_TOTAL_ITEM_NUM (FL_SMALL_ITEM_NUM + FL_BIG_ITEM_NUM)
#define FL_SMALL_ITEM_NUM 4
#define FL_BIG_ITEM_NUM 2
#define FL_BLOCK_MAGIC_NUM 0x5A000000

#define FL_BLOCK_FLAG_INIT 0xFFFFFFFF

#define FL_SUPER_BLOCK_FLAG_INIT 0xFFFFFFFF
#define FL_SUPER_BLOCK_FLAG_USED 0xFFFFFF5A
#define FL_SUPER_BLOCK_FLAG_OLD 0xFFFF5A5A

#define FL_SUPER_BLOCK_STATUS_INIT 0
#define FL_SUPER_BLOCK_STATUS_USED 1
#define FL_SUPER_BLOCK_STATUS_OLD 2
#define FL_SUPER_BLOCK_STATUS_UNKNOWN 3

#define FL_LOAD_FLAG_OK 1
#define FL_LOAD_FLAG_DATA_RUINED 2

#define FL_READ_ITEM_FLAG_OK 1
#define FL_READ_ITEM_FLAG_NO_DATA 2
#define FL_READ_ITEM_FLAG_RUINED 3

#define FL_PRE_WRITE_ITEM_FLAG_SAVE 1
#define FL_PRE_WRITE_ITEM_FLAG_NOT_SAVE 2

#define FL_ERASE_FLAG_NONE 0
#define FL_ERASE_FLAG_TOP 1
#define FL_ERASE_FLAG_BOTTOM 2

#define FL_SIZE_OF(a) (sizeof(a) / sizeof(int32u))

typedef enum{
	FLDT_Uart = 0,
	FLDT_PPS,
	FLDT_TMark,
	FLDT_NMEA,
	FLDT_Eph = 0x80,
	FLDT_Alm
}FLData_TYPE;

typedef struct{
	int8u len;
}FLItem_Prop_Object;

typedef struct{
	int32u last_addr0;
	int32u last_addr1;
}FLItem_Addr_Object, *FLItem_Addr_Handle;

typedef struct{
#define FL_PAR_CONST 0xA5A5
	int16u parity;
	int16u crc;
}FLCheck_Object;

typedef struct{
	FLCheck_Object check;
	void* data;
}FLItem_Update_Object, *FLItem_Update_Handle;

typedef struct{
	int16u size;
}FLBlock_Object;

typedef struct{
	int32u base;
	int32u id;
	int32u size;
}FLBlock_Addr_Object, *FLBlock_Addr_Handle;

typedef struct{
	FLItem_Update_Object update_items[FL_TOTAL_ITEM_NUM];
	FLBlock_Object small_pool_block;
	FLBlock_Object big_pool_block;
}FLItem_Update_Manager;

typedef struct{
	int32u aUartBRate[2];
	int32u aUartOwner[2];
}RecvCmd_Uart_Object;

typedef struct{
	int32u bPPS;
	int32u vPPSWidth;
	int32s vCableDelay;
}RecvCmd_PPS_Object;

typedef struct{
	int32u bTMark;
	int32u vTMarkCYC;
	int32u vTMarkWidth;
}RecvCmd_TMark_Object;

typedef struct{
	int32u aNmeaCYC[13];
}RecvCmd_NMEA_Object;

typedef struct {
	int32u toc, toe, e, sqrt_a;
	int32s a0, a1, cuc, m0, cus, cic, cis, i0, crc, crs, omega_dot, omega0, w;
	int16u wn;
	int16s tgd1, tgd2, a2, delta_n, i_dot;
	int8u health, aodc, urai, aode;
} StBD2RawEphemeris, *hBD2RawEphemeris;

typedef struct {
	int32u sqrt_a, e;  
	int32s omega0, omega_dot, w, m0;
	int16u wn;
	int16s a1, a0, delta_i;
	int8u toa;
} StBD2RawAlmanac, *hBD2RawAlmanac;

#ifdef __cplusplus
extern "C"
{
#endif
	void FlashWriteItem(int8u _type, void* _data);
	void FlashFlush();
	void FlashLoadData();
	int8u FlashReadItem(int8u _type, void* _data);
	void FlashFSReset();
#ifdef __cplusplus
}
#endif

#endif
