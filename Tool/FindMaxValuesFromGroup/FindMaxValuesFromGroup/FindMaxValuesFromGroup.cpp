// FindMaxValuesFromGroup.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>

#define MAX_VALUES_COUNT 16
#define GROUP_COUNT 256
#define CN0_LEVEL	60

typedef unsigned long 	int32u;
typedef signed long	int32s;
typedef unsigned char	int8u;
typedef signed char	int8s;
typedef unsigned short	int16u;
typedef signed short	int16s;
typedef unsigned long long	int64u;
typedef signed long long	int64s;
typedef float		float32;
typedef double		float64;

typedef struct
{
	int32u index;
	int32u value;
}IndexValuePair;

typedef struct
{
	int8u	cn0;
	float64	np;
}RefCN0;

IndexValuePair data[MAX_VALUES_COUNT];
int32u group[GROUP_COUNT];

const RefCN0 std_cn0_tab_50bps[CN0_LEVEL] =
{
	{1 , 1.0239 },{2 , 1.0301 },{3 , 1.0378 },{4 , 1.0476 },{5 , 1.0599 },
	{6 , 1.0753 },{7 , 1.0948 },{8 , 1.1191 },{9 , 1.1497 },{10, 1.1881 },
	{11, 1.2362 },{12, 1.2964 },{13, 1.3717 },{14, 1.4656 },{15, 1.5824 },
	{16, 1.7274 },{17, 1.9068 },{18, 2.1277 },{19, 2.3982 },{20, 2.7273 },
	{21, 3.1245 },{22, 3.5993 },{23, 4.1604 },{24, 4.8144 },{25, 5.5648 },
	{26, 6.4102 },{27, 7.3434 },{28, 8.3504 },{29, 9.4111 },{30, 10.5	},
	{31, 11.5889},{32, 12.6496},{33, 13.6566},{34, 14.5898},{35, 15.4352},
	{36, 16.1856},{37, 16.8396},{38, 17.4007},{39, 17.8755},{40, 18.2727},
	{41, 18.6018},{42, 18.8723},{43, 19.0932},{44, 19.2726},{45, 19.4176},
	{46, 19.5344},{47, 19.6283},{48, 19.7036},{49, 19.7638},{50, 19.8119},
	{51, 19.8503},{52, 19.8809},{53, 19.9052},{54, 19.9247},{55, 19.9401},
	{56, 19.9524},{57, 19.9622},{58, 19.9699},{59, 19.9761},{60, 19.981 }
};

int32u FindMaxValuesFromGroup(const int32u* p_group, int32u _group_cnt, IndexValuePair* p_result, int32u _max_values_count)
{
	int32s i,j,k = 0;
	int32u stack_top = 0, flag = 0;
	for (i = 0; i < _group_cnt; i++)
	{
		if (stack_top == 0)
		{
			p_result[0].index = i;
			p_result[0].value = p_group[i];
			stack_top++;
		}
		else if (stack_top < _max_values_count)
		{
			for (j = stack_top - 1; j >= 0; j--)
			{
				if (p_group[i] > p_result[j].value)
				{
					p_result[j+1].index = p_result[j].index;
					p_result[j+1].value = p_result[j].value;
					if (j == 0)
					{
						p_result[j].index = i;
						p_result[j].value = p_group[i];
					}
					k++;
				}
				else
				{
					p_result[j+1].index = i;
					p_result[j+1].value = p_group[i];
					k++;
					break;
				}
			}
			stack_top++;
		}
		else
		{
			flag = 0;
			for (j = stack_top - 1; j >= 0; j--)
			{
				if (flag == 0)
				{
					if (p_group[i] > p_result[j].value)
					{
						flag = 1;
						k++;
					}
					else
					{
						break;
						k++;
					}
				}
				else
				{
					if (p_group[i] > p_result[j].value)
					{
						p_result[j+1].index = p_result[j].index;
						p_result[j+1].value = p_result[j].value;
						if (j == 0)
						{
							p_result[j].index = i;
							p_result[j].value = p_group[i];
						}
						k++;
					}
					else
					{
						p_result[j+1].index = i;
						p_result[j+1].value = p_group[i];
						k++;
						break;
					}
				}
			}
		}
	}
	return k;
}

void test_FindMaxValuesFromGroup()
{
	int i, j, cnt, t;
	for (i = 0; i < GROUP_COUNT; i++)
	{
		group[i] = rand();
	}

	cnt = FindMaxValuesFromGroup(group, GROUP_COUNT, data, MAX_VALUES_COUNT);

	for (i = 0; i < MAX_VALUES_COUNT; i++)
	{
		for (j = i; j < GROUP_COUNT; j++)
		{
			if (group[i] < group[j])
			{
				t = group[i];
				group[i] = group[j];
				group[j] = t;
			}
		}
	}

	for (i = 0; i < MAX_VALUES_COUNT; i++)
	{
		if (data[i].value != group[i])
		{
			break;
		}
	}
}

int8u find_cn0_from_table(const RefCN0* _table, float64 _value)
{
	if (_value < _table[0].np)
	{
		return 0;
	}
	else if (_value > _table[CN0_LEVEL - 1].np)
	{
		return CN0_LEVEL;
	}
	else
	{
		int8u t_start = 0, t_end = CN0_LEVEL - 1, t;
		while (t_start != t_end - 1)
		{
			t = (t_start + t_end) / 2;
			if (_value >= _table[t].np)
			{
				t_start = t;
			}
			else
			{
				t_end = t;
			}
		}
		return _table[t_start].cn0;
	}
}

void test_find_cn0_from_table()
{
	float64 value = 19;
	int32u cn0;
	cn0 = find_cn0_from_table(std_cn0_tab_50bps, value);
	cn0++;
}

int32u ReversBits(int32u x)
{
	x = ((x & 0x55555555) <<  1) | ((x >>  1) & 0x55555555); 
	x = ((x & 0x33333333) <<  2) | ((x >>  2) & 0x33333333); 
	x = ((x & 0x0F0F0F0F) <<  4) | ((x >>  4) & 0x0F0F0F0F); 
	x = (x << 24) | ((x & 0xFF00) << 8) | 
		((x >> 8) & 0xFF00) | (x >> 24); 
	return x;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int32u x = 0x46c0a61b;
	x = ReversBits(x);
	test_FindMaxValuesFromGroup();
	test_find_cn0_from_table();

	return 0;
}

