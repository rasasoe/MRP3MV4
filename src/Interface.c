#define F_CPU 16000000UL //이걸 정의하지 않으면 _delay_ms()함수에서 정확한 딜레이가 불가능하고 경고를 발생시킴
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Interface.h"
#include "Move.h"
#include "Motor.h"

volatile unsigned char sec=0, cnt0=0, cnt1=0, cnt2=0;
volatile unsigned char rx1_flg=0, rx1_buff=0, Y_Cnt=0, X_Cnt=0, keyCnt=0, BTrxdata;
volatile int speed=0, next_speed=0, acc=4, wspeed=0, next_wspeed=0, acc_W=2;


//Function : MCU 초기화
//Parameter : 없음 / Return : 없음
void MCU_init(void)
{
	//USART1 initialization / Communication Parameters: 8 Data, 1 Stop, No Parity
	//USART1 Receiver: On / USART1 Transmitter: On
	//USART1 Mode: Asynchronous / USART1 Baud Rate: 9600

	UCSR1A=0x00;
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1H=0x00;
	UBRR1L=0x5F;

	//Clock value: 14.400 kHz
	TCCR1A=0x00;
	TCCR1B=0x05;
	//////////////////
	//TCNT1H=0xC7;//1초
	//TCNT1L=0xC0;
	//TCNT1H=0xFA;//0.1초
	//TCNT1L=0x60;
	TCNT1H=0xFF;//0.01초
	TCNT1L=0x70;
	//////////////////
	TIMSK=0x04;

	//INT5,INT6,INT7 Mode: Falling Edge
	/*
	DDRE&=~0XE0;
	EICRA=0x00;
	EICRB=0xA8;
	EIMSK=0xE0;
	EIFR=0xE0;
	*/
	//ADC initialization
	//ADC Clock frequency: 921.600 kHz / ADC Voltage Reference: AREF pin
	//Only the 8 most significant bits of
	//the AD conversion result are used
	/*
	PORTF&=~0X78;
	ADMUX=ADC_VREF_TYPE & 0xff;
	ADCSRA=0x84;
	*/
	sei();
}

ISR(TIMER1_OVF_vect)
{
	//TCNT1H=0xC7; TCNT1L=0xC0; //1초
	//TCNT1H=0xFA; TCNT1L=0x60; //0.1초
	TCNT1H=0xFF; TCNT1L=0x70; //0.01초

	++sec;

	if(next_speed>speed){
		speed+=acc;
		if(next_speed<=speed)speed=next_speed;
	}
	else if(next_speed<speed){
		speed-=acc;
		if(next_speed>=speed)speed=next_speed;
	}
	if(next_wspeed>wspeed){
		wspeed+=acc_W;
		if(next_wspeed<=wspeed)wspeed=next_wspeed;
	}
	else if(next_wspeed<wspeed){
		wspeed-=acc_W;
		if(next_wspeed>=wspeed)wspeed=next_wspeed;
	}
}
/*
ISR(INT5_vect)
{

}
ISR(INT6_vect)
{

}
ISR(INT7_vect)
{

}*/

ISR(USART1_RX_vect)
{
	char data;

	data=UDR1;
	while(!(UCSR1A&0xB8));
	rx1_flg=1;
	rx1_buff=data;
}

char getchar1(void)
{
	char data;

	data=rx1_buff;
	rx1_buff=0;
	cli();
	rx1_flg=0;
	sei();
	return data;
}

void putchar1(char data)
{
	while(!(UCSR1A&0x20));
	UDR1=data;
}

// Read the 8 most significant bits
// of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
	ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=0x40;
	// Wait for the AD conversion to complete
	while((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	return ADCH;
}
// Function : LCD,LED,Buzzer,Switch를 사용하기 위해 초기화
// Parameter : 없음
// Return	: 없음
void Interface_init(void)
{
	DDRC|+0xF0;
	DDRD&=~0xF0;

	TWDR = 0xFF;
	TWBR = 0x48;
	//TWDR = 0xFF;

	LED_PORT &= ~((1 << LED1) | (1 << LED2) | (1 << LED3));
	LED_DDR |= (1 << LED1) | (1 << LED2) | (1 << LED3);

	BZ_DDR |= (1 << BZ);

	SW_PORT = 0x80;
	SW_DDR &= ~((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));

	WriteI2C(0x00, 0xF0);
	WriteI2C(0x05, 0x20);
}
// Function : I/O Expander에 데이터 및 명령어를 보낼 때 사용(외부 출력 상위4비트)
// Parameter :
// cmd - 명령어
// data - i2c에 보낼 데이터
// Return : 없음

void WriteI2C(uint8_t cmd, uint8_t data)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	while(!(TWCR & (1 << TWINT)));

	TWDR = 0x40;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWDR = cmd;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

// Function : I/O Expander의 입력 포트를 읽기 위해 사용
// Parameter :
// cmd - 명령어
// Return	: 외부입력 데이터 상위 4비트
unsigned char ReadI2C(uint8_t cmd)
{
	uint8_t data;

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	while(!(TWCR & (1 << TWINT)));

	TWDR = 0x40;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	while(!(TWCR & (1 << TWINT)));

	TWDR = 0x41;
	TWCR = (_BV(TWINT) | _BV(TWEN));
	while(!(TWCR & _BV(TWINT)));

	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	data = TWDR;

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

	return data;
}

// Function : I2C를 사용하여 LCD모듈로 한바이트 전송
// Parameter :
// data - 전송할 한바이트 데이터
// Return	: 없음
void lcd_write_data(unsigned char data)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	while(!(TWCR & (1 << TWINT)));

	TWDR = SLA << 1;

	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

// Function : 원하는 라인에 한줄의 문자열 출력
// Parameter :
//		line - 출력할 라인번호(0~3)
//		String - 출력할 문자열(최대 20자)
// Return	: 없음
void lcd_display_str(unsigned char Y_line, unsigned char X_line,char *string)
{
	int i=X_line;

	if((Y_line<4) && (X_line<20)){
		lcd_write_data(0x1B);
		lcd_write_data(0x4C);
		lcd_write_data(X_line);
		lcd_write_data(Y_line);
		delay_us(100);

		while(((i++)<20)&&*string){
			lcd_write_data(*string++);
			delay_us(40);
		}
	}
}

void write_lcd_data(unsigned char Y_line, unsigned char X_line, long data)
{
	if(Y_line<4 && X_line<20){
		lcd_write_data(0x1B);
		lcd_write_data(0x4C);
		lcd_write_data(X_line);
		lcd_write_data(Y_line);
		delay_us(100);
		lcd_write_data(data);
		delay_us(40);
	}
}

void lcd_clear_screen(void){ lcd_write_data(0x1B); lcd_write_data(0x43); _delay_ms(20); }

void display_char(unsigned char line, unsigned char col, unsigned char data) ////lcd char 데이터 표시
{
	unsigned char val=0, val_buff=0;

	val=data;

	val_buff=val/100;
	write_lcd_data(line, col, val_buff+'0');
	val=val-(100*val_buff);

	val_buff=val/10;
	write_lcd_data(line, 1+col, val_buff+'0');
	val=val-(10*val_buff);

	write_lcd_data(line, 2+col, val+'0');
}

void display_int(unsigned char line, unsigned char col, unsigned int data)
{
	unsigned int val=0, val_buff=0;

	val=data;

	val_buff=val/10000;
	write_lcd_data(line, col, val_buff+'0');
	val=val-(10000*val_buff);

	val_buff=val/1000;
	write_lcd_data(line, 1+col, val_buff+'0');
	val=val-(1000*val_buff);

	val_buff=val/100;
	write_lcd_data(line, 2+col, val_buff+'0');
	val=val-(100*val_buff);

	val_buff=val/10;
	write_lcd_data(line, 3+col, val_buff+'0');
	val=val-(10*val_buff);

	write_lcd_data(line, 4+col, val+'0');
}

void display_long(unsigned char line, unsigned char col, unsigned long data) //lcd long 데이터 표시
{
	unsigned long val=0, val_buff=0;

	val=data;

	val_buff=val/1000000000;
	write_lcd_data(line, col, val_buff+'0');
	val=val-(1000000000*val_buff);
	
	val_buff=val/100000000;
	write_lcd_data(line, 1+col, val_buff+'0');
	val=val-(100000000*val_buff);

	val_buff=val/10000000;
	write_lcd_data(line, 2+col, val_buff+'0');
	val=val-(10000000*val_buff);

	val_buff=val/1000000;
	write_lcd_data(line, 3+col, val_buff+'0');
	val=val-(1000000*val_buff);

	val_buff=val/100000;
	write_lcd_data(line, 4+col, val_buff+'0');
	val=val-(100000*val_buff);


	val_buff=val/10000;
	write_lcd_data(line, 5+col, val_buff+'0');
	val=val-(10000*val_buff);


	val_buff=val/1000;
	write_lcd_data(line, 6+col, val_buff+'0');
	val=val-(1000*val_buff);


	val_buff=val/100;
	write_lcd_data(line, 7+col, val_buff+'0');
	val=val-(100*val_buff);


	val_buff=val/10;
	write_lcd_data(line, 8+col, val_buff+'0');
	val=val-(10*val_buff);

write_lcd_data(line, 9+col, val+'0');
}

void delay_us(unsigned char time_us)
{
	register unsigned char i;
	for(i = 0; i < time_us; i++)
	{
		asm volatile(" PUSH R0 ");
		asm volatile(" POP R0 ");
		asm volatile(" PUSH R0 ");
		asm volatile(" POP R0 ");
		asm volatile(" PUSH R0 ");
		asm volatile(" POP R0 ");
	}
}


void delay_ms(unsigned int time_ms)
{
	register unsigned int i;
	for(i = 0; i < time_ms; i++)
	{
		delay_us(250);
		delay_us(250);
		delay_us(250);
		delay_us(250);
	}
}

void BT_Init(void){
	// USART1 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART1 Receiver: On
	// USART1 Transmitter: On
	// USART1 Mode: Asynchronous
	// USART1 Baud Rate: 9600
	UCSR1A=0x00;
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1H=0x00;
	UBRR1L=0x5F;
}

void BT_Pairmode(void){
	unsigned char i;

	while(1){
		if((SW1)&&(keyCnt==0)){ //페어링모드들어가기
			putchar1('A'); putchar1('T'); putchar1(0x0d);		//wake up
			delay_ms(100);								//wake up delay

			putchar1('A'); putchar1('T'); putchar1('+'); putchar1('E'); putchar1('N');
			putchar1('T'); putchar1('E'); putchar1('R'); putchar1('P');
			putchar1('A'); putchar1('I'); putchar1('R'); putchar1('M'); putchar1('O');
			putchar1('D'); putchar1('E'); putchar1(','); putchar1('0'); putchar1(0X0d);
			keyCnt++;
		}

		if(SW2){ //lcd초기화(전체화면 지우기)
			lcd_clear_screen();
			Y_Cnt=0, X_Cnt=0, keyCnt=0;
		}

		if((SW3)&&(keyCnt==0)){ //컴퓨터쪽으로 "AT enter(0x0d)" 보내기
			putchar1('A'); putchar1('T'); putchar1(0x0d);
			keyCnt++;
		}

		if((SW1)&&(SW2)&&(SW3)){ //페어링모드 빠져나오기
			for(i=0; i<3; i++){ //LED3번 깜빡이고 메인 WHILE 문으로 이동
				PORTB = 7;
				delay_ms(100);
				PORTB = 0;
				delay_ms(100);
			}
			break;
		}

		if(rx1_flg == 1){
			BTrxdata = getchar1();
			if(0x0d != BTrxdata){
				write_lcd_data(Y_Cnt, X_Cnt++, BTrxdata);
			}
			else{
				Y_Cnt++; X_Cnt=0;
			}
		}

		if(X_Cnt>=20){
			Y_Cnt++, X_Cnt=0;
		}

		if(Y_Cnt == 4){
			Y_Cnt=0;
		}
	}
}
