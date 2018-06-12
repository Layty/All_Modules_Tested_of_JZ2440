/**********************************************************************************
  * @brief      : 	JZ2440v2������LCD����ͷ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#ifndef _LCD_H
#define _LCD_H


enum {
	kNORMAL = 0,
	kINVERT = 1,
};

/* NORMAL : ��������
 * INVERT : ��ת����
 */
typedef struct SLcdPinsPolarity {
	int iDe;    /* normal: �ߵ�ƽʱ���Դ������� */
	int iPwren; /* normal: �ߵ�ƽ��Ч */
	int iVclk;  /* normal: ���½��ػ�ȡ���� */
	int iRgb;   /* normal: �ߵ�ƽ��ʾ1 */
	int iHsync; /* normal: ������ */
	int iVsync; /* normal: ������ */
}SLcdPinsPolarity, *PSLcdPinsPolarity;

typedef struct SLcdTimeSequence {
	/* ��ֱ���� */
	int iTvp; /* vysnc������ */
	int iTvb; /* �ϱߺڿ�, Vertical Back porch */
	int iTvf; /* �±ߺڿ�, Vertical Front porch */

	/* ˮƽ���� */
	int iThp; /* hsync������ */
	int iThb; /* ��ߺڿ�, Horizontal Back porch */
	int iThf; /* �ұߺڿ�, Horizontal Front porch */

	int iVclk;
}SLcdTimeSequence, *PSLcdTimeSequence;


typedef struct SLcdParams {
	char *pchName;
	
	/* ���ż��� */
	SLcdPinsPolarity sPinsPol;
	
	/* ʱ�� */
	SLcdTimeSequence sTimeSeq;
	
	/* �ֱ���, bpp */
	int iXres;
	int iYres;
	int iBpp;
	
	/* framebuffer�ĵ�ַ */
	unsigned int nFbBase;
}SLcdParams, *PSLcdParams;

/**********************************************************************************
  * @brief       : 	��ʼ��LCD
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
int InitLcd(void);

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
void GetLcdParams(unsigned int *fb_base, int *xres, int *yres, int *bpp);

/**********************************************************************************
  * @brief       : 	ʹ��LCD
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void EnableLcd(void);

/**********************************************************************************
  * @brief       : 	�ر�LCD
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DisableLcd(void);

/**********************************************************************************
  * @brief       : 	ע��LCD
  * @param[in]   : 	plcd	LCD����
  * @param[out]  : 	��
  * @return      : 	i	ע��ɹ���i��ʾlcd��lcd�����е�������
  					-1	ע��ʧ��
  * @others      : 	��
***********************************************************************************/
int RegisterLcd(PSLcdParams plcd);

/**********************************************************************************
  * @brief       : 	LCD���Ժ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestLCD(void);

#endif /* _LCD_H */

