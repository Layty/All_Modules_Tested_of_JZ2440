/****************************************************************************************************
  * @brief      : 	JZ2440v2������nor flash����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "my_printf.h"
#include "string_utils.h"
#include "nor_flash.h"
#include "function.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"
#include "timer.h"

#define NOR_FLASH_BASE  0  /* jz2440, nor-->cs0, base addr = 0 */

/**********************************************************************************
  * @brief       : 	��nor flashд��һ����
  * @param[in]   : 	base	nor flash��ַ
  					offset	��ַƫ����
  					val		��д�����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	����:   55H 98 
					������: ��(0 + (0x55)<<1)д��0x98
***********************************************************************************/
static void NorWriteWord(unsigned int base, unsigned int offset, unsigned int val)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	*vpnTemp = val;
}

/**********************************************************************************
  * @brief       : 	��nor flashд��һ������
  * @param[in]   : 	offset	��ַƫ����
  					cmd		��д�������
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	offset�ǻ���NOR�ĽǶȿ���
***********************************************************************************/
static void NorCmd(unsigned int offset, unsigned int cmd)
{
	NorWriteWord(NOR_FLASH_BASE, offset, cmd);
}

/**********************************************************************************
  * @brief       : 	��nor flash��ȡһ���ֵ�����
  * @param[in]   : 	base	nor flash��ַ
  					offset	��ַƫ����
  * @param[out]  : 	��
  * @return      : 	����һ���ֵ�����
  * @others      : 	��
***********************************************************************************/
static unsigned int NorReadWord(unsigned int base, unsigned int offset)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	return *vpnTemp;
}

/**********************************************************************************
  * @brief       : 	��nor flash��ȡ����
  * @param[in]   : 	offset	��ַƫ����
  * @param[out]  : 	��
  * @return      : 	���ص�����
  * @others      : 	��
***********************************************************************************/
static unsigned int NorDat(unsigned int offset)
{
	return NorReadWord(NOR_FLASH_BASE, offset);
}

/**********************************************************************************
  * @brief       : 	�ȴ�nor flash����
  * @param[in]   : 	addr	�������ĵ�ַ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void NorWaitReady(unsigned int addr)
{
	unsigned int iVal;
	unsigned int iPre;

	iPre = NorDat(addr>>1);
	iVal = NorDat(addr>>1);
	while ((iVal & (1<<6)) != (iPre & (1<<6)))
	{
		iPre = iVal;
		iVal = NorDat(addr>>1);		
	}
}

/**********************************************************************************
  * @brief		 :	����nor flash��ɨ�躯��
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	����NOR FLASH��CFIģʽ
    				��ȡ������Ϣ
***********************************************************************************/
static void DoScanNorFlash(void)
{
	char szStr[4];
	unsigned int uiSize;
	int iRegions;
	int i;
	int iRegionInfoBase;
	int iBlockAddr, iBlocks, iBlockSize;
	int j;
	int iCnt;

	int iVendor, iDevice;
	
	/* ��ӡ����ID���豸ID */
	NorCmd(0x555, 0xaa);    /* ���� */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0x90);    /* read id */
	iVendor = NorDat(0);
	iDevice = NorDat(1);
	NorCmd(0, 0xf0);        /* reset */
	
	NorCmd(0x55, 0x98);  /* ����cfiģʽ */

	szStr[0] = NorDat(0x10);
	szStr[1] = NorDat(0x11);
	szStr[2] = NorDat(0x12);
	szStr[3] = '\0';
	printf("str = %s\n\r", szStr);

	/* ��ӡ���� */
	uiSize = 1<<(NorDat(0x27));
	printf("vendor id = 0x%x, device id = 0x%x, nor size = 0x%x, %dM\n\r", iVendor, iDevice, uiSize, uiSize/(1024*1024));

	/* ��ӡ������������ʼ��ַ */
	/* ���ʽ���:
	 *    erase block region : ���溬��1������block, ���ǵĴ�Сһ��
	 * һ��nor flash����1������region
	 * һ��region����1������block(����)
	 * Erase block region information:
	 *    ǰ2�ֽ�+1    : ��ʾ��region�ж��ٸ�block 
	 *    ��2�ֽ�*256  : ��ʾblock�Ĵ�С
	 */

	iRegions = NorDat(0x2c);
	iRegionInfoBase = 0x2d;
	iBlockAddr = 0;
	printf("Block/Sector start Address:\n\r");
	iCnt = 0;
	for (i = 0; i < iRegions; i++)
	{
		iBlocks = 1 + NorDat(iRegionInfoBase) + (NorDat(iRegionInfoBase+1)<<8);
		iBlockSize = 256 * (NorDat(iRegionInfoBase+2) + (NorDat(iRegionInfoBase+3)<<8));
		iRegionInfoBase += 4;

		//printf("\n\rregion %d, blocks = %d, block_size = 0x%x, block_addr = 0x%x\n\r", i, blocks, block_size, block_addr);

		for (j = 0; j < iBlocks; j++)
		{
			/* ��ӡÿ��block����ʼ��ַ */
			//printf("0x%08x ", block_addr);
			printHex(iBlockAddr);
			PutChar(' ');
			iCnt++;
			iBlockAddr += iBlockSize;
			if (iCnt % 5 == 0)
				printf("\n\r");
		}
	}
	printf("\n\r");
	/* �˳�CFIģʽ */
	NorCmd(0, 0xf0);
}

/**********************************************************************************
  * @brief		 :	����nor flash�ò�������
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***********************************************************************************/
static void DoEraseNorFlash(void)
{
	unsigned int iAddr;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	iAddr = get_uint();

	printf("erasing ...\n\r");
	NorCmd(0x555, 0xaa);    /* ���� */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0x80);	 /* erase sector */
	
	NorCmd(0x555, 0xaa);    /* ���� */
	NorCmd(0x2aa, 0x55); 
	NorCmd(iAddr>>1, 0x30);	 /* ����������ַ */
	NorWaitReady(iAddr);
}

/**********************************************************************************
  * @brief		 :	����nor flash��д�뺯��
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***********************************************************************************/
static void DoWriteNorFlash(void)
{
	unsigned int iAddr;
	unsigned char szStr[100];
	int i, j;
	unsigned int ival;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to write: ");
	iAddr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)szStr);

	printf("writing ...\n\r");

	/* str[0],str[1]==>16bit 
	 * str[2],str[3]==>16bit 
	 */
	i = 0;
	j = 1;
	while (szStr[i] && szStr[j])
	{
		ival = szStr[i] + (szStr[j]<<8);
		
		/* ��д */
		NorCmd(0x555, 0xaa);	 /* ���� */
		NorCmd(0x2aa, 0x55); 
		NorCmd(0x555, 0xa0);	 /* program */
		NorCmd(iAddr>>1, ival);
		/* �ȴ���д��� : ������, Q6�ޱ仯ʱ��ʾ���� */
		NorWaitReady(iAddr);

		i += 2;
		j += 2;
		iAddr += 2;
	}

	ival = szStr[i];
	/* ��д */
	NorCmd(0x555, 0xaa);	 /* ���� */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0xa0);	 /* program */
	NorCmd(iAddr>>1, ival);
	/* �ȴ���д��� : ������, Q6�ޱ仯ʱ��ʾ���� */
	NorWaitReady(iAddr);
}

/**********************************************************************************
  * @brief		 :	����nor flash�ö�ȡ����
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***********************************************************************************/
static void DoReadNorFlash(void)
{
	unsigned int iAddr;
	volatile unsigned char *vpchTemp;
	int i, j;
	unsigned char chTemp;
	unsigned char szStr[16];
	
	/* ��õ�ַ */
	printf("Enter the address to read: ");
	iAddr = get_uint();

	vpchTemp = (volatile unsigned char *)iAddr;

	printf("Data : \n\r");
	/* ���ȹ̶�Ϊ64 */
	for (i = 0; i < 4; i++)
	{
		/* ÿ�д�ӡ16������ */
		for (j = 0; j < 16; j++)
		{
			/* �ȴ�ӡ��ֵ */
			chTemp = *vpchTemp++;
			szStr[j] = chTemp;
			printf("%02x ", chTemp);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* ���ӡ�ַ� */
			if (szStr[j] < 0x20 || szStr[j] > 0x7e)  /* �������ַ� */
				PutChar('.');
			else
				PutChar(szStr[j]);
		}
		printf("\n\r");
	}
}

/**********************************************************************************
  * @brief		 :	����nor flash���ܺ���
  * @param[in]	 :	��
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***********************************************************************************/
void TestNorFlash(void)
{
	char c;
	unsigned int uiSize;
	int iVendor, iDevice;
	unsigned char result[33];

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(176, 5, "NOR TEST", 0xe3170d, 0);

	if(!isBootFromNorFlash())
	{
		PrintFbString8x16(40, 120, "This start is nand flash start, can not test nor flash! ! !", 0x4169e1, 1);
		Delay(3500000);
		PrintFbString8x16(40, 120, "This start is nand flash start, can not test nor flash! ! !", 0xffffff, 1);

		MainPage();

		return;
	}
	
	DisplayReturnButton();


	/* ��ӡ����ID���豸ID */
	NorCmd(0x555, 0xaa);    /* ���� */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0x90);    /* read id */
	iVendor = NorDat(0);
	iDevice = NorDat(1);
	NorCmd(0, 0xf0);        /* reset */
	
	NorCmd(0x55, 0x98);  /* ����cfiģʽ */

	/* ��ȡ���� */
	uiSize = 1<<(NorDat(0x27));
	/* �˳�CFIģʽ */
	NorCmd(0, 0xf0);
	
	printf("vendor id = 0x%x, device id = 0x%x, nor size = 0x%x, %dM\n\r", iVendor, iDevice, uiSize, uiSize/(1024*1024));

	Convert(result,(unsigned int)iVendor,16);
	PrintFbString8x16(20, 70, "vendor id  =  0x", 0x0, 0);
	PrintFbString8x16(148, 70, result, 0x0, 0);

	Convert(result,(unsigned int)iDevice,16);
	PrintFbString8x16(20, 90, "device id  =  0x", 0x0, 0);
	PrintFbString8x16(148, 90, result, 0x0, 0);
	
	Convert(result,(unsigned int)uiSize,16);
	PrintFbString8x16(20, 110, "nor  size  =  0x", 0x0, 0);
	PrintFbString8x16(148, 110, result, 0x0, 0);
	PrintFbString8x16(212, 110, "byte, ", 0x0, 0);
	
	Convert(result,(unsigned int)(uiSize/(1024*1024)),10);
	PrintFbString8x16(20, 130, "nor  size  =    ", 0x0, 0);
	PrintFbString8x16(148, 130, result, 0x0, 0);
	PrintFbString8x16(210, 130, "M", 0x0, 0);

	PrintFbString8x16(40, 240, "If you want to test the nor read-write function, please connect to the PC serial port tool and re-enter this page.", 0xff0000, 0);

	PrintFbString16x32(288, 40, "NOTE:", 0x0b1746, 0);
	PrintFbString8x16(320, 85, 	"Please carefully per", 0x4169e1, 0);
	PrintFbString8x16(288, 110, "form the erase and write", 0x4169e1, 0);
	PrintFbString8x16(288, 130, "operations, otherwise th", 0x4169e1, 0);
	PrintFbString8x16(288, 155, "e program may die!", 0x4169e1, 0);

	/* ��ӡ�˵�, ������ѡ��������� */
	printf("\n\r");
	printf("       Nor Flash Test\n\r");		
	printf("[s] Scan nor flash\n\r");
	printf("[e] Erase nor flash\n\r");
	printf("[w] Write nor flash\n\r");
	printf("[r] Read nor flash\n\r");
	printf("[q] quit\n\r");
	printf("Enter selection: ");
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}

		c = GetChar();
		printf("%c\n\r", c);
		if(c == 0)
		{
			break;
		}
		

		/* ��������:
		 * 1. ʶ��nor flash
		 * 2. ����nor flashĳ������
		 * 3. ��дĳ����ַ
		 * 4. ��ĳ����ַ
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				MainPage();
				return;
				break;
				
			case 's':
			case 'S':
				DoScanNorFlash();
				printf("\n\r");
				printf("       Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'e':
			case 'E':
				DoEraseNorFlash();
				printf("\n\r");
				printf("	   Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'w':
			case 'W':
				DoWriteNorFlash();
				printf("\n\r");
				printf("	   Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'r':
			case 'R':
				DoReadNorFlash();
				printf("\n\r");
				printf("	   Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;
			default:
				break;
		}
	}

}

