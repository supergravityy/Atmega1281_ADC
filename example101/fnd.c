#include "board.h"
#include <avr/io.h>
#include <avr/interrupt.h> // ���ͷ�Ʈ �Լ� ����
#include "fnd.h"			// �Լ� ���� ����������, CPU�� �򰥷� �ϱ⿡ ������ ��� ������� ����

inline static void fnd_port_init(void)
{
    DDRC = 0xff;            // C��Ʈ��ü�� ��¸��
    DDRG = 0x07;            // G��Ʈ�� 0~2 ������ 3to8 ���ڴ��� �Է�
    DDRB |= _BV(LED_COLOR); // PB4 �� LED�� �Һ��� ���� (���߿� �ϳ��� ���ð���)
}

inline static void fnd_select(uint8_t fnd_num)
{
    PORTG = fnd_num; 
	// PG0~PG2 �� ������ 3 to 8���ڴ�(���not)�� ���۽�Ű��, �̷� ���� D F/F�� ���õ� 
}

inline static void fnd_deselect(void)
{
    PORTG = 0x07;
	// D F/F�� �Է»��¸� �ٲپ, D F/F�� ����� �����ϰԲ�
}

void fnd_init(void)
{
    fnd_port_init();
    fnd_clear_all();
    led_write(0xff);// led�� ���� �� ����
}

void fnd_write(uint8_t fnd_num, uint8_t value)
{
    uint8_t sreg;
    
    sreg = SREG;            // ���� �������͸� ������ ����
    cli();                  // CPU�� ���̻� ���ͷ�Ʈ�� ���� ����

    fnd_select(fnd_num);
    PORTC = value;
    fnd_deselect();

    if(sreg&0x80)           // cli()�� �����ϱ� ��, ���ͷ�Ʈ �����ϴ� �������Ϳ��ٸ�
        sei();              // �ٽ� ���ͷ�Ʈ�� ����       
}

inline void fnd_clear(uint8_t fnd_num)
{
    fnd_write(fnd_num, 0);
}

void fnd_clear_all(void)
{
    fnd_write(0, 0);
    fnd_write(1, 0);
    fnd_write(2, 0); // fnd�� �ٷ�� D F/F ��ü��, fnd�� ������ ����ϴ� C��Ʈ�� (��ü off) �ʱ�ȭ
}

void fnd_write_number(uint8_t fnd_num, uint8_t num, uint8_t dot_on)
{
    static uint8_t num_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f,
                                  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
    uint8_t value;
    
    value = num_code[num] | (dot_on<<7);		// �Ҽ������� + ����~>����bit�� = value
    fnd_write(fnd_num, value);					// fnd�� ���������� ���� ���� fnd_write �Լ� ȣ��
}

void led_write(uint8_t value)
{
    uint8_t sreg;
    
    sreg = SREG;           
    cli();                  

    fnd_select(3);			// D F/F �׹�°�� led�� ���
    PORTC = value;
    fnd_deselect();			// �� D F/F�� �ٽ� �����Ҷ����� ���� ����

    if(sreg&0x80)           
        sei();              
}

void fnd_write_numbers(uint16_t num)
{
	static uint8_t num_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x5e, 0x5e, 0x79, 0x71};
	// ���� -> fnd ��µǰ� PORTC�� ���� ��Ʈȭ
	
	uint8_t value[3] = {0x0,0x0,0x0};
	// �� ��������, �ʱ�ȭ
	uint8_t select_FF[3] = {0x0, 0x01, 0x02};
	// �� ��������, �ʱ�ȭ, �̸��̸� �ڵ�ȭ�� ��Ű��
	
	if((num>999 || num<0))
	// ǥ�������� ���� �Ѱ踦 �Ѿ�⿡, �����߻� -> Err ǥ��
	{
		// num_code�� ����� ���� fnd_wirte_number ������ �̷�� ����, 0x3f�� 0�ϰ��̴�.
		// �̸� ��������, E�� r�� �����Ͽ���
		value[2] = 0x79; //E
		value[1] = 0x50; //r
		value[0] = 0x50;
		goto label;
	}
	
	// �� �ڸ����� ü�� �ɷ��� �Ѱ����� (�Ҽ����� ����)
	
	value[0] = num_code[num / 100]; // U8�� ������ ���� (100�ڸ�)
	
	uint8_t remnant = num % 100; // 10�ڸ��� + 1�ڸ���
	
	value[1] = num_code[remnant / 10]; // U6�� ������ ���� (10�ڸ�)
	value[2] = num_code[remnant % 10]; // U4�� ������ ���� (1�ڸ�)
	
	label: //Err�� ����ϱ� ���� ��
	
	fnd_write(select_FF[2],value[0]); // U8�� ���ù�ȣ
	fnd_write(select_FF[1],value[1]); // U6�� ���ù�ȣ
	fnd_write(select_FF[0],value[2]); // U4�� ���ù�ȣ
}