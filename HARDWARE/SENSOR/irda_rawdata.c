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

 /* ��ʵ��GPIO�Ļ������� */
static void CfgIrdaDataAsEint(void)
{
	/* ����Ϊ�ж����� */
	GPFCON &= ~(3<<2);
	GPFCON |= (2<<2);

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<4);  /* eint1 */

}

static int GetIrdaData(void)
{
	if (GPFDAT & (1<<1))
		return 1;
	else
		return 0;
}

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


/* ע���ж� */
void InitIrda(void)
{
	CfgIrdaDataAsEint();
	RegisterIrq(1, IrdaIrq);
}

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

