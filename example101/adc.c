#include "adc.h"

void adc_init()
{
	ADCSRA = _BV(ADEN)|_BV(ADPS0)|_BV(ADPS1)|_BV(ADPS2)|_BV(ADIE)|_BV(ADATE); 
	// 변환기 활성화, prescaler = 128이 될것, 인터럽트 활성화, 오토모드 활성화 
	// 아직 ADSC는 활성화 X -> 빼고 나머지 다 1
	ADCSRB = 0;						// 프리 러닝이기에, 여전히 0이다
	ADMUX = _BV(REFS0);				// REFS 0,1 = 01 -> AVcc
}

uint16_t adc_get_result(uint8_t channel_num)
{
	switch (channel_num) // 다른비트 터치하지 말것
	{					// case 뒤에 바로 정수: 을 붙이자
		case 0:
			ADMUX = (ADMUX & 0xE0) ;
			break;
		case 1:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX0) ;
			break;
		case 2:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX1) ;
			break;
		case 3:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX0)|_BV(MUX1) ;
			break;
		case 4:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX2) ;
			break;
		case 5:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX2)|_BV(MUX0) ;
			break;	
		case 6:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX2)|_BV(MUX1) ;
			break;
		case 7:
			ADMUX = (ADMUX & 0xE0) | _BV(MUX2)|_BV(MUX1)|_BV(MUX0) ;
			break;
		default: 
			fnd_write_numbers(1000); // Err 출력
			goto move;
	}
	
	ADCSRA |= _BV(ADSC); // 변환시작
	
	while(!(ADCSRA&_BV(ADIF))) ; // 싱글변환이기에, ADSC = clear, ADIF = set됨.
	// 폴링방식
	
	uint16_t result = ADCL|(ADCH<<8) ; // 10 bit 완성
	// ADLAR = 0 이기에, ADCH가 msb를 갖고, ADCL이 lsb를 포함.
	/* 주의! ADCH를 먼저 읽게 되면, 프리러닝상태에서 다음 변환을 시작하기에, 예기치 않게 ADCL을 시간내(13.5 타임슬롯)에 못읽는 경우
		새로 변환된, 값들이 새로 들어오게 되어, 이전 ADCL의 값이 사라질 수가 있다 */ 
	return result;
	
move:
	return '\0';
	// 잘못 채널번호를 잘못 입력시 NULL 리턴
}

