/**********************************************************************************
  * @brief      : 	JZ2440v2�����尴�����Դ���Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "key.h"
#include "s3c2440_soc.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"

int iTestStatus = 0;		//���ڱ�ʾ�Ƿ���뵽�˰�������
int iPhotoresistorEnd = 0;		//�����ڽ��й����������ʱ������ҳ��ı�־

/* SRCPND ������ʾ�ĸ��жϲ�����, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK ���������ж�, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND ������ʾ��ǰ���ȼ���ߵġ����ڷ������ж�, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/**********************************************************************************
  * @brief       : 	�����жϴ�����
  * @param[in]   : 	irq	�жϺ�
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void KeyEintIrq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;
	
	if (irq == 0) /* eint0 : s2 ���� D12 */
	{
		if (val1 & (1<<0)) /* s2 --> gpf6 */
		{
			/* �ɿ� */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT0 is released!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT0 is released!", 0xffffff,1);
			}
			GPFDAT |= (1<<6);
		}
		else
		{
			/* ���� */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT0 is pressed!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT0 is pressed!", 0xffffff,1);
			}
			GPFDAT &= ~(1<<6);
		}
		
	}
	else if (irq == 2) /* eint2 : s3 ���� D11 */
	{
		if (val1 & (1<<2)) /* s3 --> gpf5 */
		{
			/* �ɿ� */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT2 is released!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT2 is released!", 0xffffff,1);
			}
			GPFDAT |= (1<<5);
		}
		else
		{
			/* ���� */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT2 is pressed!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT2 is pressed!", 0xffffff,1);
			}
			GPFDAT &= ~(1<<5);
		}
		
	}
	else if (irq == 5) /* eint8_23, eint11--s4 ���� D10, eint19---s5 ��������LED */
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (val2 & (1<<3)) /* s4 --> gpf4 */
			{
				/* �ɿ� */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT11 is released!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT11 is released!", 0xffffff,1);
				}
				GPFDAT |= (1<<4);
			}
			else
			{
				/* ���� */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT11 is pressed!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT11 is pressed!", 0xffffff,1);
				}
				GPFDAT &= ~(1<<4);
			}
		}
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* �ɿ� */
				/* Ϩ������LED */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT19 is released!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT19 is released!", 0xffffff,1);
				}
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* ����: ��������LED */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT19 is pressed!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT19 is pressed!", 0xffffff,1);
				}

				iPhotoresistorEnd = 1;
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}

	EINTPEND = val;
}

/****************************************************************************************
  * @brief       : 	��ʼ��������GPIO�ܽ�
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
*****************************************************************************************/
void InitKey(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));

	/* ����GPIOΪ�ж����� */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3������Ϊ�ж����� */

	GPGCON &= ~((3<<6) | (3<<22));
	GPGCON |= ((2<<6) | (2<<22));   /* S4,S5������Ϊ�ж����� */
	

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* ����EINTMASKʹ��eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));

	RegisterIrq(0, KeyEintIrq);
	RegisterIrq(2, KeyEintIrq);
	RegisterIrq(5, KeyEintIrq);	
}

/**********************************************************************************************
  * @brief       : 	���԰������ܣ�����LCD����ʾ���������ɿ�״̬
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************************/
void TestKey(void)
{

	iTestStatus = 1;
	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(176, 5, "KEY TEST", 0xe3170d, 0);
	DisplayReturnButton();
	
	/* ѭ������ */
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			iTestStatus = 0;
			break;
		}	
	}		
}


