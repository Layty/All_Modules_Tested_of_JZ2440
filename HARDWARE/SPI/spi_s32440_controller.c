/****************************************************************************************************
  * @brief      : 	JZ2440v2������SPI ����������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "timer.h"
#include "spi_s3c2440_controller.h"


/* s3c2440 SPI controller */

/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 SPI�����������GPIO����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void s3c2440SPIGPIOInit(void)
{
    /* GPF1 OLED_CSn output */
    GPFCON &= ~(3<<(1*2));
    GPFCON |= (1<<(1*2));
    GPFDAT |= (1<<1);

    /* GPG2 FLASH_CSn output
    * GPG4 OLED_DC  output
    * GPG5 SPIMISO   
    * GPG6 SPIMOSI   
    * GPG7 SPICLK    
    */
    GPGCON &= ~((3<<(2*2)) | (3<<(4*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2)));
    GPGCON |= ((1<<(2*2)) | (1<<(4*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2)));
    GPGDAT |= (1<<2);
}

/**********************************************************************************
  * @brief       : 	ʹ��S3C2440 SPI����������һ���ֽ�����
  * @param[in]   : 	val	�����͵�����
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void SendByteSPIS3c2440Controller(unsigned char val)
{
	unsigned long long iPre = GetSystemTimeUs();
	unsigned long long iNow;
	
    while (!(SPSTA1 & 1))
    {
		iNow = GetSystemTimeUs();
		if(DeltaTimeUs(iPre,iNow) == 1000000)
			return;
    }
    SPTDAT1 = val;    
}

/**********************************************************************************
  * @brief       : 	ʹ��S3C2440 SPI����������һ���ֽ�����
  * @param[in]   : 	val	�����͵�����
  * @param[out]  : 	��
  * @return      : 	���ؽ��յ�һ���ֽڵ�����
  * @others      : 	��
***********************************************************************************/
unsigned char RecvByteSPIS3c2440Controller(void)
{
	unsigned long long iPre = GetSystemTimeUs();
	unsigned long long iNow;
	
    SPTDAT1 = 0xff;    
    while (!(SPSTA1 & 1))
    {
		iNow = GetSystemTimeUs();
		if(DeltaTimeUs(iPre,iNow) == 1000000)
			return 0xff;
    }
    return SPRDAT1;    
}

/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 SPI����������ؼĴ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void s3c2440SPIControllerInit(void)
{
    /* OLED  : 100ns, 10MHz
    * FLASH : 104MHz
    * ȡ10MHz
    * 10 = 50 / 2 / (Prescaler value + 1)
    * Prescaler value = 1.5 = 2
    * Baud rate = 50/2/3=8.3MHz
    */
    SPPRE0 = 2;
    SPPRE1 = 2;

    /* [6:5] : 00, polling mode
    * [4]   : 1 = enable 
    * [3]   : 1 = master
    * [2]   : 0
    * [1]   : 0 = format A
    * [0]   : 0 = normal mode
    */
    SPCON0 = (1<<4) | (1<<3);
    SPCON1 = (1<<4) | (1<<3);
    
}

/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 SPI������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitSPIS3c2440Controller(void)
{
    /* ��ʼ������ */
    s3c2440SPIGPIOInit();

    s3c2440SPIControllerInit();
}

