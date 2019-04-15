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
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"


/* INVYTEC */
#include "../defines.h"
#include "interfaces.h"
#include "../Src/interfaces/uartGPS.h"


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
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart6;

extern uint8 tempData;

extern osThreadId GPSTaskHandle;

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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    UART_HandleTypeDef *uartPort = (UART_HandleTypeDef*) UARTGPSData.InstanceUartPort;
    
    if (huart->Instance == uartPort->Instance )	//GPS UART
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
        #define RX_RAW_HEADER              UARTGPSData.cRawHeader
        #define RX_DATABYTE                UARTGPSData.u8RxDataByte        

        if(RX_BUFFER_INDEX < RX_BUFFER_SIZE )
        {
            // NMEA protocol machine states
            switch(RxUARTGPSStates)
            {
                case NMEAPro_Idle:
                {
                    break;
                }

                case NMEAPro_RawData:
                {
                    
                    if ((RX_BUFFER_INDEX+1)<RX_BUFFER_SIZE) 
                    {
                        //save data into cDataUARTGPSRx(u16RxBufferIndex)
                        RX_BUFFER = RX_DATABYTE;
                        if ( (RX_RAW_HEADER==RX_DATABYTE)   &&
                             (RX_RAW_HEADER!=0x00) )
                        {
                            
                            if ( (RX_FRAME_INDEX+1) < SIZE_BUFF_HEADERS )  
                            {
                               //Save Header Position into u16RxHeadersArray(u8RxHeadersArrayIndex)
                                RX_HEADER_ARRAY = RX_BUFFER_INDEX;
                                //Reset End Position
                                RX_END_ARRAY = 0;
                                //Add +1 to u16RxBufferIndex
                                RX_FRAME_INDEX++; 
                            }
                            else
                            {
                                UARTGPSMoreRawHeaderRecv = true;
                            }
                        }
                        RX_BUFFER_INDEX++;
                    }
                    else
                    {
                        UARTGPSBufferFull = true;
                    }
                    break;
                } 
                
                case NMEAPro_WaitingHeader:
                {
                    if ( RX_DATABYTE == 0x00 )
                    {
                        HAL_UART_Receive_IT(huart, (uint8_t*)&RX_DATABYTE, 1);
                        return;
                    }
                    if(RX_DATABYTE == GPS_NMEAHeaderByte)
                    {
                        if (RX_FRAME_INDEX < SIZE_BUFF_HEADERS )
                        {
                            //save data into cDataUARTGPSRx(u16RxBufferIndex)
                            RX_BUFFER = RX_DATABYTE;
                            //Save Header Position into u16RxHeadersArray(u8RxHeadersArrayIndex)
                            RX_HEADER_ARRAY = RX_BUFFER_INDEX;
                            //Reset End Position
                            RX_END_ARRAY = 0;
                            //Add +1 to u16RxBufferIndex
                            RX_BUFFER_INDEX++; 
                            //Change state to NMEAPro_WaitingEnd
                            RxUARTGPSStates = NMEAPro_WaitingEnd;
                            UARTGPSReceivingData = true;
                        }
                        else
                        {
                            uartGPS_ResetUARTGPSData(NMEAPro_WaitingHeader);
                        }
                    }
                    break;
                }
                
                 case NMEAPro_WaitingEnd:
                {
                    
                    if ( (RX_BUFFER_INDEX+1) <= RX_BUFFER_SIZE )
                    {
                        //save data into cDataUARTGPSRx(u16RxBufferIndex)
                        RX_BUFFER = RX_DATABYTE;
            			// Detectar fin de comando
            		    if(RX_DATABYTE == GPS_CR_Byte | RX_DATABYTE == GPS_LF_Byte )
                        {
                            //Save End Position into u16RxEndssArray(u8RxEndsIndex)
                            RX_END_ARRAY = RX_BUFFER_INDEX;
            				RxUARTGPSStates = NMEAPro_WaitingHeader;
                            UARTGPSCommandRx = true;
                            UARTGPSReceivingData = false;
                            //Add +1 to u8RxFrameIndex
                            RX_FRAME_INDEX++;
                            vTaskResume(GPSTaskHandle);
        			    }
                        //Add +1 to u16RxBufferIndex
                        RX_BUFFER_INDEX++;

                    }
                    else
                    {
                        uartGPS_ResetUARTGPSData(NMEAPro_WaitingHeader);
                    }
                    break;
                }
                               

                
                default:
                {
                    uartGPS_ResetUARTGPSData(NMEAPro_WaitingHeader);
                    break;
                }
            }
        }
        else
        {
            uartGPS_ResetUARTGPSData(NMEAPro_WaitingHeader);
        }

        //activate UART6 receive interrupt every time
        HAL_UART_Receive_IT(huart, (uint8_t*)&RX_DATABYTE, 1);

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
        #undef RX_RAW_HEADER
        #undef RX_DATABYTE                
    }
    else if (huart->Instance == huart4.Instance)
    {   
        HAL_GPIO_TogglePin(YellowLed_GPIO_Port , YellowLed_Pin);
        HAL_UART_Transmit_IT( &huart4, (uint8_t*) &tempData, 1);
        //vTaskDelay(1);
        HAL_UART_Receive_IT( &huart4, &tempData, 1);
    }
}

/***********************************************************************************
End of function 
***********************************************************************************/



/**********************************************************************************
Name:          	uartGPS_ChangeUartUARTGPSData
Description:    allow changes GPS  uart port
Parameters:     Port
Returns:        none
**********************************************************************************/
void * Interfaces_SelectUartUARTGPSData(void *uport)
{
   eUartPorts port = *(eUartPorts *) uport;
   void *rta = NULL;
    switch (port)
    {
        case UART1:
        {
            rta = NULL;
            break;
        }
        case UART2:
        {
            rta = (UART_HandleTypeDef*) &huart2;
            break;
        }
        case UART3:
        {
            rta = NULL;
            break;
        }
        case UART4:
        {
            rta = (UART_HandleTypeDef*) &huart4;
            break;
        }
        case UART5:
        {
            rta = NULL;
            break;
        }
        case UART6:
        {
            rta = (UART_HandleTypeDef*) &huart6;
            break;
        }
        case UART7:
        {
            rta = NULL;
            break;
        }
        case UART8:
        {
            rta = NULL;
            break;
        }
        default:
        {
            rta = NULL;
            break;
        }
    }
    if (rta != NULL )
    {
        HAL_UART_Receive_IT(rta, (uint8_t*)&UARTGPSData.u8RxDataByte, 1);
    }
    return rta;
}
/***********************************************************************************
End of function uartGPS_ChangeUartUARTGPSData
***********************************************************************************/





/**********************************************************************************
Name:         InterfacesSendDataGPS
Description:    
Parameters:   data, bytes
Returns:        
**********************************************************************************/
void InterfacesSendDataGPS( uint8 *data, uint16 bytes)
{   
    if ( UARTGPSData.InstanceUartPort == NULL )
    {
        return;
    }
    HAL_UART_Transmit_IT((UART_HandleTypeDef*) UARTGPSData.InstanceUartPort, data,  bytes);
}
/***********************************************************************************
End of function 
***********************************************************************************/



/**********************************************************************************
Name:         InterfacesSendDataGPS
Description:    
Parameters:   data, bytes
Returns:        
**********************************************************************************/
void InterfacesSendDataCli( uint8 *data, uint16 bytes)
{   
    HAL_UART_Transmit_IT((UART_HandleTypeDef*) &huart4, data,  bytes);
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

