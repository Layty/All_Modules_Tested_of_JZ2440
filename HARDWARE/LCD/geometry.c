/**********************************************************************************
  * @brief      : 	JZ2440v2������LCD���Ƽ���ͼ�����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "framebuffer.h"
#include "geometry.h"

 //-------------��Բ������������Բ�ģ��뾶����ɫ----------	
 // 	   ��1/8Բ Ȼ������7/8�Գƻ�  
 // 		 ---------------->X  
 // 		 |(0,0)   0  
 // 		 |	   7	 1	
 // 		 |	  6 	  2  
 // 		 |	   5	 3	
 // 	  (Y)V		  4  
 //  
 // 	 L = x^2 + y^2 - r^2  
 /**********************************************************************************
  * @brief       : 	��LCD��ָ��λ�û���һ���ض��뾶�ض���ɫ��Բ
  * @param[in]   : 	x	���Ƶ�Բ��x����
  					y	���Ƶ�Բ��y����
  					r	���Ƶ�Բ�İ뾶
  					color	���Ƶ�Բ����ɫ
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
 void DrawCircle(int x, int y, int r, int color)  
 {	
	 int a, b, num;  
	 a = 0;  
	 b = r;  
	 while(22 * b * b >= r * r) 		 // 1/8Բ����  
	 {	
		 PutFbPixel(x + a, y - b,color); // 0~1  
		 PutFbPixel(x - a, y - b,color); // 0~7  
		 PutFbPixel(x - a, y + b,color); // 4~5  
		 PutFbPixel(x + a, y + b,color); // 4~3  
   
		 PutFbPixel(x + b, y + a,color); // 2~3  
		 PutFbPixel(x + b, y - a,color); // 2~1  
		 PutFbPixel(x - b, y - a,color); // 6~7  
		 PutFbPixel(x - b, y + a,color); // 6~5  
		   
		 a++;  
		 num = (a * a + b * b) - r*r;  
		 if(num > 0)  
		 {	
			 b--;  
			 a--;  
		 }	
	 }	
 }	
   
 /**********************************************************************************
   * @brief 	  :  ��LCD�л���һ��ָ����ʼ����ض���ɫ������
   * @param[in]   :  x1		��������x����
   					 y1		��������y����
   					 x2		�����������x����
   					 y2		�����������y����
   					 color	��������ɫ
   * @param[out]  :  ��
   * @return	  :  ��
   * @others	  :  ��
 ***********************************************************************************/
 void DrawLine(int x1,int y1,int x2,int y2,int color)	
 {	
	 int dx,dy,e;  
	 dx=x2-x1;	 
	 dy=y2-y1;	
	 if(dx>=0)	
	 {	
		 if(dy >= 0) // dy>=0  
		 {	
			 if(dx>=dy) // 1/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1<=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1+=1;e-=dx;}	   
					 x1+=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 2/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1<=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1+=1;e-=dy;}	   
					 y1+=1;  
					 e+=dx;  
				 }	
			 }	
		 }	
		 else			// dy<0  
		 {	
			 dy=-dy;   // dy=abs(dy)  
			 if(dx>=dy) // 8/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1<=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1-=1;e-=dx;}	   
					 x1+=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 7/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1>=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1+=1;e-=dy;}	   
					 y1-=1;  
					 e+=dx;  
				 }	
			 }	
		 }	   
	 }	
	 else //dx<0  
	 {	
		 dx=-dx;	 //dx=abs(dx)  
		 if(dy >= 0) // dy>=0  
		 {	
			 if(dx>=dy) // 4/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1>=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1+=1;e-=dx;}	   
					 x1-=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 3/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1<=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1-=1;e-=dy;}	   
					 y1+=1;  
					 e+=dx;  
				 }	
			 }	
		 }	
		 else			// dy<0  
		 {	
			 dy=-dy;   // dy=abs(dy)  
			 if(dx>=dy) // 5/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1>=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1-=1;e-=dx;}	   
					 x1-=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 6/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1>=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1-=1;e-=dy;}	   
					 y1-=1;  
					 e+=dx;  
				 }	
			 }	
		 }	   
	 }	
 }	

 /**********************************************************************************
   * @brief 	  :  ����У׼���������õ����������ʮ�ּ�
   * @param[in]   :  x	ʮ�ּܵ�x����
   					 y	ʮ�ּܵ�y����
   					 color	ʮ�ּܵ���ɫ
   * @param[out]  :  ��
   * @return	  :  ��
   * @others	  :  ��
 ***********************************************************************************/
void FbDispCross(int x, int y, unsigned int color)
{
	DrawLine(x-10, y, x+10, y, color);
	DrawLine(x, y-10, x, y+10, color);
}

/********************************************************************************************
  * @brief		 :	��LCD��ָ��λ�û���һ��ָ����С��ָ����ɫ�ľ���
  * @param[in]	 :	x  ���ε�x����
					y  ���ε�y����
					color  ���ε���ɫ
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
*********************************************************************************************/
void DrawRectangle(int x,int y,int length,int high,int color)
{
	int i;
	for(i = 0; i < 3 ; i++)
	{
		DrawLine(x,y+i,x+length,y+i,color);
		DrawLine(x+length-i,y,x+length-i,y+high,color);
		DrawLine(x+length,y+high-i,x,y+high-i,color);
		DrawLine(x+i,y+high,x+i,y,color);
	}	
}

