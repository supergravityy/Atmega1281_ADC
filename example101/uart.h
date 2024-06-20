#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdio.h>
#include "board.h"

#define BAUDRATE 115200L

void uart_init();
int8_t uart_getch();
void uart_putch(int8_t);
void uart_puts(int8_t *str);
void num2ascii(int8_t);
void stdio_init();

#endif
