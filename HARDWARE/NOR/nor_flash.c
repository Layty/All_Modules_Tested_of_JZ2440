#include "my_printf.h"
#include "string_utils.h"
#include "nor_flash.h"


#define NOR_FLASH_BASE  0  /* jz2440, nor-->cs0, base addr = 0 */


/* ����:   55H 98 
 * ������: ��(0 + (0x55)<<1)д��0x98
 */
void NorWriteWord(unsigned int base, unsigned int offset, unsigned int val)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	*vpnTemp = val;
}

/* offset�ǻ���NOR�ĽǶȿ��� */
void NorCmd(unsigned int offset, unsigned int cmd)
{
	NorWriteWord(NOR_FLASH_BASE, offset, cmd);
}

unsigned int NorReadWord(unsigned int base, unsigned int offset)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	return *vpnTemp;
}

unsigned int NorDat(unsigned int offset)
{
	return NorReadWord(NOR_FLASH_BASE, offset);
}


void NorWaitReady(unsigned int addr)
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


/* ����NOR FLASH��CFIģʽ
 * ��ȡ������Ϣ
 */
void DoScanNorFlash(void)
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

void DoEraseNorFlash(void)
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

void DoWriteNorFlash(void)
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

void DoReadNorFlash(void)
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




