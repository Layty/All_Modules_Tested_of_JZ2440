/****************************************************************************************************
  * @brief      : 	JZ2440v2�������¶ȴ�����DS18B20����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"

/********************************NOTE**************************************************

ʹ��GPG6����ds18b20��DATA����

******************************************************************************************/

/* rom commands */
#define SEARCH_ROM    0xF0
#define READ_ROM      0x33
#define MATCH_ROM     0x55
#define SKIP_ROM      0xCC
#define ALARM_ROM     0xEC

/* functions commands */
#define CONVERT_TEAMPERATURE 0x44
#define WRITE_SCRATCHPAD     0x4E
#define READ_SCRATCHPAD      0xBE
#define COPY_SCRATCHPAD      0x48
#define RECALL_EEPROM        0xB8
#define READ_POWER_SUPPLY    0xB4


/**********************************************************************************
  * @brief       : 	����GPIO��Ϊ�������ΪDs18b20��������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void CfgGPIOAsOutputForDs18b20(void)
{
	GPGCON &= ~(3<<12);
	GPGCON |= (1<<12);
}

/**********************************************************************************
  * @brief       : 	����GPIO��Ϊ�����������Խ���Ds18b20������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void CfgGPIOAsInputForDs18b20(void)
{
	GPGCON &= ~(3<<12);
}

/**********************************************************************************
  * @brief       : 	ʹ���ض���ֵ����Ds18b20
  * @param[in]   : 	val	��Ҫ���õ�����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SetDs18b20Data(int val)
{
	if (val)
		GPGDAT |= (1<<6);
	else
		GPGDAT &= ~(1<<6);
}

/**********************************************************************************
  * @brief       : 	�õ�Ds18b20��һλ����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	1��0   ����һ��λ��ֵ
  * @others      : 	��
***********************************************************************************/
static int GetDs18b20Data(void)
{
	if (GPGDAT & (1<<6))
		return 1;
	else
		return 0;
}

/**********************************************************************************
  * @brief       : 	ʹ���ض���ֵ����Ds18b20����ʱ�ض���ʱ��
  * @param[in]   : 	val	��Ҫ���õ�����
  					us	������������Ҫ��ʱ��ʱ��(��λΪus)
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SetDs18b20DataValForTime(int val, int us)
{
	CfgGPIOAsOutputForDs18b20();
	SetDs18b20Data(val);
	uDelay(us);
}

/**********************************************************************************
  * @brief       : 	�ͷ�Ds18b20
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void ReleaseDs18b20Data(void)
{
	CfgGPIOAsInputForDs18b20();
}

/**********************************************************************************
  * @brief       : 	��ʼ��Ds18b20
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	0	��ʼ���ɹ�
  					1	��ʼ��ʧ��
  * @others      : 	��
***********************************************************************************/
static int InitializationDs18b20(void)
{
	int val;
	
	SetDs18b20DataValForTime(0, 500);
	ReleaseDs18b20Data();
	uDelay(80);

	val = GetDs18b20Data();
	uDelay(250);
	return val;
}

/**********************************************************************************
  * @brief       : 	��Ds18b20д��һ��λ������
  * @param[in]   : 	val	д���һ��λ������	ȡֵ��Χ{0,1}
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void WriteDs18b20Bit(int val)
{
	if (0 == val)
	{
		SetDs18b20DataValForTime(0, 60);		
		ReleaseDs18b20Data();
		uDelay(2);
	}
	else
	{
		SetDs18b20DataValForTime(0, 2);		
		ReleaseDs18b20Data();
		uDelay(60);
	}
}

/**********************************************************************************
  * @brief       : 	��Ds18b20��ȡһ��λ������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	val	��ȡ��һ��λ������	ȡֵ��Χ{0,1}
  * @others      : 	��
***********************************************************************************/
static int ReadDs18b20Bit(void)
{
	int val;
	
	SetDs18b20DataValForTime(0, 2);		
	ReleaseDs18b20Data();
	uDelay(10);
	val = GetDs18b20Data();
	uDelay(50);
	return val;
}

/**********************************************************************************
  * @brief       : 	��Ds18b20д��һ���ֽڵ�����
  * @param[in]   : 	data	д���һ���ֽ�����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void WriteDs18b20Byte(unsigned char data)
{
	int i;
	for (i = 0; i < 8; i++)
	{

		WriteDs18b20Bit(data & (1<<i));
	}
}

/**********************************************************************************
  * @brief       : 	��Ds18b20��ȡһ���ֽڵ�����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	data	��ȡ��һ���ֽڵ�����
  * @others      : 	��
***********************************************************************************/
static unsigned char ReadDs18b20Byte(void)
{
	int i;
	unsigned char data = 0;

	for (i = 0; i < 8; i++)
	{
		if (ReadDs18b20Bit() == 1)
			data |= (1<<i);
	}

	return data;
}


/**********************************************************************************
  * @brief       : 	��Ds18b20д��һ��Rom����
  * @param[in]   : 	cmd		��Ҫд���Rom����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void WriteDs18b20RomCmd(unsigned char cmd)
{
	WriteDs18b20Byte(cmd);
}

/**********************************************************************************
  * @brief       : 	��Ds18b20д��һ��Function����
  * @param[in]   : 	cmd		��Ҫд���Function����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void WriteDs18b20FunctionCmd(unsigned char cmd)
{
	WriteDs18b20Byte(cmd);
}

/**********************************************************************************
  * @brief       : 	��ȡDs18b20��Rom�е�����
  * @param[in]   : 	��
  * @param[out]  : 	rom		��Ŷ�ȡ������
  * @return      : 	0	��ȡ�ɹ�
  					1	��ȡʧ��
  * @others      : 	��
***********************************************************************************/
static int ReadDs18b20Rom(unsigned char rom[])
{
	int i;
	
	if (InitializationDs18b20() != 0)
	{
		printf("ds18b20_initialization err!\n\r");
		return -1;
	}

	WriteDs18b20RomCmd(READ_ROM);
	
	for (i = 0; i < 8; i++)
	{
		rom[i] = ReadDs18b20Byte();
	}

	return 0;
}

/**********************************************************************************
  * @brief       : 	��DS18B20����ʱ�ȴ������
  * @param[in]   : 	time_out 	��ʱʱ��
  * @param[out]  : 	��
  * @return      : 	0	�յ�DS18B20�Ļ�Ӧ�ź�
  					-1	δ�յ�DS18B20�Ļ�Ӧ�ź�
  * @others      : 	��
***********************************************************************************/
static int WaitDs18b20WhenProcessing(int timeout_us)
{
	while (timeout_us--)
	{
		if (ReadDs18b20Bit() == 1)
			return 0;  /* ok */
		uDelay(1);
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	����DS18B20ת��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	0	�����ɹ�
  					-1	����ʧ��
  * @others      : 	��
***********************************************************************************/
static int StartDs18b20Convert(void)
{
	if (InitializationDs18b20() != 0)
	{
		printf("ds18b20_initialization err!\n\r");
		return -1;
	}

	WriteDs18b20RomCmd(SKIP_ROM);
	WriteDs18b20FunctionCmd(CONVERT_TEAMPERATURE);

	/* �ȴ�/�ж�ת���ɹ� */
	if (0 != WaitDs18b20WhenProcessing(1000000))
	{
		printf("ds18b20_wait_when_processing err!\n\r");
		return -1;
	}

	return 0;	
}

/**********************************************************************************
  * @brief       : 	��ȡDS18B20��ram����
  * @param[in]   : 	��
  * @param[out]  : 	ram		���ڴ�Ŷ�ȡ��ram������
  * @return      : 	0	��ȡ�ɹ�
  					-1	��ȡʧ��
  * @others      : 	��
***********************************************************************************/
static int ReadDs18b20Ram(unsigned char ram[])
{
	int i;
	
	if (InitializationDs18b20() != 0)
	{
		printf("ds18b20_initialization err!\n\r");
		return -1;
	}

	WriteDs18b20RomCmd(SKIP_ROM);
	WriteDs18b20FunctionCmd(READ_SCRATCHPAD);

	for (i = 0; i < 9; i++)
	{
		ram[i] = ReadDs18b20Byte();
	}

	return 0;
}


/**********************************************************************************
  * @brief       : 	��ȡDS18B20�е��¶�����
  * @param[in]   : 	��
  * @param[out]  : 	temp	���ڴ�Ŷ�ȡ���¶�����
  * @return      : 	0	��ȡ�ɹ�
  					err	��ȡʧ��
  * @others      : 	��
***********************************************************************************/
static int ReadDs18b20Temperature(double *temp)
{
	int err;
	unsigned char ram[9];
	double val[] = {0.0625, 0.125, 0.25, 0.5, 1, 2, 4, 8, 16, 32, 64};
	double sum = 0;
	int i;
	
	err = StartDs18b20Convert();
	if (err)
		return err;

	err = ReadDs18b20Ram(ram);
	if (err)
		return err;

	/* �����¶� */

	/* ���жϾ��� */
	if ((ram[4] & (3<<5)) == 0) /* ����: 9bit */
		i = 3;
	else if ((ram[4] & (3<<5)) == (1<<5)) /* ����: 10bit */
		i = 2;
	else if ((ram[4] & (3<<5)) == (2<<5)) /* ����: 11bit */
		i = 1;
	else
		/* ������ 12 bit */
		i = 0;
	
	for (; i < 8; i++)
	{
		if (ram[0] & (1<<i))
			sum += val[i];
	}

	for (i = 0; i < 3; i++)
	{
		if (ram[1] & (1<<i))
			sum += val[8+i];
	}

	if (ram[1] & (1<<3))
		sum = 0 - sum;

	*temp = sum;
	return 0;
}

/**********************************************************************************
  * @brief       : 	��ʼ��DS18B20״̬
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void InitDs18b20State(void)
{
	ReleaseDs18b20Data();
}

/**********************************************************************************
  * @brief       : 	����DS18B20�¶ȴ�����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestDs18b20(void)
{
	double temp = 0;
	int m = 0,n = 0;
	char chBuffer[10];

	InitDs18b20State();

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(140, 5, "Ds18b20 TEST", 0xe3170d, 0);

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
		
		if (0 == ReadDs18b20Temperature(&temp))
		{

			PrintFbString8x16(130, 110, "ds18b20 temperature: ", 0xffffff, 0);
			Convert(chBuffer, m, 10);
			PrintFbString8x16(298, 110,chBuffer, 0xffffff, 0);
			PrintFbString8x16(314, 110, ".", 0xffffff, 0);
			Convert(chBuffer, n, 10);
			PrintFbString8x16(322, 110,chBuffer, 0xffffff, 0);
			
			m = (int)temp;	/* 3.01, m = 3 */
			temp = temp - m;	/* С������: 0.01 */
			n = temp * 10000;  /* 10 */
			
			/* �ڴ����ϴ�ӡ */
			printf("ds18b20 temperature: %d.%04d\n\r", m, n);  /* 3.010v */


			PrintFbString8x16(130, 110, "ds18b20 temperature: ", 0x4169e1, 0);
			Convert(chBuffer, m, 10);
			PrintFbString8x16(298, 110,chBuffer, 0x4169e1, 0);
			PrintFbString8x16(314, 110, ".", 0x4169e1, 0);
			Convert(chBuffer, n, 10);
			PrintFbString8x16(322, 110,chBuffer, 0x4169e1, 0);

			mDelay(1500);
		}
	}
}

