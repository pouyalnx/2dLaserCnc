#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include <stdio.h>
extern void stdout_goxy(int x,int y);
extern int stdout_putchar(int ch);
extern int keyRead(void);
/*
extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);
*/


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








void app(void *arg){
	int dir=1;
	int stpA,stpB;
	osThreadNew(tick,NULL,NULL);
	osDelay(100);
	/*
	printf("Stepper. D27<\n");
	while(1){
		dir*=-1;
		stpA=4000;
		stpB=4000;
		stdout_goxy(2,0);
		if(dir==1) printf("dir >>>\n");
		else		  printf("dir <<<\n");
		stepper_move(0,stpA*dir);
		stepper_move(1,stpB*dir);
		stpA=stepper_isbusy(0);
		stpB=stepper_isbusy(1);
		while(stepper_isbusy(0) || stepper_isbusy(1)){
			osDelay(20);
			stdout_goxy(2,0);
			printf("stp %d/%d\n",stepper_isbusy(0),stpA);
			printf("stp %d/%d\n",stepper_isbusy(1),stpB);
		}
		osDelay(50);
	}	
	*/
}








