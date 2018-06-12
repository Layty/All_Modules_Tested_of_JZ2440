/**************************************************************************************************************************
  * @brief      : 	JZ2440v2������LCD��ʾ�������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***************************************************************************************************************************/

#include "lcd.h"
#include "framebuffer.h"
#include "font.h"
#include "timer.h"

extern const unsigned char fontdata_8x16[];
extern const unsigned char asc2_3216[95][128];

/* LCD���� */
static unsigned int sg_uiFbBase;
static int sg_iXres, sg_iYres, sg_Bpp;

/**************************************************************************************************************************
  * @brief       : 	��ʼ�����幦�ܣ��õ�lcd��ʾ���Ĳ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***************************************************************************************************************************/
void InitFont(void)
{
	GetLcdParams(&sg_uiFbBase, &sg_iXres, &sg_iYres, &sg_Bpp);
}

/**************************************************************************************************************************
  * @brief       : 	��lcd��ʾ����ָ��λ�����ָ����ɫ��8x16��С������
  * @param[in]   : 	x	���ֵ�x����
  					y	���ֵ�y����
  					c	��ʾ���ַ�
  					color	��ʾ���ַ���ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***************************************************************************************************************************/
static void PrintFbChar8x16(int x, int y, char c, unsigned int color)
{
	int i, j;
	
	/* ����c��ascii����fontdata_8x16�еõ��������� */
	unsigned char *pchDots = (unsigned char *)&fontdata_8x16[c * 16];

	unsigned char uchData;
	int iBit;

	/* ���ݵ��������ö�Ӧ���ص���ɫ */
	for (j = y; j < y+16; j++)
	{
		uchData = *pchDots++;
		iBit = 7;
		for (i = x; i < x+8; i++)
		{
			/* ���ݵ����ĳλ�����Ƿ�����ɫ */
			if (uchData & (1<<iBit))
				PutFbPixel(i, j, color);
			iBit--;
		}
	}
}

/**************************************************************************************************************************
  * @brief       : 	��lcd��ʾ����ָ��λ�����ָ����ɫ��16x32��С������
  * @param[in]   : 	x	���ֵ�x����
  					y	���ֵ�y����
  					c	��ʾ���ַ�
  					color	��ʾ���ַ���ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***************************************************************************************************************************/
static void PrintFbChar16x32(int x, int y, char c, unsigned int color)
{
	int t,t1;
	unsigned char uchTemp;
	unsigned char ucSize;
	unsigned int uiYstart = y;
	ucSize=(32/8+((32%8)?1:0))*(32/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	

	/* ���ݵ��������ö�Ӧ���ص���ɫ */
	c=c-' ';
	for (t = 0; t < ucSize; t++)
	{
		uchTemp = asc2_3216[(int)c][t];

		for(t1=0;t1<8;t1++)
		{			    
			if(uchTemp&0x80)
				PutFbPixel(x,y,color);
				
			uchTemp<<=1;
			y++;
			if(y>=sg_iYres)
				return;		//��������
			if((y-uiYstart)== 32)
			{
				y=uiYstart;
				x++;
				if(x>=sg_iXres)
					return;	//��������
				break;
			}
		} 
	}
}


/**************************************************************************************************************************
  * @brief       : 	��lcd��ʾ����ָ��λ�ÿ�ʼ���ָ����ɫ��8x16��С���ַ���
  * @param[in]   : 	x	�ַ�����x����
  					y	�ַ�����y����
  					c	��ʾ���ַ���
  					color	��ʾ���ַ�����ɫ
  					isDefer	�Ƿ������ӡ�ַ����е��ַ�			1�������ӡ	0���������ӡ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***************************************************************************************************************************/
void PrintFbString8x16(int x, int y, char* str, unsigned int color,int isDefer)
{
	int i = 0;
	
	while (str[i])
	{
		if (str[i] == '\n')
			y = y+16;
		else if (str[i] == '\r')
			x = 0;
		else
		{
			PrintFbChar8x16(x, y, str[i], color);
			if(isDefer)
				Delay(8000);
			x = x+8;
			if (x >= sg_iXres) /* ���� */
			{
				x = 0;
				y = y+16;
			}
		}
		i++;
	}
}

/************************************************************************************************
  * @brief       : 	��lcd��ʾ����ָ��λ�ÿ�ʼ���ָ����ɫ��16x32��С���ַ���
  * @param[in]   : 	x	�ַ�����x����
  					y	�ַ�����y����
  					c	��ʾ���ַ���
  					color	��ʾ���ַ�����ɫ
  					isDefer	�Ƿ������ӡ�ַ����е��ַ�			1�������ӡ	0���������ӡ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
*************************************************************************************************/
void PrintFbString16x32(int x, int y, char* str, unsigned int color,int isDefer)
{
	int i = 0;
	
	while (str[i])
	{
		if (str[i] == '\n')
			y = y+16;
		else if (str[i] == '\r')
			x = 0;
		else
		{
			PrintFbChar16x32(x, y, str[i], color);
			if(isDefer)
				Delay(8000);
			x = x+16;
			if (x >= sg_iXres) /* ���� */
			{
				x = 0;
				y = y+32;
			}
		}
		i++;
	}
}


