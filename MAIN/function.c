/**********************************************************************************
  * @brief      : 	JZ2440v2������ȫ������Ҫ���ܺ�������ͷ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "function.h"
#include "my_printf.h"
#include "s3c2440_soc.h"
#include "uart.h"
#include "font.h"
#include "timer.h"
#include "led.h"
#include "lcd.h"
#include "framebuffer.h"
#include "geometry.h"
#include "touchscreen.h"
#include "tslib.h"
#include "nand_flash.h"
#include "key.h"
#include "i2c_controller.h"
#include "oled.h"
#include "spi_flash.h"
#include "spi_gpio_simulate.h"


STsParameters g_sTsParam;
SButton g_sButton[12];
SButton g_sButtonReturn;

/**********************************************************************************
  * @brief       : 	��ʼ����������Ҫ�Ĵ�������ť����13��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitButton(void)
{
	g_sButton[0].iID = TEST_LED;
	g_sButton[0].iX = 20;
	g_sButton[0].iY = 45;
	g_sButton[0].iLength = BUTTON_LENGTH;
	g_sButton[0].iHigh = BUTTON_HIGH;
	g_sButton[0].sName = "LED Test";

	g_sButton[1].iID = TEST_KEY;
	g_sButton[1].iX = 170;
	g_sButton[1].iY = 45;
	g_sButton[1].iLength = BUTTON_LENGTH;
	g_sButton[1].iHigh = BUTTON_HIGH;
	g_sButton[1].sName = "KEY Test";

	
	g_sButton[2].iID = TEST_LCD;
	g_sButton[2].iX = 320;
	g_sButton[2].iY = 45;
	g_sButton[2].iLength = BUTTON_LENGTH;
	g_sButton[2].iHigh = BUTTON_HIGH;
	g_sButton[2].sName = "LCD Test";
	

	g_sButton[3].iID = TEST_TS;
	g_sButton[3].iX = 20;
	g_sButton[3].iY = 99;
	g_sButton[3].iLength = BUTTON_LENGTH;
	g_sButton[3].iHigh = BUTTON_HIGH;
	g_sButton[3].sName = " TS Test";

	g_sButton[4].iID = TEST_I2C;
	g_sButton[4].iX = 170;
	g_sButton[4].iY = 99;
	g_sButton[4].iLength = BUTTON_LENGTH;
	g_sButton[4].iHigh = BUTTON_HIGH;
	g_sButton[4].sName = "I2C Test";

	g_sButton[5].iID = TEST_SPI;
	g_sButton[5].iX = 320;
	g_sButton[5].iY = 99;
	g_sButton[5].iLength = BUTTON_LENGTH;
	g_sButton[5].iHigh = BUTTON_HIGH;
	g_sButton[5].sName = "SPI Test";

	g_sButton[6].iID = TEST_IDR;
	g_sButton[6].iX = 20;
	g_sButton[6].iY = 153;
	g_sButton[6].iLength = BUTTON_LENGTH;
	g_sButton[6].iHigh = BUTTON_HIGH;
	g_sButton[6].sName = "IDR Test";

	g_sButton[7].iID = TEST_DHT1;
	g_sButton[7].iX = 170;
	g_sButton[7].iY = 153;
	g_sButton[7].iLength = BUTTON_LENGTH;
	g_sButton[7].iHigh = BUTTON_HIGH;
	g_sButton[7].sName = "DHT1Test";

	g_sButton[8].iID = TEST_DS18;
	g_sButton[8].iX = 320;
	g_sButton[8].iY = 153;
	g_sButton[8].iLength = BUTTON_LENGTH;
	g_sButton[8].iHigh = BUTTON_HIGH;
	g_sButton[8].sName = "DS18Test";

	g_sButton[9].iID = TEST_0038T;
	g_sButton[9].iX = 20;
	g_sButton[9].iY = 207;
	g_sButton[9].iLength = BUTTON_LENGTH;
	g_sButton[9].iHigh = BUTTON_HIGH;
	g_sButton[9].sName = "0038Test";

	g_sButton[10].iID = TEST_NOR;
	g_sButton[10].iX = 170;
	g_sButton[10].iY = 207;
	g_sButton[10].iLength = BUTTON_LENGTH;
	g_sButton[10].iHigh = BUTTON_HIGH;
	g_sButton[10].sName = "NOR Test";

	g_sButton[11].iID = TEST_NAND;
	g_sButton[11].iX = 320;
	g_sButton[11].iY = 207;
	g_sButton[11].iLength = BUTTON_LENGTH;
	g_sButton[11].iHigh = BUTTON_HIGH;
	g_sButton[11].sName = "NANDTest";

	g_sButtonReturn.iID = TEST_RETURN;
	g_sButtonReturn.iX = RETURN_BUTTON_X;
	g_sButtonReturn.iY = RETURN_BUTTON_Y;
	g_sButtonReturn.iLength = BUTTON_LENGTH;
	g_sButtonReturn.iHigh = BUTTON_HIGH;
	g_sButtonReturn.sName = " RETURN ";

}

/**********************************************************************************
  * @brief       : 	�жϲ�����ҳ��ķ��ذ�ť�Ƿ񱻵��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	1	������ҳ��ķ��ذ�ť�����
  					0	������ҳ��ķ��ذ�ťδ�����
  * @others      : 	��
***********************************************************************************/
int isClickReturn(void)
{
	int i;
	int x = 0;
	int y = 0;
	int iPressure = 0;
	int iXsum = 0;
	int iYsum = 0;
	int iPreX = 0;
	int iPreY = 0;
	int iCnt = 0;
	int iRes = 0;

	ReadTs(&x, &y, &iPressure);

	if(x == 0 && y == 0 && iPressure == 0)
	{
		return iRes;	
	}
	else
	{
		iXsum += x;
		iYsum += y;
		iPreX = x;
	   	iPreY = y;		
		iCnt++;

		for(i=0; i<5; i++)
		{
			ReadTs(&x, &y, &iPressure);
			if(x != 0 || y != 0 || iPressure != 0)
			{
				if(x<iXsum+20 && x>iXsum-20 && y<iPreY+20 && y>iPreY-20)
				{
					iXsum += x;
					iYsum += y;
					iCnt++;
				}
			}

		}
	}


	x = iXsum/iCnt;
	y = iYsum/iCnt;
	
	if( (x > g_sButtonReturn.iX) && (x < g_sButtonReturn.iX+BUTTON_LENGTH) && (y > g_sButtonReturn.iY) && (y < g_sButtonReturn.iY+BUTTON_HIGH))
	{
		iRes = 1;
		ClickedButton(g_sButtonReturn.iX,g_sButtonReturn.iY,g_sButtonReturn.sName,0xFFFFFF);
		Delay(2000);
	}

	ReadTs(&x, &y, &iPressure);
	ReadTs(&x, &y, &iPressure);
	ReadTs(&x, &y, &iPressure);
	return iRes;
}

/**********************************************************************************
  * @brief       : 	���д�������ť�����ʱ�ı䰴ť����ɫ��ʶ��ť�����
  * @param[in]   : 	x	������İ�ť��x����
  					y	������İ�ť��y����
  					name	������İ�ť������
  					color	������İ�ť�����ֵ���ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void ClickedButton(int x,int y,char * name,unsigned int color)
{
	int i = 0;
	for(i = 0; i < 45; i++)
		DrawLine(x, y + i, x + 140, y+i, 0x03a89e);
			
	PrintFbString16x32(x+8, y+6, name, color, 0);
	return;
}

/**********************************************************************************
  * @brief       : 	������������ʱ���Ի�ȡ�����ͨ�������ҳ���е���һ����ť�����ʵ��
  * @param[in]   : 	��
  * @param[out]  : 	value	���Դ�Ų������id
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void GetTestItem(int* value)
{
	int x = 0;
	int y = 0;
	int iXsum = 0;
	int iYsum = 0;
	int iCnt = 0;
	int iPreX = 0;
	int iPreY = 0;
	int iPressure = 0;

	ReadTs(&x, &y, &iPressure);
	if(x == 0 && y == 0 && iPressure == 0)
	{
		ReadTs(&x, &y, &iPressure);
		ReadTs(&x, &y, &iPressure);
		ReadTs(&x, &y, &iPressure);
		return;
	}
	else
	{
		iXsum += x;
		iYsum += y;
		iPreX = x;
	   	iPreY = y;
	   	iCnt++;

		while(iPressure)
		{
			x = 0;
			y = 0;
			iPressure = 0;
			ReadTs(&x, &y, &iPressure);
			if(x<iPreX+30 && x>iPreX-30 && y<iPreY+30 && y>iPreY-30)
			{
				iXsum += x;
				iYsum += y;
				iPreX = x;
   				iPreY = y;
				iCnt++;
			}
		}
		
		x = iXsum/iCnt;
		y = iYsum/iCnt;
		
		if( (x > g_sButton[TEST_LED].iX) && (x < g_sButton[TEST_LED].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_LED].iY) && (y < g_sButton[TEST_LED].iY+BUTTON_HIGH))
			*value = TEST_LED;
		else if( (x > g_sButton[TEST_KEY].iX) && (x < g_sButton[TEST_KEY].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_KEY].iY) && (y < g_sButton[TEST_KEY].iY+BUTTON_HIGH))
			*value = TEST_KEY;
		else if( (x > g_sButton[TEST_LCD].iX) && (x < g_sButton[TEST_LCD].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_LCD].iY) && (y < g_sButton[TEST_LCD].iY+BUTTON_HIGH))
			*value = TEST_LCD;
		else if( (x > g_sButton[TEST_TS].iX) && (x < g_sButton[TEST_TS].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_TS].iY) && (y < g_sButton[TEST_TS].iY+BUTTON_HIGH))
			*value = TEST_TS;
		else if( (x > g_sButton[TEST_I2C].iX) && (x < g_sButton[TEST_I2C].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_I2C].iY) && (y < g_sButton[TEST_I2C].iY+BUTTON_HIGH))
			*value = TEST_I2C;
		else if( (x > g_sButton[TEST_SPI].iX) && (x < g_sButton[TEST_SPI].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_SPI].iY) && (y < g_sButton[TEST_SPI].iY+BUTTON_HIGH))
			*value = TEST_SPI;
		else if( (x > g_sButton[TEST_IDR].iX) && (x < g_sButton[TEST_IDR].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_IDR].iY) && (y < g_sButton[TEST_IDR].iY+BUTTON_HIGH))
			*value = TEST_IDR;
		else if( (x > g_sButton[TEST_DHT1].iX) && (x < g_sButton[TEST_DHT1].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_DHT1].iY) && (y < g_sButton[TEST_DHT1].iY+BUTTON_HIGH))
			*value = TEST_DHT1;
		else if( (x > g_sButton[TEST_DS18].iX) && (x < g_sButton[TEST_DS18].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_DS18].iY) && (y < g_sButton[TEST_DS18].iY+BUTTON_HIGH))
			*value = TEST_DS18;
		else if( (x > g_sButton[TEST_0038T].iX) && (x < g_sButton[TEST_0038T].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_0038T].iY) && (y < g_sButton[TEST_0038T].iY+BUTTON_HIGH))
			*value = TEST_0038T;
		else if( (x > g_sButton[TEST_NOR].iX) && (x < g_sButton[TEST_NOR].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_NOR].iY) && (y < g_sButton[TEST_NOR].iY+BUTTON_HIGH))
			*value = TEST_NOR;
		else if( (x > g_sButton[TEST_NAND].iX) && (x < g_sButton[TEST_NAND].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_NAND].iY) && (y < g_sButton[TEST_NAND].iY+BUTTON_HIGH))
			*value = TEST_NAND;

		if(*value != ORIGINAL)
		{
			printf("GetTestItem x = %d,y = %d\n\r",x,y);
			ClickedButton(g_sButton[*value].iX,g_sButton[*value].iY,g_sButton[*value].sName,0xFFFFFF);
			Delay(20000);
		}

		DisplayTestItemIcon(g_sButton[*value].iX,g_sButton[*value].iY,g_sButton[*value].sName,0x0);

		ReadTs(&x, &y, &iPressure);
		ReadTs(&x, &y, &iPressure);
		ReadTs(&x, &y, &iPressure);
		printf("GetTestItem iCnt = %d\n\r",iCnt);
		   	
	}	
	return;	
}

/**********************************************************************************
  * @brief       : 	��ʼ��������У׼����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void InitTsParam(void)
{
	g_sTsParam.iFlag = 0;
	g_sTsParam.iA_ts_x = 0;
	g_sTsParam.iA_ts_y = 0;
	g_sTsParam.iB_ts_x = 0;
	g_sTsParam.iB_ts_y = 0;
	g_sTsParam.iC_ts_x = 0;
	g_sTsParam.iC_ts_y = 0;
	g_sTsParam.iD_ts_x = 0;
	g_sTsParam.iD_ts_y = 0;
	g_sTsParam.iE_ts_x = 0;
	g_sTsParam.iE_ts_y = 0;

}

/**********************************************************************************
  * @brief       : 	��ʼ��������У׼
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitCalibration(void)
{
	unsigned int iFbBase;
	int iXres, iYres, iBpp;
	int iRes;
	
	ReadNand(0x00020000, (unsigned char*)&g_sTsParam, sizeof(STsParameters));

	if(g_sTsParam.iFlag == 0x12345678)
	{
		SetTsCalibrateParams(&g_sTsParam);
		printf(" SetTsCalibrateParams\n\r");
		return;
	}
	
	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	GetLcdParams(&iFbBase, &iXres, &iYres, &iBpp);

	/* ���� */
	ClearScreen(0x0);

	/* ��ʾ������ʾ��׼ */
	PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0xffffff,0);

	while(1)
	{
		iRes = CalibrateTs(&g_sTsParam);
		if(iRes == 1)
		{
			g_sTsParam.iFlag = 0x12345678;
			NandErase(0x00020000, 128*1024);
			NandWrite(0x00020000, (unsigned char*)&g_sTsParam, sizeof(STsParameters));
			break;
		}

		PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0x0,0);
		PrintFbString8x16(70, iYres - 95, "Calibration failed, Please try again!", 0xffffff,1);
		Delay(2500000);
		PrintFbString8x16(70, iYres - 95, "Calibration failed, Please try again!", 0x0,1);
		PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0xffffff,0);
	}
	
	/* ��ʾ������ʾ�滭 */
	PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0x0,0);
	PrintFbString8x16(70, iYres - 95, "OK! Calibration Triumphantly !", 0xffffff,1);
	Delay(2500000);
}


/**********************************************************************************
  * @brief       : 	�ڲ�����ҳ���ָ��λ����ʾ���������������Ŀ�Ĵ�������ť
  * @param[in]   : 	x	��ʾ��������ť��x����
  					y	��ʾ��������ť��y����
  					name	��ʾ��������ť������
  					color	��ʾ��������ť���ֵ���ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DisplayTestItemIcon(int x,int y,char * name,unsigned int color)
{
	int i;
	
	for(i = 0; i < 45; i++)
		DrawLine(x, y + i, x + 140, y+i, 0xffffff);

	DrawRectangle(x,y,140,45,0);

	PrintFbString16x32(x+8, y+6, name, color, 0);

	return;
}

/**********************************************************************************
  * @brief       : 	�ڲ�����ҳ���ָ��λ����ʾ����������ҳ��Ĵ�������ť
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DisplayReturnButton(void)
{
	int i;
	
	for(i = 0; i < 45; i++)
		DrawLine(g_sButtonReturn.iX, g_sButtonReturn.iY + i, g_sButtonReturn.iX + 140, g_sButtonReturn.iY+i, 0x03a89e);

	DrawRectangle(g_sButtonReturn.iX,g_sButtonReturn.iY,140,45,0);

	PrintFbString16x32(g_sButtonReturn.iX+8, g_sButtonReturn.iY+6, g_sButtonReturn.sName, 0xffffff, 0);

	return;
}

/**********************************************************************************
  * @brief       : 	����������һϵ�г�ʼ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void BootInit(void)
{
	InitTsParam();
	InitButton();
	InitLed();
	InitKey();
	InitNand();
	InitLcd();
	EnableLcd();
	InitFont();
	InitTimer();
	InitTouchScreen();
	GetLcdParamsforFb();
	InitCalibration();
}

/**********************************************************************************
  * @brief       : 	����������ӭ���漰�����Ϣ��ʾ
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void BootScreen(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	unsigned char *puchBpp8;
	unsigned short *pusBpp16;
	unsigned int *puiBpp32;
	unsigned int iRgb = 0X0;
	
	GetLcdParams(&uiFbBase, &iXres, &iYres, &iBpp);

	/* ��framebuffer��д���� */
	if (iBpp == 8)
	{
		/* palette[0] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 0;		
	}
	else if (iBpp == 16)
	{

		/* black */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = Convert32BppTo16Bpp(0x7cfc00);
			
	}
	else if (iBpp == 32)
	{
		/* black */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0x7cfc00;
	}

	Delay(500000);

	PrintFbString8x16(10, 10, "Welcome to the project show: JZ2440v2 full module test", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 50, "Author: LeiChuan", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 90, "QQ: 793805526", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 130, "Email: sagacity_lc@163.com", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 170, "Github: git@github.com:sagacity-leichuan/", iRgb, 1);	
	PrintFbString8x16(10, 190, "        All_Modules_Tested_of_JZ2440.git", iRgb, 1);

	
	Delay(2000000);
	PrintFbString8x16(10, 230, "Now,Project show starts right away......", iRgb, 1);

	Delay(3500000);

}


/**********************************************************************************
  * @brief       : 	�������е���ҳ����ʾ����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void MainPage(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	int iPressure;
	int i;
	unsigned char *puchBpp8;
	unsigned short *pusBpp16;
	unsigned int *puiBpp32;
	unsigned int iRgb = 0X0;

	
	GetLcdParams(&uiFbBase, &iXres, &iYres, &iBpp);

	/* ��framebuffer��д���� */
	if (iBpp == 8)
	{
		/* palette[0] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 0;		
	}
	else if (iBpp == 16)
	{

		/* black */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = Convert32BppTo16Bpp(0xf4a460);
			
	}
	else if (iBpp == 32)
	{
		/* black */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0xf4a460;
	}


	PrintFbString16x32(176, 5, "MainPage", 0xffffff, 0);

	for(i = 0; i < 12; i++)
	{
		DisplayTestItemIcon(g_sButton[i].iX,g_sButton[i].iY,g_sButton[i].sName,iRgb);
	}

	ReadTs(&x, &y, &iPressure);
	ReadTs(&x, &y, &iPressure);
	ReadTs(&x, &y, &iPressure);
	
}

/**********************************************************************************
  * @brief       : 	jz2440������spi��ص�ģ����Ժ������򣬰���spi flash �� oled
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestSPI(void)
{
	unsigned int mid = 0, pid = 0;
	char c;

	/* ��ʼ�� */
#ifdef SPIGPIO
	InitSPIGPIO();
#else
	InitSPIS3c2440Controller();
#endif

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(90, 5, "SPI-Flash/OLED TEST", 0xe3170d, 0);
	
	InitOLED();
    PrintOLEDString(0,0,"leichuan, sagacity_lc@163.com");

    ReadSPIFlashID(&mid, &pid);
 	
    printf("SPI Flash : MID = 0x%02x, PID = 0x%02x\n\r", mid, pid);

	if((mid == 0xff) || (pid  == 0xff))
	{
		PrintFbString8x16(40, 120, "The corresponding module failed to connect properly!!!", 0x4169e1, 1);
		mDelay(5000);
		MainPage();
		return;
	}
    
    InitSPIFlash();
    
	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(90, 5, "SPI-Flash/OLED TEST", 0xe3170d, 0);
	DisplayReturnButton();


	PrintFbString16x32(90, 40, "NOTE:", 0x0b1746, 0);
	PrintFbString8x16(122, 85, 	"You can feel free to", 0x4169e1, 0);
	PrintFbString8x16(90, 110, "test the read and write f", 0x4169e1, 0);
	PrintFbString8x16(90, 130, "unctions without any imp", 0x4169e1, 0);
	PrintFbString8x16(90, 155, "act on the program!", 0x4169e1, 0);

	PrintFbString8x16(40, 240, "If you want to test SPI-Flash read-write function, please connect to PC serial port tool and re-enter this page.", 0xff0000, 0);

	/* ��ӡ�˵�, ������ѡ��������� */
	printf("[w] Write SPI-Flash\n\r");
	printf("[r] Read SPI-Flash\n\r");
	printf("[e] Erase SPI-Flash\n\r");
	printf("[q] quit\n\r");
	printf("Enter selection: ");
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}

		c = GetChar();
		
		printf("%c\n\r", c);
		
		if(c == 0)
		{
			break;
		}

		/* ��������:
		 * 3. ��дĳ����ַ
		 * 4. ��ĳ����ַ
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				MainPage();
				return;
				return;
				break;
				
			case 'w':
			case 'W':
				DoWriteSPIFLASH();
				printf("\n\r[w] Write SPI-Flash\n\r");
				printf("[r] Read SPI-Flash\n\r");
				printf("[e] Erase SPI-Flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'r':
			case 'R':
				DoReadSPIFLASH();
				printf("\n\r[w] Write SPI-Flash\n\r");
				printf("[r] Read SPI-Flash\n\r");
				printf("[e] Erase SPI-Flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");	
				break;

			case 'e':
			case 'E':
				DoEraseSPIFLASH();
				printf("\n\r[w] Write SPI-Flash\n\r");
				printf("[r] Read SPI-Flash\n\r");
				printf("[e] Erase SPI-Flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;
				
			default:
				break;
		}
	}
}

/**********************************************************************************
  * @brief       : 	����������ת������Ӧ���Ƶ��ַ���
  * @param[in]   : 	c	��ת������������
  					radix	��Ҫת���Ľ��ƣ�ȡֵ��Χ{10,16}
  * @param[out]  : 	buf		���ڴ洢ת���Ľ��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void Convert(unsigned char buf[],unsigned int c, int radix)
{
	unsigned int iTemp;
	unsigned char chBuf[33];
	int i = 0;
	int j;

	if(c == 0)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}
	
	while(c != 0)  
    {  
    	if(radix == 16)
		{
	        iTemp=c%16;
	        if(iTemp>=0 && iTemp<10)  
	        {  
	            chBuf[i]=iTemp +'0';  
	            i++;  
	        }  
	        else  
	        {  
	            chBuf[i]=iTemp+'A'-10;       
	            i++;  
	        }	        	
	        c = c/16;	        
        }
        else if(radix == 10)
        {
			iTemp=c%10;  
	        if(iTemp>=0 && iTemp<10)  
	        {  
	            chBuf[i]=iTemp +'0';  
	            i++;  
	        }	        
	        c = c/10;
        }
    }

	for(j=0;j<i;j++)
		buf[j]=chBuf[i-j-1];
    
    buf[i]='\0'; 
}

