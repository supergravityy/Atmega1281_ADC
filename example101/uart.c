#include "uart.h"

void uart_init()
{
	// USART0을 사용할 것
	UBRR0 = (F_CPU/8)/BAUDRATE - 1;				// 전송률계산시 사용되는 seed값 (2배속모드이기에)
	UCSR0A = _BV(U2X0);							// 2배속모드
	UCSR0B = _BV(RXEN0)|_BV(TXEN0)|_BV(RXCIE0);	// 전송,수신 활성화, 수신 인터럽트 활성화
	UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);			// UPM0,1 = 00 -> 패리티비트 안씀, USBS0 = 0 -> 1개스탑비트
	// UCSZ2,1,0 = 010 -> 송수신 데이터 크기가 8비트
}

int8_t uart_getch() // 수신용
{
	while(!(UCSR0A & _BV(RXC0)));		// 수신버퍼가 비어있어야, RXC = clear
										// 수신버퍼가 비어서, 거짓일때 루프탈출	
	
	return UDR0;						// UDR에 저장된 값을 리턴
} 

void uart_putch(int8_t ch)
{
	while (!(UCSR0A & _BV(UDRE0))); 
	
		UDR0 = ch; 
}

void uart_puts(int8_t *str)
{
	uint8_t i = 0;
	
	while(str[i] != '\0')				// 문자의 맨 마지막은 NULL이다.
	{
		uart_putch(str[i]);
		i++;
	}				
}

void num2ascii(int8_t ch)			// 8비트를 반으로 쪼개, 해당하는 숫자->기호로 치환하여 전송
{
	int8_t tmp;
	
	tmp = (ch>>4) & 0x0f;			// 상위 4비트
	
	if(tmp>=0 && tmp<= 9)			// 0 <= 상위 4비트 <= 9
		uart_putch(tmp + '0');		// 숫자(아스키) 전송
		
	else
		uart_putch(tmp -10 + 'A');	// 상위 4비트가 숫자가 아닐시, A~F이다. 여기서 A = 10 이기에, 빼주고, 'A' 오프셋부터 시작
		
	tmp = ch & 0x0f;				// 하위 4비트
	
	if (tmp>=0 && tmp<= 9)			// 0 <= 상위 4비트 <= 9
		uart_putch(tmp + '0');		// 숫자(아스키) 전송
	
	else
		uart_putch(tmp -10 + 'A');	// 문자전송
}

void stdio_init()
{
	fdevopen(uart_putch,uart_getch);
}