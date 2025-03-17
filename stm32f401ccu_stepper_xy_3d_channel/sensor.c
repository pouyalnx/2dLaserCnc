#include "stm32f4xx.h"

extern int sensorRead(int id);
extern void sensorInit(void);


#define X_MOVE 8
#define X_MOVE_read (GPIOA->IDR & GPIO_IDR_ID8)

#define Y_MOVE 9
#define Y_MOVE_read (GPIOA->IDR & GPIO_IDR_ID9)

#define Z_MOVE 10
#define Z_MOVE_read (GPIOA->IDR & GPIO_IDR_ID10)


#define UP_SEWING 0
#define UP_SEWING_read (GPIOA->IDR & GPIO_IDR_ID0)

static int isInit=0;


int sensorRead(int id){
	if(isInit==0) sensorInit();
	if(id==UP_SEWING && UP_SEWING_read) return 1;
	if(id==X_MOVE && X_MOVE_read) return 1;
	if(id==Y_MOVE && Y_MOVE_read) return 1;
	if(id==Z_MOVE && Z_MOVE_read) return 1;
	return 0;
}

void sensorInit(void){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;

	
	GPIOA->MODER&=~(GPIO_MODER_MODE0);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD0_0;


	GPIOA->MODER&=~(GPIO_MODER_MODE8);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT8);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR8_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD8_0;
	
	
	GPIOA->MODER&=~(GPIO_MODER_MODE9);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT9);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD9_0;
	
	
	GPIOA->MODER&=~(GPIO_MODER_MODE10);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT10);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR10_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD10_0;
	isInit=1;
}






