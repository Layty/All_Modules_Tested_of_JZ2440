/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板nand flash代码头文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H

/**********************************************************************************
  * @brief       : 	初始化nand flash
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitNand(void);

/**********************************************************************************
  * @brief       : 	读取nand flash数据
  * @param[in]   : 	addr	读取数据的开始地址
  					len		读取数据的长度
  * @param[out]  : 	buf		存放读取的数据的数据缓冲区
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ReadNand(unsigned int addr, unsigned char *buf, unsigned int len);

/**********************************************************************************
  * @brief       : 	写入nand flash
  * @param[in]   : 	addr	数据写入的开始地址
  					len		数据写入的长度
  					buf		数据写入的数据缓冲区		
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void NandWrite(unsigned int addr, unsigned char *buf, unsigned int len);

/**********************************************************************************
  * @brief       : 	擦除nand flash
  * @param[in]   : 	addr	擦除的开始地址
  					len		擦除的长度
  * @param[out]  : 	无
  * @return      : 	0	擦除成功
  					-1	擦除失败
  * @others      : 	无
***********************************************************************************/
int NandErase(unsigned int addr, unsigned int len);

/**********************************************************************************
  * @brief       : 	测试nand flash函数
  * @param[in]   : 	无	
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestNandFlash(void);

#endif /* _NAND_FLASH_H */

