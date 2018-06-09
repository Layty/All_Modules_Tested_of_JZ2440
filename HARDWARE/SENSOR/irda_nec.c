#include "irda_rawdata.h"
#include "circle_buffer.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"


/*
 * �ӻ��ͻ������л����������,
 * �����ó�address, data
 */

#define DURATION_BASE  563
#define DURATION_DELTA (DURATION_BASE/2)

#define DURATION_HEAD_LOW    (16*DURATION_BASE)
#define DURATION_HEAD_HIGH   (8*DURATION_BASE)
#define DURATION_REPEAT_HIGH (4*DURATION_BASE)
#define DURATION_DATA_LOW    (1*DURATION_BASE)
#define DURATION_DATA1_HIGH  (3*DURATION_BASE)
#define DURATION_DATA0_HIGH  (1*DURATION_BASE)
#define DURATION_END_LOW     (1*DURATION_BASE)

static int DurationInMargin(int duration, int us)
{
	if ((duration > (us - DURATION_DELTA)) && (duration < us + DURATION_DELTA))
		return 1;
	else
		return 0;
}

/*
 * ����ֵ: 0-�õ�����, 1-�õ��ظ���, -1 : ʧ��
 */
int ReadIrdaNec(int *address, int *data)
{
	SIrdaRawdataEvent event;	
	int i;
	unsigned int val = 0;
	unsigned char byte[4];

	while (1)
	{

		if(isClickReturn())
		{
			return 2;
		}
		
		if (0 == GetIrEvent(&event))
		{
			/* �������� */
			/* 1. �ж��Ƿ�Ϊ9MS�ĵ����� */
			if (event.iPol == 0 && \
				DurationInMargin(event.iDuration, DURATION_HEAD_LOW))
			{
				/* ���к����ж� */
				/* 2. ����һ������������ */
				if (0 == GetIrEventTimeout(&event, 10000))
				{
					/* 3. �ж����Ƿ�4.5ms�ĸ�����
					 *    ���� 2.25ms�ĸ�����
					 */
					if (event.iPol == 1 && \
						DurationInMargin(event.iDuration, DURATION_HEAD_HIGH))
					{
						/* 4.5ms�ĸ����� */
						/* 4. �ظ�����32λ���� */
						for (i = 0; i < 32; i++)
						{
							/* 5. ��0.56ms�ĵ����� */
							if (0 == GetIrEventTimeout(&event, 10000))
							{
								if (event.iPol == 0 && \
									DurationInMargin(event.iDuration, DURATION_DATA_LOW))
								{
									/* 6. ����һ������, �ж����� 0.56ms/1.68ms�ĸ����� */
									if (0 == GetIrEventTimeout(&event, 10000))
									{
										if (event.iPol == 1 && \
											DurationInMargin(event.iDuration, DURATION_DATA1_HIGH))
										{
											/* �õ���bit 1 */
											val |= (1<<i);
										}
										else if (event.iPol == 1 && \
												DurationInMargin(event.iDuration, DURATION_DATA0_HIGH))
										{
											/* �õ���bit 0 */
										}
										else
										{
											printf("%s %d\n\r", __FUNCTION__, __LINE__);
											return -1;
										}
									}
									else
									{
										printf("%s %d\n\r", __FUNCTION__, __LINE__);
										return -1;
									}
								}
								else
								{
									printf("%s %d\n\r", __FUNCTION__, __LINE__);
									return -1;
								}
							}
							else
							{
								printf("%s %d\n\r", __FUNCTION__, __LINE__);
								return -1;
							}
						}

						/* 7. �õ���32λ����, �ж������Ƿ���ȷ */
						byte[0] = val & 0xff;
						byte[1] = (val>>8) & 0xff;
						byte[2] = (val>>16) & 0xff;
						byte[3] = (val>>24) & 0xff;

						//printf("get data: %x %x %x %x\n\r", byte[0], byte[1], byte[2], byte[3]);
						byte[1] = ~byte[1];
						byte[3] = ~byte[3];
						
						if (byte[0] != byte[1])
						{
							/* ��Щң��������ȫ����NEC�淶 */
							//printf("%s %d\n\r", __FUNCTION__, __LINE__);
							//return -1;
						}
						if (byte[2] != byte[3])
						{
							printf("%s %d\n\r", __FUNCTION__, __LINE__);
							return -1;
						}
						*address = byte[0];
						*data    = byte[2];
						return 0;
						
					}
					else if (event.iPol == 1 && \
						DurationInMargin(event.iDuration, DURATION_REPEAT_HIGH))
					{
						/* 2.25ms�ĸ����� */
						return 1;  /* �ظ��� */
					}
					else
					{
						printf("%s %d\n\r", __FUNCTION__, __LINE__);
						return -1;  /* ���� */
					}
				}
			}
			else
			{
				//printf("%s %d\n\r", __FUNCTION__, __LINE__);
				return -1; /* ��Ч����δ��ʼ */
			}
		}
	}
}

void TestIrdaNec(void)
{
	int address = 0;
	int data = 0;
	int ret;
	int count = 0;
	char chBuffer[10];
	
	InitIrda();

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(140, 5, "IrdaNec TEST", 0xe3170d, 0);

	PrintFbString8x16(40, 120, "Please ensure that the corresponding module is properly connected.", 0x4169e1, 1);
	mDelay(5000);
	PrintFbString8x16(40, 120, "Please ensure that the corresponding module is properly connected.", 0xffffff, 1);

	
	DisplayReturnButton();

	while (1)
	{

		if(isClickReturn())
		{
			MainPage();
			break;
		}
		
		ret = ReadIrdaNec(&address, &data);
		if (ret == 0)
		{
			count = 1;
			printf("irda_nec_read: address = 0x%x, data = 0x%x\n\r", address, data);

			PrintFbString8x16(50, 70, "irda_nec_read: ", 0x4169e1, 0);
			Convert(chBuffer, address, 16);
			PrintFbString8x16(170,95, "address = 0x", 0x4169e1, 0);
			PrintFbString8x16(266,95,chBuffer, 0x4169e1, 0);			
			Convert(chBuffer, data, 16);
			PrintFbString8x16(170, 120, "data = 0x", 0x4169e1, 0);
			PrintFbString8x16(242, 120,chBuffer, 0x4169e1, 0);
			Delay(1000000);
			PrintFbString8x16(50, 70, "irda_nec_read: ", 0xffffff, 0);
			Convert(chBuffer, address, 16);
			PrintFbString8x16(170,95, "address = 0x", 0xffffff, 0);
			PrintFbString8x16(266,95,chBuffer, 0xffffff, 0);			
			Convert(chBuffer, data, 16);
			PrintFbString8x16(170, 120, "data = 0x", 0xffffff, 0);
			PrintFbString8x16(242, 120,chBuffer, 0xffffff, 0);
			
		}
		else if (ret == 1)
		{
			count++;
			printf("This is a repeat code\n\r");
			Convert(chBuffer, count, 10);
			PrintFbString8x16(150, 120, chBuffer, 0x4169e1, 1);
			PrintFbString8x16(158, 120, "th iteration of 0x", 0x4169e1, 1);
			Convert(chBuffer, data, 16);
			PrintFbString8x16(302, 120, chBuffer, 0x4169e1, 1);
			PrintFbString8x16(318, 120, " code.", 0x4169e1, 1);

			Delay(400000);
			
			Convert(chBuffer, count, 10);
			PrintFbString8x16(150, 120, chBuffer, 0xffffff, 1);
			PrintFbString8x16(158, 120, "th iteration of 0x", 0xffffff, 1);
			Convert(chBuffer, data, 16);
			PrintFbString8x16(302, 120, chBuffer, 0xffffff, 1);
			PrintFbString8x16(318, 120, " code.", 0xffffff, 1);
		}
		else if (ret == 2)
		{
			MainPage();
			return;
		}
	}
}

