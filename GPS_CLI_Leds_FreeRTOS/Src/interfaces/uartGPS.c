/*******************************************************************************
* Filename: .c
* Developer(s): Jorge Rios
* Language: ANSI C
* Abstract: 
* ------------------------------------------------------------------------------
* Copyright (c) 2009, Investigacion y Tecnologia S.A.
* Investigacion y Tecnologia S.A.
* Carrera 19  82 - 38
* Bogota.
* Colombia.
*
* http: www.invytec.com
* mail: hardware@invytec.com
*
* All rights reserved.
* ------------------------------------------------------------------------------
* Notes:
*
*******************************************************************************/

/*******************************************************************************
* HEADER FILES
*******************************************************************************/
/* ansi c */

/* propietary */
#include "uartGPS.h"
#include "interfaces.h"
#include "../libs/GPS.h"



/* operation system */



/*******************************************************************************
* LOCAL CONSTANT DECLARATION
*******************************************************************************/

/*******************************************************************************
* LOCAL MACRO DECLARATION
*******************************************************************************/



/*******************************************************************************
* LOCAL NEW DATA TYPES
*******************************************************************************/

/**
@name  	    data type Name 
@brief	    Description
*/

           
/*******************************************************************************
* LOCAL DATA DECLARATION
*******************************************************************************/


/*******************************************************************************
* Global Data Declaration
*******************************************************************************/

//Variable Maquina de estados para recepcion de tramas NMEA
volatile eNMEAProtocolRxMachineStates RxUARTGPSStates;
//Variable Estructura para controlar recepcion de datos NMEA
volatile sUARTGPSData UARTGPSData;
// Flags para control de funciones de la UART GPS flags
volatile union byte_def UARTGPSPacketFlags;


/*******************************************************************************
* GLOBAL FUNCTION DECLARATION
*******************************************************************************/


/*******************************************************************************
* LOCAL FUNCTIONS DECLARATIONS
*******************************************************************************/


/*******************************************************************************
* LOCAL FUNCTIONS DEFINITIONS
*******************************************************************************/


/**********************************************************************************
Name:          	
Description:    
Parameters:     
Returns:        
**********************************************************************************/
void uartGPS_init(eUartPorts uport)
{
    UARTGPSData.InstanceUartPort = Interfaces_SelectUartUARTGPSData(&uport);
}
/***********************************************************************************
End of function 
***********************************************************************************/



/**********************************************************************************
Name:           uartGPS_ResetUARTGPSData
Description:    It sends protocol data by UART interface
Parameters:     String to send, Uart number interface
Returns:        none
**********************************************************************************/
void uartGPS_ResetUARTGPSData(eNMEAProtocolRxMachineStates state)
{
#define RX_BUFFER_SIZE             SIZE_BUFF_UART_GPS_RX
#define RX_FRAME_INDEX             UARTGPSData.u8RxFrameIndex
#define RX_FRAME_PROCESSED_INDEX   UARTGPSData.u8RxFrameProcessedIndex
#define RX_END_ARRAY               UARTGPSData.u16RxEndsFrameArray[RX_FRAME_INDEX]
#define RX_END_ARRAY_POS(i)        UARTGPSData.u16RxEndsFrameArray[i]
#define RX_HEADER_ARRAY            UARTGPSData.u16RxHeadersFrameArray[RX_FRAME_INDEX]
#define RX_HEADER_ARRAY_POS(i)     UARTGPSData.u16RxHeadersFrameArray[i]
#define RX_BUFFER_INDEX            UARTGPSData.u16RxBufferIndex
#define RX_BUFFER_PROCESSED_INDEX  UARTGPSData.u16RxBufferProcessedIndex
#define RX_BUFFER                  UARTGPSData.cDataUARTGPSRx[RX_BUFFER_INDEX]
#define RX_BUFFER_POS(i)           UARTGPSData.cDataUARTGPSRx[i]  
    
    // overflow del buffer    
    //initial state
    RxUARTGPSStates = state;
    //Reset Frame Index
    RX_FRAME_INDEX = 0;
    //Reset Rx Buffer index
    RX_BUFFER_INDEX = 0;
    //Reset Frame Processed Index
    RX_FRAME_PROCESSED_INDEX = 0;
    //Reset Buffer Processed Index
    RX_BUFFER_PROCESSED_INDEX = 0;
    
    //Reset Rx Header Array
    for (int i=0; i<SIZE_BUFF_HEADERS; i++)
    {
        RX_HEADER_ARRAY_POS(i) = 0;
        RX_END_ARRAY_POS(i) = 0;
    }
    
    for ( int i= 0; i<RX_BUFFER_SIZE; i++)
    {
        RX_BUFFER_POS(i) = 0x00;
    }
    UARTGPSPacketRxError = true;
    UARTGPSCommandRx = false;
    UARTGPSReceivingData = false;
    
    GPSData.bValidFrame = false;
    

#undef RX_BUFFER_SIZE 
#undef RX_FRAME_INDEX 
#undef RX_FRAME_PROCESSED_INDEX
#undef RX_END_ARRAY
#undef RX_END_ARRAY_POS
#undef RX_HEADER_ARRAY
#undef RX_HEADER_ARRAY_POS
#undef RX_BUFFER_INDEX
#undef RX_BUFFER_PROCESSED_INDEX
#undef RX_BUFFER
#undef RX_BUFFER_POS
}
/***********************************************************************************
End of function uartGPS_ResetUARTGPSData
***********************************************************************************/


 /**********************************************************************************
Name:          	uartGPS_ChangeUartUARTGPSData
Description:    allow changes GPS  uart port
Parameters:     Port
Returns:        none
**********************************************************************************/
void uartGPS_SelectUartUARTGPSData(eUartPorts port)
{
     UARTGPSData.InstanceUartPort = Interfaces_SelectUartUARTGPSData(&port);
}
/***********************************************************************************
End of function uartGPS_ChangeUartUARTGPSData
***********************************************************************************/


/**********************************************************************************
Name:          	
Description:    
Parameters:     
Returns:        
**********************************************************************************/
void uartGPS_SendDataGPS( uint8 *data, uint16 bytes)
{
    InterfacesSendDataGPS(data, bytes);
}
/***********************************************************************************
End of function 
***********************************************************************************/





/**********************************************************************************
Name:          	
Description:    
Parameters:     
Returns:        
**********************************************************************************/
/***********************************************************************************
End of function 
***********************************************************************************/



/* [] END OF FILE */


