/****************************************************************************************************
  * @brief      : 	JZ2440v2������led����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "led.h"
#include "timer.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"

/**********************************************************************************
  * @brief       : 	��ʼ��led�����Դ
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	0	��ʵ������
  * @others      : 	��
***********************************************************************************/
int InitLed(void)
{
	/* ����GPFCON��GPF4/5/6����Ϊ������� */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));
	return 0;
}

/**********************************************************************************
  * @brief       : 	led���Ժ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestLED(void)
{
	int iVal = 0;  /* val: 0b000, 0b111 */
	int iTmp = 0;

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(176, 5, "LED TEST", 0xe3170d, 0);
	PrintFbString8x16(100, 110, "     LED testing is in progress......", 0x0,0);
	DisplayReturnButton();
	
	/* ѭ������ */
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}
		
		iTmp = ~iVal;
		iTmp &= 7;
		GPFDAT &= ~(7<<4);
		GPFDAT |= (iTmp<<4);
		Delay(100000);
		iVal++;
		if (iVal == 8)
			iVal =0;
		
	}	
}

