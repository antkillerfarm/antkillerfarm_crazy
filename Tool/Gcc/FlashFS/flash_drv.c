#include "flash_fs.h"
#include "flash_drv.h"
#include "crc16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FL_ADDR_OFFSET(a) (a / sizeof(int32u))
int32u flash_buf[FL_ADDR_OFFSET(FL_POOL_TOTAL_SIZE)];

int32u FlashReadWord(int32u _addr_offset)
{
	return *(flash_buf + FL_ADDR_OFFSET(_addr_offset));
}

bool FlashProgram(int32u _addr_offset, int32u* _data, int16u _len)
{
	int16u i;
	int32u data0, data1;
	int32u* p_addr = flash_buf + FL_ADDR_OFFSET(_addr_offset);
	for (i = 0; i < _len; i++)
	{
		data0 = *p_addr;
		data1 = *(_data + i);
		data0 = data0 & data1;
		*p_addr = data0;
		p_addr++;
	}
}

bool FlashSectorErase(int32u _addr_offset)
{
	memset(flash_buf + FL_ADDR_OFFSET(_addr_offset), 0xFF, SECTOR_SIZE);
	return TRUE;
}

void InitFlash()
{
	memset(flash_buf, 0xFF, FL_POOL_TOTAL_SIZE);
}

void SaveFlashToFile(int8u* _name)
{
	FILE* f = fopen(_name, "wb");
	fwrite(flash_buf, 1, FL_POOL_TOTAL_SIZE, f);
	fclose(f);
}

void LoadFlashFromFile(int8u* _name)
{
	FILE* f = fopen(_name, "rb");
	fread(flash_buf, 1, FL_POOL_TOTAL_SIZE, f);
	fclose(f);
}

int16u _LibBK_Crc16( int8u *pByte, int32u size )
{
	union_2byte residue;
	int32u index;

	residue._int16u = 0;
	for(index = 0; index < size; index++,pByte++){
		CRC16( *pByte, &(residue._byte[0]), &(residue._byte[1]));
	}

	return( residue._int16u );
}

int32u CalcCRCForItem(int32u* p_data, int32u _size)
{
	FLCheck_Object flcheck;
	int32u* p = (int32u*)(&flcheck);
	flcheck.crc = _LibBK_Crc16((int8u*)(p_data), _size);
	flcheck.parity = FL_PAR_CONST;
	return *p;
}
