#include "s3c2440_soc.h"
#include "touchscreen.h"
#include "interrupt.h"
#include "my_printf.h"
#include "timer.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"
#include "function.h"
#include "geometry.h"

#define ADC_INT_BIT (10)
#define TC_INT_BIT  (9)

#define INT_ADC_TC   (31)

extern SButton g_sButtonReturn;


/* ADCTSC's bits */
#define WAIT_PEN_DOWN    (0<<8)
#define WAIT_PEN_UP      (1<<8)

#define YM_ENABLE        (1<<7)
#define YM_DISABLE       (0<<7)

#define YP_ENABLE        (0<<6)
#define YP_DISABLE       (1<<6)

#define XM_ENABLE        (1<<5)
#define XM_DISABLE       (0<<5)

#define XP_ENABLE        (0<<4)
#define XP_DISABLE       (1<<4)

#define PULLUP_ENABLE    (0<<3)
#define PULLUP_DISABLE   (1<<3)

#define AUTO_PST         (1<<2)

#define WAIT_INT_MODE    (3)
#define NO_OPR_MODE      (0)

static volatile int gs_viTsTimerEnable = 0;

static volatile int gs_viTsX;
static volatile int gs_viTsY;
static volatile int gs_viTsPressure;
static volatile int gs_viTsDataValid = 0;


static int s_TestXArray[16];
static int s_TestYArray[16];

static void EnterWaitPenDownMode(void)
{
	ADCTSC = WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

static void EnterWaitPenUpMode(void)
{
	ADCTSC = WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

static void EnterAutoMeasureMode(void)
{
	ADCTSC = AUTO_PST | NO_OPR_MODE;
}

static int isInAutoMode(void)
{
	return ADCTSC & AUTO_PST;
}

void IsrTc(void)
{
	if (ADCDAT0 & (1<<15))
	{
		EnterWaitPenDownMode();
		ReportTsXY(0,0,0);
	}
	else	
	{
		/* ����"�Զ�����"ģʽ */
		EnterAutoMeasureMode();

		/* ����ADC */
		ADCCON |= (1<<0);
	}
}

static void EnableTsTimer(void)
{
	gs_viTsTimerEnable = 1;
}

static void DisableTsTimer(void)
{
	gs_viTsTimerEnable = 0;
}

static int GetStatusOfTsTimer(void)
{
	return gs_viTsTimerEnable;
}

void ReportTsXY(int x, int y, int pressure)
{
	if (gs_viTsDataValid == 0)
	{
		gs_viTsX = x;
		gs_viTsY = y;
		gs_viTsPressure = pressure;
		gs_viTsDataValid = 1;
		//printf("ReportTsXY gs_viTsPressure = %d\n\r", gs_viTsPressure);
	}
	
}

void ReadTsRaw(int *px, int *py, int *ppressure)
{
	if(gs_viTsDataValid == 0)
	{
		*px = 0;
		*py = 0;
		*ppressure = 0;
		return;
	}
		
	
	*px = gs_viTsX;
	*py = gs_viTsY;
	*ppressure = gs_viTsPressure;

	gs_viTsDataValid = 0;

}

/* ÿ10ms�ú���������һ�� 
 */
void TouchscreenTimerIrq(void)
{
	
	/* ����������Ա�����, ����"�Զ�����ģʽ", ����ADC */
	if (GetStatusOfTsTimer() == 0)
		return;

	if (isInAutoMode())
		return;

	/* ֻ����"�ȴ��ж�ģʽ"�²ſ���ʹ��ADCDAT0'BIT 15���жϴ�����״̬ */

	if (ADCDAT0 & (1<<15)) /* ����ɿ� */
	{
		//printf("touchscreen_timer_irq timer set pen down\n\r");
		DisableTsTimer();
		EnterWaitPenDownMode();
		//printf("touchscreen_timer_irq clear pressure\n\r");
		ReportTsXY(0, 0, 0);
		Delay(10);
		ReportTsXY(0, 0, 0);
		//printf("leave touchscreen_timer_irq\n\r");
		return;
	}
	else  /* ����״̬ */
	{
		/* ����"�Զ�����"ģʽ */
		EnterAutoMeasureMode();
		
		/* ����ADC */
		ADCCON |= (1<<0);

		//printf("continue adc measure\n\r");
	}
}

void IsrAdc(void)
{
	int x = ADCDAT0;
	int y = ADCDAT1;

	static int s_iAdcCnt = 0;
	static int s_iAdcX = 0;
	static int S_iAdcY = 0;

	/* ����ADC�ж�ʱ, TS����"�Զ�����ģʽ" */

	/* ֻ����"�ȴ��ж�ģʽ"�²ſ���ʹ��ADCDAT0'BIT 15���жϴ�����״̬ */

	EnterWaitPenUpMode();
	
	if (!(ADCDAT0 & (1<<15))) /* �����Ȼ���²Ŵ�ӡ */
	{			
		/* ��1������ADC��:
		 *   a. Ҫ��������N��, ���N������, ��ƽ��ֵ���ϱ�
		 *   b. �õ�N�����ݺ�, ������TIMER 
		 */
		s_iAdcX += (x & 0x3ff);
		S_iAdcY += (y & 0x3ff);

		s_TestXArray[s_iAdcCnt] = (x & 0x3ff);
		s_TestYArray[s_iAdcCnt] = (y & 0x3ff);
		
		s_iAdcCnt++;

		if (s_iAdcCnt == 16)
		{
			s_iAdcX >>= 4;
			S_iAdcY >>= 4;
			
			//printf("Isr_Adc adc_x = %08d,adc_y = %08d\n\r",s_iAdcX,S_iAdcY);
			ReportTsXY(s_iAdcX, S_iAdcY, 1);

			s_iAdcCnt = 0;
			s_iAdcX = 0;
			S_iAdcY = 0;
			
			/* ������ʱ�����ٴζ�ȡ���� */
			/* ������TS����"�ȴ��ж�ģʽ" */
			EnterWaitPenUpMode();
			EnableTsTimer();
		}
		else
		{
			/* �ٴ�����ADC */
			/* ����"�Զ�����"ģʽ */
			EnterAutoMeasureMode();
			
			/* ����ADC */
			ADCCON |= (1<<0);
		}
		
	}
	else
	{
		s_iAdcCnt = 0;
		s_iAdcX = 0;
		S_iAdcY = 0;
		DisableTsTimer();
		//printf("timer disable\n\r");
		EnterWaitPenDownMode();
		ReportTsXY(0, 0, 0);
		Delay(10);
		ReportTsXY(0, 0, 0);
	}

	//enter_wait_pen_up_mode();  /* ����ADCʱ��Ӧ�ý������ģʽ, ����Ӱ������ */

}

void AdcTsIntHandle(int irq)
{
	//Delay(100);
	if (SUBSRCPND & (1<<TC_INT_BIT))  /* ����Ǵ������ж� */
		IsrTc();

	if (SUBSRCPND & (1<<ADC_INT_BIT))  /* ADC�ж� */
		IsrAdc();
		
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);
}

void InitAdcTsInt(void)
{
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);

	/* ע���жϴ����� */
	RegisterIrq(31, AdcTsIntHandle);	

	/* ʹ���ж� */
	INTSUBMSK &= ~((1<<ADC_INT_BIT) | (1<<TC_INT_BIT));
}


void InitAdcTsReg(void)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (0<<3);

	/*  ���´�����, ��ʱһ���ٷ���TC�ж�
	 *  ��ʱʱ�� = ADCDLY * �������� = ADCDLY * 1 / 12000000 = 5ms
	 */
	ADCDLY = 60000;	
}


void InitTouchScreen(void)
{
	/* ���ô������ӿ�:�Ĵ��� */
	InitAdcTsReg();

	/* �����ж� */
	InitAdcTsInt();

	/* ע�ᶨʱ�������� */
	RegisterTimer("touchscreen", TouchscreenTimerIrq);

	/* �ô���������������"�ȴ��ж�ģʽ" */
	EnterWaitPenDownMode();
}

void print_test_array(void)
{
	int i;

	printf("test array x : ");
	for (i = 0; i < 16; i++)
		printf("%08d ", s_TestXArray[i]);
	printf("\n\r");

	printf("test array y : ");
	for (i = 0; i < 16; i++)
		printf("%08d ", s_TestYArray[i]);
	printf("\n\r");
}

//void ts_read_raw_test(int *px, int *py, int *ppressure)
//{
//	while (g_ts_data_valid == 0);
//	*px = g_ts_x;
//	*py = g_ts_y;
//	*ppressure = g_ts_pressure;
//	print_test_array();
//	g_ts_data_valid = 0;
//}

void TestTouchScreen(void)
{
	unsigned int iFbBase;
	int iXres, iYres, iBpp;

	int x, y, iPressure;
	int i;

	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	GetLcdParams(&iFbBase, &iXres, &iYres, &iBpp);

	/* ���� */
	ClearScreen(0x0);

	/* ��ʾ������ʾ�滭 */
	PrintFbString16x32(176, 5, " TS TEST", 0x7cfc00, 0);
	PrintFbString8x16(70, 70, "OK! To draw!", 0xffffff,0);
	DisplayTestItemIcon(g_sButtonReturn.iX,g_sButtonReturn.iY,g_sButtonReturn.sName,0x0);

	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}
		
		i = ReadTs(&x, &y, &iPressure);
		if (i == 0)
		{
			if (iPressure)
			{
				DrawLine(x-1, y-1,x+1,y+1, 0xff0000);
				DrawLine(x-1, y, x+1, y, 0xff0000);
				DrawLine(x-1, y+1, x+1, y+1, 0xff0000);
			}
		}
		
	}
}


