#include "stm32f4xx.h"
/*
 linear mm to step based on our hardware

	stp=(mm*motor_step_per_revolution*a4988_mode)/(belt_pich_mm*poly_tooth_count)
	mm=(step*belt_pich_mm*poly_tooth_count)/(motor_step_per_revolution*a4988_mode)

	motor_step_per_revolution=	200
	a4988_mode=						4
	belt_pich_mm=					2
	poly_tooth_count=				20
	
	
	//simpled to
	stp=mm*200*4/(2*20)-->stp=mm*20 or stp=5*mm*mode


	notic we will use stepx,stepy postion kind so all calculations are based on step

*/
//////////////////////////////////////////////////////////////////////////

extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);

//not really nessarry
extern void stepper_init(void);
extern void stepper_enable(void);
extern void stepper_disable(void);

//////////////////////////////////////////////////////////////////////////
#define CS_SET		GPIOB->BSRR=GPIO_BSRR_BS0	
#define CS_CLR		GPIOB->BSRR=GPIO_BSRR_BR0	

#define DIR0_SET	GPIOB->BSRR=GPIO_BSRR_BS2	
#define DIR0_CLR	GPIOB->BSRR=GPIO_BSRR_BR2	

#define STP0_SET	GPIOB->BSRR=GPIO_BSRR_BS1	
#define STP0_CLR	GPIOB->BSRR=GPIO_BSRR_BR1	


#define DIR1_SET	GPIOB->BSRR=GPIO_BSRR_BS6
#define DIR1_CLR	GPIOB->BSRR=GPIO_BSRR_BR6	

#define STP1_SET	GPIOB->BSRR=GPIO_BSRR_BS5	
#define STP1_CLR	GPIOB->BSRR=GPIO_BSRR_BR5	
//////////////////////////////////////////////////////////////////////////
static int ch0_ticks;
static int ch1_ticks;
static int isInited=0;

void TIM2_IRQHandler(void){
	TIM2->SR=0;
	if(ch0_ticks){
		if(ch0_ticks&1) STP0_CLR;
		else				 STP0_SET;
		ch0_ticks--;
	}
	if(ch1_ticks){
		if(ch1_ticks&1) STP1_CLR;
		else				 STP1_SET;
		ch1_ticks--;
	}
}
//////////////////////////////////////////////////////////////////////////
void stepper_init(void){
	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	
	GPIOB->MODER|=GPIO_MODER_MODE0_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD0_0;	
	
	GPIOB->MODER|=GPIO_MODER_MODE1_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT1);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR1_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD1_0;	
	
	GPIOB->MODER|=GPIO_MODER_MODE2_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT2);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR2_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD2_0;

	
	GPIOB->MODER|=GPIO_MODER_MODE5_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT5);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR5_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD5_0;	
	
	GPIOB->MODER|=GPIO_MODER_MODE6_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT6);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR6_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD6_0;
	
	GPIOB->BSRR=GPIO_BSRR_BR0|GPIO_BSRR_BR1|GPIO_BSRR_BR2|GPIO_BSRR_BR5|GPIO_BSRR_BR6;
	
	ch0_ticks=0;
	ch1_ticks=0;
	
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	TIM2->CR1=TIM_CR1_DIR;
	TIM2->PSC=84-1;
	TIM2->ARR=1000-1;
	TIM2->EGR=TIM_EGR_UG;
	TIM2->DIER=TIM_DIER_UIE;
	TIM2->SR=0;
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM2->CR1|=TIM_CR1_CEN;
	isInited=1;
}


int stepper_isbusy(int ch){
	if(isInited==0) stepper_init();
	if(ch==0) return ch0_ticks; //changed to value for monitor
	if(ch==1) return ch1_ticks; //changed to value for monitor
	return -1;
}

int stepper_move(int ch,int steps){
	if(isInited==0) stepper_init();
	if(stepper_isbusy(ch)) return stepper_isbusy(ch);
	CS_CLR;
	if(ch==0){
		if(steps<0){ steps=-steps; DIR0_CLR;}
		else{                      DIR0_SET;}
		ch0_ticks=steps*2;
		return 1;
	}
	if(ch==1){
		if(steps<0){ steps=-steps; DIR1_CLR;}
		else{                      DIR1_SET;}
		ch1_ticks=steps*2;
		return 1;
	}
	return -1;
}


void stepper_enable(void){
	if(isInited==0) stepper_init();
	CS_CLR;
}
void stepper_disable(void){
	if(isInited==0) stepper_init();
	CS_SET;
}


