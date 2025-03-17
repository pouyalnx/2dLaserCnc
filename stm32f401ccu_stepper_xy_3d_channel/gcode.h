#ifndef __GCODE
#define __GCODE

#define		gArg(A)		(A-'A')
#define     abs(i)      (i>-1?i:-i)

typedef struct gCodeStruct{
	char   cword;					//Normally G or M 
	int	 ccode;					// can be 0 or 28 or else
	int    key[26];				// each home reprsent	one English digit	0--'A'	1--'B' 
	float value[26];				// for access key[gArg(el)] or val[gArg(el)]
}gCode;


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
///	Gcode Execute Area	this part only designed for Having gcode inside a4988
//////////////////////////////////////////////////////////////////////////////////////////////

/*
	we wroten couple versions of Gcode each of them was designed to one purpose this one
	maybe final version depond on exprence on which one is simplest for work and operation
	
	we have one parse part that it job parse inputs according to Gcode syntax
	this function at same time just can parse on line so if u have file or string 
	first u should split it into lines and then feed to function
	maybe ask why we dont use all togther because it was complex on excute and
	aslo it mcu not mpu or mmu	
	
	
*/

/*
	how gcode struct work
	after each command that sended to gexec u will be return data in this struct

	cword that depend on command G01 or G23 or ...
	alphabet will be stored in cword
   and numerical part will be saved in ccode

	arguments are for each argument in input if have value it presented home in key will be one
   and value home have value

	//EX
	G0	X23 Y33
	
   cword='G'
	ccode=0
	key[gArg('X')]=1
	value[gArg('X')]=23.0
	key[gArg('Y')]=1
	value[gArg('Y')]=33.0
*/



#define GCODE_OK								 0
#define GCODE_ERROR_ENDLINE             1
#define GCODE_ERROR_SEMICOLUMN          2
#define GCODE_ERROR_NULL                3
#define GCODE_ERROR_FORMAT              4
#define GCODE_ERROR_UNSUPPORTED_CMD     5


extern int gCodeParse(char *inp,gCode *gout);


#endif
