/****************************************************************************************************
  * @brief      : 	JZ2440v2�����崮�ڴ�ӡ�{ԇ����Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/ 

#include  "my_printf.h"

//==================================================================================================
typedef char *  va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)( ap=ap + _INTSIZEOF(t), ap- _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )

//==================================================================================================
unsigned char hex_tab[]={'0','1','2','3','4','5','6','7',\
		                 '8','9','a','b','c','d','e','f'};

 /**********************************************************************************
   * @brief 	  :  �򴮿�ݔ��һ���ַ�
   * @param[in]   :  c	�ַ��ı���
   * @param[out]  :  ��
   * @return	  :  0	��ʵ������
   * @others	  :  ��
 ***********************************************************************************/
static int outc(int c) 
{
	__out_putchar(c);
	return 0;
}

/**********************************************************************************
  * @brief		 :	�򴮿�ݔ��һ���ַ���
  * @param[in]	 :	s  �ַ����׵�ַ
  * @param[out]  :	��
  * @return 	 :	0  ��ʵ������
  * @others 	 :	��
***********************************************************************************/
static int outs (const char *s)
{
	while (*s != '\0')	
		__out_putchar(*s++);
	return 0;
}

/**********************************************************************************
  * @brief		 :	�򴮿�ݔ��һ������
  * @param[in]	 :	n  �����������
  					base	����Ľ���
  					lead	��λ���ַ�
  					maxwidth	����Ŀ��
  * @param[out]  :	��
  * @return 	 :	0  ��ʵ������
  * @others 	 :	��
***********************************************************************************/
static int out_num(long n, int base,char lead,int maxwidth) 
{
	unsigned long m=0;
	char buf[MAX_NUMBER_BYTES], *s = buf + sizeof(buf);
	int count=0,i=0;
			

	*--s = '\0';
	
	if (n < 0){
		m = -n;
	}
	else{
		m = n;
	}
	
	do{
		*--s = hex_tab[m%base];
		count++;
	}while ((m /= base) != 0);
	
	if( maxwidth && count < maxwidth)
	{
		for (i=maxwidth - count; i; i--)	
			*--s = lead;
	}

	if (n < 0)
		*--s = '-';
	
	return outs(s);
}
   

/*reference :   int vprintf(const char *format, va_list ap); */
/**********************************************************************************
  * @brief		 :	�򴮿�ݔ����׼���ַ���
  * @param[in]	 :	fmt  	����ʽ���ַ���
  					ap	�����б�
  * @param[out]  :	��
  * @return 	 :	0  ��ʵ������
  * @others 	 :	��
***********************************************************************************/
static int my_vprintf(const char *fmt, va_list ap) 
{
	char lead=' ';
	int  maxwidth=0;
	
	 for(; *fmt != '\0'; fmt++)
	 {
		if (*fmt != '%') 
		{
			outc(*fmt);
			continue;
		}
			
		//format : %08d, %8d,%d,%u,%x,%f,%c,%s 
		    fmt++;
		if(*fmt == '0')
		{
			lead = '0';
			fmt++;	
		}

		lead=' ';
		maxwidth=0;
		
		while(*fmt >= '0' && *fmt <= '9')
		{
			maxwidth *=10;
			maxwidth += (*fmt - '0');
			fmt++;
		}
		
			switch (*fmt) 
				{
					case 'd': out_num(va_arg(ap, int),          10,lead,maxwidth); break;
					case 'o': out_num(va_arg(ap, unsigned int),  8,lead,maxwidth); break;				
					case 'u': out_num(va_arg(ap, unsigned int), 10,lead,maxwidth); break;
					case 'x': out_num(va_arg(ap, unsigned int), 16,lead,maxwidth); break;
					case 'c': outc(va_arg(ap, int   )); break;		
					case 's': outs(va_arg(ap, char *)); break;		  		
				
					default:  
						outc(*fmt);
						break;
			}
	}
	return 0;
}


//reference :  int printf(const char *format, ...); 
/**********************************************************************************
  * @brief		 :	�򴮿�ݔ����׼���ַ����������������
  * @param[in]	 :	fmt  	����ʽ���ַ���
  					......	�����
  * @param[out]  :	��
  * @return 	 :	0  ��ʵ������
  * @others 	 :	��
***********************************************************************************/

int printf(const char *fmt, ...) 
{
	va_list ap;

	va_start(ap, fmt);
	my_vprintf(fmt, ap);	
	va_end(ap);
	return 0;
}

/**********************************************************************************
  * @brief		 :	�򴮿�ݔ��һ��ʮ�����Ƶ���
  * @param[in]	 :	val	�����������
  * @param[out]  :	��
  * @return 	 :	��
  * @others 	 :	��
***********************************************************************************/
void printHex(unsigned int val)
{
	int i;
	unsigned char arr[8];

	/* ��ȡ��ÿһλ��ֵ */
	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   /* arr[0] = 2, arr[1] = 1, arr[2] = 0xF */
	}

	/* ��ӡ */
	Puts("0x");
	for (i = 7; i >=0; i--)
	{
		if (arr[i] <= 9)
			PutChar(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			PutChar(arr[i] - 0xA + 'A');
	}
}

