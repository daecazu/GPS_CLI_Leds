/*******************************************************************************
* Filename: GPS.h
* Developer(s): Jorge Yesid Rios Ortiz
* Language: ANSI C
* Abstract: 
* References: 
*******************************************************************************/
#ifndef GPS_H
#define GPS_H
/*******************************************************************************
* HEADER FILES
*******************************************************************************/
/* ansi c */

/* propietary */
#include "../defines.h"
#include "../interfaces/uartGPS.h"
#include "GPS_Definitions.h"
#include "MiscFunctions.h"

/* operation system */



/*******************************************************************************
* GLOBAL CONSTANT DECLARATION
*******************************************************************************/

/*******************************************************************************
* GLOBAL MACRO DECLARATION
*******************************************************************************/


/*******************************************************************************
* GLOBAL NEW DATA TYPES
*******************************************************************************/

/**
@name  	    uFlagsErrNmeaData
@brief	    Byte union flag of error validation NMEA data
*/
typedef union 
{
  char Byte;
  struct 
  {
    char MILISECONDS        :1;
    char SATELITES_NUMBER   :1;
    char HDOP               :1;
    char STATUS             :1;
    char LATITUD            :1;
    char LONGITUD           :1;
    
  } Bits;
}uFlagsErrNmeaData;


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


/**
@name  	    sGPSConfig
@brief	    Contain configuration of class GPS mainly NMEA type and fields of interest
*/
 typedef struct
 {
  uint8  aFrameName[4];
  uint8  ui8WeightNMEA; 
  uint32 ui32FrameFields; 
 }sGPSConfig;

/**
@name  	    sGPSDataProcess
@brief	    Structure whose function is store data referent to procees of GPS information by UART2 
*/
typedef struct
{
    bool     bFlagHeaderOk;
    uint32   ui32ExpectedNMEAMask;
    uint32   ui32MaskHeaderTypeCheckProcess;
}sGPSDataProcess;


//Estructura de Fecha
typedef struct
{
    uint8  u8Day;
    uint8  u8Month;
    uint16 u16Year;
} sGPSDate;

//Estructura de Tiempo
typedef struct
{
    uint8  u8Hour;
    uint8  u8Minutes;
    uint8  u8Seconds;
    uint32 u32mSeconds;
} sGPSTime;


//Estructura de Fecha/Tiempo
typedef struct
{
    sGPSDate Date;
    sGPSTime Time;
} sGPSDateTime;

/**
@name  	    sGPSValidationParameters
@brief	    Configurable structure of upper and lower validation parameters
*/
typedef struct
{ 
    int8              cStatusUP;
    int8              cStatusDown;
    uint8             ui8NSatsUp;    
    uint8             ui8NSatsDown;
    uint8             ui8FixQualityUp;
    uint8             ui8FixQualityDown;
    uint32            ui32EpochUp;
    uint32            ui32EpochDown;
    float32           fHdopUp;
    float32           fHdopDown;
    float32           fHeightUp;
    float32           fHeightDown;
    float32           fLatitudeUp;
    float32           fLatitudeDown;
    float32           fSpeedKmhUp;
    float32           fSpeedKmhDown;    
    float32           fLongitudeUp;
    float32           fLongitudeDown;
    float32           fMagCourseUp;
    float32           fMagCourseDown;
    float32           fSpeedKnotUp;
    float32           fSpeedKnotDown;  
    float32           fTrueCourseUp;
    float32           fTrueCourseDown;
    sGPSDateTime      sDateTimeUp;
    sGPSDateTime      sDateTimeDown;
    uFlagsErrNmeaData FlagsErrorNmeaData; 
}sGPSValidationParameters;


typedef struct
{
    char   cOrientation;
    uint8  u8Degrees;
    uint8  u8Minutes;
    uint16 u16MilMin;
} sGPSCoordinate;

typedef struct
{ 
    volatile bool           bValidFrame;
    volatile int8           cStatus;
    volatile uint8          ui8NSats;
    volatile uint8          ui8FixQuality;
    volatile uint8          u8OrientationByte;
    volatile uint32         ui32Epoch;
    volatile uint32         ui32MiliSecondsTime;
    volatile float32        fHdop;
    volatile float32        fHeight;
    volatile float32        fSpeedKmh;
    volatile float32        fMagCourse;
    volatile float32        fSpeedKnot;
    volatile float32        fTrueCourse;
    volatile sGPSDateTime   sDateTime;        
    volatile sGPSCoordinate sLatitude;
    volatile sGPSCoordinate sLongitude;
}sGPSData;


/*******************************************************************************
* GLOBAL DATA DECLARATION
*******************************************************************************/

//Variable de datos válidos
extern volatile sGPSData GPSData;
//Variable de datos temporales mientras se validan
extern volatile sGPSData GPSDataBackUp;

/*******************************************************************************
* GLOBAL FUNCTION DECLARATION
*******************************************************************************/

void GPS_Init(eUartPorts uport);
uint8 GPS_ProcessNMEA(void);
void GPS_PrintGpsData(char *data);

/********************************************************************************
* Abbreviation Table
* 
*******************************************************************************/

/*******************************************************************************
* END OF FILE
*******************************************************************************/
#endif 
