/*******************************************************************************
* Filename: GPS_Definitions.h
* Developer(s): Jorge Yesid Rios Ortiz
* Language: ANSI C
* Abstract: 
* References: 
*******************************************************************************/
#ifndef GPS_DEFINITIONS_H
#define GPS_DEFINITIONS_H
/*******************************************************************************
* HEADER FILES
*******************************************************************************/
/* ansi c */

/* propietary */


/* operation system */

/* processor expert */

/*******************************************************************************
* GLOBAL CONSTANT DECLARATION
*******************************************************************************/

/*******************************************************************************
* GLOBAL MACRO DECLARATION
*******************************************************************************/

 /// Mask select RMC fields 
#define RMC_TIME                1
#define RMC_STATUS              2
#define RMC_LATITUDE            4
#define RMC_LAT_SIGN            8
#define RMC_LONGITUDE           16
#define RMC_LON_SIGN            32
#define RMC_SPEEDKNOT           64
#define RMC_COURSE              128
#define RMC_DATE                256
#define RMC_MAGV                512
#define RMC_MAGV_SIGN           1024

/// Mask select GGA fields 
#define GGA_TIME                1
#define GGA_LATITUDE            2
#define GGA_LAT_SIGN            4
#define GGA_LONGITUDE           8
#define GGA_LON_SIGN            16
#define GGA_FIX_STATUS          32
#define GGA_NUMSATS             64
#define GGA_HDOP                128
#define GGA_HEIGHT              256
#define GGA_HEIGHT_UNIT         512
#define GGA_GEO_SEP             1024
#define GGA_GEO_SEP_UNIT        2048
#define GGA_DGPS_AGE            4096
#define GGA_DGPS_ID             8192

/// Mask select VTG fields
#define VTG_TRUE_COURSE         1
#define VTG_TRUE_TAG            2
#define VTG_MAG_COURSE          4
#define VTG_MAG_TAG             8
#define VTG_SPEED_KNOTS         16
#define VTG_SP_KN_TAG           32
#define VTG_SPEED_KMH           64
#define VTG_SP_KM_TAG           128

/// Index of switch to get data
#define RMC_INDEX_TIME          1
#define RMC_INDEX_STATUS        2
#define RMC_INDEX_LATITUDE      3
#define RMC_INDEX_LAT_SIGN      4
#define RMC_INDEX_LONGITUDE     5
#define RMC_INDEX_LON_SIGN      6
#define RMC_INDEX_SPEEDKNOT     7
#define RMC_INDEX_COURSE        8
#define RMC_INDEX_DATE          9
#define RMC_INDEX_MAGV          10
#define RMC_INDEX_MAGV_SIGN     11

/// Index of switch to get data
#define GGA_INDEX_TIME          33
#define GGA_INDEX_LATITUDE      34
#define GGA_INDEX_LAT_SIGN      35  
#define GGA_INDEX_LONGITUDE     36
#define GGA_INDEX_LON_SIGN      37
#define GGA_INDEX_FIX_QUALITY   38
#define GGA_INDEX_NUMSATS       39
#define GGA_INDEX_HDOP          40
#define GGA_INDEX_HEIGHT        41
#define GGA_INDEX_HEIGHT_UNIT   42
#define GGA_INDEX_GEO_SEP       43
#define GGA_INDEX_GEO_SEP_UNIT  44
#define GGA_INDEX_DGPS_AGE      45
#define GGA_INDEX_DGPS_ID       46

/// Index of switch to get data
#define VTG_INDEX_TRUE_COURSE   65
#define VTG_INDEX_TRUE_TAG      66
#define VTG_INDEX_MAG_COURSE    67
#define VTG_INDEX_MAG_TAG       68
#define VTG_INDEX_SPEED_KNOTS   69
#define VTG_INDEX_SP_KN_TAG     70
#define VTG_INDEX_SPEED_KMH     71 
#define VTG_INDEX_SP_KM_TAG     72

///Validation data mask
#define VALIDATION_STATUS    	    1
#define VALIDATION_LATITUDE	        2
#define VALIDATION_LONGITUDE	    4
#define VALIDATION_SPEEDKNOT 	    8	
#define VALIDATION_DATE_STATUS	    16
#define VALIDATION_TIME  		    32	
#define VALIDATION_MILISECONDSTIME	64
#define VALIDATION_FIXQUALITY	    128
#define VALIDATION_NSATS		    256
#define VALIDATION_HDOP			    512
#define VALIDATION_HEIGHT		    1024
#define VALIDATION_TRUECOURSE	    2048
#define VALIDATION_MAGCOURSE	    4096
#define VALIDATION_SPEEDKMH		    8192
#define VALIDATION_EPOCH		    16384

///Define base year
#define BASE_YEAR        2000 // this define should be update each centenary   

#define INDEX_DATE_DAY      0
#define INDEX_DATE_MONTH    1
#define INDEX_DATE_YEAR     2

#define INDEX_TIME_HOUR     0
#define INDEX_TIME_MINUTE   1
#define INDEX_TIME_SECOND   2

///Length of time
#define     TIME_SIZE       3
///Length of date           
#define     DATE_SIZE       3

#define FRAME_AMOUNT        3



/*******************************************************************************
* GLOBAL NEW DATA TYPES
*******************************************************************************/

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
#endif 

