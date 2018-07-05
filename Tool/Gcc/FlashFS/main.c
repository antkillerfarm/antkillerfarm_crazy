#include "flash_fs.h"
#include "flash_drv.h"
#include <stdio.h>
#include <stdlib.h>

RecvCmd_Uart_Object uart0 = {0x1234, 0x5678, 0x9012, 0x3456};
RecvCmd_Uart_Object uart1 = {0x1111, 0x5555, 0x9999, 0x3333};
RecvCmd_PPS_Object pps0 = {0,0,0};
RecvCmd_TMark_Object tmark0 = {0x998, 0x997, 0xFFFF1234};
RecvCmd_NMEA_Object nmea0 = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd};

StBD2RawEphemeris eph0 = {1, 3, 5, 7, 9, 2, 4, 6, 8, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 3, 1, 4, 1, 5, 9, 2, 6};
StBD2RawAlmanac alm0 = {5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8};

void testcase_0()
{
	InitFlash();
	SaveFlashToFile("t1.dat");
}

void testcase_1()
{
	InitFlash();
	FlashLoadData();
	FlashWriteItem(FLDT_Uart, &uart0);
	FlashFlush();
	SaveFlashToFile("t2.dat");
	FlashWriteItem(FLDT_Uart, &uart1);
	FlashWriteItem(FLDT_PPS, &pps0);
	FlashFlush();
	SaveFlashToFile("t3.dat");
}

void testcase_2()
{
	int32u check = CalcCRCForItem((int32u *)(&uart0), sizeof(RecvCmd_Uart_Object));
	FLCheck_Object* p_fl_check = (FLCheck_Object*)(&check);
	printf("testcase_2:sizeof(FLCheck_Object):%d\r\n", sizeof(FLCheck_Object));
	printf("testcase_2:CRC:%x\r\n", check);
	printf("testcase_2:parity:%x crc:%x\r\n", p_fl_check->parity, p_fl_check->crc);
	check = CalcCRCForItem((int32u *)(&pps0), sizeof(RecvCmd_PPS_Object));
	printf("testcase_2:sizeof(RecvCmd_PPS_Object):%d\r\n", sizeof(RecvCmd_PPS_Object));
	printf("testcase_2:CRC:%x\r\n", check);
}

void testcase_3()
{
	LoadFlashFromFile("t3.dat");
	FlashLoadData();
	FlashWriteItem(FLDT_Uart, &uart1);
	FlashFlush();
	SaveFlashToFile("t4.dat");
	FlashWriteItem(FLDT_Uart, &uart0);
	FlashWriteItem(FLDT_PPS, &pps0);
	FlashWriteItem(FLDT_TMark, &tmark0);
	FlashWriteItem(FLDT_NMEA, &nmea0);
	FlashWriteItem(FLDT_Eph, &eph0);
	FlashWriteItem(FLDT_Alm, &alm0);
	FlashFlush();
	SaveFlashToFile("t5.dat");
}

void testcase_4()
{
	int32u i;
	LoadFlashFromFile("t5.dat");
	FlashLoadData();
	for (i = 0; i < 440; i++)
	{
		uart0.aUartBRate[0] += i;
		pps0.bPPS += 2 * i;
		FlashWriteItem(FLDT_Uart, &uart0);
		FlashWriteItem(FLDT_PPS, &pps0);
		FlashFlush();	
	}
	SaveFlashToFile("t6.dat");
}

void testcase_5()
{
	LoadFlashFromFile("t6.dat");
	FlashLoadData();
	SaveFlashToFile("t7.dat");
}

int main()
{
	testcase_1();
	FlashFSReset();
	testcase_3();
	FlashFSReset();
	testcase_4();
	FlashFSReset();
	testcase_5();
}
