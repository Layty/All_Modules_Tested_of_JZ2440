#include "geometry.h"
#include "tslib.h"
#include "my_printf.h"
#include "touchscreen.h"


static volatile double gs_dKx;
static volatile double gs_dKy;

static volatile int gs_iTsXc, gs_iTsYc;
static volatile int gs_iLcdXc, gs_iLcdYc;
static volatile int gs_TsXYSwap = 0;


static unsigned int gs_uiFbBase;
static int gs_iXres, gs_iYres, gs_iBpp;

int GetLcdXFrmTsX(int ts_x)
{
	return gs_dKx * (ts_x - gs_iTsXc) + gs_iLcdXc;
}

int GetLcdYFrmTsY(int ts_y)
{
	return gs_dKy * (ts_y - gs_iTsYc) + gs_iLcdYc;
}

static int SeekDifference(int x, int y)
{
	int iTemp = x - y;

	if(iTemp > 0)
		return iTemp;
	else
		return iTemp*(-1);
}

int SetTsCalibrateParams(PSTsParameters tsparam)
{

	/* X�᷽�� */
	int ts_s1, ts_s2;
	int lcd_s;

	/* Y�᷽�� */
	int ts_d1, ts_d2;
	int lcd_d;

	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	GetLcdParams(&gs_uiFbBase, &gs_iXres, &gs_iYres, &gs_iBpp);

	ts_s1 = tsparam->iB_ts_x - tsparam->iA_ts_x;
	ts_s2 = tsparam->iC_ts_x - tsparam->iD_ts_x;
	lcd_s = gs_iXres-50-50;

	ts_d1 = tsparam->iD_ts_y - tsparam->iA_ts_y;
	ts_d2 = tsparam->iC_ts_y - tsparam->iB_ts_y;
	lcd_d = gs_iYres-50-50;

	gs_dKx = ((double)(2*lcd_s)) / (ts_s1 + ts_s2);
	gs_dKy = ((double)(2*lcd_d)) / (ts_d1 + ts_d2);

	gs_iTsXc = tsparam->iE_ts_x;
	gs_iTsYc = tsparam->iE_ts_y;

	gs_iLcdXc = gs_iXres/2;
	gs_iLcdYc = gs_iYres/2;

	gs_TsXYSwap = tsparam->iTsXYSwap;

	
	printf("A lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(tsparam->iA_ts_x), GetLcdYFrmTsY(tsparam->iA_ts_y));
	printf("B lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(tsparam->iB_ts_x), GetLcdYFrmTsY(tsparam->iB_ts_y));
	printf("C lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(tsparam->iC_ts_x), GetLcdYFrmTsY(tsparam->iC_ts_y));
	printf("D lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(tsparam->iD_ts_x), GetLcdYFrmTsY(tsparam->iD_ts_y));
	printf("E lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(tsparam->iE_ts_x), GetLcdYFrmTsY(tsparam->iE_ts_y));

	return 0;
}


void GetCalibratePointData(int lcd_x, int lcd_y, int *px, int *py)
{
	int iPressure = 0;
	int iPreX  = -1;
	int iPreY  = -1;
	int x, y;
	int iSumX = 0, iSumY = 0;
	int iCnt = 0;
	
	FbDispCross(lcd_x, lcd_y, 0xffffff);

	/* �ȴ���� */

	do {
		ReadTsRaw(&x, &y, &iPressure); 	
	} while (!iPressure); //Ŀ���ǵȴ�����

	do {
		
		ReadTsRaw(&x, &y, &iPressure);

		if((iPreX!= -1)&&(iPreY!= -1))
		{
			if(SeekDifference(x,iPreX)>50 || SeekDifference(y,iPreY)>50)
				continue;
		}
		
		iPreX = x;
		iPreY = y;
		if (iCnt < 128)
		{
			iSumX += x;
			iSumY += y;
			iCnt++;
		}
		//printf("get_calibrate_point_data get raw data: x = %08d, y = %08d, cnt = %d, pressure = %08d\n\r", x, y, iCnt,iPressure);
	} while (iPressure);

	*px = iSumX / iCnt;
	*py = iSumY / iCnt;

	//printf("get_calibrate_point_data return raw data: px = %08d, py = %08d\n\r", *px, *py);

	/* ֱ���ɿ��ŷ��� */
	FbDispCross(lcd_x, lcd_y, 0x0);
}


int isTsXYSwap(int a_ts_x, int a_ts_y, int b_ts_x, int b_ts_y)
{
	int iDx = b_ts_x - a_ts_x;
	int iDy = b_ts_y - a_ts_y;

	if (iDx < 0)
		iDx = 0 - iDx;
	if (iDy < 0)
		iDy = 0 - iDy;

	if(iDx > iDy)
		return 0; /* xyû�з�ת */
	else
		return 1; /* xy���� */
}

void SwapXY(int *px, int *py)
{
	int iTmp = *px;
	*px = *py;
	*py = iTmp;
}

/*
----------------------------
|                          |
|  +(A)              (B)+  |
|                          |
|                          |
|                          |
|            +(E)          |
|                          |
|                          |
|                          |
|  +(D)              (C)+  |
|                          |
----------------------------

*/

int CalibrateTs(PSTsParameters tsparam)
{

	int a_ts_x, a_ts_y;
	int b_ts_x, b_ts_y;
	int c_ts_x, c_ts_y;
	int d_ts_x, d_ts_y;
	int e_ts_x, e_ts_y;

	/* X�᷽�� */
	int ts_s1, ts_s2;
	int lcd_s;

	/* Y�᷽�� */
	int ts_d1, ts_d2;
	int lcd_d;

	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	GetLcdParams(&gs_uiFbBase, &gs_iXres, &gs_iYres, &gs_iBpp);

	/* ����ABCDE, ѭ��: ��ʾ"+"���������tsԭʼֵ */
	/* A(50, 50) */
	GetCalibratePointData(50, 50, &a_ts_x, &a_ts_y);
	printf("a_ts_x = %08d, a_ts_y = %08d\n\r", a_ts_x, a_ts_y);

	/* B(xres-50, 50) */
	GetCalibratePointData(gs_iXres-50, 50, &b_ts_x, &b_ts_y);
	printf("b_ts_x = %08d, b_ts_y = %08d\n\r", b_ts_x, b_ts_y);

	/* C(xres-50, yres-50) */
	GetCalibratePointData(gs_iXres-50, gs_iYres-50, &c_ts_x, &c_ts_y);
	printf("c_ts_x = %08d, c_ts_y = %08d\n\r", c_ts_x, c_ts_y);

	/* D(50, yres-50) */
	GetCalibratePointData(50, gs_iYres-50, &d_ts_x, &d_ts_y);
	printf("d_ts_x = %08d, d_ts_y = %08d\n\r", d_ts_x, d_ts_y);
	
	/* E(xres/2, yres/2) */
	GetCalibratePointData(gs_iXres/2, gs_iYres/2, &e_ts_x, &e_ts_y);
	printf("e_ts_x = %08d, e_ts_y = %08d\n\r", e_ts_x, e_ts_y);

	/* ȷ������������XY�Ƿ�ת */
	gs_TsXYSwap = isTsXYSwap(a_ts_x, a_ts_y, b_ts_x, b_ts_y);
	printf("gs_TsXYSwap = %08d\n\r", gs_TsXYSwap);

	if (gs_TsXYSwap)
	{
		/* �Ե����е��XY���� */
		SwapXY(&a_ts_x, &a_ts_y);
		SwapXY(&b_ts_x, &b_ts_y);
		SwapXY(&c_ts_x, &c_ts_y);
		SwapXY(&d_ts_x, &d_ts_y);
		SwapXY(&e_ts_x, &e_ts_y);
	}

	/* ȷ����ʽ�Ĳ��������� */

	
	printf("000 a_ts_x = %08d, a_ts_y = %08d\n\r", a_ts_x, a_ts_y);	
	printf("000 b_ts_x = %08d, b_ts_y = %08d\n\r", b_ts_x, b_ts_y);
	printf("000 c_ts_x = %08d, c_ts_y = %08d\n\r", c_ts_x, c_ts_y);
	printf("000 d_ts_x = %08d, d_ts_y = %08d\n\r", d_ts_x, d_ts_y);
	printf("000 e_ts_x = %08d, e_ts_y = %08d\n\r", e_ts_x, e_ts_y);
	
	ts_s1 = b_ts_x - a_ts_x;
	ts_s2 = c_ts_x - d_ts_x;
	lcd_s = gs_iXres-50-50;
	printf("ts_s1 = %08d ts_s2 = %08d lcd_s = %08d\n\r", ts_s1, ts_s2,lcd_s);

	ts_d1 = d_ts_y - a_ts_y;
	ts_d2 = c_ts_y - b_ts_y;
	lcd_d = gs_iYres-50-50;
	printf("ts_d1 = %08d ts_d2 = %08d lcd_d = %08d\n\r", ts_d1, ts_d2,lcd_d);

	gs_dKx = ((double)(2*lcd_s)) / (ts_s1 + ts_s2);
	gs_dKy = ((double)(2*lcd_d)) / (ts_d1 + ts_d2);

	gs_iTsXc = e_ts_x;
	gs_iTsYc = e_ts_y;

	gs_iLcdXc = gs_iXres/2;
	gs_iLcdYc = gs_iYres/2;

	printf("A lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(a_ts_x), GetLcdYFrmTsY(a_ts_y));
	printf("B lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(b_ts_x), GetLcdYFrmTsY(b_ts_y));
	printf("C lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(c_ts_x), GetLcdYFrmTsY(c_ts_y));
	printf("D lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(d_ts_x), GetLcdYFrmTsY(d_ts_y));
	printf("E lcd_x = %08d, lcd_y = %08d\n\r", GetLcdXFrmTsX(e_ts_x), GetLcdYFrmTsY(e_ts_y));

	if(GetLcdXFrmTsX(a_ts_x)<40 || GetLcdXFrmTsX(a_ts_x)>60)
		return 0;
	else if(GetLcdYFrmTsY(a_ts_y)<40 || GetLcdYFrmTsY(a_ts_y)>60)
		return 0;
	else if(GetLcdXFrmTsX(b_ts_x)<420 || GetLcdXFrmTsX(b_ts_x)>440)
		return 0;
	else if(GetLcdYFrmTsY(b_ts_y)<40 || GetLcdYFrmTsY(b_ts_y)>60)
		return 0;
	else if(GetLcdXFrmTsX(c_ts_x)<420 || GetLcdXFrmTsX(c_ts_x)>440)
		return 0;
	else if(GetLcdYFrmTsY(c_ts_y)<212 || GetLcdYFrmTsY(c_ts_y)>232)
		return 0;
	else if(GetLcdXFrmTsX(d_ts_x)<40 || GetLcdXFrmTsX(d_ts_x)>60)
		return 0;
	else if(GetLcdYFrmTsY(d_ts_y)<212 || GetLcdYFrmTsY(d_ts_y)>232)
		return 0;
	else if(GetLcdXFrmTsX(e_ts_x)<230 || GetLcdXFrmTsX(e_ts_x)>250)
		return 0;
	else if(GetLcdYFrmTsY(e_ts_y)<126 || GetLcdYFrmTsY(e_ts_y)>146)
		return 0;
	else
	{
		tsparam->iA_ts_x = a_ts_x;
		tsparam->iA_ts_y = a_ts_y;
		tsparam->iB_ts_x = b_ts_x;
		tsparam->iB_ts_y = b_ts_y;
		tsparam->iC_ts_x = c_ts_x;
		tsparam->iC_ts_y = c_ts_y;
		tsparam->iD_ts_x = d_ts_x;
		tsparam->iD_ts_y = d_ts_y;
		tsparam->iE_ts_x = e_ts_x;
		tsparam->iE_ts_y = e_ts_y;
		tsparam->iTsXYSwap = gs_TsXYSwap;
		return 1;
	}
}

/*
 * ��TSԭʼ����, ת��ΪLCD����
 */
int ReadTs(int *lcd_x, int *lcd_y, int *lcd_pressure)
{
	int ts_x = 0, ts_y = 0, ts_pressure = 0;
	int tmp_x, tmp_y;
	
	ReadTsRaw(&ts_x, &ts_y, &ts_pressure);

	if(ts_x == 0 && ts_y == 0 && ts_pressure == 0)
	{
		*lcd_x = 0;
		*lcd_y = 0;
		*lcd_pressure = 0;
		return 1;	
	}

	if (gs_TsXYSwap)
	{
		SwapXY(&ts_x, &ts_y);
	}

	//printf(" XXXX ts_x = %d, ts_y = %d\n\r", ts_x, ts_y);

	/* ʹ�ù�ʽ���� */
	tmp_x = gs_dKx * (ts_x - gs_iTsXc) + gs_iLcdXc;
	tmp_y = gs_dKy * (ts_y - gs_iTsYc) + gs_iLcdYc;

	
	if (tmp_x < 0 || tmp_x >= gs_iXres || tmp_y < 0 || tmp_y >= gs_iYres)
	{
		return -1;
	}

	printf(" tmp_x = %d, tmp_y = %d ts_pressure = %d\n\r", tmp_x, tmp_y,ts_pressure);	
	
	*lcd_x = tmp_x;
	*lcd_y = tmp_y;
	*lcd_pressure = ts_pressure;
	return 0;
}




