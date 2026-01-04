#include "user_comm.h"

void usb_test(void)
{
    CDC_Transmit_FS((uint8_t *)"USB CDC Test\r\n", 14);
}
