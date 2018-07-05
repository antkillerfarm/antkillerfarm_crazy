// strlib.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "types.h"
#include "strlib.h"
#include <string.h>

int8u buf[1024];
int8u buf1[1024];

int _tmain(int argc, _TCHAR* argv[])
{
	//int
	//int32s t1 = -10000000000;
	//fw_sprintf(buf,(int8u *)("%d"),t1);

	//float
	//float64 t2 = 2e-30;
	//fw_sprintf(buf,(int8u *)("%20s"),"war3");

	int32u i = 123;
	int8u i1 = 88;
	int32s i2 = -111;
	int16u i3 = 66;
	float64 i4 = 12.345;
	float64 i5 = 345.123;
	int64s i6 = -1234567890123;
	int32s i7 = 0;

	fw_sprintf(buf, (int8u *)("%10u/%x %d-%2d/%02x/%d %2d/%2d/%2d/%2d %d/%2d/%3d %d/%2d/%d/%3d %d %.1f/%.1f %.1f/%.8f/%.5f %.1f/%.1f/%.1f %.1f/%.1f/%.1f %d %d/%f\r\n"),
				i2,
				i1,//bb_info.trk_info[i].mode,
				i1,//bb_info.trk_info[i].p_sv->gnss_type,
				i,//bb_info.trk_info[i].p_sv->prn+1,
				i1,//((GPSEphemeris*)(bb_info.trk_info[i].p_sv->p_sv_eph))->health,
				i1,//bb_info.trk_info[i].fix_used,
				i1,//bb_info.trk_info[i].rt_cn0,
				i1,//bb_info.trk_info[i].fir_cn0[1],
				i1,//bb_info.trk_info[i].fir_cn0[3],
				i1,//bb_info.trk_info[i].iir_cn0[1],
				i,//bb_info.trk_info[i].p_sv->eph_valid | (bb_info.trk_info[i].p_sv->eph_usable << 1),
				i2,//(int32s)bb_info.trk_info[i].p_sv->el_eph,
				i2,//(int32u)bb_info.trk_info[i].p_sv->az_eph,
				i1,//ch_nav_msg[i].msg_chk_right,
				i1,//ch_nav_msg[i].ts_valid_flag,
				i1,//ch_nav_msg[i].sf_id+1,
				i3,//ch_nav_msg[i].bit_cnt,
				i2,//(int32s)bb_info.trk_info[i].doppler,
				i4,//bb_info.trk_info[i].p_sv->vD1,
				i4,//bb_info.trk_info[i].p_sv->vD2,
				i4,//bb_info.trk_info[i].pr,
				i4,//bb_info.trk_info[i].dpr,
				i4,//bb_info.trk_info[i].ddpr,
				i4,//bb_info.trk_info[i].p_sv->pos.x,
				i4,//bb_info.trk_info[i].p_sv->pos.y,
				i4,//bb_info.trk_info[i].p_sv->pos.z,
				i4,//bb_info.trk_info[i].p_sv->vel.x,
				i4,//bb_info.trk_info[i].p_sv->vel.y,
				i4,//bb_info.trk_info[i].p_sv->vel.z,
				i,//((pGPSEphemeris)bb_info.trk_info[i].p_sv->p_sv_eph)->toe,
				i,//(int32u)bb_info.trk_info[i].IIRDprInitFlag,
				i5//bb_info.trk_info[i].chip_nco
				);

	strcpy((char *)buf, "Ff,-111#ggggg^123@-123456789 123456");
	fw_sscanf(buf,(int8u *)"%x,%d#%5s^%*d@%ld %6s",
				&i2,
				&i7,
				buf1,
				&i6,
				buf1
				);
	return 0;
}

