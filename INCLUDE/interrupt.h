#ifndef _INTERRUPT_H
#define _INTERRUPT_H

typedef void(*irq_func)(int);

/* ��ʼ���жϿ����� */
void key_interrupt_init(void);

/* ��ʼ������, ��Ϊ�ж�Դ */
void key_eint_init(void);

/* �жϴ�����ע�� */
void register_irq(int irq, irq_func fp);

#endif

