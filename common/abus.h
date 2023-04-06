#pragma once

void abus_init();

#define CARD_SELECT   ((value & (1u << CONFIG_PIN_APPLEBUS_DEVSEL-CONFIG_PIN_APPLEBUS_DATA_BASE)) == 0)
#define CARD_DEVSEL   ((address & 0xcf80) == 0xc080)
#define CARD_IOSEL    (((address & 0xcf00) >= 0xc100) && ((address & 0xcf00) < 0xc700))
#define CARD_IOSTROBE ((address & 0xc800) == 0xc800)

enum {
    ABUS_MAIN_SM = 0,
    ABUS_DEVICE_READ_SM = 1,
};
