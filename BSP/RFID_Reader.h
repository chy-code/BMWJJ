#ifndef _RFID_READER_H
#define _RFID_READER_H

#include <stdbool.h>

#define RFID_ERR_NO_CARD		-1
#define RFID_ERR_ACTIVATION		-2
#define RFID_ERR_UNSUPP_CARD	-3

void BSP_RFID_Init(void);

int BSP_RFID_GetCardNO(char *cardNo);

const char * BSP_RFID_StrError(int ret);

#endif
