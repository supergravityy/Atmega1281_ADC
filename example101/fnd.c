#include "board.h"
#include <avr/io.h>
#include <avr/interrupt.h> // 인터럽트 함수 때문
#include "fnd.h"			// 함수 정의 순서때문에, CPU가 헷갈려 하기에 원형이 담긴 헤더파일 선언

inline static void fnd_port_init(void)
{
    DDRC = 0xff;            // C포트전체는 출력모드
    DDRG = 0x07;            // G포트의 0~2 까지만 3to8 디코더의 입력
    DDRB |= _BV(LED_COLOR); // PB4 는 LED의 불빛을 결정 (둘중에 하나만 선택가능)
}

inline static void fnd_select(uint8_t fnd_num)
{
    PORTG = fnd_num; 
	// PG0~PG2 의 값으로 3 to 8디코더(출력not)를 동작시키고, 이로 인해 D F/F가 선택됨 
}

inline static void fnd_deselect(void)
{
    PORTG = 0x07;
	// D F/F의 입력상태를 바꾸어서, D F/F가 출력을 저장하게끔
}

void fnd_init(void)
{
    fnd_port_init();
    fnd_clear_all();
    led_write(0xff);// led에 불이 다 들어옴
}

void fnd_write(uint8_t fnd_num, uint8_t value)
{
    uint8_t sreg;
    
    sreg = SREG;            // 상태 레지스터를 변수에 저장
    cli();                  // CPU는 더이상 인터럽트를 받지 않음

    fnd_select(fnd_num);
    PORTC = value;
    fnd_deselect();

    if(sreg&0x80)           // cli()를 시작하기 전, 인터럽트 실행하는 레지스터였다면
        sei();              // 다시 인터럽트를 실행       
}

inline void fnd_clear(uint8_t fnd_num)
{
    fnd_write(fnd_num, 0);
}

void fnd_clear_all(void)
{
    fnd_write(0, 0);
    fnd_write(1, 0);
    fnd_write(2, 0); // fnd를 다루는 D F/F 전체에, fnd에 정보를 출력하는 C포트값 (전체 off) 초기화
}

void fnd_write_number(uint8_t fnd_num, uint8_t num, uint8_t dot_on)
{
    static uint8_t num_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f,
                                  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
    uint8_t value;
    
    value = num_code[num] | (dot_on<<7);		// 소수점여부 + 정수~>이진bit열 = value
    fnd_write(fnd_num, value);					// fnd에 실질적으로 값을 쓰는 fnd_write 함수 호출
}

void led_write(uint8_t value)
{
    uint8_t sreg;
    
    sreg = SREG;           
    cli();                  

    fnd_select(3);			// D F/F 네번째는 led를 담당
    PORTC = value;
    fnd_deselect();			// 이 D F/F를 다시 수정할때까지 값을 저장

    if(sreg&0x80)           
        sei();              
}

void fnd_write_numbers(uint16_t num)
{
	static uint8_t num_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x5e, 0x5e, 0x79, 0x71};
	// 정수 -> fnd 출력되게 PORTC에 맞춰 비트화
	
	uint8_t value[3] = {0x0,0x0,0x0};
	// → 내림차순, 초기화
	uint8_t select_FF[3] = {0x0, 0x01, 0x02};
	// → 내림차순, 초기화, 미리미리 자동화를 시키자
	
	if((num>999 || num<0))
	// 표현가능한 수의 한계를 넘어섰기에, 오류발생 -> Err 표시
	{
		// num_code의 내용과 기존 fnd_wirte_number 내용을 미루어 보아, 0x3f는 0일것이다.
		// 이를 바탕으로, E와 r을 구현하였다
		value[2] = 0x79; //E
		value[1] = 0x50; //r
		value[0] = 0x50;
		goto label;
	}
	
	// 각 자릿수를 체에 걸러서 넘겨주자 (소수점은 배제)
	
	value[0] = num_code[num / 100]; // U8에 쓰여질 숫자 (100자리)
	
	uint8_t remnant = num % 100; // 10자리수 + 1자리수
	
	value[1] = num_code[remnant / 10]; // U6에 쓰여질 숫자 (10자리)
	value[2] = num_code[remnant % 10]; // U4에 쓰여질 숫자 (1자리)
	
	label: //Err을 출력하기 위한 라벨
	
	fnd_write(select_FF[2],value[0]); // U8의 선택번호
	fnd_write(select_FF[1],value[1]); // U6의 선택번호
	fnd_write(select_FF[0],value[2]); // U4의 선택번호
}