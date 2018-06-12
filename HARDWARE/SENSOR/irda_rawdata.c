/****************************************************************************************************
  * @brief      : 	JZ2440v2�����������մ�����ԭʼ���ݴ���Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "irda_rawdata.h"
#include "circle_buffer.h"
#include "timer.h"
#include "my_printf.h"
#include "interrupt.h"

/* IRDA���� : EINT1/GPF1 */

static unsigned long long gs_ulLastTime = 0;

/* 
 * ����GPIO, ע���ж�
 * ���жϴ�������:
      ��¼�жϷ�����ʱ��,
      ���ϴ��жϵ�ʱ��Ƚ�, �����������
      ��ȡ���ż���
      �����ݷ��뻷�ͻ�����
 */

 /**************************************************************************************************
   * @brief 	  :  ����GPIO��Ϊ�ж�����Ϊ���պ���ĸߵͱ�Ե�жϴ���
   * @param[in]   :  ��
   * @param[out]  :  ��
   * @return	  :  ��
   * @others	  :  ��
 ***************************************************************************************************/
static void CfgGPIOAsEintForIrdaData(void)
{
	/* ����Ϊ�ж����� */
	GPFCON &= ~(3<<2);
	GPFCON |= (2<<2);

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<4);  /* eint1 */

}

/**************************************************************************************************
  * @brief		 :	���IrdaData
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	1	���һλ1����
  					0	���һλ0����
  * @others 	 :	��
***************************************************************************************************/
static int GetIrdaData(void)
{
	if (GPFDAT & (1<<1))
		return 1;
	else
		return 0;
}

/**************************************************************************************************
  * @brief		 :	��������жϴ�����
  * @param[in]	 :	irq	�жϺ�
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***************************************************************************************************/
void IrdaIrq(int irq)
{
	/* ���жϴ�������:
		 ��¼�жϷ�����ʱ��,
		 ���ϴ��жϵ�ʱ��Ƚ�, �����������
		 ��ȡ���ż���
		 �����ݷ��뻷�ͻ�����
	*/
	SIrdaRawdataEvent event;
	unsigned long long cur = GetSystemTimeUs();
	
	event.iDuration = DeltaTimeUs(gs_ulLastTime, cur);
	event.iPol      = !GetIrdaData();
	PutIrEvent(&event);
	gs_ulLastTime = cur;
}


/**************************************************************************************************
  * @brief		 :	��ʼ��������չ���
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***************************************************************************************************/
void InitIrda(void)
{
	CfgGPIOAsEintForIrdaData();
	RegisterIrq(1, IrdaIrq);
}

/**************************************************************************************************
  * @brief		 :	���Ժ������ԭʼ���ݵĹ���
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***************************************************************************************************/
void TestIrdaRawdata(void)
{
	SIrdaRawdataEvent event;
	unsigned long long pre = 0, cur;
	
	InitIrda();

	while (1)
	{
		if (0 == GetIrEvent(&event))
		{
			cur = GetSystemTimeUs();
			if (DeltaTimeUs(pre, cur) > 1000000)
				printf("\n\r");
			pre = cur;
			printf("%s %d us | ", event.iPol? "hight" : "low", event.iDuration);
		}
	}
}

