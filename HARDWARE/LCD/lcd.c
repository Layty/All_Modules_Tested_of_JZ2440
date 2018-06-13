/**********************************************************************************
  * @brief      : 	JZ2440v2������LCD����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "lcd.h"
#include "lcd_controller.h"
#include "string_utils.h"
#include "lcd_4.3.h"
#include "timer.h"
#include "function.h"
#include "geometry.h"
#include "font.h"

#define LCD_NUM 10

static PSLcdParams g_psArrayLcd[LCD_NUM];   //LCD����
static PSLcdParams g_psLcdSelected;			//��ǰѡ���LCD

/**********************************************************************************
  * @brief       : 	ע��LCD
  * @param[in]   : 	plcd	LCD����
  * @param[out]  : 	��
  * @return      : 	i	ע��ɹ���i��ʾlcd��lcd�����е�������
  					-1	ע��ʧ��
  * @others      : 	��
***********************************************************************************/
int RegisterLcd(PSLcdParams plcd)
{
	int i;
	for (i = 0; i < LCD_NUM; i++)
	{
		if (!g_psArrayLcd[i])
		{
			g_psArrayLcd[i] = plcd;
			return i;
		}
	}
	return -1;		
}

/**********************************************************************************
  * @brief       : 	��������ѡ��LCD
  * @param[in]   : 	name	LCD����
  * @param[out]  : 	��
  * @return      : 	i	ѡ��ɹ���i��ʾlcd��lcd�����е�������
  					-1	ѡ��ʧ��
  * @others      : 	��
***********************************************************************************/
static int SelectLcd(char *name)
{
	int i;
	for (i = 0; i < LCD_NUM; i++)
	{
		if (g_psArrayLcd[i] && !strcmp(g_psArrayLcd[i]->pchName, name))
		{
			g_psLcdSelected = g_psArrayLcd[i];
			return i;
		}
	}
	return -1;		
}

/**********************************************************************************
  * @brief       : 	��ȡLCD��ز���
  * @param[in]   : 	��
  * @param[out]  : 	fb_base	lcd���Դ����ַ
  					xres	lcd��x�������ֵ
  					yres	lcd��y�������ֵ
  					bpp		lcd��bpp
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void GetLcdParams(unsigned int *fb_base, int *xres, int *yres, int *bpp)
{
	*fb_base = g_psLcdSelected->nFbBase;
	*xres = g_psLcdSelected->iXres;
	*yres = g_psLcdSelected->iYres;
	*bpp = g_psLcdSelected->iBpp;
}

/**********************************************************************************
  * @brief       : 	ʹ��LCD
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void EnableLcd(void)
{
	EnableLcdController();
}

/**********************************************************************************
  * @brief       : 	�ر�LCD
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DisableLcd(void)
{
	DisableLcdController();
}

/**********************************************************************************
  * @brief       : 	��ʼ��LCD
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
int InitLcd(void)
{
	/* ע��LCD */
	Add4Point3Lcd();

	/* ע��LCD������ */
	AddLcdContollerS3c2440();
	
	/* ѡ��ĳ��LCD */
	SelectLcd("lcd_4.3");

	/* ѡ��ĳ��LCD������ */
	SelectLcdController("s3c2440");

	/* ʹ��LCD�Ĳ���, ��ʼ��LCD������ */
	InitLcdController(g_psLcdSelected);

	return 0;
}

/**********************************************************************************
  * @brief       : 	LCD���Ժ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestLCD(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	unsigned char *puchBpp8;
	unsigned short *pusBpp16;
	unsigned int *puiBpp32;
	
	GetLcdParams(&uiFbBase, &iXres, &iYres, &iBpp);

	/* ��framebuffer��д���� */
	if (iBpp == 8)
	{
		/* ��LCD��������ĺ�ɫ */

		/* bpp: palette[12] */

		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 12;

		Delay(500000);

		/* palette[47] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 47;

		Delay(500000);

		/* palette[88] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 88;
			
		Delay(500000);

		/* palette[0] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 0;
			
	}
	else if (iBpp == 16)
	{
		/* ��LCD��������ĺ�ɫ */

		/* 565: 0xf800 */

		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0xf800;
			
		Delay(500000);

		/* green */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0x7e0;

		Delay(500000);

		/* blue */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0x1f;

		Delay(500000);

		/* black */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0;
			
	}
	else if (iBpp == 32)
	{
		/* ��LCD��������ĺ�ɫ */

		/* 0xRRGGBB */

		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0xff0000;

		Delay(500000);

		/* green */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0x00ff00;

		Delay(500000);

		/* blue */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0x0000ff;

		Delay(500000);

		/* black */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0;

	}

	Delay(1000000);
	
	/* ���� */
	DrawLine(0, 0, iXres - 1, 0, 0xff0000);
	DrawLine(iXres - 1, 0, iXres - 1, iYres - 1, 0xffff00);
	DrawLine(0, iYres - 1, iXres - 1, iYres - 1, 0xff00aa);
	DrawLine(0, 0, 0, iYres - 1, 0xff00ef);
	DrawLine(0, 0, iXres - 1, iYres - 1, 0xff4500);
	DrawLine(iXres - 1, 0, 0, iYres - 1, 0xff0780);

	Delay(1000000);

	/* ��Բ */
	DrawCircle(iXres/2, iYres/2, iYres/4, 0xff00);

	/* ������� */
	PrintFbString8x16(10, 10, "leichuan   sagacity_lc@163.com", 0xffff, 1);

	Delay(3000000);

	MainPage();
}

