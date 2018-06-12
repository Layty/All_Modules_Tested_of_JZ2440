/**************************************************************************************************************************
  * @brief      : 	JZ2440v2������LCD���Դ棨��ʾ������������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***************************************************************************************************************************/

#include "lcd.h"
#include "framebuffer.h"

/* ���LCD���� */
static unsigned int sg_uiFbBase;
static int sg_iXres, sg_iYres, sg_Bpp;

/**********************************************************************************
  * @brief       : 	��ȡlcd��������framebufferʹ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void GetLcdParamsforFb(void)
{
	GetLcdParams(&sg_uiFbBase, &sg_iXres, &sg_iYres, &sg_Bpp);
}

/**********************************************************************************
  * @brief       : 	��32λ��bppת��Ϊ16λ��bpp
  * @param[in]   : 	rgb	32λ��bpp����
  * @param[out]  : 	��
  * @return      : 	16λ��bpp����
  * @others      : 	��
***********************************************************************************/
unsigned short Convert32BppTo16Bpp(unsigned int rgb)
{
	int r = (rgb >> 16)& 0xff;
	int g = (rgb >> 8) & 0xff;
	int b = rgb & 0xff;

	/* rgb565 */
	r = r >> 3;
	g = g >> 2;
	b = b >> 3;

	return ((r<<11) | (g<<5) | (b));
}

/**********************************************************************************
  * @brief       : 	���Դ���ָ�����������Ϊָ������ɫ
  * @param[in]   : 	x	����������x����
  					y	����������y����
  					color	���������ص���ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void PutFbPixel(int x, int y, unsigned int color)
{
	unsigned char  *pchBpp8;  /* 8bpp */
	unsigned short *pusBpp16;  /* 16bpp */
	unsigned int   *puiBpp32; /* 32bpp */

	unsigned int pixel_base = sg_uiFbBase + (sg_iXres * sg_Bpp / 8) * y + x * sg_Bpp / 8;

	switch (sg_Bpp)
	{
		case 8:
			pchBpp8 = (unsigned char *) pixel_base;
			*pchBpp8 = color;
			break;
		case 16:
			pusBpp16 = (unsigned short *) pixel_base;
			*pusBpp16 = Convert32BppTo16Bpp(color);
			break;
		case 32:
			puiBpp32 = (unsigned int *) pixel_base;
			*puiBpp32 = color;
			break;
	}
}

/**********************************************************************************
  * @brief       : 	���Դ�������������Ϊͳһ���ض���ɫ
  * @param[in]   : 	color	�����õ���ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void ClearScreen(unsigned int color)
{
	int x, y;
	unsigned char  *pchBpp8;  /* 8bpp */
	unsigned short *pusBpp16;  /* 16bpp */
	unsigned int   *puiBpp32; /* 32bpp */


	/* ��framebuffer��д���� */
	if (sg_Bpp == 8)
	{
		/* bpp: palette[color] */

		pchBpp8 = (unsigned char *)sg_uiFbBase;
		for (x = 0; x < sg_iXres; x++)
			for (y = 0; y < sg_iYres; y++)
				*pchBpp8++ = color;
	}
	else if (sg_Bpp == 16)
	{
		/* ��LCD��������ĺ�ɫ */

		/* 565: 0xf800 */

		pusBpp16 = (unsigned short *)sg_uiFbBase;
		for (x = 0; x < sg_iXres; x++)
			for (y = 0; y < sg_iYres; y++)
				*pusBpp16++ = Convert32BppTo16Bpp(color);
			
	}
	else if (sg_Bpp == 32)
	{
		puiBpp32 = (unsigned int *)sg_uiFbBase;
		for (x = 0; x < sg_iXres; x++)
			for (y = 0; y < sg_iYres; y++)
				*puiBpp32++ = color;

	}
}

