/*******************************************************************************
* Filename: uartGPS.h
* Developer(s): Jorge Yesid Rios Ortiz
* Language: ANSI C
* Abstract: 
* References: 
*******************************************************************************/
#ifndef UARTGPS_H
#define UARTGPS_H
/*******************************************************************************
* HEADER FILES
*******************************************************************************/
/* ansi c */

/* propietary */
#include "../defines.h"

/* operation system */

#include "stm32f091xc.h"


/*******************************************************************************
* GLOBAL CONSTANT DECLARATION
*******************************************************************************/

// Flags para control de funciones de la UART GPS flags
extern volatile union byte_def UARTGPSPacketFlags;
//#define Expansion	                UARTGPSPacketFlags.bitFlag.b7
//#define Expansion	                UARTGPSPacketFlags.bitFlag.b6
#define UARTGPSMoreRawHeaderRecv    UARTGPSPacketFlags.bitFlag.b5
#define UARTGPSBufferFull           UARTGPSPacketFlags.bitFlag.b4
#define UARTGPSReceivingData	    UARTGPSPacketFlags.bitFlag.b3
#define UARTGPSTransmitingData	    UARTGPSPacketFlags.bitFlag.b2
#define UARTGPSCommandRx  	        UARTGPSPacketFlags.bitFlag.b1
#define UARTGPSPacketRxError	    UARTGPSPacketFlags.bitFlag.b0

// For GPS protocol UART reception
#define GPS_NMEAHeaderByte      0x24  // Header server protocol '$'
#define GPS_CR_Byte             0x0D  // final server protocol '<CR>' 0x0D
#define GPS_LF_Byte             0x0A  // final server protocol '<CR>' 0x0D


/*******************************************************************************
* GLOBAL MACRO DECLARATION
*******************************************************************************/

// UART GPS Struct data
#define SIZE_BUFF_UART_GPS_TX      255    // Tamano en bytes del buffer serial 1 de Tx
#define SIZE_BUFF_UART_GPS_RX	   512   //140  // Tamano en bytes del buffer serial 1 de Rx (Recepcion max, agenda FSK)
#define SIZE_BUFF_HEADERS          45
#define SIZE_BUFF_ENDS             SIZE_BUFF_HEADERS



/*******************************************************************************
* GLOBAL NEW DATA TYPES
*******************************************************************************/

typedef enum 
{
    UART1 = 0x01,
    UART2 = 0x02,
    UART3 = 0x03,
    UART4 = 0x04,
    UART5 = 0x05,
    UART6 = 0x06,
    UART7 = 0x07,
    UART8 = 0x08,
} eUartPorts;

//Maquina de estados para recepcion de tramas NMEA
typedef enum 
{
    NMEAPro_Idle = 0x00,
    NMEAPro_RawData = 0x01,
	NMEAPro_WaitingHeader = 0x02,
	NMEAPro_WaitingEnd = 0x03,
} eNMEAProtocolRxMachineStates;


//Estructura para controlar recepcion de datos NMEA
typedef struct
{
    char   cRawHeader;
    uint8  u8RxDataByte;
    uint8  u8RxFrameIndex;
	uint8  u8RxFrameProcessedIndex;
	uint8  cDataUARTGPSRx[SIZE_BUFF_UART_GPS_RX];
	uint8  cDataUARTGPSTx[SIZE_BUFF_UART_GPS_TX];
    uint16 u16RxBufferIndex;
    uint16 u16RxBufferProcessedIndex;
    uint16 u16RxEndsFrameArray[SIZE_BUFF_ENDS];    
    uint16 u16RxHeadersFrameArray[SIZE_BUFF_HEADERS];
    void   *InstanceUartPort;
    
} sUARTGPSData;


/*******************************************************************************
* GLOBAL DATA DECLARATION
*******************************************************************************/
//Variable structura para controlar recepcion de datos NMEA
extern volatile eNMEAProtocolRxMachineStates RxUARTGPSStates;
//Variable Maquina de estados para recepcion de tramas NMEA
extern volatile sUARTGPSData UARTGPSData;



/*******************************************************************************
* GLOBAL FUNCTION DECLARATION
*******************************************************************************/
void uartGPS_init(eUartPorts uport);
void uartGPS_SelectUartUARTGPSData(eUartPorts port);
void uartGPS_ResetUARTGPSData(eNMEAProtocolRxMachineStates state);
void uartGPS_SendDataGPS( uint8 *data, uint16 bytes);


/********************************************************************************
* Abbreviation Table
* 
*******************************************************************************/

/*******************************************************************************
* END OF FILE
*******************************************************************************/

#endif /* UARTGPS_H */
