#include <stdint.h>
#include <stm32f10x.h>


void delay(uint32_t ticks) {
	for (volatile uint32_t i = 0; i < ticks; i++) {
		__NOP();
	}
}

int __attribute((noreturn)) main(void) {
	// Включаем тактирование портов A и C
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Для кнопки (PA0)
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Для светодиода (PC13)

	GPIOC->CRH &= ~GPIO_CRH_CNF13; 
	GPIOC->CRH |= GPIO_CRH_MODE13_0; 

	GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); 
	GPIOA->CRL |= GPIO_CRL_CNF0_1; 
	GPIOA->ODR |= (1U << 0);

	// Переменные для управления частотой
	// 0 - медленно, 1 - быстро, 2 - очень быстро
	uint32_t delays[3] = {2000000, 500000, 100000}; 
	uint8_t speed_mode = 0;

	while (1) {
		// Если на пине PA0 низкий уровень (кнопка нажата)
		if (!(GPIOA->IDR & (1U << 0))) {
			delay(50000);
			
			// Проверяем еще раз, что кнопка действительно нажата
			if (!(GPIOA->IDR & (1U << 0))) {
				// Меняем режим скорости (по кругу: 0 -> 1 -> 2 -> 0)
				speed_mode++;
				if (speed_mode > 2) {
					speed_mode = 0;
				}
				
				// Ждем, пока пользователь отпустит кнопку, 
				// чтобы не переключать скорость, пока палец на кнопке
				while (!(GPIOA->IDR & (1U << 0))); 
				delay(50000);
			}
		}

		// Мигание светодиодом 
		GPIOC->ODR |= (1U << 13); // Включить
		delay(delays[speed_mode]); // Задержка в зависимости от режима
		GPIOC->ODR &= ~(1U << 13); // Выключить
		delay(delays[speed_mode]); // Задержка
	}
}