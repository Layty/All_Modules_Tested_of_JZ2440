/**************************************************************************
  * @brief      : JZ2440v2������ADC�ɼ�����Դ�ļ�
  * @version    : V0.0
  * @note       : ��
  * @history    : ��
***************************************************************************/

#include "s3c2440_soc.h"

/**************************************************************************
  * @brief       :  ��ʼ��ADC
  * @param[in]   :  channel ����ADC�Ĳɼ�ͨ����ȡֵΪ {0,1,2,3,4,5,7}
  * @param[out]  :  ��
  * @return      :  ��
  * @others      :  ��
***************************************************************************/
static void InitADC(int channel)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (channel<<3);

	ADCDLY = 0xff;	
}

/**************************************************************************
  * @brief       :  ��ȡADCת��������
  * @param[in]   :  channel ����ADC�Ĳɼ�ͨ����ȡֵΪ {0,1,2,3,4,5,7}
  * @param[out]  :  ��
  * @return      :  ADC��ȡ��ת�����֮�������
  * @others      :  ��
***************************************************************************/
int ReadADC(int channel)
{
	InitADC(channel);
	
	/* ����ADC */
	ADCCON |= (1<<0);

	while (!(ADCCON & (1<<15)));  /* �ȴ�ADC���� */

	return ADCDAT0 & 0x3ff;
}

