/**********************************************************************************
  * @brief      : 	JZ2440v2������LCD����������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "s3c2440_lcd_controller.h"
#include "lcd.h"
#include "lcd_controller.h"
#include "s3c2440_soc.h"


#define HCLK 100

/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 lcd������GPIO�ܽ�
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitJz2440LcdPin(void)
{
	/* ��ʼ������ : �������� */
	GPBCON &= ~0x3;
	GPBCON |= 0x01;

	/* LCDר������ */
	GPCCON = 0xaaaaaaaa;
	GPDCON = 0xaaaaaaaa;

	/* PWREN */
	GPGCON |= (3<<8);
}

/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 lcd������
  * @param[in]   : 	plcdparams	lcd�����ṹ��ָ��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitS3c2440LcdController(PSLcdParams plcdparams)
{
	int iPixelPlace;
	unsigned int nAddr;

	InitJz2440LcdPin();
	
	/* [17:8]: clkval, vclk = HCLK / [(CLKVAL+1) x 2]
	 *                   9   = 100M /[(CLKVAL+1) x 2], clkval = 4.5 = 5
	 *                 CLKVAL = 100/vclk/2-1
	 * [6:5]: 0b11, tft lcd
	 * [4:1]: bpp mode
	 * [0]  : LCD video output and the logic enable/disable
	 */
	 /*��ӳ���ʱ�䣬���������*/
	int iClkval = (double)HCLK/plcdparams->sTimeSeq.iVclk/2-1+0.5;
	//int clkval = 5;
	int iBppMode = plcdparams->iBpp == 8  ? 0xb :\
				  plcdparams->iBpp == 16 ? 0xc :\
				  0xd;  /* 0xd: 24,32bpp */
	LCDCON1 = (iClkval<<8) | (3<<5) | (iBppMode<<1) ;

	/* [31:24] : VBPD    = tvb - 1
	 * [23:14] : LINEVAL = line - 1
	 * [13:6]  : VFPD    = tvf - 1
	 * [5:0]   : VSPW    = tvp - 1
	 */
	LCDCON2 = 	((plcdparams->sTimeSeq.iTvb - 1)<<24) | \
	            ((plcdparams->iYres - 1)<<14)         | \
				((plcdparams->sTimeSeq.iTvf - 1)<<6)  | \
				((plcdparams->sTimeSeq.iTvp - 1)<<0);

	/* [25:19] : HBPD	 = thb - 1
	 * [18:8]  : HOZVAL  = �� - 1
	 * [7:0]   : HFPD	 = thf - 1
	 */
	LCDCON3 =	((plcdparams->sTimeSeq.iThb - 1)<<19) | \
				((plcdparams->iXres - 1)<<8)		      | \
				((plcdparams->sTimeSeq.iThf - 1)<<0);

	/* 
	 * [7:0]   : HSPW	 = thp - 1
	 */
	LCDCON4 =	((plcdparams->sTimeSeq.iThp - 1)<<0);

    /* �����������ż���, ����16bpp, �����ڴ������ش�ŵĸ�ʽ
     * [12] : BPP24BL
	 * [11] : FRM565, 1-565
	 * [10] : INVVCLK, 0 = The video data is fetched at VCLK falling edge
	 * [9]  : HSYNC�Ƿ�ת
	 * [8]  : VSYNC�Ƿ�ת
	 * [7]  : INVVD, rgb�Ƿ�ת
	 * [6]  : INVVDEN
	 * [5]  : INVPWREN
	 * [4]  : INVLEND
	 * [3]  : PWREN, LCD_PWREN output signal enable/disable
	 * [2]  : ENLEND
	 * [1]  : BSWP
	 * [0]  : HWSWP
	 */

	iPixelPlace = plcdparams->iBpp == 32 ? (0) : \
	             plcdparams->iBpp == 16 ? (1) : \
	             (1<<1);  /* 8bpp */
	
	LCDCON5 = (plcdparams->sPinsPol.iVclk<<10) |\
	          (plcdparams->sPinsPol.iRgb<<7)   |\
	          (plcdparams->sPinsPol.iHsync<<9) |\
	          (plcdparams->sPinsPol.iVsync<<8) |\
 			  (plcdparams->sPinsPol.iDe<<6)    |\
			  (plcdparams->sPinsPol.iPwren<<5) |\
			  (1<<11) | iPixelPlace;

	/* framebuffer��ַ */
	/*
	 * [29:21] : LCDBANK, A[30:22] of fb
	 * [20:0]  : LCDBASEU, A[21:1] of fb
	 */
	nAddr = plcdparams->nFbBase & ~(1<<31);
	LCDSADDR1 = (nAddr >> 1);

	/* 
	 * [20:0] : LCDBASEL, A[21:1] of end addr
	 */
	nAddr = plcdparams->nFbBase + plcdparams->iXres*plcdparams->iYres*plcdparams->iBpp/8;
	nAddr >>=1;
	nAddr &= 0x1fffff;
	LCDSADDR2 = nAddr;//	
}

/**********************************************************************************
  * @brief       : 	ʹ��S3C2440 lcd������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void EnableS3c2440LcdController(void)
{
	/* �������� : GPB0 */
	GPBDAT |= (1<<0);
	
	/* pwren    : ��LCD�ṩAVDD  */
	LCDCON5 |= (1<<3);
	
	/* LCDCON1'BIT 0 : ����LCD�������Ƿ�����ź� */
	LCDCON1 |= (1<<0);
}

/**********************************************************************************
  * @brief       : 	ȡ��ʹ��S3C2440 lcd������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DisableS3c2440LcdController(void)
{
	/* �������� : GPB0 */
	GPBDAT &= ~(1<<0);

	/* pwren	: ��LCD�ṩAVDD  */
	LCDCON5 &= ~(1<<3);

	/* LCDCON1'BIT 0 : ����LCD�������Ƿ�����ź� */
	LCDCON1 &= ~(1<<0);
}

/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 lcd��������ɫ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	���õ�ɫ��֮ǰ, �ȹر�lcd_controller
***********************************************************************************/
void InitS3c2440LcdControllerPalette(void)
{
	volatile unsigned int *vnpPaletteBase =  (volatile unsigned int *)0x4D000400;
	int i;

	int iBit = LCDCON1 & (1<<0);

	/* LCDCON1'BIT 0 : ����LCD�������Ƿ�����ź� */
	if (iBit)
		LCDCON1 &= ~(1<<0);

	for (i = 0; i < 256; i++)
	{
		/* ��16λ : rgb565 */	
		*vnpPaletteBase++ = i;
	}

	if (iBit)
		LCDCON1 |= (1<<0);
}

struct SLcdController g_sS3c2440LcdController = {
	.pchName    = "s3c2440",
	.Init    = InitS3c2440LcdController,
	.Enable  = EnableS3c2440LcdController,
	.Disable = DisableS3c2440LcdController,
	.InitPalette = InitS3c2440LcdControllerPalette,
};

/**********************************************************************************
  * @brief       : 	ע��S3C2440 lcd������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void AddS3c2440LcdContoller(void)
{
	RegisterLcdController(&g_sS3c2440LcdController);
}

