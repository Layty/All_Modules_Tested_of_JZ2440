#ifndef _INTERRUPT_H
#define _INTERRUPT_H

typedef void(*IrqFunc)(int);

/* ��ʼ���жϿ����� */
void InitKeyInterrupt(void);

/* ��ʼ������, ��Ϊ�ж�Դ */
void InitKeyEint(void);

/* �жϴ�����ע�� */
void RegisterIrq(int irq, IrqFunc fp);

#endif

