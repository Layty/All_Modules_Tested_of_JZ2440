#include "s3c2440_soc.h"
#include "nand_flash.h"

int isBootFromNorFlash(void)
{
	volatile int *p = (volatile int *)0;
	int val;

	val = *p;
	*p = 0x12345678;
	if (*p == 0x12345678)
	{
		/* д�ɹ�, ��nand���� */
		*p = val;
		return 0;
	}
	else
	{
		/* NOR�������ڴ�һ��д */
		return 1;
	}
}


void Copy2Sdram(void)
{
	/* Ҫ��lds�ļ��л�� __code_start, __bss_start
	 * Ȼ���0��ַ�����ݸ��Ƶ�__code_start
	 */

	extern int __code_start, __bss_start;

	volatile unsigned int *dest = (volatile unsigned int *)&__code_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *src = (volatile unsigned int *)0;
	unsigned int len = (unsigned int)(&__bss_start) - (unsigned int)(&__code_start);

	if (isBootFromNorFlash())
	{
		while (dest < end)
		{
			*dest++ = *src++;
		}
	}
	else
	{
		InitNand();
		ReadNand((unsigned int)src, (unsigned char *)dest, len);
	}

}



void CleanBss(void)
{
	/* Ҫ��lds�ļ��л�� __bss_start, _end
	 */
	extern int _end, __bss_start;

	volatile unsigned int *start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *end = (volatile unsigned int *)&_end;


	while (start <= end)
	{
		*start++ = 0;
	}
}

void InitSdram(void)
{
	BWSCON = 0x22000000;

	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	REFRESH  = 0x8404f5;

	BANKSIZE = 0xb1;

	MRSRB6   = 0x20;
	MRSRB7   = 0x20;
}


