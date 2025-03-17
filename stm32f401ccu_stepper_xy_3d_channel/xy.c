#include "stm32f4xx.h"

#define A4899_MODE_MUL	16.0

extern int xy_move(int x,int y,int z);
extern int xy_moved(int dx,int dy,int dz);
extern int xy_isbusy(void);
extern void xy_get(int *x,int *y,int *z);
extern void xy_set(int x,int y,int z);

extern int xy_mm2stp(float mm);
extern float xy_stp2mm(int stp);

extern void xy_enable(void);
extern void xy_disable(void);
extern int xy_init(void);

//////////////////////////////////////////////////////////////////////////
#define CS_SET		GPIOB->BSRR=GPIO_BSRR_BS0	
#define CS_CLR		GPIOB->BSRR=GPIO_BSRR_BR0	

#define DIRX_SET	GPIOB->BSRR=GPIO_BSRR_BS2	
#define DIRX_CLR	GPIOB->BSRR=GPIO_BSRR_BR2	

#define STPX_SET	GPIOB->BSRR=GPIO_BSRR_BS1	
#define STPX_CLR	GPIOB->BSRR=GPIO_BSRR_BR1	


#define DIRY_SET	GPIOB->BSRR=GPIO_BSRR_BS6
#define DIRY_CLR	GPIOB->BSRR=GPIO_BSRR_BR6	

#define STPY_SET	GPIOB->BSRR=GPIO_BSRR_BS5	
#define STPY_CLR	GPIOB->BSRR=GPIO_BSRR_BR5	


#define DIRZ_SET	GPIOA->BSRR=GPIO_BSRR_BS1
#define DIRZ_CLR	GPIOA->BSRR=GPIO_BSRR_BR1	

#define STPZ_SET	GPIOA->BSRR=GPIO_BSRR_BS0	
#define STPZ_CLR	GPIOA->BSRR=GPIO_BSRR_BR0	

//////////////////////////////////////////////////////////////////////////
//all positions are based on stepper tick
static int xy_x;
static int xy_y;
static int xy_z;

static int x_ticks;
static int y_ticks;
static int z_ticks;

static int x_dir;
static int y_dir;
static int z_dir;

static int isInited=0;

void TIM2_IRQHandler(void){
	TIM2->SR=0;
	if(x_ticks){
		if(x_ticks&1) { STPX_CLR; xy_x+=x_dir;}
		else				 STPX_SET;
		x_ticks--;
	}
	if(y_ticks){
		if(y_ticks&1) { STPY_CLR; xy_y+=y_dir;}
		else				 STPY_SET;
		y_ticks--;
	}
	if(z_ticks){
		if(z_ticks&1) { STPZ_CLR; xy_z+=z_dir;}
		else				 STPZ_SET;
		z_ticks--;
	}
}
///////////////////////////////////////////////////////////////////////////////////

int xy_moved(int dx,int dy,int dz){
	if(isInited==0) xy_init();
	if(xy_isbusy()) return xy_isbusy();
	CS_CLR;
	
	if(dx!=0){
		if(dx<0){ dx=-dx; x_dir=-1; DIRX_CLR;}
		else{             x_dir=1;  DIRX_SET;}
		x_ticks=dx*2;
	}
	if(dy!=0){
		if(dy<0){ dy=-dy; y_dir=-1; DIRY_CLR;}
		else{             y_dir=1;  DIRY_SET;}
		y_ticks=dy*2;
	}
	if(dz!=0){
		if(dz<0){ dz=-dz; z_dir=-1; DIRZ_CLR;}
		else{             z_dir=1;  DIRZ_SET;}
		z_ticks=dz*2;
	}
	return 0;
}



int xy_move(int x,int y,int z){
	if(isInited==0) xy_init();
	if(xy_isbusy()) return xy_isbusy();
	return xy_moved(x-xy_x,y-xy_y,z-xy_z);
}


void xy_get(int *x,int *y,int *z){
	*x=xy_x;
	*y=xy_y;
	*z=xy_z;
}


void xy_set(int x,int y,int z){
	xy_x=x;
	xy_y=y;
	xy_z=z;
}

int xy_init(void){
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

	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER|=GPIO_MODER_MODE0_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD0_0;	
	
	GPIOA->MODER|=GPIO_MODER_MODE1_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT1);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR1_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD1_0;

	GPIOA->BSRR=GPIO_BSRR_BR0|GPIO_BSRR_BR1;

	x_ticks=0;
	y_ticks=0;
	z_ticks=0;
	
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
	
	xy_x=0; //in new versions we will add to switch to stop motors at the ends
	xy_y=0;
	xy_z=0;	
	
	return 0;
}

int xy_isbusy(void){
	if(isInited==0) xy_init();
	return x_ticks+y_ticks+z_ticks;
}

void xy_enable(void){
	if(isInited==0) xy_init();
	CS_CLR;
}
void xy_disable(void){
	if(isInited==0) xy_init();
	CS_SET;
}

/////////////////////////////////////////////////////////////////////////////////
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

int xy_mm2stp(float mm){
	return (int)(mm*(float)5.0*(float)A4899_MODE_MUL);
}
float xy_stp2mm(int stp){
	return (float)stp/(float)(5.0*A4899_MODE_MUL);
}


