/****************************************************************************************************
  * @brief      : 	JZ2440v2������SPI Flash����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/
#include "s3c2440_soc.h"
#include "spi_gpio_simulate.h"
#include "spi_s3c2440_controller.h"
#include "timer.h"
#include "my_printf.h"
#include "function.h"
#include "string_utils.h"

/**********************************************************************************
  * @brief       : 	����SPI Flash��CS״̬
  * @param[in]   : 	val	�����õ�ֵ   		ȡֵ{0,1}
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SetSPIFlashCS(char val)
{
    if (val)
        GPGDAT |= (1<<2);
    else
        GPGDAT &= ~(1<<2);
}

/**********************************************************************************
  * @brief       : 	��SPI Flash���͵�ַ
  * @param[in]   : 	addr	�����͵ĵ�ַ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SendSPIFlashAddr(unsigned int addr)
{
#ifdef SPIGPIO

    SendByteSPIGPIO(addr >> 16);
    SendByteSPIGPIO(addr >> 8);
    SendByteSPIGPIO(addr & 0xff);
#else

	SendByteSPIS3c2440Controller(addr >> 16);
	SendByteSPIS3c2440Controller(addr >> 8);
	SendByteSPIS3c2440Controller(addr & 0xff);	
#endif
}

/**********************************************************************************
  * @brief       : 	��ȡSPI Flash��ID
  * @param[in]   : 	��
  * @param[out]  : 	pMID	��Ż�ȡ��MID
  					pDID	��Ż�ȡ��DID
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void ReadSPIFlashID(int *pMID, int *pDID)
{
#ifdef SPIGPIO     //ʹ��gpioģ��
    SetSPIFlashCS(0); /* ѡ��SPI FLASH */

    SendByteSPIGPIO(0x90);

    SendSPIFlashAddr(0);

    *pMID = RecvByteSPIGPIO();
    *pDID = RecvByteSPIGPIO();

    SetSPIFlashCS(1);
#else	 //ʹ��ʱs3c2440�����spi������
	SetSPIFlashCS(0); /* ѡ��SPI FLASH */

	SendByteSPIS3c2440Controller(0x90);

	SendSPIFlashAddr(0);

	*pMID = RecvByteSPIS3c2440Controller();
	*pDID = RecvByteSPIS3c2440Controller();

	SetSPIFlashCS(1);
#endif
}

/**********************************************************************************
  * @brief       : 	SPI Flashдʹ��
  * @param[in]   : 	enable	ʹ��״̬ ȡֵ{0,1}
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SPIFlashWriteEnable(int enable)
{
#ifdef SPIGPIO
    if (enable)
    {
        SetSPIFlashCS(0);
        SendByteSPIGPIO(0x06);
        SetSPIFlashCS(1);
    }
    else
    {
        SetSPIFlashCS(0);
        SendByteSPIGPIO(0x04);
        SetSPIFlashCS(1);
    }
 #else
	 if (enable)
	 {
		 SetSPIFlashCS(0);
		 SendByteSPIS3c2440Controller(0x06);
		 SetSPIFlashCS(1);
	 }
	 else
	 {
		 SetSPIFlashCS(0);
		 SendByteSPIS3c2440Controller(0x04);
		 SetSPIFlashCS(1);
	 }
 #endif
}

/**********************************************************************************
  * @brief       : 	��ȡSPI Flash״̬�Ĵ���1
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	val	״̬�Ĵ���1��ֵ
  * @others      : 	��
***********************************************************************************/
static unsigned char SPIFlashReadStatusReg1(void)
{
#ifdef SPIGPIO
    unsigned char val;
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x05);
    val = RecvByteSPIGPIO();
    SetSPIFlashCS(1);
    return val;
#else
	unsigned char val;
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x05);
	val = RecvByteSPIS3c2440Controller();
	SetSPIFlashCS(1);
	return val;
#endif
}

/**********************************************************************************
  * @brief       : 	��ȡSPI Flash״̬�Ĵ���2
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	val	״̬�Ĵ���2��ֵ
  * @others      : 	��
***********************************************************************************/
static unsigned char SPIFlashReadStatusReg2(void)
{
#ifdef SPIGPIO
    unsigned char val;
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x35);
    val = RecvByteSPIGPIO();
    SetSPIFlashCS(1);
    return val;
#else
	unsigned char val;
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x35);
	val = RecvByteSPIS3c2440Controller();
	SetSPIFlashCS(1);
	return val;
#endif
}

/**********************************************************************************
  * @brief       : 	��SPI Flashæ�ǽ��еȴ�
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SPIFlashWaitWhenBusy(void)
{
    while (SPIFlashReadStatusReg1() & 1);
}

/**********************************************************************************
  * @brief       : 	дSPI Flash״̬�Ĵ���
  * @param[in]   : 	reg1	��д���״̬�Ĵ���1��ֵ
  					reg2	��д���״̬�Ĵ���2��ֵ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SPIFlashWriteStatusReg(unsigned char reg1, unsigned char reg2)
{  

    SPIFlashWriteEnable(1);  
#ifdef SPIGPIO    
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x01);
    SendByteSPIGPIO(reg1);
    SendByteSPIGPIO(reg2);
    SetSPIFlashCS(1);
#else
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x01);
	SendByteSPIS3c2440Controller(reg1);
	SendByteSPIS3c2440Controller(reg2);
	SetSPIFlashCS(1);
#endif
    SPIFlashWaitWhenBusy();
}

/**********************************************************************************
  * @brief       : 	���SPI Flash�ļĴ�������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SPIFlashClearProtectForStatusReg(void)
{
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(1<<7);
    reg2 &= ~(1<<0);

    SPIFlashWriteStatusReg(reg1, reg2);
}

/**********************************************************************************
  * @brief       : 	���SPI Flash�����ݱ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void SPIFlashClearProtectForData(void)
{
    /* cmp=0,bp2,1,0=0b000 */
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(7<<2);
    reg2 &= ~(1<<6);

    SPIFlashWriteStatusReg(reg1, reg2);
}

/**********************************************************************************
  * @brief       : 	����SPI Flash�����ݣ��̶�����4K����
  * @param[in]   : 	addr	�������׵�ַ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void EraseSPIFlashSector(unsigned int addr)
{
    SPIFlashWriteEnable(1);  
#ifdef SPIGPIO
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x20);
    SendSPIFlashAddr(addr);
    SetSPIFlashCS(1);
#else
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x20);
	SendSPIFlashAddr(addr);
	SetSPIFlashCS(1);
#endif
    SPIFlashWaitWhenBusy();
}

/**********************************************************************************
  * @brief       : 	��SPI Flashд������
  * @param[in]   : 	addr	д����׵�ַ
  					buf		��д������ݻ�������ַ
  					len		��д������ݳ���
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void ProgramSPIFlash(unsigned int addr, unsigned char *buf, int len)
{
    int i;
#ifdef SPIGPIO   
    SPIFlashWriteEnable(1);  

    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x02);
    SendSPIFlashAddr(addr);

    for (i = 0; i < len; i++)
        SendByteSPIGPIO(buf[i]);
    
    SetSPIFlashCS(1);
#else
	SPIFlashWriteEnable(1);  

	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x02);
	SendSPIFlashAddr(addr);

	for (i = 0; i < len; i++)
		SendByteSPIS3c2440Controller(buf[i]);

	SetSPIFlashCS(1);
#endif

    SPIFlashWaitWhenBusy();
    
}

/**********************************************************************************
  * @brief       : 	��SPI Flash��ȡ����
  * @param[in]   : 	addr	��ȡ���׵�ַ
  					buf		��ȡ�����ݴ�ŵĻ�������ַ
  					len		����ȡ�����ݳ���
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void ReadSPIFlash(unsigned int addr, unsigned char *buf, int len)
{
    int i;
#ifdef SPIGPIO    
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x03);
    SendSPIFlashAddr(addr);
    for (i = 0; i < len; i++)
        buf[i] = RecvByteSPIGPIO();

    SetSPIFlashCS(1); 
#else
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x03);
	SendSPIFlashAddr(addr);
	for (i = 0; i < len; i++)
		buf[i] = RecvByteSPIS3c2440Controller();

	SetSPIFlashCS(1); 

#endif
}

/**********************************************************************************
  * @brief       : 	��ʼ��SPI Flash
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitSPIFlash(void)
{
    SPIFlashClearProtectForStatusReg();
    SPIFlashClearProtectForData();
}

/**********************************************************************************
  * @brief       : 	������SPI Flashд�뺯��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DoWriteSPIFLASH(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* ��õ�ַ */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)str);

	printf("writing ...\n\r");
	ProgramSPIFlash(addr, str, strlen((char *)str)+1);
}

/**********************************************************************************
  * @brief       : 	������SPI Flash��ȡ����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DoReadSPIFLASH(void)
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

	ReadSPIFlash(addr, buf, 64);
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

/**********************************************************************************
  * @brief       : 	������SPI Flash��������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DoEraseSPIFLASH(void)
{
	unsigned int addr;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	EraseSPIFlashSector(addr);
}

