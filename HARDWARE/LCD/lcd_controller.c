/*********************************************************************************************
  * @brief      : 	JZ2440v2������LCD��������ع��ܺ�������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
**********************************************************************************************/

#include "lcd_controller.h"
#include "s3c2440_lcd_controller.h"
#include "string_utils.h"


#define LCD_CONTROLLER_NUM 10     //lcd�������ܸ���

static PSLcdController g_psArrayLcdController[LCD_CONTROLLER_NUM]; //lcd���������� 
static PSLcdController g_pSLcdControllerSelected;	//��ǰ��ѡ�е�lcd������

/**********************************************************************************
  * @brief       : 	ע��LCD������
  * @param[in]   : 	plcdcon 	��ע���LCD�������ṹ��ָ��
  * @param[out]  : 	��
  * @return      : 	i	ע��ɹ���i��ʾ��LCD��������LCD�����������е�����
  					-1	ע��ʧ��
  * @others      : 	��
***********************************************************************************/
int RegisterLcdController(PSLcdController plcdcon)
{
	int i;
	for (i = 0; i < LCD_CONTROLLER_NUM; i++)
	{
		if (!g_psArrayLcdController[i])
		{
			g_psArrayLcdController[i] = plcdcon;
			return i;
		}
	}
	return -1;		
}

/**********************************************************************************
  * @brief       : 	��������ѡ��ָ����LCD������
  * @param[in]   : 	name 	��ע���LCD������������
  * @param[out]  : 	��
  * @return      : 	i	ѡ��ɹ���i��ʾ��ѡ���LCD��������LCD�����������е�����
  					-1	ѡ��ʧ��
  * @others      : 	��
***********************************************************************************/
int SelectLcdController(char *name)
{
	int i;
	for (i = 0; i < LCD_CONTROLLER_NUM; i++)
	{
		if (g_psArrayLcdController[i] && !strcmp(g_psArrayLcdController[i]->pchName, name))
		{
			g_pSLcdControllerSelected = g_psArrayLcdController[i];
			return i;
		}
	}
	return -1;		
}


/**********************************************************************************
  * @brief       : 	����ָ����LCD��������ʼ����ǰ��LCD������
  * @param[in]   : 	plcdparams		LCD�����ṹ��ָ��
  * @param[out]  : 	��
  * @return      : 	0	�ɹ�
  					-1	ʧ��
  * @others      : 	��
***********************************************************************************/
int InitLcdController(PSLcdParams plcdparams)
{
	/* ������ѡ���LCD�������ĳ�ʼ������ */
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Init(plcdparams);
		return 0;
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	ʹ�ܵ�ǰ��LCD������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void EnableLcdController(void)
{
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Enable();
	}
}

/**********************************************************************************
  * @brief       : 	�رյ�ǰ��LCD������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void DisableLcdController(void)
{
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Disable();
	}
}

/**********************************************************************************
  * @brief       : 	���S3c2440 LCD������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void AddLcdContollerS3c2440(void)
{
	AddS3c2440LcdContoller();
}

