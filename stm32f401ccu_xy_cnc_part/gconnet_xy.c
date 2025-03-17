#include "cmsis_os2.h"
#include "gcode.h"
/////////////////////////////////////////////////
extern int g_exec(gCode *inp);
/////////////////////////////////////////////////
extern int xy_move(int x,int y,int z);
extern int xy_moved(int dx,int dy,int dz);
extern int xy_isbusy(void);
extern void xy_set(int x,int y,int z);
extern void xy_get(int *x,int *y,int *z);
extern int xy_mm2stp(float mm);
extern float xy_stp2mm(int stp);
/////////////////////////////////////////////////
extern int sensorRead(int id);
/////////////////////////////////////////////////
extern int 	stepper_isbusy(int ch);

extern int	stepper_move(int ch,int steps);
/////////////////////////////////////////////////
static int use_move_d=0;

static int G28(void){
	while(sensorRead(8)==1){
		xy_moved(1,0,0);
		while(xy_isbusy()) osDelay(2);
	}
	while(sensorRead(8)==0){
		xy_moved(-1,0,0);
		while(xy_isbusy()) osDelay(2);
	}

	
	while(sensorRead(9) && sensorRead(10)){
		xy_moved(0,1,1);
		while(xy_isbusy()) osDelay(2);
	}
	while(sensorRead(9)==0 && sensorRead(10)==0){
		xy_moved(0,-1,-1);
		while(xy_isbusy()) osDelay(2);
	}
	
	xy_set(0,0,0);
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
	int ox,oy,oz;

	if(use_move_d){
		xy_moved(isx*x,isy*y,isy*y);
		while(xy_isbusy()) osDelay(2);		
	}
	else{
		xy_get(&ox,&oy,&oz);
		if(isx==1 && isy==1) xy_move(x,y,y);
		if(isx==1 && isy==0) xy_move(x,oy,oy);
		if(isx==0 && isy==1) xy_move(ox,y,y);
		while(xy_isbusy()) osDelay(2);
	}
	return 0;
}	

static int G1(int isx,int x,int isy,int y){
	int ox,oy,oz;
	if(use_move_d){
		xy_moved(isx*x,isy*y,isy*y);
		while(xy_isbusy()) osDelay(2);		
	}
	else{
		xy_get(&ox,&oy,&oz);
		if(isx==1 && isy==1) xy_move(x,oy,oy);
		if(isx==1 && isy==0) xy_move(x,oy,oy);
		if(isx==0 && isy==1) xy_move(ox,oy,oy);
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






