#include "stm32f4xx.h"


extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);

//not really nessarry
extern void stepper_init(void);
extern void stepper_enable(void);
extern void stepper_disable(void);

//////////////////////////////////////////////////////////////////////////
#define CS_SET		GPIOB->BSRR=GPIO_BSRR_BS0	
#define CS_CLR		GPIOB->BSRR=GPIO_BSRR_BR0	

#define DIR0_SET	GPIOA->BSRR=GPIO_BSRR_BS3	
#define DIR0_CLR	GPIOA->BSRR=GPIO_BSRR_BR3	

#define STP0_SET	GPIOA->BSRR=GPIO_BSRR_BS2	
#define STP0_CLR	GPIOA->BSRR=GPIO_BSRR_BR2	

//////////////////////////////////////////////////////////////////////////
static int ch0_ticks;
static int isInited=0;

void TIM3_IRQHandler(void){
	TIM3->SR=0;
	if(ch0_ticks){
		if(ch0_ticks&1) STP0_CLR;
		else				 STP0_SET;
		ch0_ticks--;
	}
	
}
//////////////////////////////////////////////////////////////////////////
void stepper_init(void){
	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOAEN;
	
	GPIOB->MODER|=GPIO_MODER_MODE0_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD0_0;	

	
	GPIOA->MODER|=GPIO_MODER_MODE2_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT2);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR2_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD2_0;	
	
	GPIOA->MODER|=GPIO_MODER_MODE3_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT3);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR3_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD3_0;
	
	GPIOA->BSRR=GPIO_BSRR_BR2|GPIO_BSRR_BR3;
	
	ch0_ticks=0;

	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
	TIM3->CR1=TIM_CR1_DIR;
	TIM3->PSC=84-1;
	TIM3->ARR=1000-1;
	TIM3->EGR=TIM_EGR_UG;
	TIM3->DIER=TIM_DIER_UIE;
	TIM3->SR=0;
	NVIC_EnableIRQ(TIM3_IRQn);
	TIM3->CR1|=TIM_CR1_CEN;
	isInited=1;

}


int stepper_isbusy(int ch){
	if(isInited==0) stepper_init();
	if(ch==0) return ch0_ticks; //changed to value for monitor
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
		return 0;
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


