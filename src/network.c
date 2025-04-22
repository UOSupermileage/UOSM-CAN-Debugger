//
// Created by Jeremy Cote on 2025-04-22.
//

#include "network.h"

#include "MCP251XFD.h"
#include <hardware/gpio.h>

#define MCP_INT_PIN 22

void gpio_callback(uint gpio, uint32_t events) {

}

bool CAN_Init() {
    gpio_set_irq_enabled_with_callback(MCP_INT_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    //--- Configure module on Ext1 ---
    eERRORRESULT ErrorExt1 = ERR__NO_DEVICE_DETECTED;
    ErrorExt1 = Init_MCP251XFD(CANEXT1, &MCP2517FD_Ext1_Config);
    if (ErrorExt1 != ERR_OK)
        return ErrorExt1;

    ErrorExt1 = MCP251XFD_ConfigureTimeStamp(CANEXT1, true, MCP251XFD_TS_CAN20_SOF_CANFD_SOF,
                                             TIMESTAMP_TICK(SYSCLK_Ext1), true);
    if (ErrorExt1 != ERR_OK) return ErrorExt1;
    ErrorExt1 = MCP251XFD_ConfigureFIFOList(CANEXT1, &MCP2517FD_Ext1_FIFOlist[0], MCP2517FD_EXT1_FIFO_COUNT);
    if (ErrorExt1 != ERR_OK) return ErrorExt1;
    ErrorExt1 = MCP251XFD_ConfigureFilterList(CANEXT1, MCP251XFD_D_NET_FILTER_DISABLE,
                                              &MCP2517FD_Ext1_FilterList[0], MCP2517FD_EXT1_FILTER_COUNT);
    if (ErrorExt1 != ERR_OK) return ErrorExt1;
    ErrorExt1 = MCP251XFD_StartCANFD(CANEXT1);
}