/**************************************************************************
  * @brief      : 	JZ2440v2�����������մ������û��λ���������ͷ�ļ�
  * @version    : 	V0.0
  * @note       : 	��
  * @history    : 	��
***************************************************************************/

#ifndef _CIRCLE_BUFFER_H
#define _CIRCLE_BUFFER_H

/**************************************************************************
  * @brief       : 	��PSIrdaRawdataEvent���ݷ��뻷�λ�����
  * @param[in]   : 	PSIrdaRawdataEvent����
  * @param[out]  : 	��
  * @return      : 	0  PSIrdaRawdataEvent���ݷ��뻷�λ������ɹ�
  				  	-1  PSIrdaRawdataEvent���ݷ��뻷�λ�����ʧ��
  * @others      : 	��
***************************************************************************/
int PutIrEvent(PSIrdaRawdataEvent pd);

/**************************************************************************
  * @brief       : 	�ӻ��λ���������һ��PSIrdaRawdataEvent����
  * @param[in]   : 	��
  * @param[out]  : 	pd	PSIrdaRawdataEvent����
  * @return      : 	0  �ӻ��λ���������PSIrdaRawdataEvent���ݳɹ�
  				  	-1  �ӻ��λ���������PSIrdaRawdataEvent����ʧ��
  * @others      : 	��
***************************************************************************/
int GetIrEvent(PSIrdaRawdataEvent pd);

/**************************************************************************
  * @brief       : 	����ʱʱ��Ĵӻ��λ���������һ��PSIrdaRawdataEvent����
  * @param[in]   : 	timeout_us	��ʱʱ��
  * @param[out]  : 	pd	PSIrdaRawdataEvent����
  * @return      : 	0  �ӻ��λ���������PSIrdaRawdataEvent���ݳɹ�
  				  	-1  �ӻ��λ���������PSIrdaRawdataEvent����ʧ��
  * @others      : 	��
***************************************************************************/
int GetIrEventTimeout(PSIrdaRawdataEvent pd, int timeout_us);

#endif

