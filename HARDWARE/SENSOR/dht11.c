/**********************************************************************************
  * @brief      : 	JZ2440v2��������ʪ��dht11����������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "s3c2440_soc.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"

/********************************NOTE**************************************************
ʹ��GPG5����dht11��DATA���� 

����GPIO��ȡDHT11������ 
1. ������������18MS�ĵ�����: start�ź�
2. start�źű�Ϊ��, 20-40us֮��, dht11����������ά��80us
	Ȼ������80us: ��Ӧ�ź�
3. ֮���������, ��λ����
	bit0 : 50us������, 26-28us������
	bit1 : 50us������, 70us������
4. ������40bit: 8bitʪ����������+8bitʪ��С������
				 +8bit�¶���������+8bit�¶�С������
				 +8bitУ���
***********************************************************************************/


/**********************************************************************************
  * @brief       : 	����GPIO��Ϊ�������ΪDht11��������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void CfgGPIOAsOutputForDht11(void)
{
	GPGCON &= ~(3<<10);
	GPGCON |= (1<<10);
}

/**********************************************************************************
  * @brief       : 	����GPIO��Ϊ�����������Խ���Dht11������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void CfgGPIOAsInputForDht11(void)
{
	GPGCON &= ~(3<<10);
}

/**********************************************************************************
  * @brief       : 	ʹ���ض���ֵ����Dht11
  * @param[in]   : 	val	��Ҫ���õ�����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SetDht11Data(int val)
{
	if (val)
		GPGDAT |= (1<<5);
	else
		GPGDAT &= ~(1<<5);
}

/**********************************************************************************
  * @brief       : 	�õ�Dht11��һλ����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	1��0   ����һ��λ��ֵ
  * @others      : 	��
***********************************************************************************/
static int GetDht11Data(void)
{
	if (GPGDAT & (1<<5))
		return 1;
	else
		return 0;
}

/**********************************************************************************
  * @brief       :	��ʼ��Dht11
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void InitDht11(void)
{
	CfgGPIOAsOutputForDht11();
	SetDht11Data(1);
	mDelay(2000);
}

/**********************************************************************************
  * @brief       : 	����Dht11
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void StartDht11(void)
{
	SetDht11Data(0);
	mDelay(20);
	CfgGPIOAsInputForDht11();
}

/**********************************************************************************
  * @brief       : 	�ȴ�Dht11�Ļ�Ӧ
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	0	�յ�Dht11�Ļ�Ӧ
  					1	δ�յ�Dht11�Ļ�Ӧ
  * @others      : 	��
***********************************************************************************/
static int WaitDht11Ack(void)
{
	uDelay(60);
	return GetDht11Data();
}

/**********************************************************************************
  * @brief       : 	�����ض���ֵ���ȴ�Dht11�Ļ�Ӧ
  * @param[in]   : 	val	��Ҫ�ȴ���ֵ
  					timeout_us	��ʱʱ��(��λΪus)
  * @param[out]  : 	��
  * @return      : 	0	�յ�Dht11�Ļ�Ӧ
  					1	δ�յ�Dht11�Ļ�Ӧ
  * @others      : 	��
***********************************************************************************/
static int WaitDht11ForVal(int val, int timeout_us)
{
	while (timeout_us--)
	{
		if (GetDht11Data() == val)
			return 0; /* ok */
		uDelay(1);
	}
	return -1; /* err */
}

/**********************************************************************************
  * @brief       : 	�õ�Dht11��һ���ֽڵ�����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	data	һ���ֽڵ�Dht11����
  					-1		��ȡһ���ֽڵ�Dht11����ʧ��
  * @others      : 	��
***********************************************************************************/
static int RecvDht11Byte(void)
{
	int i;
	int data = 0;
	
	for (i = 0; i < 8; i++)
	{
		if (WaitDht11ForVal(1, 1000))
		{
			printf("dht11 wait for high data err!\n\r");
			return -1;
		}
		
		uDelay(40);
		data <<= 1;
		
		if (GetDht11Data() == 1)
			data |= 1;
		
		if (WaitDht11ForVal(0, 1000))
		{
			printf("dht11 wait for low data err!\n\r");
			return -1;
		}
	}	
	return data;
}

/**********************************************************************************
  * @brief       : 	��ȡDht11������
  * @param[in]   : 	��
  * @param[out]  : 	hum		ʪ������
  					temp	�¶�����
  * @return      : 	0	��ȡ�ɹ�
  					-1	��ȡʧ��
  * @others      : 	��
***********************************************************************************/
static int ReadDht11(int *hum, int *temp)
{
	unsigned char hum_m, hum_n;
	unsigned char temp_m, temp_n;
	unsigned char check;	
	
	StartDht11();
	
	if (0 != WaitDht11Ack())
	{
		printf("dht11 not ack, err!\n\r");
		return -1;
	}

	if (0 != WaitDht11ForVal(1, 1000))  /* �ȴ�ACK��Ϊ�ߵ�ƽ, ��ʱʱ����1000us */
	{
		printf("dht11 wait for ack high err!\n\r");
		return -1;
	}

	if (0 != WaitDht11ForVal(0, 1000))  /* ���ݽ׶�: �ȴ��͵�ƽ, ��ʱʱ����1000us */
	{
		printf("dht11 wait for data low err!\n\r");
		return -1;
	}

	hum_m  = RecvDht11Byte();
	hum_n  = RecvDht11Byte();
	temp_m = RecvDht11Byte();
	temp_n = RecvDht11Byte();
	check  = RecvDht11Byte();

	CfgGPIOAsOutputForDht11();
	SetDht11Data(1);

	if (hum_m + hum_n + temp_m + temp_n == check)
	{
		*hum  = hum_m;
		*temp = temp_m;
		mDelay(2000);  /* ��ȡ������2S, ���ܶ�̫Ƶ�� */
		return 0;
	}
	else
	{
		printf("dht11 checksum err!\n\r");
		return -1;
	}
}

/**********************************************************************************
  * @brief       : 	Dht11���Ժ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestDht11(void)
{
	int hum, temp;
	char chBuffer[10];
	
	InitDht11();

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(160, 5, "Dht11 TEST", 0xe3170d, 0);

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
		
		if (ReadDht11(&hum, &temp) !=0 )
		{
			printf("\n\rdht11 read err!\n\r");
			InitDht11();

			PrintFbString8x16(105, 120, "dht11 read err! Please return!!!", 0x4169e1, 0);
			mDelay(1000);
			PrintFbString8x16(105, 120, "dht11 read err! Please return!!!", 0xffffff, 0);
		}
		else
		{
			printf("\n\rDHT11 : %d humidity, %d temperature\n\r", hum, temp);

			PrintFbString16x32(120, 75, "DHT11 : ", 0x4169e1, 0);
			Convert(chBuffer, hum, 10);
			PrintFbString8x16(176, 115, chBuffer, 0x4169e1, 0);
			PrintFbString8x16(200, 115, "humidity", 0x4169e1, 0);
			Convert(chBuffer, temp, 10);
			PrintFbString8x16(176, 140, chBuffer, 0x4169e1, 0);
			PrintFbString8x16(200, 140, "temperature", 0x4169e1, 0);

			mDelay(1000);

			PrintFbString16x32(120, 75, "DHT11 : ", 0xffffff, 0);
			Convert(chBuffer, hum, 10);
			PrintFbString8x16(176, 115, chBuffer, 0xffffff, 0);
			PrintFbString8x16(200, 115, "humidity", 0xffffff, 0);
			Convert(chBuffer, temp, 10);
			PrintFbString8x16(176, 140, chBuffer, 0xffffff, 0);
			PrintFbString8x16(200, 140, "temperature", 0xffffff, 0);		
		}	
	}
}

