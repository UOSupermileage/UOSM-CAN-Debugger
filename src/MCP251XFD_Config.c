//
// Created by Jeremy Cote on 2025-04-22.
//

#include <hardware/spi.h>
#include <hardware/gpio.h>
#include <pico/time.h>
#include "MCP251XFD.h"

eERRORRESULT SPI_Init(void *pIntDev, uint8_t chipSelect, const uint32_t sckFreq) {
    spi_init(pIntDev, sckFreq);

    gpio_init(chipSelect);
    gpio_set_dir(chipSelect, GPIO_OUT);
    gpio_put(chipSelect, 1);

    gpio_set_function(16, GPIO_FUNC_SPI);  // MISO
    gpio_set_function(18, GPIO_FUNC_SPI);  // SCK
    gpio_set_function(19, GPIO_FUNC_SPI);  // MOSI

    return ERR_NONE;
}

eERRORRESULT SPI_Transfer(void *pIntDev, uint8_t chipSelect, uint8_t *txData, uint8_t *rxData, size_t size) {
    gpio_put(chipSelect, 0);

    spi_write_read_blocking(pIntDev, txData, rxData, size);

    gpio_put(chipSelect, 1);
}

uint32_t GetCurrentTimeMs(void) {
    return to_ms_since_boot(get_absolute_time());
}

// TODO: This raises an error, probably incorrect
uint16_t ComputeCRC16(const uint8_t* data, size_t size) {
    uint16_t crc = 0xFFFF; // Initial CRC value
    uint16_t polynomial = 0x1021; // CRC-CCITT polynomial

    for (size_t i = 0; i < size; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

MCP251XFD MCP251XFD_Ext1 = {
    .UserDriverData = NULL,
    //--- Driver configuration ---
    .DriverConfig = MCP251XFD_DRIVER_NORMAL_USE
                    | MCP251XFD_DRIVER_INIT_SET_RAM_AT_0
                    | MCP251XFD_DRIVER_CLEAR_BUFFER_BEFORE_READ,
    //--- IO configuration ---
    .GPIOsOutLevel = MCP251XFD_GPIO0_LOW | MCP251XFD_GPIO1_HIGH,
    //--- Interface driver call functions ---
    .SPI_ChipSelect = 17, // Here the chip select of the EXT1 interface is 1
    .InterfaceDevice = spi0, // Here this point to the address memory of the peripheral SPI0
    .fnSPI_Init = &SPI_Init,
    .fnSPI_Transfer = &SPI_Transfer,
    //--- Time call function ---
    .fnGetCurrentms = &GetCurrentTimeMs,
    //--- CRC16-CMS call function ---
    .fnComputeCRC16 = NULL,
    //--- Interface clocks ---
    .SPIClockSpeed = 10000000, // 10MHz
};

MCP251XFD_BitTimeStats MCP2517FD_Ext1_BTStats;
uint32_t SYSCLK_Ext1;

MCP251XFD_Config MCP2517FD_Ext1_Config = {
    //--- Controller clocks ---
    .XtalFreq = 40000000, // CLKIN is a 40MHz crystal
    .OscFreq = 0,
    .SysclkConfig = MCP251XFD_SYSCLK_IS_CLKIN,
    .ClkoPinConfig = MCP251XFD_CLKO_DivBy10, // TODO: Example uses MCP251XFD_CLKO_SOF
    .SYSCLK_Result = &SYSCLK_Ext1,

    //--- CAN configuration ---
    .NominalBitrate = 1000000, // Nominal Bitrate to 1Mbps // TODO: Could we push this higher?
    .DataBitrate = 2000000, // Data Bitrate to 2Mbps // TODO: Could we push this higher?
    .BitTimeStats = &MCP2517FD_Ext1_BTStats,
    .Bandwidth = MCP251XFD_NO_DELAY,
    .ControlFlags = MCP251XFD_CAN_RESTRICTED_MODE_ON_ERROR
                    | MCP251XFD_CAN_ESI_REFLECTS_ERROR_STATUS
                    | MCP251XFD_CAN_RESTRICTED_RETRANS_ATTEMPTS
                    | MCP251XFD_CANFD_BITRATE_SWITCHING_ENABLE
                    | MCP251XFD_CAN_PROTOCOL_EXCEPT_AS_FORM_ERROR
                    | MCP251XFD_CANFD_USE_ISO_CRC
                    | MCP251XFD_CANFD_DONT_USE_RRS_BIT_AS_SID11,

    //--- GPIOs and Interrupts pins ---
    .GPIO0PinMode = MCP251XFD_PIN_AS_GPIO0_OUT,
    .GPIO1PinMode = MCP251XFD_PIN_AS_INT1_RX,
    .INTsOutMode = MCP251XFD_PINS_PUSHPULL_OUT,
    .TXCANOutMode = MCP251XFD_PINS_PUSHPULL_OUT,

    //--- Interrupts ---
    .SysInterruptFlags = MCP251XFD_INT_ENABLE_ALL_EVENTS,
};

MCP251XFD_RAMInfos Ext1_TEF_RAMInfos;
MCP251XFD_RAMInfos Ext1_TXQ_RAMInfos;
MCP251XFD_RAMInfos Ext1_FIFOs_RAMInfos[MCP2517FD_EXT1_FIFO_COUNT - 2];
MCP251XFD_FIFO MCP2517FD_Ext1_FIFOlist[MCP2517FD_EXT1_FIFO_COUNT] =
        {
                { .Name = MCP251XFD_TEF, .Size = MCP251XFD_FIFO_10_MESSAGE_DEEP, .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_OBJ,
                        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_EVENT_FIFO_NOT_EMPTY_INT,
                        .RAMInfos = &Ext1_TEF_RAMInfos, },
                { .Name = MCP251XFD_TXQ, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,
                        .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
                        .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
                        .RAMInfos = &Ext1_TXQ_RAMInfos, },
                { .Name = MCP251XFD_FIFO1, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_RECEIVE_FIFO, .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
                        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[0], }, // SID: 0x000..0x1FF ; No EID
                { .Name = MCP251XFD_FIFO2, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_8BYTE,
                        .Direction = MCP251XFD_TRANSMIT_FIFO, .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
                        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[1], }, // SID: 0x200..0x3FF ; No EID
                { .Name = MCP251XFD_FIFO3, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_RECEIVE_FIFO, .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
                        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[2], }, // SID: 0x400..0x5FF ; No EID
                { .Name = MCP251XFD_FIFO4, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_RECEIVE_FIFO, .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
                        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[3], }, // SID: 0x600..0x7FF ; No EID
                { .Name = MCP251XFD_FIFO5, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS,
                        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
                        .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[4], },
                { .Name = MCP251XFD_FIFO6, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS,
                        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
                        .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[5], },
                { .Name = MCP251XFD_FIFO7, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS,
                        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
                        .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[6], },
                { .Name = MCP251XFD_FIFO8, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_64BYTE,
                        .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS,
                        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
                        .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
                        .RAMInfos = &Ext1_FIFOs_RAMInfos[7], },
        };

// TODO: We can filter our data into multiple queues for logical processing
MCP251XFD_Filter MCP2517FD_Ext1_FilterList[MCP2517FD_EXT1_FILTER_COUNT] = {
    { .Filter = MCP251XFD_FILTER4, .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_SID,
            .AcceptanceID = MCP251XFD_ACCEPT_ALL_MESSAGES, .AcceptanceMask = MCP251XFD_ACCEPT_ALL_MESSAGES, .PointTo = MCP251XFD_FIFO1, },
};