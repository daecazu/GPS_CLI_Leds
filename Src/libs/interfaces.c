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
#include "defines.h"

/* by INVYTEC */

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
 char Rx_indx_CLI, Rx_data_CLI, Rx_Buffer_CLI[100];
 char Transfer_cplt_CLI = 0;

 char Rx_indx_gps, Rx_data_gps, Rx_Buffer_gps[100];
 char Transfer_cplt_gps = 0;

/*******************************************************************************
* Global Data Declaration
*******************************************************************************/


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
    uint8 i = 0;
    if (huart->Instance == USART6)	//GPS UART
    {
        //clear Rx_Buffer_gps before receiving new data	
        if (Rx_indx_gps==0) {for (i=0;i<100;i++) Rx_Buffer_gps[i]=0;}

        //if received data different from ascii 13 or 10 (\n or \r)
        //if (Rx_data_gps!=13 & Rx_data_gps!=10)
        if (Rx_Buffer_gps[Rx_indx_gps]==10 | Rx_Buffer_gps[Rx_indx_gps]==13)
        {
        //if received data = 13 or 10			
        Rx_indx_gps=0;
        //transfer complete, data is ready to read
        Transfer_cplt_gps=1;
        }
        else
        {
        //add data to Rx_Buffer
        Rx_indx_gps++;
        }
        if (Rx_indx_gps > 99 )
        {
        Rx_indx_gps = 0;
        }
        //activate UART2 receive interrupt every time
        HAL_UART_Receive_IT(huart, (uint8_t*)&Rx_Buffer_gps[Rx_indx_gps], 1);
    }
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