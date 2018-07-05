#include "types.h"
#include "nmea_crc.h"

int32u _LibNMEA_Xor(const int8u *p_chk_str)
{
	int32u check_sum = 0;

	while ((*p_chk_str) != '\0') {
		check_sum ^= (*p_chk_str);
		p_chk_str++;
	}

	return check_sum;
}
