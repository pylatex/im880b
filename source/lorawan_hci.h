/**
 * Libreria para control del modulo iM880B-L con la pila LoRaWAN mediante HCI.
 * La gracia es que como minimo tenga dos funciones:
 *  - De envio de mensaje HCI (a ser usada en main posiblemente)
 *  - De recepcion de mensaje HCI (posiblemente en interrupciones)
 */

#ifndef LORAWAN_HCI_H
#define	LORAWAN_HCI_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    //--------------------------------------------------------------------------
    //  Section CONST
    //--------------------------------------------------------------------------
    
    // SLIP Protocol Characters
    #define SLIP_END        0xC0
    #define SLIP_ESC        0xDB
    #define SLIP_ESC_END    0xDC
    #define SLIP_ESC_ESC    0xDD

    ////Endpoint (SAP) Identifier
    #define DEVMGMT_SAP_ID                          0x01
    #define LORAWAN_SAP_ID                          0x10
    
    ////Device Management SAP Message Identifier:
    
    // Status Identifier
    #define DEVMGMT_STATUS_OK                   0x00
    #define DEVMGMT_STATUS_ERROR                0x01
    #define DEVMGMT_STATUS_CMD_NOT_SUPPORTED    0x02
    #define DEVMGMT_STATUS_WRONG_PARAMETER      0x03
    #define DEVMGMT_STATUS_WRONG_DEVICE_MODE    0x04

    // Message Identifier --------------------------value---PayloadOctets
    #define DEVMGMT_MSG_PING_REQ                    0x01    //0
    #define DEVMGMT_MSG_PING_RSP                    0x02    //1

    #define DEVMGMT_MSG_RESET_REQ                   0x07    //0
    #define DEVMGMT_MSG_RESET_RSP                   0x08    //1

    #define DEVMGMT_MSG_GET_DEVICE_INFO_REQ         0x03    //0
    #define DEVMGMT_MSG_GET_DEVICE_INFO_RSP         0x04    //10

    #define DEVMGMT_MSG_GET_FW_VERSION_REQ          0x05    //0
    #define DEVMGMT_MSG_GET_FW_VERSION_RSP          0x06    //1+n
    #define DEVMGMT_MSG_GET_FW_INFO_REQ             DEVMGMT_MSG_GET_FW_VERSION_REQ  //WiMOD LoraWAN HCI PDF ver 1.13
    #define DEVMGMT_MSG_GET_FW_INFO_RSP             DEVMGMT_MSG_GET_FW_VERSION_RSP  //WiMOD LoraWAN HCI PDF ver 1.13

    #define DEVMGMT_MSG_GET_DEVICE_STATUS_REQ       0x17    //0
    #define DEVMGMT_MSG_GET_DEVICE_STATUS_RSP       0x18    //60

    #define DEVMGMT_MSG_GET_RTC_REQ                 0x0F    //0
    #define DEVMGMT_MSG_GET_RTC_RSP                 0x10    //5
    #define DEVMGMT_MSG_SET_RTC_REQ                 0x0D    //4
    #define DEVMGMT_MSG_SET_RTC_RSP                 0x0E    //1

    #define DEVMGMT_MSG_SET_RTC_ALARM_REQ           0x31    //4
    #define DEVMGMT_MSG_SET_RTC_ALARM_RSP           0x32    //1
    #define DEVMGMT_MSG_RTC_ALARM_IND               0x38    //1
    #define DEVMGMT_MSG_GET_RTC_ALARM_REQ           0x35    //0
    #define DEVMGMT_MSG_GET_RTC_ALARM_RSP           0x36    //6
    #define DEVMGMT_MSG_CLEAR_RTC_ALARM_REQ         0x33    //0
    #define DEVMGMT_MSG_CLEAR_RTC_ALARM_RSP         0x34    //1

    #define DEVMGMT_MSG_GET_OPMODE_REQ              0x0B    //0
    #define DEVMGMT_MSG_GET_OPMODE_RSP              0x0C    //2
    #define DEVMGMT_MSG_SET_OPMODE_REQ              0x09    //1
    #define DEVMGMT_MSG_SET_OPMODE_RSP              0x0A    //1

    ////LoRaWAN SAP Message Identifier:
    
    // Status Identifier
    #define LORAWAN_STATUS_OK                       0x00
    #define	LORAWAN_STATUS_ERROR                    0x01
    #define	LORAWAN_STATUS_CMD_NOT_SUPPORTED        0x02
    #define	LORAWAN_STATUS_WRONG_PARAMETER          0x03
    #define LORAWAN_STATUS_WRONG_DEVICE_MODE        0x04
    #define LORAWAN_STATUS_NOT_ACTIVATED            0x05
    #define LORAWAN_STATUS_BUSY                     0x06
    #define LORAWAN_STATUS_QUEUE_FULL               0x07
    #define LORAWAN_STATUS_LENGTH_ERROR             0x08
    #define LORAWAN_STATUS_NO_FACTORY_SETTINGS      0x09
    #define LORAWAN_STATUS_CHANNEL_BLOCKED_BY_DC    0x0A
    #define LORAWAN_STATUS_CHANNEL_NOT_AVAILABLE    0x0B

    // Message Identifier --------------------------value---PayloadOctets
    #define LORAWAN_MSG_ACTIVATE_DEVICE_REQ         0x01    //36
    #define LORAWAN_MSG_ACTIVATE_DEVICE_RSP         0x02    //1

    #define LORAWAN_MSG_REACTIVATE_DEVICE_REQ       0x1D    //0
    #define LORAWAN_MSG_REACTIVATE_DEVICE_RSP       0x1E    //5

    #define LORAWAN_MSG_SET_JOIN_PARAM_REQ          0x05    //24
    #define LORAWAN_MSG_SET_JOIN_PARAM_RSP          0x06    //1
    #define LORAWAN_MSG_JOIN_NETWORK_REQ            0x09    //0
    #define LORAWAN_MSG_JOIN_NETWORK_RSP            0x0A    //1
    #define LORAWAN_MSG_JOIN_TRANSMIT_IND           0x0B    //1 (+3)
    #define LORAWAN_MSG_JOIN_NETWORK_TX_IND         LORAWAN_MSG_JOIN_TRANSMIT_IND   //WiMOD LoraWAN HCI PDF ver 1.13
    #define LORAWAN_MSG_JOIN_NETWORK_IND            0x0C    //1 (+4 or +9)

    #define LORAWAN_MSG_SEND_UDATA_REQ              0x0D    //1+n
    #define LORAWAN_MSG_SEND_UDATA_RSP              0x0E    //1 (+4)
    #define LORAWAN_MSG_SEND_UDATA_IND              0x0F    //1 (+2)
    #define LORAWAN_MSG_SEND_UDATA_TX_IND           LORAWAN_MSG_SEND_UDATA_IND      //WiMOD LoraWAN HCI PDF ver 1.13
    #define LORAWAN_MSG_RECV_UDATA_IND              0x10    //1+n (+5)

    #define LORAWAN_MSG_SEND_CDATA_REQ              0x11    //1+n
    #define LORAWAN_MSG_SEND_CDATA_RSP              0x12    //1 (+4)
    #define LORAWAN_MSG_SEND_CDATA_IND              0x13    //1 (+3)
    #define LORAWAN_MSG_SEND_CDATA_TX_IND           LORAWAN_MSG_SEND_CDATA_IND      //WiMOD LoraWAN HCI PDF ver 1.13
    #define LORAWAN_MSG_RECV_CDATA_IND              0x14    //1+n (+5)

    #define LORAWAN_MSG_RECV_ACK_IND                0x15    //1(+5) (OBSOLETE - search in PDF for details)
    #define LORAWAN_MSG_RECV_NODATA_IND             0x16    //1

    #define LORAWAN_MSG_SET_RSTACK_CONFIG_REQ       0x19    //6
    #define LORAWAN_MSG_SET_RSTACK_CONFIG_RSP       0x1A    //1
    #define LORAWAN_MSG_GET_RSTACK_CONFIG_REQ       0x1B    //0
    #define LORAWAN_MSG_GET_RSTACK_CONFIG_RSP       0x1C    //7

    #define LORAWAN_MSG_GET_DEVICE_EUI_REQ          0x27    //0
    #define LORAWAN_MSG_GET_DEVICE_EUI_RSP          0x28    //9
    #define LORAWAN_MSG_SET_DEVICE_EUI_REQ          0x25    //8
    #define LORAWAN_MSG_SET_DEVICE_EUI_RSP          0x26    //1

    #define LORAWAN_MSG_GET_CUSTOM_CFG_REQ          0x33    //0
    #define LORAWAN_MSG_GET_CUSTOM_CFG_RSP          0x34    //2
    #define LORAWAN_MSG_SET_CUSTOM_CFG_REQ          0x31    //1
    #define LORAWAN_MSG_SET_CUSTOM_CFG_RSP          0x32    //1

    #define LORAWAN_MSG_FACTORY_RESET_REQ           0x23    //0
    #define LORAWAN_MSG_FACTORY_RESET_RSP           0x24    //1

    #define LORAWAN_MSG_DEACTIVATE_DEVICE_REQ       0x21    //0
    #define LORAWAN_MSG_DEACTIVATE_DEVICE_RSP       0x22    //1

    #define LORAWAN_MSG_GET_NWK_STATUS_REQ          0x29    //0
    #define LORAWAN_MSG_GET_NWK_STATUS_RSP          0x2A    //2

    #define LORAWAN_MSG_SEND_MAC_CMD_REQ            0x2B    //1+n
    #define LORAWAN_MSG_SEND_MAC_CMD_RSP            0x2C    //1
    #define LORAWAN_MSG_RECV_MAC_CMD_IND            0x2D    //1+n (+5)

    //--------------------------------------------------------------------------
    //  Function Prototypes
    //--------------------------------------------------------------------------
    
    //Envio de un comando HCI
    bool SendHCI (unsigned char* payload, unsigned char size);
    
    //Procesamiento de HCI entrante.
    bool ProcessHCI (unsigned char valor);

#ifdef	__cplusplus
}
#endif

#endif	/* LORAWAN_HCI_H */