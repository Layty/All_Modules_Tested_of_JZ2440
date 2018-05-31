#include "s3c2440_soc.h"
#include "nand_flash.h"
#include "my_printf.h"
#include "string_utils.h"
#include "function.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"

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

void Convert(unsigned char buf[],unsigned int c, int radix)
{
	unsigned int iTemp;
	unsigned char chBuf[33];
	int i = 0;
	int j;
	
	while(c != 0)  
    {  
    	if(radix == 16)
		{
	        iTemp=c%16;
	        if(iTemp>=0 && iTemp<10)  
	        {  
	            chBuf[i]=iTemp +'0';  
	            i++;  
	        }  
	        else  
	        {  
	            chBuf[i]=iTemp+'A'-10;       
	            i++;  
	        }	        	
	        c = c/16;	        
        }
        else if(radix == 10)
        {
			iTemp=c%10;  
	        if(iTemp>=0 && iTemp<10)  
	        {  
	            chBuf[i]=iTemp +'0';  
	            i++;  
	        }	        
	        c = c/10;
        }
    }

	for(j=0;j<i;j++)
		buf[j]=chBuf[i-j-1];
    
    buf[i]='\0'; 
}

void TestNandFlash(void)
{
	char c;
	unsigned int iTemp;
	unsigned char chBuf[5]={0};
	unsigned char result[33];

	InitNand();
	
	SelectNand(); 
	NandCmd(0x90);
	NandAddrByte(0x00);

	chBuf[0] = NandData();
	chBuf[1] = NandData();	
	chBuf[2] = NandData();
	chBuf[3] = NandData();
	chBuf[4] = NandData();	
	DeselectNand();

	/* ���� */
	ClearScreen(0xffffff);

	/* ��ʾ������ʾ */
	PrintFbString16x32(176, 5, "NAND TEST", 0xe3170d, 0);
	DisplayReturnButton();

    Convert(result,(unsigned int)chBuf[0],16);
	PrintFbString8x16(20, 40, "maker   id  = 0x", 0x0, 0);
	PrintFbString8x16(148, 40, result, 0x0, 0);

	Convert(result,(unsigned int)chBuf[1],16);
	PrintFbString8x16(20, 60, "device  id  = 0x", 0x0, 0);
	PrintFbString8x16(148, 60, result, 0x0, 0);
	
	Convert(result,(unsigned int)chBuf[2],16);
	PrintFbString8x16(20, 80, "3rd byte    = 0x", 0x0, 0);
	PrintFbString8x16(148, 80, result, 0x0, 0);

	Convert(result,(unsigned int)chBuf[3],16);
	PrintFbString8x16(20, 100, "4th byte    = 0x", 0x0, 0);
	PrintFbString8x16(148, 100, result, 0x0, 0);

	iTemp = (unsigned int)(1 << (chBuf[3] & 0x03));
	Convert(result,iTemp,10);
	PrintFbString8x16(20, 120, "page  size  = ", 0x0, 0);
	PrintFbString8x16(132, 120, result, 0x0, 0);
	PrintFbString8x16(180, 120, "kb", 0x0, 0);

	iTemp = (unsigned int)(64 << ((chBuf[3] >> 4) & 0x03));
	Convert(result,iTemp,10);
	PrintFbString8x16(20, 140, "block size  = ", 0x0, 0);
	PrintFbString8x16(132, 140, result, 0x0, 0);
	PrintFbString8x16(180, 140, "kb", 0x0, 0);
	
	Convert(result,(unsigned int)chBuf[4],16);
	PrintFbString8x16(20, 160, "5th byte    = 0x", 0x0, 0);
	PrintFbString8x16(148, 160, result, 0x0, 0);

	PrintFbString8x16(40, 240, "If you want to test the nand read-write function, please connect to the PC serial port tool and re-enter this page.", 0xff0000, 0);

	PrintFbString16x32(288, 40, "NOTE:", 0x0b1746, 0);
	PrintFbString8x16(320, 85, 	"Please carefully per", 0x4169e1, 0);
	PrintFbString8x16(288, 110, "form the erase and write", 0x4169e1, 0);
	PrintFbString8x16(288, 130, "operations, otherwise th", 0x4169e1, 0);
	PrintFbString8x16(288, 155, "e program may die!", 0x4169e1, 0);

	/* ��ӡ�˵�, ������ѡ��������� */
	printf("\n\r");
	printf("            Nand Flash Test\n\r");
	printf("[s] Scan nand flash\n\r");
	printf("[e] Erase nand flash\n\r");
	printf("[w] Write nand flash\n\r");
	printf("[r] Read nand flash\n\r");
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
		if(c == 0)
		{
			break;
		}
		printf("%c\n\r", c);

		/* ��������:
		 * 1. ʶ��nand flash
		 * 2. ����nand flashĳ������
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
				PrintNandChipID();
				printf("\n\r");
				printf("            Nand Flash Test\n\r");
				printf("[s] Scan nand flash\n\r");
				printf("[e] Erase nand flash\n\r");
				printf("[w] Write nand flash\n\r");
				printf("[r] Read nand flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'e':
			case 'E':
				DoEraseNandFlash();
				printf("\n\r");
				printf("            Nand Flash Test\n\r");
				printf("[s] Scan nand flash\n\r");
				printf("[e] Erase nand flash\n\r");
				printf("[w] Write nand flash\n\r");
				printf("[r] Read nand flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'w':
			case 'W':
				DoWriteNandFlash();
				printf("\n\r");
				printf("            Nand Flash Test\n\r");
				printf("[s] Scan nand flash\n\r");
				printf("[e] Erase nand flash\n\r");
				printf("[w] Write nand flash\n\r");
				printf("[r] Read nand flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'r':
			case 'R':
				DoReadNandFlash();
				printf("\n\r");
				printf("            Nand Flash Test\n\r");
				printf("[s] Scan nand flash\n\r");
				printf("[e] Erase nand flash\n\r");
				printf("[w] Write nand flash\n\r");
				printf("[r] Read nand flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;
			default:
				break;
		}
	}
}





