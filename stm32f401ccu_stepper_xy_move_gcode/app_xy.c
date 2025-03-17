#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "gcode.h"
extern int g_exec(gCode *inp);
extern void stdout_goxy(int x,int y);
extern int stdout_putchar(int ch);
extern int keyRead(void);



void tick(void *arg){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE13_0;
	GPIOC->OTYPER&=~(GPIO_OTYPER_OT13);
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD13_0;
	while(1){
		GPIOC->ODR=1<<13;
		osDelay(50);
		GPIOC->ODR=1<<29;
		osDelay(100);
	}	
}
/*
#define CNT	6
static char cmd[CNT][64]={
	"G28",
	"G0 X120 Y120;",
	"G1 X-50 Y50;",
	"G1 X-50 Y-50;",
	"G1 X50 Y-50;",
	"G1 X50 Y50 ;",
};
*/
#define CNT 7533
extern const char drawing[][24];
static char cmd_buffer[64];
extern int servo_move(int angle);


void app(void *arg){
	int i=0,j=0;
	gCode gcode;
	osThreadNew(tick,NULL,NULL);
	osDelay(100);
	printf(".Dp27<\n");
	for(i=0;i<CNT;i++){
		
		for(j=0;j<32;j++) cmd_buffer[j]=drawing[i][j];
		cmd_buffer[32]=0;
		stdout_goxy(1,0);
		printf("%s->%d\n",cmd_buffer,gCodeParse(cmd_buffer,&gcode));
		printf("%d/%d\n",i,CNT);
		g_exec(&gcode);
	}	
}








