#include "stm32f4xx.h"
extern int servo_move(int angle);
extern int servo_get(void);
/// attention when use this driver please give enough time to servo to move then add new move

#define SERVO0_CLR	GPIOB->BSRR=GPIO_BSRR_BR7
#define SERVO0_SET	GPIOB->BSRR=GPIO_BSRR_BS7

///////////////////////////////////////////////////////////////////////
void TIM3_IRQHandler(void){
	if(TIM3->SR & TIM_SR_CC1IF) SERVO0_SET;
	if(TIM3->SR & TIM_SR_UIF) SERVO0_CLR;
	TIM3->SR=0;
}

static int isInited=0;
static int angel;

static int angeltoUs(int angle){
	if(angle<0)		angle=0;
	if(angle>180)	angle=180;
	//	0-->1000 us
	// 1800---> 4600 us linear estimate
	return 20000-(10*angle+500);
}

static void servo_init(void){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	
	GPIOB->MODER|=GPIO_MODER_MODE7_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT7);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD7_0;	

	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
	TIM3->CR1=TIM_CR1_DIR;
	TIM3->PSC=84-1;
	TIM3->ARR=20000-1;//20ms
	TIM3->EGR=TIM_EGR_UG;
	TIM3->CCR1=(uint16_t)angeltoUs(0);
	TIM3->DIER=TIM_DIER_UIE|TIM_DIER_CC1IE;
	TIM3->SR=0;
	angel=-1;
	TIM3->CCMR1=0;
	NVIC_EnableIRQ(TIM3_IRQn);
	TIM3->CR1|=TIM_CR1_CEN;
	isInited=1;
}



int servo_move(int iangle){
	if(isInited==0) servo_init();
	if(iangle<0)		iangle=0;
	if(iangle>180)		iangle=180;
	angel=iangle; 
	TIM3->CCR1=(uint16_t)angeltoUs(iangle);
	TIM3->EGR=TIM_EGR_UG;
	return 0;
}
int servo_get(void){
	return angel;
}

///////////////////////////////////////////////////////////////////////









