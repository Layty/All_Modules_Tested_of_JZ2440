
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

int InitLcd(void);
void GetLcdParams(unsigned int *fb_base, int *xres, int *yres, int *bpp);
void EnableLcd(void);
void DisableLcd(void);
int RegisterLcd(PSLcdParams plcd);
void TestLCD(void);



#endif /* _LCD_H */


