/* Interface for CRC-24Q cyclic redundancy chercksum code
 *
 * This file is Copyright (c) 2010 by the GPSD project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 */
#ifndef _CRC24Q_H_
#define _CRC24Q_H_

/* interface function declaration */
#ifdef __cplusplus
extern "C"
{
#endif
        int32u CRC_24Q(int8u *data, int32u len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC24Q_H_ */
