#ifndef _SENSOR_H
#define _SENSOR_H


void InitADC(int channel);
int ReadADC(int channel);

void TestPhotoresistor(void);
void TestDs18b20(void);

/**********************************************************************************
  * @brief       : 	Dht11���Ժ���
  * @param[in]   : 	��
  * @param[out]  : 	��
  * @return      : 	��
  * @others      : 	��
***********************************************************************************/
void TestDht11(void);
void TestIrdaNec(void);


#endif

