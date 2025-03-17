#include "cmsis_os2.h"
#include "gcode.h"
#define PEN_UP 	150
#define PEN_DOWN	105
#define WAIT		50
//#define MOVE_WAIT	5
/////////////////////////////////////////////////
extern int g_exec(gCode *inp);
/////////////////////////////////////////////////
extern int xy_move(int x,int y);
extern int xy_moved(int dx,int dy);
extern int xy_isbusy(void);
extern void xy_get(int *x,int *y);
extern int xy_mm2stp(float mm);
extern float xy_stp2mm(int stp);
extern int servo_move(int angle);
extern int servo_get(void);
/////////////////////////////////////////////////
static int use_move_d=0;

static int G28(void){
	servo_move(PEN_UP);
	osDelay(100);
	xy_move(0,0);
	while(xy_isbusy()) osDelay(2);
	return 0;
}

static int G90(void){
	use_move_d=0;
	return 0;
}

static int G91(void){
	use_move_d=1;
	return 0;
}

static int G0(int isx,int x,int isy,int y){
	int ox,oy;
	if(servo_get()!=PEN_UP){
		servo_move(PEN_UP);
		osDelay(WAIT);	
	}
	if(use_move_d){
		xy_moved(isx*x,isy*y);
		while(xy_isbusy()) osDelay(2);		
	}
	else{
		xy_get(&ox,&oy);
		if(isx==1 && isy==1) xy_move(x,y);
		if(isx==1 && isy==0) xy_move(x,oy);
		if(isx==0 && isy==1) xy_move(ox,y);
		while(xy_isbusy()) osDelay(2);
	}
	return 0;
}	

static int G1(int isx,int x,int isy,int y){
	int ox,oy;
		if(servo_get()!=PEN_DOWN){
			servo_move(PEN_DOWN);
			osDelay(WAIT);
		}
	if(use_move_d){
		xy_moved(isx*x,isy*y);
		while(xy_isbusy()) osDelay(2);		
	}
	else{
		xy_get(&ox,&oy);
		if(isx==1 && isy==1) xy_move(x,y);
		if(isx==1 && isy==0) xy_move(x,oy);
		if(isx==0 && isy==1) xy_move(ox,y);
		while(xy_isbusy()) osDelay(2);
	}
	return 0;
}	
/////////////////////////////////////////////////
int g_exec(gCode *inp){
	if(inp->cword=='G' && inp->ccode==28) return G28();
	if(inp->cword=='G' && inp->ccode==90) return G90();
	if(inp->cword=='G' && inp->ccode==91) return G91();
	if(inp->cword=='G' && inp->ccode==0)  return G0(inp->key[gArg('X')],xy_mm2stp(inp->value[gArg('X')]),inp->key[gArg('Y')],xy_mm2stp(inp->value[gArg('Y')]));
	if(inp->cword=='G' && inp->ccode==1)  return G1(inp->key[gArg('X')],xy_mm2stp(inp->value[gArg('X')]),inp->key[gArg('Y')],xy_mm2stp(inp->value[gArg('Y')]));
	return -1;
}






