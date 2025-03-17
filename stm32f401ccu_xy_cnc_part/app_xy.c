#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "gcode.h"
extern int g_exec(gCode *inp);

extern void stdout_goxy(int x,int y);
extern int stdout_putchar(int ch);

extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);

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

//////////////////////////////////////////////////
static char cmd_buffer[64];
#define L	80
extern const char gcode_b[][32];
//////////////////////////////////////////////////
void app(void *arg){
	gCode cmd_code;
	int   cmd_stat;
	osThreadNew(tick,NULL,NULL);
	osDelay(100);
	printf("Dp78<\n");
	stdout_goxy(1,0);

	sprintf(cmd_buffer,"G28");
	cmd_stat=gCodeParse(cmd_buffer,&cmd_code);
	g_exec(&cmd_code);
	

	while(1){
		for(int i=0;i<L;i++){
			for(int j=0;j<32;j++) cmd_buffer[j]=gcode_b[i][j];
			stdout_goxy(1,0);		
			printf("%s",cmd_buffer);
			cmd_stat=gCodeParse(cmd_buffer,&cmd_code);
			g_exec(&cmd_code);			
		}
		/*
		sprintf(cmd_buffer,"G0 X50 Y50");
		cmd_stat=gCodeParse(cmd_buffer,&cmd_code);
		g_exec(&cmd_code);
		

		sprintf(cmd_buffer,"G1 X50 Y50");
		cmd_stat=gCodeParse(cmd_buffer,&cmd_code);
		g_exec(&cmd_code);

		sprintf(cmd_buffer,"G0 X200 Y100");
		cmd_stat=gCodeParse(cmd_buffer,&cmd_code);
		g_exec(&cmd_code);
		

		sprintf(cmd_buffer,"G1 X100 Y200");
		cmd_stat=gCodeParse(cmd_buffer,&cmd_code);
		g_exec(&cmd_code);
		*/
	}
}








