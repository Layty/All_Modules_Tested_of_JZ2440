/**********************************************************************************
  * @brief      : 	i2c����������ͷ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***********************************************************************************/

#ifndef _I2C_CONTROLLER_H
#define _I2C_CONTROLLER_H

typedef struct SI2cMsg {
	unsigned int iAddr;  /* 7bits */
	int iFlags;  /* 0 - write, 1 - read */
	int iLen;
	int iCntTransferred;
	int iErr;
	unsigned char *pchBuf;
}SI2cMsg, *PSI2cMsg;

typedef struct SI2cController {
	int (*Init)(void);
	int (*MasterXfer)(PSI2cMsg msgs, int num);
	char *pchName;
}SI2cController, *PSI2cController;

/**********************************************************************************
  * @brief       : 	��ʼ��I2C������
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void InitI2c(void);

/**********************************************************************************
  * @brief       : 	ע��I2C������
  * @param[in]   : 	p	��ע���I2C������
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void RegisterI2cController(PSI2cController p);

/**********************************************************************************
  * @brief       : 	ʵ�� i2c_transfer �ӿں��������Դ���I2C��Ϣ
  * @param[in]   : 	msgs	�������I2C��Ϣ�������׵�ַ
  					num		�������I2C��Ϣ�ĸ���
  * @param[out]  : 	��
  * @return      : 	0	����ɹ�
  					err	�������
  * @others      : 	��
***********************************************************************************/
int TransferI2c(PSI2cMsg msgs, int num);

#endif /* _I2C_CONTROLLER_H */

