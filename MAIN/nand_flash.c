
#include "s3c2440_soc.h"
#include "nand_flash.h"
#include "my_printf.h"
#include "string_utils.h"


#define TXD0READY   (1<<2)

void nand_init(void)
{
#define TACLS   0
#define TWRPH0  1
#define TWRPH1  0
	/* ����ʱ�� */
	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* ʹ��NAND Flash������, ��ʼ��ECC, ��ֹƬѡ */
	NFCONT = (1<<4)|(1<<1)|(1<<0);	
}

void nand_select(void)
{
	NFCONT &= ~(1<<1);	
}

void nand_deselect(void)
{
	NFCONT |= (1<<1);	
}

void nand_cmd(unsigned char cmd)
{
	volatile int i;
	NFCCMD = cmd;
	for (i = 0; i < 10; i++);
}

void nand_addr_byte(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0; i<10; i++);
}


void nand_addr(unsigned int addr)
{
	unsigned int col  = addr % 2048;
	unsigned int page = addr / 2048;
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void nand_page(unsigned int page)
{
	volatile int i;
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void nand_col(unsigned int col)
{
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
}


void nand_wait_ready(void)
{
	while (!(NFSTAT & 1));
}

unsigned char nand_data(void)
{
	return NFDATA;
}

int nand_bad(unsigned int addr)
{
	unsigned int col  = 2048;
	unsigned int page = addr / 2048;
	unsigned char val;

	/* 1. ѡ�� */
	nand_select();
	
	/* 2. ����������00h */
	nand_cmd(0x00);
	
	/* 3. ������ַ(��5������) */
	nand_col(col);
	nand_page(page);
	
	/* 4. ����������30h */
	nand_cmd(0x30);
	
	/* 5. �ж�״̬ */
	nand_wait_ready();

	/* 6. ������ */
	val = nand_data();
	
	/* 7. ȡ��ѡ�� */		
	nand_deselect();


	if (val != 0xff)
		return 1;  /* bad blcok */
	else
		return 0;
}


void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int col = addr % 2048;
	int i = 0;
		
	while (i < len)
	{

		if (!(addr & 0x1FFFF) && nand_bad(addr)) /* һ��blockֻ�ж�һ�� */
		{
			addr += (128*1024);  /* ������ǰblock */
			continue;
		}

		/* 1. ѡ�� */
		nand_select();
		
		
		/* 2. ����������00h */
		nand_cmd(0x00);

		/* 3. ������ַ(��5������) */
		nand_addr(addr);

		/* 4. ����������30h */
		nand_cmd(0x30);

		/* 5. �ж�״̬ */
		nand_wait_ready();

		/* 6. ������ */
		for (; (col < 2048) && (i < len); col++)
		{
			buf[i] = nand_data();
			i++;
			addr++;
		}
		
		col = 0;


		/* 7. ȡ��ѡ�� */		
		nand_deselect();
		
	}
}

void nand_w_data(unsigned char val)
{
	NFDATA = val;
}



void nand_chip_id(void)
{ 
	unsigned char buf[5]={0};
	
	nand_select(); 
	nand_cmd(0x90);
	nand_addr_byte(0x00);

	buf[0] = nand_data();
	buf[1] = nand_data();	
	buf[2] = nand_data();
	buf[3] = nand_data();
	buf[4] = nand_data();	
	nand_deselect(); 	

	printf("maker   id  = 0x%x\n\r",buf[0]);
	printf("device  id  = 0x%x\n\r",buf[1]);	
	printf("3rd byte    = 0x%x\n\r",buf[2]);		
	printf("4th byte    = 0x%x\n\r",buf[3]);			
	printf("page  size  = %d kb\n\r",1  <<  (buf[3] & 0x03));	
	printf("block size  = %d kb\n\r",64 << ((buf[3] >> 4) & 0x03));	
	printf("5th byte    = 0x%x\n\r",buf[4]);

	
}

int nand_erase(unsigned int addr, unsigned int len)
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
	
	nand_select(); 

	while (1)
	{
		page = addr / 2048;
		
		nand_cmd(0x60);
		
		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff);

		nand_cmd(0xD0);

		nand_wait_ready();

		len -= (128*1024);
		if (len == 0)
			break;
		addr += (128*1024);
	}
	
	nand_deselect(); 	
	return 0;
}

void nand_write(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int page = addr / 2048;
	int col  = addr & (2048 - 1);
	int i = 0;

	nand_select(); 

	while (1)
	{
		nand_cmd(0x80);

		/* ������ַ */
		/* col addr */
		nand_addr_byte(col & 0xff);
		nand_addr_byte((col>>8) & 0xff);
		
		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff);

		/* �������� */
		for (; (col < 2048) && (i < len); )
		{
			nand_w_data(buf[i++]);
		}
		nand_cmd(0x10);
		nand_wait_ready();

		if (i == len)
			break;
		else
		{
			/* ��ʼ��һ��ѭ��page */
			col = 0;
			page++;
		}
		
	}
	
	nand_deselect(); 	
}



void do_read_nand_flash(void)
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

	nand_read(addr, buf, 64);
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
				putchar('.');
			else
				putchar(str[j]);
		}
		printf("\n\r");
	}
}

void do_erase_nand_flash(void)
{
	unsigned int addr;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	nand_erase(addr, 128*1024);
}


void do_write_nand_flash(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* ��õ�ַ */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)str);

	printf("writing ...\n\r");
	nand_write(addr, str, strlen((char *)str)+1);

}




