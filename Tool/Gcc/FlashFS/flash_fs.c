#include "flash_fs.h"
#include "flash_drv.h"
#include <string.h>

const FLItem_Prop_Object fl_small_item_prop[FL_SMALL_ITEM_NUM] = 
{
	{FL_SIZE_OF(RecvCmd_Uart_Object)},
	{FL_SIZE_OF(RecvCmd_PPS_Object)},
	{FL_SIZE_OF(RecvCmd_TMark_Object)},
	{FL_SIZE_OF(RecvCmd_NMEA_Object)}
};

const FLItem_Prop_Object fl_big_item_prop[FL_BIG_ITEM_NUM] = 
{
	{FL_SIZE_OF(StBD2RawEphemeris)},
	{FL_SIZE_OF(StBD2RawAlmanac)}	
};

FLItem_Addr_Object fl_small_item_addr[FL_SMALL_ITEM_NUM];
FLItem_Addr_Object fl_big_item_addr[FL_BIG_ITEM_NUM];

int8u fl_small_super_block_status[FL_SMALL_POOL_SUPER_BLOCK_NUM * 2];
int8u fl_big_super_block_status[FL_BIG_POOL_SUPER_BLOCK_NUM * 2];

int32u fl_small_free_addr = 0;
int32u fl_big_free_addr = 0;

FLItem_Update_Manager flash_item_update_manager;

int8u active_super_block_small = 0;
int8u active_super_block_big = 0;

int8u fl_small_pool_over = 0;
int8u fl_big_pool_over = 0;

void FlashFSReset()
{
	memset(fl_small_item_addr, 0, FL_SMALL_ITEM_NUM * sizeof(FLItem_Addr_Object));
	memset(fl_big_item_addr, 0, FL_BIG_ITEM_NUM * sizeof(FLItem_Addr_Object));

	memset(fl_small_super_block_status, 0, FL_SMALL_POOL_SUPER_BLOCK_NUM * 2 * sizeof(int8u));
	memset(fl_big_super_block_status, 0, FL_BIG_POOL_SUPER_BLOCK_NUM * 2 * sizeof(int8u));

	fl_small_free_addr = 0;
	fl_big_free_addr = 0;

	memset(&flash_item_update_manager, 0, sizeof(FLItem_Update_Manager));

	active_super_block_small = 0;
	active_super_block_big = 0;
}

int8u flash_prepare_write_item(int8u _type, void* _data)
{
	FLItem_Prop_Object const * p_item_prop;
	int8u flag;
	if (_type & 0x80)
	{
		p_item_prop = &(fl_big_item_prop[_type & 0x7F]);
	}
	else
	{
		p_item_prop = &(fl_small_item_prop[_type]);
	}
	int32u fl_data[p_item_prop->len];
	flag = FlashReadItem(_type, fl_data);
	if (flag == FL_READ_ITEM_FLAG_OK)
	{
		if (memcmp(fl_data, _data, p_item_prop->len * sizeof(int32u)) == 0)
		{
			return FL_PRE_WRITE_ITEM_FLAG_NOT_SAVE;
		}
		else
		{
			return FL_PRE_WRITE_ITEM_FLAG_SAVE;
		}
	}
	else if (flag == FL_READ_ITEM_FLAG_NO_DATA)
	{
		return FL_PRE_WRITE_ITEM_FLAG_SAVE;
	}
	return FL_PRE_WRITE_ITEM_FLAG_NOT_SAVE;
}

void FlashWriteItem(int8u _type, void* _data)
{
	FLItem_Prop_Object const * p_item_prop;
	int8u flag = flash_prepare_write_item(_type, _data);
	FLBlock_Object* p_block;
	FLItem_Update_Handle h_item_update;
	if (flag == FL_PRE_WRITE_ITEM_FLAG_NOT_SAVE)
	{
		return;
	}

	if (_type & 0x80)
	{
		p_item_prop = &(fl_big_item_prop[_type & 0x7F]);
		h_item_update = &(flash_item_update_manager.update_items[_type - 0x80 + FL_SMALL_ITEM_NUM]);
		p_block = &(flash_item_update_manager.big_pool_block);
	}
	else
	{
		p_item_prop = &(fl_small_item_prop[_type]);
		h_item_update = &(flash_item_update_manager.update_items[_type]);
		p_block = &(flash_item_update_manager.small_pool_block);
	}
	p_block->size += p_item_prop->len + 2;
	h_item_update->data = _data;
}

int8u flash_super_block_is_overflow(int32u _addr, FLBlock_Addr_Handle h_block_addr)
{
	int32u addr0 = h_block_addr->base + (h_block_addr->id + 1) * h_block_addr->size;
	if (_addr >= addr0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int8u find_free_super_block(int8u _type, int8u _cur_id)
{
	int8u next_id, block_cnt;
	int8u* p_block_status;

	if (_type == FL_SMALL_POOL)
	{
		block_cnt = FL_SMALL_POOL_SUPER_BLOCK_NUM;
		p_block_status = fl_small_super_block_status;
	}
	else
	{
		block_cnt = FL_BIG_POOL_SUPER_BLOCK_NUM;
		p_block_status = fl_big_super_block_status;
	}
	if (_cur_id == 2 * block_cnt - 1)
	{
		next_id = 0;
	}
	else
	{
		next_id = _cur_id + 1;
	}
	if (p_block_status[next_id] != FL_SUPER_BLOCK_STATUS_INIT)
	{
		if (_type == FL_SMALL_POOL)
		{
		        fl_small_pool_over = 1;
		}
		else
		{
		        fl_big_pool_over = 1;
		}
	}
	return next_id;
}

void rewrite_all_items_to_new_super_block(FLItem_Update_Handle h_item_update, FLItem_Addr_Handle h_item_addr,
					  FLItem_Prop_Object const * p_item_prop, int8u _item_cnt, int8u _type,
					  FLBlock_Addr_Handle h_block_addr, int32u* p_free_addr)
{
	int8u i;
	int32u size = 1, data, addr;
	FLBlock_Object block;
	block.size = 1;
	for (i = 0; i < _item_cnt; i++)
	{
		if ((h_item_update[i].data == NULL) && (h_item_addr[i].last_addr0 == 0))
		{
			continue;
		}
		block.size += 2 + p_item_prop[i].len;
	}

	int32u super_block_buf[block.size];
	super_block_buf[0] = FL_SUPER_BLOCK_FLAG_USED;
	addr = h_block_addr->base + h_block_addr->id * h_block_addr->size;
	for (i = 0; i < _item_cnt; i++)
	{
		if ((h_item_update[i].data == NULL) && (h_item_addr[i].last_addr0 == 0))
		{
			continue;
		}
		data = FL_BLOCK_MAGIC_NUM | i;
		super_block_buf[size] = data;
		size++;

		if (h_item_update[i].data != NULL)
		{
			h_item_addr[i].last_addr0 = addr + size * sizeof(int32u);
			data = CalcCRCForItem(h_item_update[i].data, p_item_prop[i].len * sizeof(int32u));
			super_block_buf[size] = data;
			size++;
			memcpy(&(super_block_buf[size]), h_item_update[i].data, p_item_prop[i].len * sizeof(int32u));
			size += p_item_prop[i].len;
		}
		else
		{
			int32u fl_data[p_item_prop[i].len];
			if (_type == FL_SMALL_POOL)
			{
				FlashReadItem(i, fl_data);
			}
			else
			{
				FlashReadItem(i + 0x80, fl_data);
			}
			h_item_addr[i].last_addr0 = addr + size * sizeof(int32u);
			data = CalcCRCForItem(fl_data, p_item_prop[i].len * sizeof(int32u));
			super_block_buf[size] = data;
			size++;
			memcpy(&(super_block_buf[size]), fl_data, p_item_prop[i].len * sizeof(int32u));
			size += p_item_prop[i].len;
		}
	}
	FlashProgram(addr, super_block_buf, size);
	*p_free_addr = addr + size * sizeof(int32u);
}

void FlashFlush()
{
	int8u i;
	int16u small_pool_block_size = 0;
	int16u big_pool_block_size = 0;
	int32u check, data, addr;
	FLItem_Update_Handle h_item_update;
	FLItem_Prop_Object const * p_item_prop;
	int32u small_pool_buf[flash_item_update_manager.small_pool_block.size];
	int32u big_pool_buf[flash_item_update_manager.big_pool_block.size];
	FLBlock_Addr_Object block_addr;

	if (flash_item_update_manager.small_pool_block.size > 0)
	{
		block_addr.base = FL_ADDR_SMALL_POOL0;
		block_addr.id = active_super_block_small;
		block_addr.size = FL_SMALL_POOL_SUPER_BLOCK_SIZE;
		if (flash_super_block_is_overflow(fl_small_free_addr + (1 + flash_item_update_manager.small_pool_block.size) 
						  * sizeof(int32u), &block_addr))
		{
			block_addr.id = find_free_super_block(FL_SMALL_POOL, active_super_block_small);
			if(fl_small_pool_over == 0)
			{
				rewrite_all_items_to_new_super_block(&(flash_item_update_manager.update_items[0]), fl_small_item_addr,
								     fl_small_item_prop, FL_SMALL_ITEM_NUM, FL_SMALL_POOL,
								     &block_addr, &fl_small_free_addr);
				addr = FL_ADDR_SMALL_POOL0 + active_super_block_small * FL_SMALL_POOL_SUPER_BLOCK_SIZE;
				data = FL_SUPER_BLOCK_FLAG_OLD;
				FlashProgram(addr, &data, 1);
				fl_small_super_block_status[active_super_block_small] = FL_SUPER_BLOCK_STATUS_OLD;
				active_super_block_small = block_addr.id;
				fl_small_super_block_status[active_super_block_small] = FL_SUPER_BLOCK_STATUS_USED;
			}
		}
		else
		{
			if (fl_small_super_block_status[active_super_block_small] == FL_SUPER_BLOCK_STATUS_INIT)
			{
				data = FL_SUPER_BLOCK_FLAG_USED;
				FlashProgram(fl_small_free_addr, &data, 1);
				fl_small_free_addr += sizeof(int32u);
				fl_small_super_block_status[active_super_block_small] = FL_SUPER_BLOCK_STATUS_USED;
			}
			for (i = 0; i < FL_SMALL_ITEM_NUM; i++)
			{
				h_item_update = &(flash_item_update_manager.update_items[i]);
				if (h_item_update->data == NULL)
				{
					continue;
				}
				p_item_prop = &(fl_small_item_prop[i]);
				data = FL_BLOCK_MAGIC_NUM | i;
				small_pool_buf[small_pool_block_size] = data;
				small_pool_block_size++;
				fl_small_item_addr[i].last_addr0 = fl_small_free_addr + small_pool_block_size * sizeof(int32u);
				check = CalcCRCForItem(h_item_update->data, p_item_prop->len * sizeof(int32u));
				small_pool_buf[small_pool_block_size] = check;
				small_pool_block_size++;			
				memcpy(&(small_pool_buf[small_pool_block_size]), h_item_update->data, p_item_prop->len * sizeof(int32u));
				small_pool_block_size += p_item_prop->len;
			}
			FlashProgram(fl_small_free_addr, small_pool_buf, small_pool_block_size);
			fl_small_free_addr += small_pool_block_size * sizeof(int32u);
		}
	}

	if (flash_item_update_manager.big_pool_block.size > 0)
	{
		block_addr.base = FL_ADDR_BIG_POOL0;
		block_addr.id = active_super_block_big;
		block_addr.size = FL_BIG_POOL_SUPER_BLOCK_SIZE;
		if (flash_super_block_is_overflow(fl_big_free_addr + (1 + flash_item_update_manager.big_pool_block.size) 
						  * sizeof(int32u), &block_addr))
		{
			block_addr.id = find_free_super_block(FL_BIG_POOL, active_super_block_big);
			if(fl_big_pool_over == 0)
			{
				rewrite_all_items_to_new_super_block(&(flash_item_update_manager.update_items[FL_SMALL_ITEM_NUM]), fl_big_item_addr,
								     fl_big_item_prop, FL_BIG_ITEM_NUM, FL_BIG_POOL,
								     &block_addr, &fl_big_free_addr);
				addr = FL_ADDR_BIG_POOL0 + active_super_block_big * FL_BIG_POOL_SUPER_BLOCK_SIZE;
				data = FL_SUPER_BLOCK_FLAG_OLD;
				FlashProgram(addr, &data, 1);
				fl_big_super_block_status[active_super_block_big] = FL_SUPER_BLOCK_STATUS_OLD;
				active_super_block_big = block_addr.id;
				fl_big_super_block_status[active_super_block_big] = FL_SUPER_BLOCK_STATUS_USED;
			}
		}
		else
		{
			if (fl_big_super_block_status[active_super_block_big] == FL_SUPER_BLOCK_STATUS_INIT)
			{
				data = FL_SUPER_BLOCK_FLAG_USED;
				FlashProgram(fl_big_free_addr, &data, 1);
				fl_big_free_addr += sizeof(int32u);
				fl_big_super_block_status[active_super_block_big] = FL_SUPER_BLOCK_STATUS_USED;
			}
			for (i = FL_SMALL_ITEM_NUM; i < FL_TOTAL_ITEM_NUM; i++)
			{
				h_item_update = &(flash_item_update_manager.update_items[i]);
				if (h_item_update->data == NULL)
				{
					continue;
				}
				p_item_prop = &(fl_big_item_prop[i - FL_SMALL_ITEM_NUM]);
				data = FL_BLOCK_MAGIC_NUM | (i - FL_SMALL_ITEM_NUM);
				big_pool_buf[big_pool_block_size] = data;
				big_pool_block_size++;
				fl_big_item_addr[i - FL_SMALL_ITEM_NUM].last_addr0 = fl_big_free_addr + big_pool_block_size * sizeof(int32u);
				check = CalcCRCForItem(h_item_update->data, p_item_prop->len * sizeof(int32u));
				big_pool_buf[big_pool_block_size] = check;
				big_pool_block_size++;
				memcpy(&(big_pool_buf[big_pool_block_size]), h_item_update->data, p_item_prop->len * sizeof(int32u));
				big_pool_block_size += p_item_prop->len;
			}
			FlashProgram(fl_big_free_addr, big_pool_buf, big_pool_block_size);
			fl_big_free_addr += big_pool_block_size * sizeof(int32u);
		}
	}

	memset(&flash_item_update_manager, 0, sizeof(FLItem_Update_Manager));
}

int8u flash_load_super_block_items(int8u _type, int8u _block_id)
{
	int32u data, addr;
	FLItem_Addr_Handle h_item_addr;
	FLItem_Prop_Object const * p_item_prop;
	int32u* p_free_addr;

	if (_type == FL_SMALL_POOL)
	{
		addr = FL_ADDR_SMALL_POOL0 + _block_id * FL_SMALL_POOL_SUPER_BLOCK_SIZE;
		h_item_addr = fl_small_item_addr;
		p_item_prop = fl_small_item_prop;
		p_free_addr = &fl_small_free_addr;
	}
	else
	{
		addr = FL_ADDR_BIG_POOL0 + _block_id * FL_BIG_POOL_SUPER_BLOCK_SIZE;
		h_item_addr = fl_big_item_addr;
		p_item_prop = fl_big_item_prop;
		p_free_addr = &fl_big_free_addr;
	}

	addr += sizeof(int32u);
	data = FlashReadWord(addr);
	while (data != FL_BLOCK_FLAG_INIT)
	{
		if ((data & 0xFF000000) != FL_BLOCK_MAGIC_NUM)
		{
			return FL_LOAD_FLAG_DATA_RUINED;
		}
		data = data & 0xFFFFFF;
		addr += sizeof(int32u);
		h_item_addr[data].last_addr0 = addr;
		addr += (p_item_prop[data].len + 1) * sizeof(int32u);
		data = FlashReadWord(addr);
	}
	*p_free_addr = addr;
	return FL_LOAD_FLAG_OK;
}

int8u flash_load_super_block_status(int32u _addr, int8u _block_cnt, int32u _block_size, int8u* p_block_status)
{
	int8u i, res;
	int32u data;
	res = 0;
	for (i = 0; i < 2 * _block_cnt; i++)
	{
		data = FlashReadWord(_addr + i * _block_size);
		if (data == FL_SUPER_BLOCK_FLAG_INIT)
		{
			p_block_status[i] = FL_SUPER_BLOCK_STATUS_INIT;
		}
		else if (data == FL_SUPER_BLOCK_FLAG_USED)
		{
			p_block_status[i] = FL_SUPER_BLOCK_STATUS_USED;
			res = i;
		}
		else if (data == FL_SUPER_BLOCK_FLAG_OLD)
		{
			p_block_status[i] = FL_SUPER_BLOCK_STATUS_OLD;
		}
		else
		{
			p_block_status[i] = FL_SUPER_BLOCK_STATUS_UNKNOWN;
		}
	}
	return res;
}

void flash_erase_super_block(int8u* p_block_status, FLBlock_Addr_Handle p_block_addr)
{
	int8u i, erase_flag = FL_ERASE_FLAG_NONE;
	int8u flag = TRUE;
	int32u erase_size, sector_num, addr;
	for (i = 0; i < p_block_addr->id; i++)
	{
		if (p_block_status[i] != FL_SUPER_BLOCK_STATUS_OLD)
		{
			flag = FALSE;
			break;
		}
	}
	if (flag)
	{
		erase_flag = FL_ERASE_FLAG_TOP;
	}
	else
	{
		flag = TRUE;
		for (i = p_block_addr->id; i < 2 * p_block_addr->id; i++)
		{
			if (p_block_status[i] != FL_SUPER_BLOCK_STATUS_OLD)
			{
				flag = FALSE;
				break;
			}
		}
		if (flag)
		{
			erase_flag = FL_ERASE_FLAG_BOTTOM;
		}
	}

	if (erase_flag == FL_ERASE_FLAG_NONE)
	{
		return;
	}
	else if (erase_flag == FL_ERASE_FLAG_TOP)
	{
		addr = p_block_addr->base;
	}
	else
	{
		addr = p_block_addr->base + p_block_addr->id * p_block_addr->size;
	}

	erase_size = p_block_addr->id * p_block_addr->size;
	sector_num = erase_size / SECTOR_SIZE;
	
	for (i = 0; i < sector_num; i++)
	{
		FlashSectorErase(addr);
		addr += SECTOR_SIZE;
	}
}

void flash_pool_erase(int32u _addr, int32u _len)
{
	int16u i, sector_num;
	sector_num = _len / SECTOR_SIZE;
	
	for (i = 0; i < sector_num; i++)
	{
		FlashSectorErase(_addr);
		_addr += SECTOR_SIZE;
	}
}

void FlashLoadData()
{
	int8u res;
	FLBlock_Addr_Object block_addr;
	active_super_block_small = flash_load_super_block_status(FL_ADDR_SMALL_POOL0, FL_SMALL_POOL_SUPER_BLOCK_NUM,
								 FL_SMALL_POOL_SUPER_BLOCK_SIZE, fl_small_super_block_status);
	active_super_block_big = flash_load_super_block_status(FL_ADDR_BIG_POOL0, FL_BIG_POOL_SUPER_BLOCK_NUM,
								   FL_BIG_POOL_SUPER_BLOCK_SIZE, fl_big_super_block_status);
	fl_small_free_addr = FL_ADDR_SMALL_POOL0 + active_super_block_small * FL_SMALL_POOL_SUPER_BLOCK_SIZE;
	fl_big_free_addr = FL_ADDR_BIG_POOL0 + active_super_block_big * FL_BIG_POOL_SUPER_BLOCK_SIZE;

	block_addr.base = FL_ADDR_SMALL_POOL0;
	block_addr.id = FL_SMALL_POOL_SUPER_BLOCK_NUM;
	block_addr.size = FL_SMALL_POOL_SUPER_BLOCK_SIZE;
	flash_erase_super_block(fl_small_super_block_status, &block_addr);

	block_addr.base = FL_ADDR_BIG_POOL0;
	block_addr.id = FL_BIG_POOL_SUPER_BLOCK_NUM;
	block_addr.size = FL_BIG_POOL_SUPER_BLOCK_SIZE;
	flash_erase_super_block(fl_big_super_block_status, &block_addr);

	if (fl_small_super_block_status[active_super_block_small] == FL_SUPER_BLOCK_STATUS_USED)
	{
		res = flash_load_super_block_items(FL_SMALL_POOL, active_super_block_small);
		if (res != FL_LOAD_FLAG_OK)
		{
			flash_pool_erase(FL_ADDR_SMALL_POOL0, FL_SMALL_POOL_SIZE);
			active_super_block_small = flash_load_super_block_status(FL_ADDR_SMALL_POOL0, FL_SMALL_POOL_SUPER_BLOCK_NUM,
							 FL_SMALL_POOL_SUPER_BLOCK_SIZE, fl_small_super_block_status);
			fl_small_free_addr = FL_ADDR_SMALL_POOL0 + active_super_block_small * FL_SMALL_POOL_SUPER_BLOCK_SIZE;
			memset(fl_small_item_addr, 0, FL_SMALL_ITEM_NUM * sizeof(FLItem_Addr_Object));
		}
#if (FL_EARSE_MODE == FL_EARSE_MODE_LONG)
		if (active_super_block_small >= FL_SMALL_POOL_SUPER_BLOCK_NUM)
		{
			block_addr.base = FL_ADDR_SMALL_POOL0;
			block_addr.id = 0;
			block_addr.size = FL_SMALL_POOL_SUPER_BLOCK_SIZE;
			rewrite_all_items_to_new_super_block(&(flash_item_update_manager.update_items[0]), fl_small_item_addr,
							     fl_small_item_prop, FL_SMALL_ITEM_NUM, FL_SMALL_POOL,
							     &block_addr, &fl_small_free_addr);
			flash_pool_erase(FL_ADDR_SMALL_POOL0 + FL_SMALL_POOL_SIZE / 2, FL_SMALL_POOL_SIZE / 2);
		}
#endif
	}

	if (fl_big_super_block_status[active_super_block_big] == FL_SUPER_BLOCK_STATUS_USED)
	{
		res = flash_load_super_block_items(FL_BIG_POOL, active_super_block_big);
		if (res != FL_LOAD_FLAG_OK)
		{
			flash_pool_erase(FL_ADDR_BIG_POOL0, FL_BIG_POOL_SIZE);
			active_super_block_big = flash_load_super_block_status(FL_ADDR_BIG_POOL0, FL_BIG_POOL_SUPER_BLOCK_NUM,
								   FL_BIG_POOL_SUPER_BLOCK_SIZE, fl_big_super_block_status);
			fl_big_free_addr = FL_ADDR_BIG_POOL0 + active_super_block_big * FL_BIG_POOL_SUPER_BLOCK_SIZE;
			memset(fl_big_item_addr, 0, FL_BIG_ITEM_NUM * sizeof(FLItem_Addr_Object));
		}
#if (FL_EARSE_MODE == FL_EARSE_MODE_LONG)
		if (active_super_block_big >= FL_BIG_POOL_SUPER_BLOCK_NUM)
		{
			block_addr.base = FL_ADDR_BIG_POOL0;
			block_addr.id = 0;
			block_addr.size = FL_BIG_POOL_SUPER_BLOCK_SIZE;
			rewrite_all_items_to_new_super_block(&(flash_item_update_manager.update_items[0]), fl_big_item_addr,
							     fl_big_item_prop, FL_BIG_ITEM_NUM, FL_BIG_POOL,
							     &block_addr, &fl_big_free_addr);
			flash_pool_erase(FL_ADDR_BIG_POOL0 + FL_BIG_POOL_SIZE / 2, FL_BIG_POOL_SIZE / 2);
		}
#endif
	}
}

int8u FlashReadItem(int8u _type, void* _data)
{
	FLItem_Prop_Object const * p_item_prop;
	FLItem_Addr_Handle h_item_addr;
	int32u data, check;
	int32u addr;
	int16u len, i;

	if (_type & 0x80)
	{
		p_item_prop = &(fl_big_item_prop[_type & 0x7F]);
		h_item_addr = &(fl_big_item_addr[_type & 0x7F]);
	}
	else
	{
		p_item_prop = &(fl_small_item_prop[_type]);
		h_item_addr = &(fl_small_item_addr[_type]);
	}

	len = p_item_prop->len;
	if (h_item_addr->last_addr0 == 0)
	{
		return FL_READ_ITEM_FLAG_NO_DATA;
	}
	addr = h_item_addr->last_addr0;
	check = FlashReadWord(addr);
	addr += sizeof(FLCheck_Object);
	int32u fl_data_buf[len];
	for (i = 0; i < len; i++)
	{
		fl_data_buf[i] = FlashReadWord(addr);
		addr += sizeof(int32u);
	}
	data = CalcCRCForItem(fl_data_buf, len * sizeof(int32u));
	if (data != check)
	{
		return FL_READ_ITEM_FLAG_RUINED;
	}
	memcpy(_data, fl_data_buf, len * sizeof(int32u));
	return FL_READ_ITEM_FLAG_OK;
}

