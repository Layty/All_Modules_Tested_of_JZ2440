
#include "s3c2440_soc.h"
#include "nand_flash.h"
#include "my_printf.h"
#include "string_utils.h"


#define TXD0READY   (1<<2)

void InitNand(void)
{
#define TACLS   0
#define TWRPH0  1
#define TWRPH1  0
	/* ����ʱ�� */
	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* ʹ��NAND Flash������, ��ʼ��ECC, ��ֹƬѡ */
	NFCONT = (1<<4)|(1<<1)|(1<<0);	
}

void SelectNand(void)
{
	NFCONT &= ~(1<<1);	
}

void DeselectNand(void)
{
	NFCONT |= (1<<1);	
}

void NandCmd(unsigned char cmd)
{
	volatile int i;
	NFCCMD = cmd;
	for (i = 0; i < 10; i++);
}

void NandAddrByte(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0; i<10; i++);
}


void NandAddr(unsigned int addr)
{
	unsigned int iCol  = addr % 2048;
	unsigned int iPage = addr / 2048;
	volatile int i;

	NFADDR = iCol & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (iCol >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	
	NFADDR  = iPage & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (iPage >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (iPage >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void NandPage(unsigned int page)
{
	volatile int i;
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void NandCol(unsigned int col)
{
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
}


void NandWaitReady(void)
{
	while (!(NFSTAT & 1));
}

unsigned char NandData(void)
{
	return NFDATA;
}

int NandBad(unsigned int addr)
{
	unsigned int col  = 2048;
	unsigned int page = addr / 2048;
	unsigned char val;

	/* 1. ѡ�� */
	SelectNand();
	
	/* 2. ����������00h */
	NandCmd(0x00);
	
	/* 3. ������ַ(��5������) */
	NandCol(col);
	NandPage(page);
	
	/* 4. ����������30h */
	NandCmd(0x30);
	
	/* 5. �ж�״̬ */
	NandWaitReady();

	/* 6. ������ */
	val = NandData();
	
	/* 7. ȡ��ѡ�� */		
	DeselectNand();


	if (val != 0xff)
		return 1;  /* bad blcok */
	else
		return 0;
}


void ReadNand(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int iCol = addr % 2048;
	int i = 0;
		
	while (i < len)
	{

		if (!(addr & 0x1FFFF) && NandBad(addr)) /* һ��blockֻ�ж�һ�� */
		{
			addr += (128*1024);  /* ������ǰblock */
			continue;
		}

		/* 1. ѡ�� */
		SelectNand();
		
		
		/* 2. ����������00h */
		NandCmd(0x00);

		/* 3. ������ַ(��5������) */
		NandAddr(addr);

		/* 4. ����������30h */
		NandCmd(0x30);

		/* 5. �ж�״̬ */
		NandWaitReady();

		/* 6. ������ */
		for (; (iCol < 2048) && (i < len); iCol++)
		{
			buf[i] = NandData();
			i++;
			addr++;
		}
		
		iCol = 0;


		/* 7. ȡ��ѡ�� */		
		DeselectNand();
		
	}
}

void NandWData(unsigned char val)
{
	NFDATA = val;
}



void PrintNandChipID(void)
{ 
	unsigned char buf[5]={0};
	
	SelectNand(); 
	NandCmd(0x90);
	NandAddrByte(0x00);

	buf[0] = NandData();
	buf[1] = NandData();	
	buf[2] = NandData();
	buf[3] = NandData();
	buf[4] = NandData();	
	DeselectNand(); 	

	printf("maker   id  = 0x%x\n\r",buf[0]);
	printf("device  id  = 0x%x\n\r",buf[1]);	
	printf("3rd byte    = 0x%x\n\r",buf[2]);		
	printf("4th byte    = 0x%x\n\r",buf[3]);			
	printf("page  size  = %d kb\n\r",1  <<  (buf[3] & 0x03));	
	printf("block size  = %d kb\n\r",64 << ((buf[3] >> 4) & 0x03));	
	printf("5th byte    = 0x%x\n\r",buf[4]);

	
}

int NandErase(unsigned int addr, unsigned int len)
{
	int page = addr / 2048;

	if (addr & (0x1FFFF))
	{
		printf("nand_erase err, addr is not block align\n\r");
		return -1;
	}
	
	if (len & (0x1FFFF))
	{
		printf("nand_erase err, len is not block align\n\r");
		return -1;
	}
	
	SelectNand(); 

	while (1)
	{
		page = addr / 2048;
		
		NandCmd(0x60);
		
		/* row/page addr */
		NandAddrByte(page & 0xff);
		NandAddrByte((page>>8) & 0xff);
		NandAddrByte((page>>16) & 0xff);

		NandCmd(0xD0);

		NandWaitReady();

		len -= (128*1024);
		if (len == 0)
			break;
		addr += (128*1024);
	}
	
	DeselectNand(); 	
	return 0;
}

void NandWrite(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int page = addr / 2048;
	int col  = addr & (2048 - 1);
	int i = 0;

	SelectNand(); 

	while (1)
	{
		NandCmd(0x80);

		/* ������ַ */
		/* col addr */
		NandAddrByte(col & 0xff);
		NandAddrByte((col>>8) & 0xff);
		
		/* row/page addr */
		NandAddrByte(page & 0xff);
		NandAddrByte((page>>8) & 0xff);
		NandAddrByte((page>>16) & 0xff);

		/* �������� */
		for (; (col < 2048) && (i < len); )
		{
			NandWData(buf[i++]);
		}
		NandCmd(0x10);
		NandWaitReady();

		if (i == len)
			break;
		else
		{
			/* ��ʼ��һ��ѭ��page */
			col = 0;
			page++;
		}
		
	}
	
	DeselectNand(); 	
}



void DoReadNandFlash(void)
{
	unsigned int addr;
	volatile unsigned char *p;
	int i, j;
	unsigned char c;
	unsigned char str[16];
	unsigned char buf[64];
	
	/* ��õ�ַ */
	printf("Enter the address to read: ");
	addr = get_uint();

	ReadNand(addr, buf, 64);
	p = (volatile unsigned char *)buf;

	printf("Data : \n\r");
	/* ���ȹ̶�Ϊ64 */
	for (i = 0; i < 4; i++)
	{
		/* ÿ�д�ӡ16������ */
		for (j = 0; j < 16; j++)
		{
			/* �ȴ�ӡ��ֵ */
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* ���ӡ�ַ� */
			if (str[j] < 0x20 || str[j] > 0x7e)  /* �������ַ� */
				PutChar('.');
			else
				PutChar(str[j]);
		}
		printf("\n\r");
	}
}

void DoEraseNandFlash(void)
{
	unsigned int addr;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	NandErase(addr, 128*1024);
}


void DoWriteNandFlash(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* ��õ�ַ */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)str);

	printf("writing ...\n\r");
	NandWrite(addr, str, strlen((char *)str)+1);

}




