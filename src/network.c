//
// Created by Jeremy Cote on 2025-04-22.
//

#include "network.h"

#include "MCP251XFD.h"
#include <hardware/gpio.h>
#include <pico/printf.h>

#include "ApplicationTypes.h"
#include "msg_interpreter.h"

#define MCP_INT_PIN 22

void gpio_callback(uint gpio, uint32_t events) {

}

bool CAN_Init() {
//    gpio_set_irq_enabled_with_callback(MCP_INT_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    //--- Configure module on Ext1 ---
    eERRORRESULT ErrorExt1 = ERR__NO_DEVICE_DETECTED;
    ErrorExt1 = Init_MCP251XFD(CANEXT1, &MCP2517FD_Ext1_Config);
    if (ErrorExt1 != ERR_OK)
        return false;

    ErrorExt1 = MCP251XFD_ConfigureTimeStamp(CANEXT1, true, MCP251XFD_TS_CAN20_SOF_CANFD_SOF,
                                             TIMESTAMP_TICK(SYSCLK_Ext1), true);
    if (ErrorExt1 != ERR_OK)
        return false;

    ErrorExt1 = MCP251XFD_ConfigureFIFOList(CANEXT1, &MCP2517FD_Ext1_FIFOlist[0], MCP2517FD_EXT1_FIFO_COUNT);
    if (ErrorExt1 != ERR_OK)
        return false;

    ErrorExt1 = MCP251XFD_ConfigureFilterList(CANEXT1, MCP251XFD_D_NET_FILTER_DISABLE,
                                              &MCP2517FD_Ext1_FilterList[0], MCP2517FD_EXT1_FILTER_COUNT);
    if (ErrorExt1 != ERR_OK)
        return false;

    // ErrorExt1 = MCP251XFD_StartCANFD(CANEXT1);
    ErrorExt1 = MCP251XFD_StartCANListenOnly(CANEXT1);
    if (ErrorExt1 != ERR_OK)
        return false;

    return true;
}

void CAN_Receive() {
    eERRORRESULT ErrorExt1 = ERR_OK;
    eMCP251XFD_FIFOstatus FIFOstatus = 0;
    setMCP251XFD_InterruptOnFIFO InterruptOnFIFO = 0;
   // printf("in here");

    ErrorExt1 = MCP251XFD_GetReceivePendingInterruptStatusOfAllFIFO(CANEXT1, &InterruptOnFIFO); // Get all FIFO status
    if (ErrorExt1 != ERR_OK){
       // printf("in here2");
        return;}

    for (eMCP251XFD_FIFO zFIFO = 1; zFIFO < MCP251XFD_FIFO_MAX; zFIFO++) { // For each receive FIFO but not TEF, TXQ
        if ((InterruptOnFIFO & (1 << zFIFO)) > 0) { // If an Interrupt is flagged
            ErrorExt1 = MCP251XFD_GetFIFOStatus(CANEXT1, zFIFO, &FIFOstatus); // Get the status of the flagged FIFO
            if (ErrorExt1 != ERR_OK){
              //  printf("in here3");
            return;}

            if ((FIFOstatus & MCP251XFD_RX_FIFO_NOT_EMPTY) > 0) { // Check FIFO not empty
               // printf("in if statement");
                uint32_t MessageTimeStamp = 0;
                uint8_t RxPayloadData[8]; // In this example, all the FIFO have 8 bytes of payload
                MCP251XFD_CANMessage ReceivedMessage;

                ReceivedMessage.PayloadData = &RxPayloadData[0]; // Add receive payload data pointer to the message structure that will be received

                ErrorExt1 = MCP251XFD_ReceiveMessageFromFIFO(CANEXT1, &ReceivedMessage, MCP251XFD_PAYLOAD_8BYTE,
                                                             &MessageTimeStamp, zFIFO);
                if (ErrorExt1 == ERR_OK) {
                    //***** Do what you want with the message *****
                    printf("Message received!");
                    printf("Message ID: %d\n", ReceivedMessage.MessageID);
                    printf("Message Timestamp: %d\n", MessageTimeStamp);
                    printf("Message DLC: %d\n", ReceivedMessage.DLC);

                    MsgInterpreter_ProcessMessage(ReceivedMessage.MessageID,ReceivedMessage.PayloadData);
                    gpio_put(PICO_DEFAULT_LED_PIN, 1);

                }
            }
        }
    }
}

void CAN_Send() {
    eERRORRESULT ErrorExt1 = ERR_OK;
    eMCP251XFD_FIFOstatus FIFOstatus = 0;
    ErrorExt1 = MCP251XFD_GetFIFOStatus(CANEXT1, MCP251XFD_TXQ, &FIFOstatus); // First get FIFO2 status
    if (ErrorExt1 != ERR_OK)
        return;
    //
    // int32_t velocity = 123;
    // uint8_t data[8];
    // data[0] = velocity;
    // data[1] = velocity >> 8;
    // data[2] = velocity >> 16;
    // data[3] = velocity >> 24;

    lights_status_t lights = {0};
    lights.hazards_enabled = 1;
    lights.left_turn_enabled = 0;
    lights.right_turn_enabled = 0;
    lights.headlights_enabled = 0;
    lights.low_beams_enabled = 0;

    lights.r = 255;
    lights.g = 0;
    lights.b = 25;

    uint32_t raw = lights.all;
    uint8_t data[4];
    data[0] = (raw >> 24) & 0xFF;
    data[1] = (raw >> 16) & 0xFF;
    data[2] = (raw >> 8) & 0xFF;
    data[3] = raw & 0xFF;


    if ((FIFOstatus & MCP251XFD_TX_FIFO_NOT_FULL) > 0) // Second check FIFO not full
    {

        MCP251XFD_CANMessage TansmitMessage;
        //***** Fill the message as you want *****3
        TansmitMessage.MessageID = 6;
        //TansmitMessage.MessageSEQ = messageSEQ;
        //TansmitMessage.ControlFlags = controlFlags;
        TansmitMessage.DLC = 4;
        TansmitMessage.PayloadData = data;
        // Send message and flush
        ErrorExt1 = MCP251XFD_TransmitMessageToFIFO(CANEXT1, &TansmitMessage, MCP251XFD_TXQ, true);

        if (ErrorExt1 != ERR_OK)
        {
            printf("Transmission failed: %d\n", ErrorExt1);
        }
    } else {
        printf("FIFO full. Cannot send CAN message\n");
    }
}