/*******************************************************************************
* Filename: GPS.h
* Developer(s): Jorge Rios, Diego Ferney
* brief:    Header class of GPS.c, contain declaration and include.
* Language: ANSI C
* Abstract: 
* References: 
*******************************************************************************/
#ifndef DEFINES_H
#define DEFINES_H
/*******************************************************************************
* HEADER FILES
*******************************************************************************/
// ansi c
#include <stdbool.h>        
#include <stddef.h>



/*******************************************************************************
* GLOBAL DEFINE DECLARATION
*******************************************************************************/
#define CustomError 	0
#define CustomOk 		1
#define CustomDifferent 0
#define CustomSame 		1
#define sleep     CyDelay
    
#define PROCESS_OK       0
#define PROCESS_ERROR   -1

/*******************************************************************************
* GLOBAL CONSTANT DECLARATION
*******************************************************************************/
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef signed   char   int8;
typedef signed   short  int16;
typedef signed   long   int32;
typedef          float  float32;

//#define NULL ((void *)0)


struct	bit_def {
	unsigned char	b0:1;
	unsigned char	b1:1;
	unsigned char	b2:1;
	unsigned char	b3:1;
	unsigned char	b4:1;
	unsigned char	b5:1;
	unsigned char	b6:1;
	unsigned char	b7:1;
};
union byte_def {
	struct	bit_def bitFlag;
	unsigned char	byte;
};




#endif /* DEFINES_H */

