/*
  In downcounting mode, the counter counts from the auto-reload value (content of the
TIMx_ARR register) down to 0, then restarts from the auto-reload value and generates a
counter underflow event.

An Update event can be generate at each counter underflow or by setting the UG bit in the
TIMx_EGR register (by software or by using the slave mode controller)
The UEV update event can be disabled by software by setting the UDIS bit in TIMx_CR1
register.

This is to avoid updating the shadow registers while writing new values in the
preload registers. Then no update event occurs until UDIS bit has been written to 0.

However, the counter restarts from the current auto-reload value, whereas the counter of the
prescaler restarts from 0 (but the prescale rate doesn’t change).

In addition, if the URS bit (update request selection) in TIMx_CR1 register is set, setting the
UG bit generates an update event UEV but without setting the UIF flag (thus no interrupt or
DMA request is sent).

This is to avoid generating both update and capture interrupts when
clearing the counter on the capture event.


When an update event occurs, all the registers are updated and the update flag (UIF bit in
TIMx_SR register) is set (depending on the URS bit):
 	 *The buffer of the prescaler is reloaded with the preload value (content of the TIMx_PSC
register).
	 *The auto-reload active register is updated with the preload value (content of the
TIMx_ARR register).

 Note that the auto-reload is updated before the counter is
reloaded, so that the next period is the expected one.


 */
#include "main.h"

void GPIO_config(void);
void TIM2_config(void);

int main(void){



	GPIO_config();
	TIM2_config();

  while (1){

  }


}

void GPIO_config(void){

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // set enable PORTA clock bus
	GPIOA->CRL = 0X200000; // set output PA5
}

void TIM2_config(void){

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN  ; //set TIM2 clock bus enable
	TIM2->PSC = 8000 - 1; // 8000000/8000 = 1000Hz
	TIM2->ARR = 500 - 1; // 1000Hz ---> 1ms period
	TIM2->CR1 |= TIM_CR1_CEN | TIM_CR1_DIR; // set down-counter,counter enable
	TIM2->DIER |= TIM_DIER_UIE; // set enable interrupt

	NVIC_SetPriority(TIM2_IRQn,1);
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void){

	if(TIM2->SR & 0X01){

		GPIOA->ODR ^= (1 << 5);
		TIM2->SR &= ~(1<<0);

	}

}
