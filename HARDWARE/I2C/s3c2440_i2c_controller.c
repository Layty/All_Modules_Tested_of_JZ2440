/****************************************************************************************************
  * @brief      : 	JZ2440v2������i2c����������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
*****************************************************************************************************/

#include "i2c_controller.h"
#include "s3c2440_soc.h"
#include "timer.h"
#include "my_printf.h"
#include "string_utils.h"
#include "interrupt.h"


static PSI2cMsg g_psCurMsg;			//��ǰ�����I2C��Ϣ


/**********************************************************************************
  * @brief       : 	�ж��Ƿ�Ϊ���һ��I2C����
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	1	�����һ��I2C����
  					0	�����һ��I2C����
  * @others      : 	��
***********************************************************************************/
static int isLastData(void)
{
	if (g_psCurMsg->iCntTransferred == g_psCurMsg->iLen - 1)
		return 1;  /* ��Ҫ��ʼ�������һ������ */
	else 
		return 0;
}

/**********************************************************************************
  * @brief       : 	���¿������ظ�һ��ack�ź�
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void ResumeIicWithAck(void)
{
	unsigned int iiccon = IICCON;
	iiccon |= (1<<7); /* ��ӦACK */
	iiccon &= ~(1<<4); /* �ָ�IIC���� */
	IICCON =  iiccon;
}

/**********************************************************************************
  * @brief       : 	���¿��������ظ�һ��ack�ź�
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
static void ResumeIicWithoutAck(void)
{
	unsigned int iiccon = IICCON;
	iiccon &= ~((1<<7) | (1<<4)); /* ����ӦACK, �ָ�IIC���� */
	IICCON =  iiccon;
}

/**********************************************************************************
  * @brief       : 	S3C2440 I2C�жϴ�����
  * @param[in]   : 	irq	�жϺ�
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void I2cInterruptFunc(int irq)
{
	int index;
	Delay(1000);
	unsigned int iicstat = IICSTAT;
	//unsigned int iiccon;

	//printf("i2c_interrupt_func! flags = %d\n\r", g_psCurMsg->iFlags);
	
	g_psCurMsg->iCntTransferred++;
	
	//printf("i2c_interrupt_func! g_psCurMsg->iCntTransferred = %d\n\r", g_psCurMsg->iCntTransferred);
	/* ÿ������һ�����ݽ�����һ���ж� */

	/* ����ÿ�δ���, ��1���ж���"�Ѿ��������豸��ַ" */

	if (g_psCurMsg->iFlags == 0)	/* write */
	{
		/* ���ڵ�1���ж�, ���Ƿ��ͳ��豸��ַ�������
		 * ��Ҫ�ж��Ƿ���ACK
		 * ��ACK : �豸����
		 * ��ACK : ���豸, ����, ֱ�ӽ�������
		 */
		if (g_psCurMsg->iCntTransferred == 0)  /* ��1���ж� */
		{
			if (iicstat & (1<<0))
			{ /* no ack */
				/* ֹͣ���� */
				IICSTAT = 0xd0;
				IICCON &= ~(1<<4);
				g_psCurMsg->iErr = -1;
				printf("tx err, no ack\n\r");
				return;
			}
		}

		if (g_psCurMsg->iCntTransferred < g_psCurMsg->iLen)
		{
			/* ���������ж�, Ҫ����������һ������
			 */
			IICDS = g_psCurMsg->pchBuf[g_psCurMsg->iCntTransferred];
			IICCON &= ~(1<<4);
		}
		else
		{
			/* ֹͣ���� */
			IICSTAT = 0xd0;
			IICCON &= ~(1<<4);
			Delay(1000);
		}
	}
	else /* read */
	{
		/* ���ڵ�1���ж�, ���Ƿ��ͳ��豸��ַ�������
		 * ��Ҫ�ж��Ƿ���ACK
		 * ��ACK : �豸����, �ָ�I2C����, ��������һ���жϲſ��Եõ���1������
		 * ��ACK : ���豸, ����, ֱ�ӽ�������
		 */
		if (g_psCurMsg->iCntTransferred == 0)  /* ��1���ж� */
		{
			if (iicstat & (1<<0))
			{ /* no ack */
				/* ֹͣ���� */
				IICSTAT = 0x90;
				IICCON &= ~(1<<4);
				g_psCurMsg->iErr = -1;
				printf("rx err, no ack\n\r");
				return;
			}
			else  /* ack */
			{
				/* ��������һ������, ��������ʱҪ����Ϊ����ӦACK */
				/* �ָ�I2C���� */
				if (isLastData())
				{
					ResumeIicWithoutAck();
				}
				else
				{
					ResumeIicWithAck();
				}
				return;
			}
		}

		/* �ǵ�1���ж�, ��ʾ�õ���һ��������
		 * ��IICDS����������
		 */
		if (g_psCurMsg->iCntTransferred < g_psCurMsg->iLen)
		{
			index = g_psCurMsg->iCntTransferred - 1;
			g_psCurMsg->pchBuf[index] = IICDS;

			/* ��������һ������, ��������ʱҪ����Ϊ����ӦACK */
			/* �ָ�I2C���� */
			if (isLastData())
			{
				ResumeIicWithoutAck();
			}
			else
			{
				ResumeIicWithAck();
			}
		}
		else
		{
			/* ����ֹͣ�ź� */
			IICSTAT = 0x90;
			IICCON &= ~(1<<4);
			Delay(1000);
		}
	}
}


/**********************************************************************************
  * @brief       : 	��ʼ��S3C2440 I2C�жϿ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	0	��ʵ������
  * @others      : 	��
***********************************************************************************/
static int InitS3c2440I2cCon(void)
{
	/* ������������I2C*/
	GPECON &= ~((3<<28) | (3<<30));
	GPECON |= ((2<<28) | (2<<30));
	
	/* ����ʱ�� */
	/* [7] : IIC-bus acknowledge enable bit, 1-enable in rx mode
	 * [6] : ʱ��Դ, 0: IICCLK = fPCLK /16; 1: IICCLK = fPCLK /512
	 * [5] : 1-enable interrupt
	 * [4] : ����Ϊ1ʱ��ʾ�жϷ�����, д��0��������ָ�I2C����
	 * [3:0] : Tx clock = IICCLK/(IICCON[3:0]+1).
	 * Tx Clock = 100khz = 50Mhz/16/(IICCON[3:0]+1)
	 */
	IICCON = (1<<7) | (0<<6) | (1<<5) | (30<<0);

	/* ע���жϴ����� */
	RegisterIrq(27, I2cInterruptFunc);

	return 0;
}

/**********************************************************************************
  * @brief       : 	S3C2440 I2C��������
  * @param[in]   : 	msg	��Ҫ���͵�I2C�ź�
  * @param[out]  : 	��
  * @return      : 	0	���ͳɹ�
  					-1	����ʧ��
  * @others      : 	��
***********************************************************************************/
static int DoMasterTx(PSI2cMsg msg)
{
	g_psCurMsg = msg;
	
	msg->iCntTransferred = -1;
	msg->iErr = 0;
	
	/* ���üĴ����������� */
	/* 1. ����Ϊ master tx mode */
	IICCON |= (1<<7); /* TX mode, ��ACK�����ͷ�SDA */
	IICSTAT = (1<<4);
		
	/* 2. �Ѵ��豸��ַд��IICDS */
	IICDS = msg->iAddr<<1;

	//printf("msg->iAddr = %d\n\r",msg->iAddr);
	
	/* 3. IICSTAT = 0xf0 , ���ݼ������ͳ�ȥ, �������жϲ��� */
	IICSTAT = 0xf0;
	
	/* �����Ĵ������ж����� */

	/* ѭ���ȴ��жϴ������ */
	while (!msg->iErr && msg->iCntTransferred != msg->iLen);

	if (msg->iErr)
	{
		printf("msg->iErr = %d\n\r",msg->iErr);
		return -1;
	}
	else
	{
		printf("msg->iErr = %d\n\r",msg->iErr);
		return 0;
	}
}

/**********************************************************************************
  * @brief       : 	S3C2440 I2C��������
  * @param[in]   : 	��
  * @param[out]  : 	msg	���ݽ��մ�ŵ�I2C��Ϣ�ṹ��
  * @return      : 	0	���ճɹ�
  					-1	����ʧ��
  * @others      : 	��
***********************************************************************************/
static int DoMasterRx(PSI2cMsg msg)
{
	g_psCurMsg = msg;

	msg->iCntTransferred = -1;
	msg->iErr = 0;
	
	/* ���üĴ����������� */
	/* 1. ����Ϊ Master Rx mode */
	IICCON |= (1<<7); /* RX mode, ��ACK���ڻ�ӦACK */
	IICSTAT = (1<<4);
		
	/* 2. �Ѵ��豸��ַд��IICDS */
	IICDS = (msg->iAddr<<1)|(1<<0);

	//printf("msg->iAddr = %d\n\r",msg->iAddr);
	
	/* 3. IICSTAT = 0xb0 , ���豸��ַ�������ͳ�ȥ, �������жϲ��� */
	IICSTAT = 0xb0;
	

	/* �����Ĵ������ж����� */

	/* ѭ���ȴ��жϴ������ */
	while (!msg->iErr && msg->iCntTransferred != msg->iLen);

	if (msg->iErr)
		return -1;
	else
		return 0;
}

/**********************************************************************************
  * @brief       : 	S3C2440 I2C�������ͺ���
  * @param[in]   : 	msg	���ݴ��͵�I2C��Ϣ�ṹ�建����ָ��
  					num	���ݴ��͵�I2C��Ϣ�ṹ�����
  * @param[out]  : 	��
  * @return      : 	0	���ͳɹ�
  					err	����ʧ��
  * @others      : 	��
***********************************************************************************/
static int S3c2440MasterXfer(PSI2cMsg msgs, int num)
{
	int i;
	int err;
	
	for (i = 0; i < num; i++)	
	{
		if (msgs[i].iFlags == 0)/* write */
			err = DoMasterTx(&msgs[i]);
		else
			err = DoMasterRx(&msgs[i]);
		if (err)
			return err;
	}
	return 0;
}

/* ʵ��   i2c_controller
          .init
          .master_xfer
          .name
 */

static SI2cController sS3c2440I2cCon = {
	.pchName = "s3c2440",
	.Init = InitS3c2440I2cCon,
	.MasterXfer = S3c2440MasterXfer,
};

/**********************************************************************************
  * @brief       : 	S3C2440 I2C������ע�ắ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void AddS3c2440I2cCon(void)
{
	RegisterI2cController(&sS3c2440I2cCon);
}

