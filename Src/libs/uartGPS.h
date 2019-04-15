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
#include "defines.h"

/* operation system */

/* processor expert */

/*******************************************************************************
* GLOBAL CONSTANT DECLARATION
*******************************************************************************/

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

typedef struct
{
    char   cRawHeader;
    uint8  u8RxFrameIndex;
	uint8  u8RxFrameProcessedIndex;
	uint8  cDataUARTGPSRx[SIZE_BUFF_UART_GPS_RX];
	uint8  cDataUARTGPSTx[SIZE_BUFF_UART_GPS_TX];
    uint16 u16RxBufferIndex;
    uint16 u16RxBufferProcessedIndex;
    uint16 u16RxEndsFrameArray[SIZE_BUFF_ENDS];    
    uint16 u16RxHeadersFrameArray[SIZE_BUFF_HEADERS];
} sUARTGPSData;


/*******************************************************************************
* GLOBAL DATA DECLARATION
*******************************************************************************/



/*******************************************************************************
* GLOBAL FUNCTION DECLARATION
*******************************************************************************/


/********************************************************************************
* Abbreviation Table
* 
*******************************************************************************/

/*******************************************************************************
* END OF FILE
*******************************************************************************/

#endif /* UARTGPS_H */
