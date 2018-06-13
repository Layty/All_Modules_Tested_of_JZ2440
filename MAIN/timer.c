/****************************************************************************************************
  * @brief      : 	JZ2440v2�����嶨ʱ��ʱ����ش���Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "interrupt.h"
#include "timer.h"
#include "string_utils.h"

#define TIMER_NUM  32

STimerDesc timer_array[TIMER_NUM];

static unsigned long long g_system_time_10ms_cnt = 0;

/**********************************************************************************
  * @brief       : 	һ��Ĵֲڵ���ʱ����
  * @param[in]   : 	d	��ʱ��ֵ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void Delay(volatile int d)
{
	while (d--);
}

/**********************************************************************************
  * @brief       : 	ע�ᶨʱ���жϴ�����
  * @param[in]   : 	name	��ʱ��������
  					fp		��ʱ���жϴ�����ָ��
  * @param[out]  : 	��
  * @return      : 	-1	ע��ʧ��
  					0	ע��ɹ�
  * @others      : 	��
***********************************************************************************/
int RegisterTimer(char *name, TimerFunc fp)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!timer_array[i].fp)
		{
			timer_array[i].name = name;
			timer_array[i].fp   = fp;
			return 0;
		}
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	ע����ʱ���жϴ�����
  * @param[in]   : 	name	��ʱ��������
  * @param[out]  : 	��
  * @return      : 	-1	ע��ʧ��
  					0	ע���ɹ�
  * @others      : 	��
***********************************************************************************/
int UnregisterTimer(char *name)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!strcmp(timer_array[i].name, name))
		{
			timer_array[i].name = NULL;
			timer_array[i].fp   = NULL;
			return 0;
		}
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	��ʱ�����жϴ�����
  * @param[in]   : 	irq	�жϺ�
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TimerIrq(int irq)
{
	int i;
	
	g_system_time_10ms_cnt++;
	
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (timer_array[i].fp)
		{
			timer_array[i].fp();
		}
	}	
}

/**********************************************************************************
  * @brief       : 	��ʼ����ʱ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitTimer(void)
{
	/* ����TIMER0��ʱ�� */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	             = 50000000/(4+1)/2
	             = 5000000
	   ÿ��1, ��Ӧ0.2us
	   ÿ��5, ��Ӧ1us
	   50000-->0 : ��Ӧ 10ms
	 */
	 
	TCFG0 = 4;  /* Prescaler 0 = 4, ����timer0,1 */
	TCFG1 &= ~0xf; /* MUX0 : 1/2 */

	/* ����TIMER0�ĳ�ֵ */
	TCNTB0 = 50000;  /* 10Ms�ж�һ�� */

	/* ���س�ֵ, ����timer0 */
	TCON |= (1<<1);   /* Update from TCNTB0 & TCMPB0 */

	/* ����Ϊ�Զ����ز����� */
	TCON &= ~(1<<1);
	TCON |= (1<<0) | (1<<3);  /* bit0: start, bit3: auto reload */

	/* �����ж� */
	RegisterIrq(10, TimerIrq);
}

/**********************************************************************************
  * @brief       : 	�رն�ʱ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
int StopTimer(void)
{
	TCON &= ~(1<<0);
	return 0;
}

/**********************************************************************************
  * @brief       : 	������ʱ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
int StartTimer(void)
{
	TCON |= (1<<0);
	return 0;
}

/**********************************************************************************
  * @brief       : 	us��ʱ����
  * @param[in]   : 	n	��Ҫ��ʱ��usֵ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	�����ٵ��ú���
***********************************************************************************/
void uDelay(int n)
{
	int cnt = n * 5;  /* u us ��Ӧn*5������ֵ */
	int pre = TCNTO0;
	int cur;
	int delta;

	while (cnt > 0)
	{
		cur = TCNTO0;
		if (cur <= pre)
			delta = pre - cur;
		else
			delta = pre + (50000 - cur);

		cnt = cnt - delta;
		pre = cur;
	}
}

/**********************************************************************************
  * @brief       : 	ms��ʱ����
  * @param[in]   : 	m  	��Ҫ��ʱ��msֵ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	�����ٵ��ú���
***********************************************************************************/
void mDelay(int m)
{
	uDelay(m*1000);
}

/**********************************************************************************
  * @brief       : 	���ϵͳ��usֵ
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	���ػ�õ�usֵ
  * @others      : 	�����ٵ��ú���
***********************************************************************************/
unsigned long long GetSystemTimeUs(void)
{
	unsigned long long us = (50000 - TCNTO0)/5;
	return g_system_time_10ms_cnt * 10 * 1000 + us;
}

/**********************************************************************************
  * @brief       : 	���us��ֵ
  * @param[in]   : 	pre	�ϴλ�õ�usֵ
  					now	��λ�õ�usֵ
  * @param[out]  : 	��
  * @return      : 	����us��ֵ
  * @others      : 	��
***********************************************************************************/
unsigned int DeltaTimeUs(unsigned long long pre, unsigned long long now)
{
	return (now - pre);
}

