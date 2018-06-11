/**************************************************************************
  * @brief      : 	JZ2440v2�����������մ������û��λ���������Դ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***************************************************************************/

#include "irda_rawdata.h"
#include "timer.h"

#define NEXT_PLACE(i) ((i+1)&0x3FF)

static SIrdaRawdataEvent g_sEvents[1024];     //���λ�����
static int g_iR = 0;       	//���λ�������λ��
static int g_iW = 0;		//���λ�����дλ��

/**************************************************************************
  * @brief       : 	�жϻ��λ������Ƿ�Ϊ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      :	0   ���λ�������Ϊ��
  				  	�� 0  ���λ�����Ϊ��
  * @others      : 	��
***************************************************************************/
static int IsIrEventBufEmpty(void)
{
	return g_iR == g_iW;
}

/**************************************************************************
  * @brief       : 	�жϻ��λ������Ƿ�Ϊ��
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	0   ���λ�������Ϊ��
  				  	�� 0  ���λ�����Ϊ��
  * @others      : 	��
***************************************************************************/
static int IsIrEventBufFull(void)
{
	return NEXT_PLACE(g_iW) == g_iR;
}

/**************************************************************************
  * @brief       : 	��PSIrdaRawdataEvent���ݷ��뻷�λ�����
  * @param[in]   : 	PSIrdaRawdataEvent����
  * @param[out]  : 	��
  * @return      : 	0  PSIrdaRawdataEvent���ݷ��뻷�λ������ɹ�
  				  	-1  PSIrdaRawdataEvent���ݷ��뻷�λ�����ʧ��
  * @others      : 	��
***************************************************************************/
int PutIrEvent(PSIrdaRawdataEvent pd)
{
	if (IsIrEventBufFull())
		return -1;
	g_sEvents[g_iW] = *pd;
	g_iW = NEXT_PLACE(g_iW);
	return 0;
}

/**************************************************************************
  * @brief       : 	�ӻ��λ���������һ��PSIrdaRawdataEvent����
  * @param[in]   : 	��
  * @param[out]  : 	pd	PSIrdaRawdataEvent����
  * @return      :	 0  �ӻ��λ���������PSIrdaRawdataEvent���ݳɹ�
  				  	-1  �ӻ��λ���������PSIrdaRawdataEvent����ʧ��
  * @others      : 	��
***************************************************************************/
int GetIrEvent(PSIrdaRawdataEvent pd)
{
	if (IsIrEventBufEmpty())
		return -1;
	*pd = g_sEvents[g_iR];
	g_iR = NEXT_PLACE(g_iR);
	return 0;
}

/**************************************************************************
  * @brief       : 	����ʱʱ��Ĵӻ��λ���������һ��PSIrdaRawdataEvent����
  * @param[in]   : 	timeout_us	��ʱʱ��
  * @param[out]  : 	pd	PSIrdaRawdataEvent����
  * @return      : 	0  �ӻ��λ���������PSIrdaRawdataEvent���ݳɹ�
  				  	-1  �ӻ��λ���������PSIrdaRawdataEvent����ʧ��
  * @others      : 	��
***************************************************************************/
int GetIrEventTimeout(PSIrdaRawdataEvent pd, int timeout_us)
{
	while (timeout_us--)
	{
		if (!IsIrEventBufEmpty())
		{
			*pd = g_sEvents[g_iR];
			g_iR = NEXT_PLACE(g_iR);
			
			return 0;
		}
		uDelay(1);
	}
	return -1;
}

