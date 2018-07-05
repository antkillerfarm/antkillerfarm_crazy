#ifndef _FLASH_DRV_H_
#define _FLASH_DRV_H_

#include "types.h"

#define SECTOR_SIZE (4 * 1024)

#ifdef __cplusplus
extern "C"
{
#endif

	int32u FlashReadWord(int32u _addr_offset);
	bool FlashProgram(int32u _addr_offset, int32u* _data, int16u _len);
	bool FlashSectorErase(int32u _addr_offset);
	void InitFlash();
	void SaveFlashToFile(int8u* _name);
	void LoadFlashFromFile(int8u* _name);
	int32u CalcCRCForItem(int32u* p_data, int32u _size);
	
#ifdef __cplusplus
}
#endif

#endif
