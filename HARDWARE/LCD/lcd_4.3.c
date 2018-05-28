
#include "lcd.h"
#include "lcd_4.3.h"

#define LCD_FB_BASE 0x33c00000

SLcdParams lcd_4_3_params = {
	.pchName = "lcd_4.3",
	.sPinsPol = {
		.iDe    = kNORMAL,	/* normal: �ߵ�ƽʱ���Դ������� */
		.iPwren = kNORMAL,    /* normal: �ߵ�ƽ��Ч */
		.iVclk  = kNORMAL,	/* normal: ���½��ػ�ȡ���� */
		.iRgb   = kNORMAL,	/* normal: �ߵ�ƽ��ʾ1 */
		.iHsync = kINVERT,    /* normal: ������ */
		.iVsync = kINVERT, 	/* normal: ������ */
	},
	.sTimeSeq = {
		/* ��ֱ���� */
		.iTvp=	10, /* vysnc������ */
		.iTvb=	2,  /* �ϱߺڿ�, Vertical Back porch */
		.iTvf=	2,  /* �±ߺڿ�, Vertical Front porch */

		/* ˮƽ���� */
		.iThp=	41, /* hsync������ */
		.iThb=	2,  /* ��ߺڿ�, Horizontal Back porch */
		.iThf=	2,  /* �ұߺڿ�, Horizontal Front porch */

		.iVclk=	9,  /* MHz */
	},
	.iXres = 480,
	.iYres = 272,
	.iBpp  = 16,  /* 16, no 24bpp */
	.nFbBase = LCD_FB_BASE,
};


void Add4Point3Lcd(void)
{
	RegisterLcd(&lcd_4_3_params);
}

