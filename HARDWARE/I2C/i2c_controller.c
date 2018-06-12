/**********************************************************************************
  * @brief      : 	i2c����������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#include "s3c2440_soc.h"
#include "function.h"
#include "i2c_controller.h"
#include "my_printf.h"
#include "string_utils.h"
#include "s3c2440_i2c_controller.h"

#define I2C_CONTROLLER_NUM 10

/* ��һ��i2c_controller����������Ÿ��ֲ�ͬоƬ�Ĳ����ṹ�� */
static PSI2cController g_psI2cControllers[I2C_CONTROLLER_NUM];
static PSI2cController g_psI2cConSelected = NULL;

/**********************************************************************************
  * @brief       : 	ע��I2C������
  * @param[in]   : 	p	��ע���I2C������
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void RegisterI2cController(PSI2cController p)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (!g_psI2cControllers[i])
		{
			g_psI2cControllers[i] = p;
			return;
		}
	}
}

/**********************************************************************************
  * @brief       : 	����������ѡ��ĳ��I2C������ 
  * @param[in]   : 	name	��ѡ���I2C����������
  * @param[out]  : 	��
  * @return      : 	0	ѡ��ɹ�
  					-1	ѡ��ʧ��
  * @others      : 	��
***********************************************************************************/
static int SelectI2cController(char *name)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (g_psI2cControllers[i] && !strcmp(name, g_psI2cControllers[i]->pchName))
		{
			g_psI2cConSelected = g_psI2cControllers[i];
			return 0;
		}
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	ʵ�� i2c_transfer �ӿں��������Դ���I2C��Ϣ
  * @param[in]   : 	msgs	�������I2C��Ϣ�������׵�ַ
  					num		�������I2C��Ϣ�ĸ���
  * @param[out]  : 	��
  * @return      : 	0	����ɹ�
  					����	�������
  * @others      : 	��
***********************************************************************************/
int TransferI2c(PSI2cMsg msgs, int num)
{
	if(g_psI2cConSelected)
	{
		return g_psI2cConSelected->MasterXfer(msgs, num);
	}
	else
	{
		return -1;
	}	
}

/**********************************************************************************
  * @brief       : 	��ʼ��I2C������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitI2c(void)
{
	/* ע�������I2C������ */
	AddS3c2440I2cCon();

	/* ѡ��ĳ��I2C������ */
	SelectI2cController("s3c2440");

	/* ��������init���� */
	g_psI2cConSelected->Init();
}

