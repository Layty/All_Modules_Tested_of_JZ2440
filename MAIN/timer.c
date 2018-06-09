#include "s3c2440_soc.h"
#include "interrupt.h"
#include "timer.h"
#include "string_utils.h"



#define TIMER_NUM  32

STimerDesc timer_array[TIMER_NUM];

static unsigned long long g_system_time_10ms_cnt = 0;

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

int StopTimer(void)
{
	TCON &= ~(1<<0);
	return 0;
}
int StartTimer(void)
{
	TCON |= (1<<0);
	return 0;
}

/* �����ٵ��ú��� */
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

void mDelay(int m)
{
	uDelay(m*1000);
}

unsigned long long GetSystemTimeUs(void)
{
	unsigned long long us = (50000 - TCNTO0)/5;
	return g_system_time_10ms_cnt * 10 * 1000 + us;
}

unsigned int DeltaTimeUs(unsigned long long pre, unsigned long long now)
{
	return (now - pre);
}


