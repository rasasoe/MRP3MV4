#ifndef __INTERFACE_H
#define __INTERFACE_H

#include <avr/io.h>

#define ADC_VREF_TYPE	0x20

#define SLA				0x07

#define LED_DDR			DDRB
#define LED_PORT		PORTB
#define LED1			0
#define LED2			1
#define LED3			2

#define BZ_DDR			DDRB
#define DZ_PORT			PORTB
#define BZ				3

#define SW_DDR			DDRB
#define SW_PORT			PINB
#define SW_MASK			0x70
#define SW1				(~PINB&0x10)
#define SW2				(~PINB&0x20)
#define SW3				(~PINB&0x40)
#define Emergency		(~PINB&0x80)


//ÇØ´ç LED¸¦ On
#define LED_ON(X)		(LED_PORT |= (1 << (X)))
//ÇØ´ç LED¸¦ Off
#define LED_OFF(X)		(LED_PORT &= ~(1 << (X)))

//Buzzer On
#define BZ_ON()			(BZ_PORT |= (1 << BZ));
//Buzzer Off
#define BZ_OFF()		(BZ_PORT &= ~(1 << BZ));

void MCU_init(void);
void Interface_init(void);
char getchar1(void);
void putchar1(char data);
unsigned char read_adc(unsigned char adc_input);
void WriteI2C(uint8_t cmd, uint8_t data);
unsigned char ReadI2C(uint8_t cmd);
void lcd_write_data(unsigned char data);
void lcd_display_str(unsigned char Y_line, unsigned char X_line,char *string);
void write_lcd_data(unsigned char Y_line, unsigned char X_line, long data);
void lcd_clear_screen(void);
void display_char(unsigned char line, unsigned char col, unsigned char data);
void display_int(unsigned char line, unsigned char col, unsigned int data);
void display_long(unsigned char line, unsigned char col, unsigned long data);
void delay_us(unsigned char time_us);
void delay_ms(unsigned int time_ms);

void BT_Init(void);
void BT_Pairmode(void);

volatile extern unsigned char sec, cnt0, cnt1, cnt2;
volatile extern unsigned char rx1_flg, rx1_buff;
volatile extern int speed, next_speed, acc, wspeed, next_wspeed, acc_W;

//volatile extern long INT_ENcoder[];

#endif				//__INTERFACE_H
