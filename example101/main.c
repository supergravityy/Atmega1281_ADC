#include <avr/io.h>
#include "board.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include <stdio.h>
#include "fnd.h"
#include "adc.h"

uint16_t digit_val;
double deci_val = 0.0;
uint8_t U8; uint8_t U6; uint8_t U4;

void interrupt_init()
{
	sei();
}

void ioport_init()
{
	PORTB |= _BV(PB4);			// 녹색 led 활성화
}

ISR(ADC_vect)
{
	deci_val = ((double)digit_val)/1024*5;	// 실수형태로 변환 (싱글엔드 모드)
	
	U8 = (uint8_t)(deci_val/1);
	U6 = (uint8_t)(deci_val*10/1 - U8*10);
	U4 = (uint8_t)(deci_val*100/1 -(U8*100) - (U6*10));
	
	fnd_write_number(0x02,U8,1);			// 이 부분에 정수 + 소수점을 출력		
	fnd_write_number(0x01,U6,0);			// 총 소수점 2째자리까지 출력, 나머지는 버림
	fnd_write_number(0x00,U4,0);
	
	// a를 누른 후 부터, led 패턴이 250ms 마다 바뀌는 것이 아닌, 2초마다 한번씩 바뀐다????
	
	//fnd_write_numbers(digit_val);			// 바로 fnd에 출력해버림, 이것만 할때는 아무문제 없음
}

ISR(USART0_RX_vect)
{
	if(UDR0 == 'a'||UDR0 == 'A')
	{
		digit_val = adc_get_result(1);	// 결과값을 입력받아서
		// a 입력받으면 바로 AD 변환시작
	}
}

void expanded_LED(uint8_t idx)
{
	static uint8_t led[] = {0x81,0xC3,0xE7,0xFF,0x7E,0x3C,0x18,0x0};
	led_write(led[idx%8]);	
}

int main(void)
{
	ioport_init();
	fnd_init();
	adc_init();
	stdio_init();
	uart_init();
	interrupt_init();
	
	uint8_t i =0;
	
	while (1)
	{
		expanded_LED(i);
		i++;
		_delay_ms(250);
	}
}
