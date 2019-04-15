/*******************************************************************************
* Filename: GPS.c
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
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

/* propietary */
#include "GPS.h"
#include "../Src/libs/MiscFunctions.h"

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

void GPS_CalculateNMEAExpected(void);
void GPS_InitializeParametersValidation(void);
uint8 GPS_GetDesiredFields(void);
int8 GPS_ValidationNMEAData(void);
uint8 GPS_NmeaChecksumVerify(uint16 u16FrameStart, uint16 u16FrameEnd);
inline uint8 GPS_IndexNMEAType(const uint8 *u8pNMEAType);

/*inline*/ int8 GPS_StoreNMEAData( uint8 u8Index, uint8 u8NMEAType, uint16 u16BufferIndexOfData);
/*inline*/ int8 GPS_StoreNMEADate(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEAHDop(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEATime(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEAHeight(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEAStatus(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEALatitude(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEASpeedKmh(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEASpeedKnot(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEALongitude(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEAMagCourse(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEATrueCourse(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEALatitudeSign(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEALongitudeSign(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEASatelitesNumber(uint16 u16Index);
/*inline*/ int8 GPS_StoreNMEAFixQualityStatus(uint16 u16Index);
/*inline*/ unsigned long GPS_TotalSecondsFromDateTimeGPS(void);



///Declarate Array Structure configuration of RMC, GGA and VTG.
 sGPSConfig sConfigNMEA[] = {
        {"RMC", 0u, RMC_TIME + RMC_STATUS + RMC_LATITUDE + RMC_LAT_SIGN + RMC_LONGITUDE + RMC_LON_SIGN + RMC_SPEEDKNOT + RMC_COURSE + RMC_DATE},
        {"GGA", 1u, GGA_TIME + GGA_LATITUDE + GGA_LAT_SIGN + GGA_LONGITUDE + GGA_LON_SIGN + GGA_FIX_STATUS + GGA_NUMSATS + GGA_HDOP + GGA_HEIGHT +  GGA_HEIGHT_UNIT + GGA_GEO_SEP + GGA_GEO_SEP_UNIT},
        {"VTG", 2u, VTG_TRUE_COURSE + VTG_MAG_COURSE + VTG_SPEED_KNOTS + VTG_SPEED_KMH},
};

uint32 ui32MaskValidation = VALIDATION_MILISECONDSTIME | VALIDATION_NSATS  | VALIDATION_HDOP | VALIDATION_STATUS | VALIDATION_LATITUDE |	VALIDATION_LONGITUDE ;

volatile sGPSData GPSTempData;        //Variable de datos v·lidos
volatile sGPSData GPSData;            //Variable de datos temporales mientras se validan

volatile sGPSDataProcess GPSDataProcess;
volatile sGPSValidationParameters  GPSValidationParameters;


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
Name:          	GPS_Init
Description:    none
Parameters:     none
Returns:        none
**********************************************************************************/
void GPS_Init(eUartPorts uport)
{
    //  Calculate NMEA to be used
    GPS_CalculateNMEAExpected();
    // Initialize values to validation of GPS Data
    GPS_InitializeParametersValidation();
    // Initialize GPS Uart
    uartGPS_init(uport);
}
/***********************************************************************************
End of function GPS_Init
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_CalculateNMEAExpected
@author     Diego Ferney Nunez.
@brief	    Calculate which types of NMEA will be expected from GPS,
            based on settings established in sConfigNMEA.
@pre        Have set in sConfigNMEA which NMEA configurated to read. 
@post       It is stored in GPSDataProcess.ui32ExpectedNMEAMask which type of NMEA will be expected. 
@param	    none. 
@return     none.
*/
/**********************************************************************************/
void GPS_CalculateNMEAExpected(void)
{   
    ///Initialize NMEAMask
    GPSDataProcess.ui32ExpectedNMEAMask = 0x00;    
    ///declarate counter to go over the "for" that calculate which NMEA are expected 
    ///Calculate _NMEA Expected
    for( uint8 i = 0; i < (sizeof(sConfigNMEA)/sizeof(sConfigNMEA[0])); i++)
    {
        /// Get weight of type of NMEA
        GPSDataProcess.ui32ExpectedNMEAMask =
            GPSDataProcess.ui32ExpectedNMEAMask + (1<<(sConfigNMEA[i].ui8WeightNMEA));
    }
}
/**********************************************************************************
End of function GPS_CalculateNMEAExpected
***********************************************************************************/    



/**********************************************************************************/
/**
@name  	    GPS_initializeParametersValidation
@author     Diego Ferney Nunez.
@brief	    Initialize structure to validation parameters whit values by default 
@pre        none.
@post       none.
@param	    none.
@return     none.
*/
/**********************************************************************************/
void GPS_InitializeParametersValidation(void)
{
    ///Status
    GPSValidationParameters.cStatusUP = 'A';
    GPSValidationParameters.cStatusDown = 'A';
    ///Latitude
    GPSValidationParameters.fLatitudeUp = 90;
    GPSValidationParameters.fLatitudeDown = -90;
    ///Longitude
    GPSValidationParameters.fLongitudeUp = 180;
    GPSValidationParameters.fLongitudeDown = -180;
    ///SpeedKnot
    GPSValidationParameters.fSpeedKnotUp = 0;
    GPSValidationParameters.fSpeedKnotDown = 0;  
    ///DateUp
    GPSValidationParameters.sDateTimeUp.Date.u8Day = 0;
    GPSValidationParameters.sDateTimeUp.Date.u8Month = 0;
    GPSValidationParameters.sDateTimeUp.Date.u16Year = 0;
    ///DateDown
    GPSValidationParameters.sDateTimeDown.Date.u8Day = 0;
    GPSValidationParameters.sDateTimeDown.Date.u8Month = 0;
    GPSValidationParameters.sDateTimeDown.Date.u16Year = 0;
    ///TimeUp
    GPSValidationParameters.sDateTimeUp.Time.u8Hour = 0;
    GPSValidationParameters.sDateTimeUp.Time.u8Minutes = 0;
    GPSValidationParameters.sDateTimeUp.Time.u8Seconds = 0;
    GPSValidationParameters.sDateTimeUp.Time.u32mSeconds = 0;
    ///TimeDown
    GPSValidationParameters.sDateTimeDown.Time.u8Hour = 0;
    GPSValidationParameters.sDateTimeDown.Time.u8Minutes = 0;
    GPSValidationParameters.sDateTimeDown.Time.u8Seconds = 0;
    GPSValidationParameters.sDateTimeDown.Time.u32mSeconds = 0;
    ///FixQuality
    GPSValidationParameters.ui8FixQualityUp = 0;
    GPSValidationParameters.ui8FixQualityDown = 0;
    ///SatelitesNumber
    GPSValidationParameters.ui8NSatsUp = 99;    
    GPSValidationParameters.ui8NSatsDown = 5;
    ///Hdop
    GPSValidationParameters.fHdopUp = 5;
    GPSValidationParameters.fHdopDown = 0;
    ///Height
    GPSValidationParameters.fHeightUp = 0;
    GPSValidationParameters.fHeightDown = 0;
    ///TrueCourse
    GPSValidationParameters.fTrueCourseUp = 0;
    GPSValidationParameters.fTrueCourseDown = 0;
    ///MagCourse
    GPSValidationParameters.fMagCourseUp = 0;
    GPSValidationParameters.fMagCourseDown = 0;
    ///SpeedKmh
    GPSValidationParameters.fSpeedKmhUp = 0;
    GPSValidationParameters.fSpeedKmhDown = 0;
    ///Epoch
    GPSValidationParameters.ui32EpochUp = 0;
    GPSValidationParameters.ui32EpochDown = 0;
}
/**********************************************************************************
End of function GPS_initializeParametersValidation
***********************************************************************************/




/**********************************************************************************/
/**
@name  	    GPS_ProcessNMEA
@author     Jorge Rios.
@brief	    Main function of GPS class, this process a NMEA frame that has been stored into buffer,
            First verifies the checksum valid, then decodes the data desired, 
            after that check if the NMEA type is desired, next validate the values 
            recived with the parameters set into u8pGPSValidationParameters and 
            finally if everything is correct copy the data from GPSData to GPSData.
@pre        Have calculated NMEA Expect whith GPS_CalculateNMEAExpected(), have set validation
            parameters whith GPS_InitializeParametersValidation(), and finally have recived NMEA frame by UART 
@post       If all goes as expected the data obtined it is stored in GPSData and the user 
            should be use GET methot. 
@param	    u8pBuffer   ->  Byte to analyze. 
@return     u8status    ->  Return status of process.
*/
/**********************************************************************************/
uint8 GPS_ProcessNMEA(void)
{
    uint8 u8status = EXIT_FAILURE;
    /*  TODO:  Revisar donde cambiar este flag... en la interrupcion?
    if ( SystemClock.bPPSRxFlag==true )
    {
        GPSData.bValidFrame = false;
    }
    */
    
    if (UARTGPSCommandRx==false)
    {
        return u8status;
    }
    ///Get Data desired from fields NMEA
    GPS_GetDesiredFields();
    ///If All data were processed
    if (GPSDataProcess.ui32MaskHeaderTypeCheckProcess == 
        GPSDataProcess.ui32ExpectedNMEAMask)
    {
        u8status = GPS_ValidationNMEAData();
        ///If are validation ok
        if( u8status == EXIT_SUCCESS)
        {
            /* if (SystemClock.u8ValidFramesToSynchro<TOTAL_VALID_FRAMES)
            {  GPSData.bValidFrame = false;
               SystemClock.u8ValidFramesToSynchro++; }
            else {            */
            GPSData = GPSTempData;
            GPSData.bValidFrame = true;
            /*} */
        }
        GPSDataProcess.ui32MaskHeaderTypeCheckProcess = 0x00;
    }
    return u8status;
    
#undef TOTAL_VALID_FRAMES
}
/**********************************************************************************
End of function GPS_ProcessNMEA
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_ValidationNMEAData
@author     Diego Ferney Nunez.
@brief	    This function validate the information recived from NMEA with the range values 
            set into u8pGPSValidationParameters. 
@pre        having set values of GPSValidationParameters, make sure you that have reached 
            all expected frames.
@post       check GPSValidationParameters.FlagsErrorNmeaData.Byte for validation errors.
@param	    none
@return     none.
*/
/**********************************************************************************/
int8 GPS_ValidationNMEAData(void)
{
    uint32 ui32MaskValidationLocal = ui32MaskValidation;
    uint8 ui8CounterValidation = 0; 
    for(ui8CounterValidation = 0; ui8CounterValidation <= 32; ui8CounterValidation++ )
    {
        ///check  
        if((ui32MaskValidationLocal & 0x01) == 0x01)
        {
            ///switch 
            switch(ui8CounterValidation)
            {
                ///Status Valid (status)
                case 0u:
                    if( ((GPSTempData.cStatus) >= (GPSValidationParameters.cStatusDown)) &&
                        (((GPSTempData.cStatus) <= (GPSValidationParameters.cStatusUP))) )
                    {
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.STATUS = false; 
                    }
                    else
                    {
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.STATUS = true; 
                    }
                    break;
                ///Latitude     
                case 1u:
                    /*if( ((GPSTempData.fLatitude) >= (GPSValidationParameters.fLatitudeDown)) &&
                        ((GPSTempData.fLatitude) <= (GPSValidationParameters.fLatitudeUp)) )
                    {*/
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.LATITUD = false;                                         
                    /*}
                    else
                    {   
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.LATITUD = true; 
                    }*/
                    break;
                ///Longitude
                case 2u:
                    /*if( ((GPSTempData.fLongitude) >= (GPSValidationParameters.fLongitudeDown)) &&
                        ((GPSTempData.fLongitude) <= (GPSValidationParameters.fLongitudeUp)) )
                        {*/
                            GPSValidationParameters.FlagsErrorNmeaData.Bits.LONGITUD = false;                                         
                     /*   }
                    else
                        {
                            GPSValidationParameters.FlagsErrorNmeaData.Bits.LONGITUD = true;
                        }*/
                    break;
                case 3u:
                    break; 
                case 4u:
                    break; 
                case 5u:
                    break; 
                ///ui32MiliSecondsTime        
                case 6u:
                    if (((GPSTempData.ui32MiliSecondsTime) >= (GPSValidationParameters.sDateTimeDown.Time.u32mSeconds)) && 
                       (((GPSTempData.ui32MiliSecondsTime) <= (GPSValidationParameters.sDateTimeUp.Time.u32mSeconds))))
                        {
                            GPSValidationParameters.FlagsErrorNmeaData.Bits.MILISECONDS = false;                                         
                        }
                    else 
                        {
                            GPSValidationParameters.FlagsErrorNmeaData.Bits.MILISECONDS = true;
                        }
                    break;    
                case 7u:
                    break; 
                ///Satelites number
                case 8u:
                    if( ((GPSTempData.ui8NSats) >= (GPSValidationParameters.ui8NSatsDown) ) && 
                        ((GPSTempData.ui8NSats) <= (GPSValidationParameters.ui8NSatsUp)) )
                    {
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.SATELITES_NUMBER = false;                                        
                    }
                    else
                    {
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.SATELITES_NUMBER = true;
                    }
                    break;
                case 9u:
                ///Hdop
                    if( ((GPSTempData.fHdop) >= (GPSValidationParameters.fHdopDown)) &&
                        ((GPSTempData.fHdop) <= (GPSValidationParameters.fHdopUp)))
                    {
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.HDOP = false;                                        
                    }
                    else
                    {
                        GPSValidationParameters.FlagsErrorNmeaData.Bits.HDOP = true;      
                    }
                    break;
                case 10u:                       
                    break;
            }
        }
        ui32MaskValidationLocal = ui32MaskValidationLocal >> 1;
    }
    return GPSValidationParameters.FlagsErrorNmeaData.Byte;
}   

/**********************************************************************************
End of function GPS_ValidationNMEAData
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_GetDesiredFields
@author     Diego Ferney Nunez.
@brief	    Get fields of interest from NMEA frame configurated in sConfigNMEA, 
            this take the information from NMEA frame stored in a buffer by UART.
@pre        Have recived NMEA frame by UART and UARTChecksum of NMEA frame ok.
@post       if all goes well the fields of interes are stored in GPSTempData.
@param	    none. 
@return     u8CountFieldsFound -> Return amount fields extrated.
*/
/**********************************************************************************/
uint8 GPS_GetDesiredFields(void)
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
#define SS 255
    
    uint16  u16StartNMEA = 0;
    uint16  u16EndNMEA = 0;
    uint16  u16LocalProcess = 0;

    uint8   u8Header[3u];
    uint8   u8IndexField = 0x00;  //?????
    
    uint8   u8IndexNMEAType = 0xFF;
    uint32  u32FrameFieldsMask = 0;
    uint16  u16CountFieldsFound = 0;
   
    static char cComandToGPS[] = "$PUBX,40,XXX,0,0,0,0,0,0*5C\r\n";
    uint8 u8ComandToGPSCRC = 0x1B;
    
//    char  bf[SS];

//printSpyDebug(0x1000);
    //UART_GPS_Stop();
    
    //GPS_PrintUARTGPSData(true, false, true);
    while ( UARTGPSCommandRx==true )
    {
        u8IndexField = 0x01;
//printSpyDebug(0x1001);
        /*
        UsrWifiEthB2_UARTWifiEthSendData((uint8*)"==============================\n",31);
        memset((void*)bf,0x00,SS);
        sprintf(bf, "Index field: %d\n", u8IndexField);
        UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
        */
        /// Check BufferRxUART for header
        u16StartNMEA = RX_HEADER_ARRAY_POS(RX_FRAME_PROCESSED_INDEX);
        u16EndNMEA = RX_END_ARRAY_POS(RX_FRAME_PROCESSED_INDEX);
        
        /*
        memset((void*)bf,0x00,SS);
        sprintf(bf, "Start NMEA: %d\nEnd NMEA: %d\n", u16StartNMEA, u16EndNMEA);
        UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
        UsrWifiEthB2_UARTWifiEthSendData((uint8*)"Trama ==> ",10);
        UsrWifiEthB2_UARTWifiEthSendData((uint8*)&RX_BUFFER_POS(u16StartNMEA),u16EndNMEA-u16StartNMEA);
        */
        if(  (u16StartNMEA < u16EndNMEA) )  //IF 1
        {
            //TODO: ERASE Print
//printSpyDebug(0x1002);
            ///Verify checksum between $ and 0x0d
            if( GPS_NmeaChecksumVerify(u16StartNMEA, u16EndNMEA) == EXIT_SUCCESS ) // IF 2
            {
                //TODO: ERASE Print
//printSpyDebug(0x1003);
                memset((void*)u8Header,0x00,3);
                /// Get header throgth first comma
                for( uint16 i=u16StartNMEA; i<=u16EndNMEA; i++ )
                {
                    if( RX_BUFFER_POS(i) == ',') // IF 3
                    {
                        //TODO: ERASE Print
//printSpyDebug(0x1004);
                        u8Header[2u] = RX_BUFFER_POS(i-1u);
                        u8Header[1u] = RX_BUFFER_POS(i-2u);
                        u8Header[0u] = RX_BUFFER_POS(i-3u);
                        break;
                    }
                }
                //TODO: ERASE Print
//printSpyDebug(0x1005);
                //REVISAR SI ESTE IF ES NECESARIO
                if ( (u8Header[2u]!=0x00) && (u8Header[1u]!=0x00) && (u8Header[0u]!=0x00) )  // IF 4
                {
                //TODO: ERASE Print
//printSpyDebug(0x1006);
                    //UsrWifiEthB2_UARTWifiEthSendData((uint8*)u8Header,3);
                    //UsrWifiEthB2_UARTWifiEthSendData((uint8*)" ==> Header\n",12);
                }
//printSpyDebug(0x1007);
                /// The NMEA Frame is of interest?
                ///Search
                GPSDataProcess.bFlagHeaderOk = false;
                for( uint16 i=0; i<(sizeof(sConfigNMEA)/sizeof(sConfigNMEA[0])) ; i++ )
                {
                    u8IndexNMEAType = GPS_IndexNMEAType(u8Header);
                    if(strstr((char*)u8Header,(char*)(sConfigNMEA[i].aFrameName)) != NULL )  // IF 5
                    {
                        //TODO: ERASE Print
//printSpyDebug(0x1008);
                        u32FrameFieldsMask = sConfigNMEA[i].ui32FrameFields;
                        //memset((void*)bf,0x00,SS);
                        //sprintf(bf, "Frame Fields Mask: %lu\n",u32FrameFieldsMask);
                        //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
                        GPSDataProcess.bFlagHeaderOk = true;
                        break;
                    }
                }
//printSpyDebug(0x1009);
                /// yes is of interest
                ///Search header NMEA ok
                ///Now get fields
                if(GPSDataProcess.bFlagHeaderOk == true)   // IF 6
                {
                    //TODO: ERASE Print
//printSpyDebug(0x1010);
                    u16LocalProcess = u16StartNMEA;
                    //memset((void*)bf,0x00,SS);
                    //sprintf(bf, "Local Process: %d\n", u16LocalProcess);
                    //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
                    ///Buscar comas(campos) entre la primer coma y el fin de trama                    
//printSpyDebug(0x1011);
                    while( u16LocalProcess<u16EndNMEA )
                    {  
                        ///Search comma string
                        if( RX_BUFFER_POS(u16LocalProcess)==',' )  // IF 7
                        {
                            ///check  
                            //TODO: ERASE Print
//printSpyDebug(0x1012);
                            if( (u32FrameFieldsMask&0x01)==0x01 )  //IF 8
                            {
                                //TODO: ERASE Print
//printSpyDebug(0x1013);
                                //SIN USO??
                                //GPSDataProcess.u8BufferPointersFields[u16CountFieldsFound] = (uint8*)&RX_BUFFER_POS(u16LocalProcess+1);
                                GPS_StoreNMEAData(u8IndexField, u8IndexNMEAType, (u16LocalProcess+1) );
                                ///Down                     
                                u16CountFieldsFound++;
                                //TODO: ERASE Print
                                //memset((void*)bf,0x00,SS);
                                //sprintf(bf, "Count Fields Found: %d \n", u16CountFieldsFound);
                                //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));                              
                            }
//printSpyDebug(0x1014);
                            /// If there aren't fields to process 
                            if(u32FrameFieldsMask == 0x00)  //IF 9
                            {
//printSpyDebug(0x1015);
                                switch (u8IndexNMEAType)  // SWITCH 1
                                {
                                    case 0u:
                                    {
//printSpyDebug(0x1016);
                                        ///Mask to check all NMEA have been processed
                                        GPSDataProcess.ui32MaskHeaderTypeCheckProcess |= 0x01;
                                        break;
                                    }
                                        case 1u:
                                    {
//printSpyDebug(0x1017);
                                        GPSDataProcess.ui32MaskHeaderTypeCheckProcess |= 0x02;
                                        break;
                                    }
                                    case 2u:
                                    {
//printSpyDebug(0x1018);
                                        GPSDataProcess.ui32MaskHeaderTypeCheckProcess |= 0x04;
                                        break;

                                    }

                                    default:
                                    {
//printSpyDebug(0x1020);
                                        break; 
                                    }
                                }

                            }                             
                            ///Store fields desired
//printSpyDebug(0x1021);
                            u32FrameFieldsMask = u32FrameFieldsMask >> 1;
                            //TODO: ERASE Print
                            //memset((void*)bf,0x00,SS);
                            //sprintf(bf, "Frame Fields Mask: %lu\n", u32FrameFieldsMask);
                            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
                            u8IndexField++;
                            //memset((void*)bf,0x00,SS);
                            //sprintf(bf, "Index Field: %d\n",u8IndexField);
                            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
                        }
//printSpyDebug(0x1022);
                        u16LocalProcess++;
                        //TODO: ERASE Print
                        //memset((void*)bf,0x00,SS);
                        //sprintf(bf, "Local Process: %d\n", u16LocalProcess);
                        //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
                    }
                    /// Clear flag
                    GPSDataProcess.bFlagHeaderOk = false;
//printSpyDebug(0x1023);
                }
                else  //ELSE 1
                {
                    //TODO: ERASE Print
//printSpyDebug(0x1024);
                    cComandToGPS[9] = u8Header[0]; 
                    cComandToGPS[10] = u8Header[1];
                    cComandToGPS[11] = u8Header[2];
                    //Enviar comando para anularlo
                    u8ComandToGPSCRC ^= cComandToGPS[9];
                    u8ComandToGPSCRC ^= cComandToGPS[10];
                    u8ComandToGPSCRC ^= cComandToGPS[11];
                    sprintf(&cComandToGPS[25], "%02X\r\n", u8ComandToGPSCRC);
                    //NEW TODO:  FUNCION PARA ENVIAR DATOS AL GPS Y BLOQUEAR  TRAMAS NO NECESARIAS
                    uartGPS_SendDataGPS((uint8*)cComandToGPS, 29);
                }
//printSpyDebug(0x1025);
            }
//printSpyDebug(0x1026);
            RX_BUFFER_PROCESSED_INDEX = u16EndNMEA+1;

            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "Buffer Processed Index; %d\n", RX_BUFFER_PROCESSED_INDEX);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            RX_FRAME_PROCESSED_INDEX++;
            if ( RX_FRAME_PROCESSED_INDEX<RX_FRAME_INDEX )
            {
//printSpyDebug(0x1027);
                UARTGPSCommandRx = true;
            }
            else
            {
//printSpyDebug(0x1028);
               UARTGPSCommandRx = false;
            }
//printSpyDebug(0x1029);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "Header Processed Index; %d\n", RX_FRAME_PROCESSED_INDEX);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
        }
        else
        {
//printSpyDebug(0x1030);
            GPSDataProcess.bFlagHeaderOk = false;
        }
    }
//printSpyDebug(0x1031);
    
    //if (RX_FRAME_PROCESSED_INDEX>=RX_FRAME_INDEX) //IF 10
    if (RX_BUFFER_INDEX>=RX_BUFFER_PROCESSED_INDEX) //IF 10
    {
//printSpyDebug(0x1032);
        uartGPS_ResetUARTGPSData(NMEAPro_WaitingHeader);
    }
//printSpyDebug(0x1033);    
    //UART_GPS_Start();
    return 0;

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
#undef SS
}
/**********************************************************************************
End of function GPS_GetDesiredFields
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_this_NmeaChecksumVerify
@author     Cesar Sarmiento, Diego Nunez
@brief	    This function returns a 0 or a 1 depending on whether the frame entered
            * As function argument has a correct (1) or false checksum (0). 
@pre        Having received NMEA frame package by UART and having get pointer of '$' and '0x0d'
@post       none
@param	    ucpNmeaFrameStart -> Pointer start of NMA to analyze, should be indicate psition of $.
            ucpNmeaFrameStop -> Pointer finish of NMA to analyze, should be indicate psition of 0x0d.
@return     ucVerificationResult -> Result of process.
*/
/**********************************************************************************/
uint8 GPS_NmeaChecksumVerify(uint16 u16FrameStart, uint16 u16FrameEnd)
{
#define RX_BUFFER_POS(i)           UARTGPSData.cDataUARTGPSRx[i]    

    uint8 u8CheckSumResult = 0x00;
    uint8 i8HexCRC[2] = { 0x00, 0x00};
            for ( int i=u16FrameStart+1; i<u16FrameEnd-3; i++)
            {
                u8CheckSumResult ^= RX_BUFFER_POS(i);
            }
            i8HexCRC[0] = RX_BUFFER_POS(u16FrameEnd-2);
            i8HexCRC[1] = RX_BUFFER_POS(u16FrameEnd-1);
            if ( u8CheckSumResult==(uint16)strtol((const char*)&i8HexCRC, NULL, 16) )
            {
                return EXIT_SUCCESS;
            }
            else
            {
                return EXIT_FAILURE;
            }

#undef RX_BUFFER_POS  
}
/**********************************************************************************
End of function GPS_NmeaChecksumVerify
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_IndexNMEAType
@author     Diego Ferney Nunez.
@brief	    Compare header string received with header string expected and return a
            weight index used to by "GPS StoreNMEAData" function to know which type
            NMEA has arrived and therefore which fields has arrived.              
@pre        Have recived a NMEA frame with checksum ok.
@post       none.
@param	    u8pPointerIndexNMEAType -> Adress Header recived
@return     Index weight NMEA Type.
*/
/**********************************************************************************/
inline uint8 GPS_IndexNMEAType(const uint8 *u8pNMEAType)
{ 
    if(strstr((char*) u8pNMEAType,"RMC") != NULL)
    {
        return 0u;                                         
    }
    else if(strstr((char*) u8pNMEAType,"GGA") != NULL)
    {
        return 1u;
    }
    else if(strstr((char*) u8pNMEAType,"VTG") != NULL)
    {
        return 2u;
    }
    else
    {
        return 0xff;
    }
}    
/**********************************************************************************
End of function GPS_IndexNMEAType
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEAData.
@author     Diego Ferney Nunez.
@brief	    If the field is required, this function store that field of NMEA frame in the
            structure GPSTempData.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       The data specified is store on the respective fields of GPSTempData.
@param	    u8pBuffer   ->  Byte to analyze. 
@return     i8status    ->  Retunr the status process.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEAData( uint8 u8Index, uint8 u8NMEAType, uint16 u16BufferIndexOfData)
{   
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]    
#define HOUR               GPSData.sDateTime.Time.u8Hour
#define MINS               GPSData.sDateTime.Time.u8Minutes 
#define SECS               GPSData.sDateTime.Time.u8Seconds
#define MSECS              GPSData.sDateTime.Time.u32mSeconds       
#define DAY                GPSData.sDateTime.Date.u8Day
#define MONTH              GPSData.sDateTime.Date.u8Month
#define YEAR               GPSData.sDateTime.Date.u16Year        
    
#define SS 255
    
    int8 i8status = EXIT_SUCCESS;

    //TODO: ERASE Print
//    int32 entero = 0; 
//    float32 fdec = 0;
//    int32 decimal = 0;

    /// u8FielToProces indicates the field to process  
    uint8_t u8Field = u8NMEAType*32u + u8Index;

    //TODO: ERASE Print
    //char  bf[SS];
    //memset((void*)bf,0x00,SS);
    //sprintf(bf, "\nGPS_StoreNMEAData:\nIndex: => %d\nu8Field: => %d\nBUFFER:\n",u16BufferIndexOfData, u8Field);
    //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));

    /*
    //TODO: ERASE Print
    //for (int i=u16BufferIndexOfData; i<u16BufferIndexOfData+23; i++)
    //{
    //    UsrWifiEthB2_UARTWifiEthSendData((uint8*)&RX_BUFFER_POS(i), 1);
    //}
    //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"\n", 1);
    */
    switch (u8Field)
    {
        /// Hour, Minutes and seconds
        case RMC_INDEX_TIME:
        case GGA_INDEX_TIME:
            i8status = GPS_StoreNMEATime(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "GPS_StoreNMEATime (HH:MM:SS.ms)=> %02d:%02d:%02d.%lu\n",HOUR,MINS,SECS,MSECS );
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// Status
        case RMC_INDEX_STATUS:
            i8status = GPS_StoreNMEAStatus(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "GPS_StoreNMEAStatus => %d\n",GPSTempData.cStatus);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// Latitude
        case RMC_INDEX_LATITUDE:
        case GGA_INDEX_LATITUDE:
            i8status = GPS_StoreNMEALatitude(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //int32 entero = GPSTempData.fLatitude;
            //float32 fdec = GPSTempData.fLatitude - entero;
            //int32 decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEALatitude => %ld.%ld\n",entero, decimal);            
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// Latitude Dir
        case RMC_INDEX_LAT_SIGN: 
        case GGA_INDEX_LAT_SIGN:
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fLatitude;
            //fdec = GPSTempData.fLatitude - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEALatitudeSign => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            i8status = GPS_StoreNMEALatitudeSign(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fLatitude;
            //fdec = GPSTempData.fLatitude - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "New value  => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// Longitude
        case RMC_INDEX_LONGITUDE:
        case GGA_INDEX_LONGITUDE:
            i8status = GPS_StoreNMEALongitude(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fLongitude;
            //fdec = GPSTempData.fLongitude - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEALongitude => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// Longitude Dir
        case RMC_INDEX_LON_SIGN:
        case GGA_INDEX_LON_SIGN:
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fLongitude;
            //fdec = GPSTempData.fLongitude - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEALongitudeSign => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            i8status = GPS_StoreNMEALongitudeSign(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fLongitude;
            //fdec = GPSTempData.fLongitude - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "New value => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));            
            break;
            
        /// RMC Speed knots
        case RMC_INDEX_SPEEDKNOT:
        /// VTG Speed knots
        case VTG_INDEX_SPEED_KNOTS:
            i8status = GPS_StoreNMEASpeedKnot(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fSpeedKnot;
            //fdec = GPSTempData.fSpeedKnot - entero;
            //decimal = (int32)(fdec*1000);
            //sprintf(bf, "GPS_StoreNMEASpeedKnot => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// Track Angle
        case RMC_INDEX_COURSE:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"RMC_INDEX_COURSE Funcion no definida\n", 37);
            break;
            
        /// Date    
        case RMC_INDEX_DATE:
            i8status = GPS_StoreNMEADate(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "GPS_StoreNMEADate (DD/MM/YYYY)=> %02d/%02d/%04d\n",DAY,MONTH,YEAR );
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        // Magnetic Variation
        case RMC_INDEX_MAGV:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"RMC_INDEX_MAGV Funcion no definida\n", 35);
            break;
            
        // Magnetic Variation sign
        case RMC_INDEX_MAGV_SIGN:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"RMC_INDEX_MAGV Funcion no definida\n", 40);
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_FIX_QUALITY:
            i8status = GPS_StoreNMEAFixQualityStatus(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "GPS_StoreNMEAFixQualityStatus => %d\n",GPSTempData.ui8FixQuality);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_NUMSATS:
            i8status = GPS_StoreNMEASatelitesNumber(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //sprintf(bf, "GPS_StoreNMEASatelitesNumber => %d\n",GPSTempData.ui8NSats);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_HDOP:
            i8status = GPS_StoreNMEAHDop(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fHdop;
            //fdec = GPSTempData.fHdop - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEAHDop => %ld.%ld\n",entero, decimal);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_HEIGHT:
            i8status = GPS_StoreNMEAHeight(u16BufferIndexOfData);
            //TODO: ERASE Print
            //memset((void*)bf,0x00,SS);
            //entero = GPSTempData.fHeight;
            //fdec = GPSTempData.fHeight - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}            
            //sprintf(bf, "GPS_StoreNMEAHeight => %ld.%ld\n",entero, decimal);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;

        /// GGA Fix status 
        case GGA_INDEX_HEIGHT_UNIT:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"GGA_INDEX_HEIGHT_UNIT Funcion no definida\n", 42);
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_GEO_SEP:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"GGA_INDEX_GEO_SEP Funcion no definida\n", 38);
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_GEO_SEP_UNIT:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"GGA_INDEX_GEO_SEP_UNIT Funcion no definida\n", 43);
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_DGPS_AGE:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"GGA_INDEX_DGPS_AGE Funcion no definida\n", 39);
            break;
            
        /// GGA Fix status 
        case GGA_INDEX_DGPS_ID:
            i8status = EXIT_FAILURE;
            //TODO: ERASE Print
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)"GGA_INDEX_DGPS_ID Funcion no definida\n", 38);
            break;   
            
        /// VTG true course 
        case VTG_INDEX_TRUE_COURSE:
            //TODO: ERASE Print
            //entero = GPSTempData.fTrueCourse;
            //fdec = GPSTempData.fTrueCourse - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEATrueCourse => %ld.%ld\n",entero, decimal);             
            i8status = GPS_StoreNMEATrueCourse(u16BufferIndexOfData);
            //TODO: ERASE Print
            //entero = GPSTempData.fTrueCourse;
            //fdec = GPSTempData.fTrueCourse - entero;
            //decimal = (int32)(fdec*1000);
            //sprintf(bf, "New value => %ld.%ld\n",entero, decimal);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// VTG Magnetic course 
        case VTG_INDEX_MAG_COURSE:
            i8status = GPS_StoreNMEAMagCourse(u16BufferIndexOfData);                
            //TODO: ERASE Print
            //entero = GPSTempData.fMagCourse;
            //fdec = GPSTempData.fMagCourse - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEAMagCourse => %ld.%ld\n",entero, decimal);
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
            break;
            
        /// VTG Speed KMh 
        case VTG_INDEX_SPEED_KMH:
            i8status = GPS_StoreNMEASpeedKmh(u16BufferIndexOfData);
            //TODO: ERASE Print
            //entero = GPSTempData.fSpeedKmh;
            //fdec = GPSTempData.fSpeedKmh - entero;
            //decimal = (int32)(fdec*1000);
            //if ( decimal<0)
            //{
            //    decimal = -decimal;
            //}
            //sprintf(bf, "GPS_StoreNMEASpeedKmh => %ld.%ld\n",entero, decimal);             
            //UsrWifiEthB2_UARTWifiEthSendData((uint8*)bf,FindSize(bf));
           break;
            
        /// No data found, returns error
        default:
            i8status = EXIT_FAILURE;
    break;
    }

    //TODO: ERASE Print
    //free (bf);
    return i8status;

#undef RX_BUFFER_POS
#undef HOUR     
#undef MINS     
#undef SECS     
#undef MSECS    
#undef DAY  
#undef MONTH
#undef YEAR 
#undef SS    
}
/**********************************************************************************
End of function GPS_StoreNMEAData
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEADate
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            store the Date from NMEA frame to GPSTempData.ui8aDate.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.       
@post       The date (Day, Month and Year) has been stored in GPSTempData.ui8aDate  
@param	    u8pBuffer -> Adress to get Time   
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEADate(uint16 u16Index)
{
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]    
#define DAY                GPSTempData.sDateTime.Date.u8Day
#define MONTH              GPSTempData.sDateTime.Date.u8Month
#define YEAR               GPSTempData.sDateTime.Date.u16Year
#define EPOCH              GPSTempData.ui32Epoch

    DAY =   MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index), RX_BUFFER_POS(u16Index+1));
    MONTH = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+2), RX_BUFFER_POS(u16Index+3));
    YEAR =  MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+4), RX_BUFFER_POS(u16Index+5)) + BASE_YEAR;
    EPOCH = GPS_TotalSecondsFromDateTimeGPS();
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS
#undef DAY      
#undef MONTH    
#undef YEAR
#undef EPOCH    
}
/**********************************************************************************
End of function GPS_StoreNMEADate
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEAHDop
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then store 
            the "Horizontal dilution of position" from NMEA frame to GPSTempData.ui8Hdop.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The "Horizontal dilution of position" has been stored in GPSTempData.ui8Hdop.
@param	    u8pBuffer -> Adress to get HDOP 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEAHDop(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)    UARTGPSData.cDataUARTGPSRx[i]
#define HDOP                GPSTempData.fHdop     
    

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};

    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    HDOP = atof((const char*) u8val);
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS
#undef HDOP
}
/**********************************************************************************
End of function GPS_StoreNMEAHDop
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEATime
@author     Diego Ferney Nunez.
@brief	    Store the Time (Hours, Minutes, Seconds) from NMEA frame to GPSTempData.ui8aTime.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       Time is stored in GPSTempData.ui8aTime.
@param	    u8pBuffer -> Adress to get Time.   
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEATime(uint16 u16Index)
{  
//TODO: Mejorar la l√≥gica de los for
#define RX_BUFFER_POS(i)      UARTGPSData.cDataUARTGPSRx[i]   
#define HOUR                  GPSTempData.sDateTime.Time.u8Hour
#define MINS                  GPSTempData.sDateTime.Time.u8Minutes
#define SECS                  GPSTempData.sDateTime.Time.u8Seconds
#define MSECS                 GPSTempData.sDateTime.Time.u32mSeconds
#define EPOCH                 GPSTempData.ui32Epoch
    

    ///Conver ASCII to Hexa then conver Hexa to Decimal finally store time
    int16  u16NumbersOfDecimals = 0;
    uint16 u16StartIndex = u16Index;
    uint16 u16DotIndex = 0;
    uint16 u16CommaIndex = 0;
    uint8  u8Val[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    HOUR = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index), RX_BUFFER_POS(u16Index+1));
    MINS = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+2), RX_BUFFER_POS(u16Index+3));
    SECS = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+4), RX_BUFFER_POS(u16Index+5));

    //Looking for miliSec between '.' and ','
    for ( uint16 i=u16StartIndex; i<=(u16StartIndex+30); i++)
    {
        if( RX_BUFFER_POS(i)=='.' )
        {
            u16DotIndex = i;
        }
        else if( RX_BUFFER_POS(i)==',' )
        {
            u16CommaIndex = i;
            break;
        }
    }

    u16NumbersOfDecimals = u16CommaIndex - u16DotIndex-1;
    if (u16NumbersOfDecimals > 0 )
    {
        if (u16NumbersOfDecimals>3)
        {
            //Ignore more than 4 decimals
            u16NumbersOfDecimals=3;
        }
        for ( uint16 i=0; i<u16NumbersOfDecimals; i++)
        {
            if ( (RX_BUFFER_POS(u16DotIndex+i+1)>=0x030) &&
                 (RX_BUFFER_POS(u16DotIndex+i+1)<=0x039) )
            {
                u8Val[i] = RX_BUFFER_POS(u16DotIndex+i+1);
            }
            else
            {
                //Something wrong, Character isn't numerical
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        //There isn't miliSecs
        u16NumbersOfDecimals = 0;
    }
    MSECS = atol((char*)(u8Val));
    switch(u16NumbersOfDecimals) 
    {
        case 0u:
            MSECS = 0x00;
            break;
        case 1u:
            MSECS = MSECS*100;
            break; 
        case 2u:
            MSECS = MSECS*10;
            break; 
        case 3u:
            MSECS = atol((char*)(u8Val));
            break; 
        default: 
            MSECS = atol((char*)(u8Val));
    }
    
    EPOCH = GPS_TotalSecondsFromDateTimeGPS();
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS
#undef HOUR     
#undef MINS     
#undef SECS     
#undef MSECS
#undef EPOCH
}
/**********************************************************************************
End of function GPS_StoreNMEATime
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEAHeight
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then 
            store the "Height" from NMEA frame to GPSTempData.i16Height
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The "Height" has been stored in GPSTempData.i16Height. 
@param	    u8pBuffer -> Adress to get HDOP Height
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEAHeight(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)    UARTGPSData.cDataUARTGPSRx[i]
#define HEIGHT              GPSTempData.fHeight
    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};

    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    HEIGHT = atof((const char*) u8val);
    return EXIT_SUCCESS;    

#undef RX_BUFFER_POS
#undef HEIGHT
}
/**********************************************************************************
End of function GPS_StoreNMEAHeight
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEAStatus
@author     Diego Ferney Nunez.
@brief	    Get Status from NMEA frame and then store this in GPSTempData.cStatus.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       the status "A" or "V" is stored in GPSTempData.cStatus.
@param	    u8pBuffer   ->  Adress to get Status. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEAStatus(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]
#define STATUS             GPSTempData.cStatus 

 //   uint8 u8val[2] = {0x00, 0x00};
    if ( RX_BUFFER_POS(u16Index)=='A' || RX_BUFFER_POS(u16Index)=='V' ||
         RX_BUFFER_POS(u16Index)=='a' || RX_BUFFER_POS(u16Index)=='v')
    {
        STATUS = RX_BUFFER_POS(u16Index);
        return EXIT_SUCCESS;
    }
    else
    {
        STATUS = 0x00;
        return EXIT_FAILURE;
    }

#undef RX_BUFFER_POS
#undef STATUS    
}
/**********************************************************************************
End of function GPS_StoreNMEAStatus
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEALatitude
@author     Diego Ferney Nunez.
@brief	    Get Latitude in sexagesimal format from NMEA frame and then store this in
            GPSTempData.fLatitude.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       none
@param	    u8pBuffer   ->  Adress to get Latitude. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEALatitude(uint16 u16Index)
{
#define RX_BUFFER_POS(i)  UARTGPSData.cDataUARTGPSRx[i]
#define LATITUDE          GPSTempData.sLatitude
    
    // First find the point to reference all decodification
    while(UARTGPSData.cDataUARTGPSRx[u16Index] >= '0'){ u16Index++; }
    if(RX_BUFFER_POS(u16Index) != '.')
    {
        LATITUDE.u8Degrees = 0;
        LATITUDE.u8Minutes = 0;
        LATITUDE.u16MilMin = 0;
        return EXIT_FAILURE;
    }
    LATITUDE.u8Degrees = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index-4), RX_BUFFER_POS(u16Index-3));
    LATITUDE.u8Minutes = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index-2), RX_BUFFER_POS(u16Index-1));
    LATITUDE.u16MilMin = (100*MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+1), RX_BUFFER_POS(u16Index+2))) +
                         MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+3), RX_BUFFER_POS(u16Index+4));
    return EXIT_SUCCESS;
    
#undef RX_BUFFER_POS
#undef LATITUDE
}
/**********************************************************************************
End of function GPS_StoreNMEALatitude
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEASpeedKmh
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            the "Speed at Kmh" from NMEA frame to GPSTempData.fSpeedKmh
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The "Speed at Kmh" has been stored in GPSTempData.fSpeedKmh    
@param	    u8pBuffer -> Adress to get SpeedKmh
@return     int8 i8status.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEASpeedKmh(uint16 u16Index)
{
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]
#define SPEED_KMH          GPSTempData.fSpeedKmh     
    

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};

    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    SPEED_KMH = atof((const char*) u8val);    
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS
#undef SPEED_KMH    
}
/**********************************************************************************
End of function GPS_StoreNMEASpeedKmh
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEASpeedKnot
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            store the Speed in Knot from NMEA frame to GPSTempData.fSpeedKnot.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.         
@post       the Speed in knot is store on GPSTempData.fSpeedKnot.
@param	    u8pBuffer   ->  Adress to get Speed. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEASpeedKnot(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]
#define SPEED_KNOT         GPSTempData.fSpeedKnot 

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};

    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    SPEED_KNOT = atof((const char*) u8val);    
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS   
#undef SPEED_KNOT
}
/**********************************************************************************
End of function GPS_StoreNMEASpeedKnot
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEALongitude
@author     Diego Ferney Nunez.
@brief	    Get Longitude in sexagesimal format from NMA frame and then store this in
            GPSTempData.fLongitude.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       none. 
@param	    u8pBuffer   ->  Adress to get Longitude. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEALongitude(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]    
#define LONGITUDE          GPSTempData.sLongitude     
    // First find the point to reference all decodification
    while(UARTGPSData.cDataUARTGPSRx[u16Index] >= '0'){ u16Index++; }
    if(RX_BUFFER_POS(u16Index) != '.')
    {
        LONGITUDE.u8Degrees = 0;
        LONGITUDE.u8Minutes = 0;
        LONGITUDE.u16MilMin = 0;
        return EXIT_FAILURE;
    }
    LONGITUDE.u8Degrees = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index-4), RX_BUFFER_POS(u16Index-3));
    LONGITUDE.u8Minutes = MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index-2), RX_BUFFER_POS(u16Index-1));
    LONGITUDE.u16MilMin = (100*MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+1), RX_BUFFER_POS(u16Index+2))) +
                         MiscFunctions_DecodASCIIDecimalToHexa(RX_BUFFER_POS(u16Index+3), RX_BUFFER_POS(u16Index+4));  
    return EXIT_SUCCESS;    

#undef RX_BUFFER_POS
#undef LONGITUDE
}
/**********************************************************************************
End of function GPS_StoreNMEALongitude
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEAMagCourse
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            store the "Magnetic Course" from NMEA frame to GPSTempData.fMagCourse.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The "Magnetic Course" has been stored in GPSTempData.fMagCourse.   
@param	    u8pBuffer -> Adress to get MagCourse. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEAMagCourse(uint16 u16Index)
{
#define RX_BUFFER_POS(i)  UARTGPSData.cDataUARTGPSRx[i]
#define MAG_COURSE        GPSTempData.fMagCourse 

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};

    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    MAG_COURSE = atof((const char*) u8val);    
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS 
#undef MAG_COURSE
}
/**********************************************************************************
End of function GPS_StoreNMEAMagCourse
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEATrueCourse
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            store the "TrueCourse" from NMEA frame to GPSTempData.fTrueCourse
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The "TrueCourse" has been stored in GPSTempData.fTrueCourse
@param	    u8pPointerTrueCourse -> Adress to get TrueCourse
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEATrueCourse(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]
#define TRUE_COURSE        GPSTempData.fTrueCourse

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};

    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    TRUE_COURSE = atof((const char*) u8val);    
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS
#undef TRUE_COURSE
}
/**********************************************************************************
End of function GPS_StoreNMEATrueCourse
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEALatitudeSign
@author     Diego Ferney Nunez.
@brief	    Get Latitude's direction "N" north or "S" south of NMA frame and then assign
            to respective sign in GPSTempData.fLatitude.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       none.
@param	    u8pBuffer   ->  Adress to get Latitude's direction. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEALatitudeSign(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]
#define ORIENTATION        GPSTempData.u8OrientationByte

    GPSTempData.sLatitude.cOrientation = RX_BUFFER_POS(u16Index);
    if(RX_BUFFER_POS(u16Index) == 'S')
    {
        GPSTempData.u8OrientationByte |= 0x04;
        return EXIT_SUCCESS;
    }
    else if (RX_BUFFER_POS(u16Index) == 'N')
    {
        GPSTempData.u8OrientationByte &= 0xFB;//0b11111011;
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }

#undef RX_BUFFER_POS
#undef ORIENTATION    
}
/**********************************************************************************
End of function GPS_StoreNMEALatitudeSign
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEALongitudeSign
@author     Diego Ferney Nunez.
@brief	    Get Longitude's direction "E" East and "W" West of NMA frame and then assgn
            to respective sign in GPSTempData.fLongitude.
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.
@post       none. 
@param	    u8pBuffer   ->  Adress to get Longitude's direction. 
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEALongitudeSign(uint16 u16Index)
{
#define RX_BUFFER_POS(i)    UARTGPSData.cDataUARTGPSRx[i]
#define ORIENTATION         GPSTempData.u8OrientationByte
    
    ///south latitude should be negative
    GPSTempData.sLongitude.cOrientation = RX_BUFFER_POS(u16Index);
    if(RX_BUFFER_POS(u16Index) == 'W')
    {
        GPSTempData.u8OrientationByte |= 0x02;//;0b00000010;
        return EXIT_SUCCESS;
    }
    else if (RX_BUFFER_POS(u16Index) == 'E')
    {
        GPSTempData.u8OrientationByte &= 0xFD;//0b11111101;
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
    
#undef RX_BUFFER_POS         
#undef ORIENTATION
}
/**********************************************************************************
End of function GPS_StoreNMEALongitudeSign
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEASatelitesNumber
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            store the "satelites number" from NMEA frame to GPSTempData.ui8NSats. 
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The satelites number has been stored in GPSTempData.ui8NSats. 
@param	    u8pBuffer -> Adress to get Satelites number   
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEASatelitesNumber(uint16 u16Index)
{
#define RX_BUFFER_POS(i)   UARTGPSData.cDataUARTGPSRx[i]
#define NUMBER_OF_SAT      GPSTempData.ui8NSats    

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};
    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    NUMBER_OF_SAT = atoi((const char*) u8val);    
    return EXIT_SUCCESS;

#undef RX_BUFFER_POS
#undef NUMBER_OF_SAT    
}
/**********************************************************************************
End of function GPS_StoreNMEASatelitesNumber
***********************************************************************************/



/**********************************************************************************/
/**
@name  	    GPS_StoreNMEAFixQualityStatus
@author     Diego Ferney Nunez.
@brief	    After valid checksum, match header and know if this are of interest then
            store the FixQuality from NMEA frame to GPSTempData.ui8FixQuality. 
@pre        Have recived a NMEA frame by UART, valid checksum ok, match header and verify if
            this field are of interest, finally should be configurated the bits into u32FrameFieldsMask.    
@post       The FixQuality has been stored in GPSTempData.ui8FixQuality
@param	    u8pBuffer -> Adress to get Fix Status.   
@return     none.
*/
/**********************************************************************************/
/*inline*/ int8 GPS_StoreNMEAFixQualityStatus(uint16 u16Index)
{   
#define RX_BUFFER_POS(i)  UARTGPSData.cDataUARTGPSRx[i]
#define FIX_QUALITY       GPSTempData.ui8FixQuality    

    uint8 u8val[15] = {0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00};
    for ( uint16 i=u16Index; i<u16Index+15; i++)
    {
        switch (RX_BUFFER_POS(i))
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                u8val[i-u16Index] = RX_BUFFER_POS(i);
                break;
            
            case ',':
                i+=100;
                break;

            default:
                i+=100;
                break;
        }
    }
    FIX_QUALITY = atoi((const char*) u8val);    
    return EXIT_SUCCESS;    

#undef RX_BUFFER_POS
#undef FIX_QUALITY    
}
/**********************************************************************************
End of function GPS_StoreNMEAFixQualityStatus
***********************************************************************************/


/**********************************************************************************
Name:          	
Description:    
Parameters:     
Returns:        
**********************************************************************************/
/*inline*/ unsigned long GPS_TotalSecondsFromDateTimeGPS(void)
{
#define YEAR  GPSTempData.sDateTime.Date.u16Year
#define MONTH GPSTempData.sDateTime.Date.u8Month
#define DAY   GPSTempData.sDateTime.Date.u8Day
#define HOUR  GPSTempData.sDateTime.Time.u8Hour
#define MINS  GPSTempData.sDateTime.Time.u8Minutes
#define SECS  GPSTempData.sDateTime.Time.u8Seconds     

    uint8  u8mon = MONTH;
    uint16 u16year = YEAR;

	if (0 >= (int) (u8mon -= 2)) 
    {
		u8mon += 12;	/* Puts Feb last since it has leap day */
		u16year -= 1;
	}
    
	return ( ( ( (unsigned long)
		         (u16year/4 - u16year/100 + u16year/400 + 367*u8mon/12 + DAY) + u16year*365 - 719499)*24 + HOUR /* now have hours */
	         )*60 + MINS /* now have minutes */
	       )*60 + SECS; /* finally seconds */

#undef YEAR      
#undef MONTH     
#undef DAY       
#undef HOUR      
#undef MINS      
#undef SECS
}
/**********************************************************************************
End of function GPS_TotalSecondsFromDateTimeGPS
***********************************************************************************/




/**********************************************************************************
Name:          	
Description:    
Parameters:     
Returns:        
**********************************************************************************/
void GPS_PrintGpsData(char *data)
{
    sprintf( data, "GPS Frame:\n  Status:%c\n  NSats:%d\n  Fix Quality:%d\n  Orientation:%d\n  Epoch:%lu\n  MiliSeconds:%lu\n  fHdop:%2.4f\n  Height:%2.4f\n  SpeedKmh:%2.4f\n  MagCourse:%2.4f\n  SpeedKnot:%2.4f\n  True Course:%2.4f\n  TimeStamp:  %02d/%02d/%04d %02d:%02d:%02d.%lu\n  Latitude:%02d%c %03dM %05dmM\n  Longitude:%02d%c %03dM %05dmM\n", 
        GPSData.cStatus, GPSData.ui8NSats, GPSData.ui8FixQuality, GPSData.u8OrientationByte,
        GPSData.ui32Epoch, GPSData.ui32MiliSecondsTime, GPSData.fHdop, GPSData.fHeight,
        GPSData.fSpeedKmh,  GPSData.fMagCourse, GPSData.fSpeedKnot,  GPSData.fTrueCourse,
        GPSData.sDateTime.Date.u8Day, GPSData.sDateTime.Date.u8Month, GPSData.sDateTime.Date.u16Year,
        GPSData.sDateTime.Time.u8Hour, GPSData.sDateTime.Time.u8Minutes, GPSData.sDateTime.Time.u8Seconds,
        GPSData.sDateTime.Time.u32mSeconds, GPSData.sLatitude.u8Degrees, GPSData.sLatitude.cOrientation,
        GPSData.sLatitude.u8Minutes, GPSData.sLatitude.u16MilMin, GPSData.sLongitude.u8Degrees,
        GPSData.sLongitude.cOrientation, GPSData.sLongitude.u8Minutes, GPSData.sLongitude.u16MilMin);
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
