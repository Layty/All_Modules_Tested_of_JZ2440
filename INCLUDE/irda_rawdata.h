
#ifndef _IRDA_RAW_H
#define _IRDA_RAW_H

typedef struct SIrdaRawdataEvent {
	int iPol; /* ���� */
	int iDuration;  /* ������, us */
}SIrdaRawdataEvent, *PSIrdaRawdataEvent;

void InitIrda(void);


#endif /* _IRDA_RAW_H */

