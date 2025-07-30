#ifndef __MOVE_H
#define __MOVE_H

#include "Motor.h"

#define PSD			0x01
#define IDT			0x02
#define PHT			0x04
#define EN			0x08
#define DISTANCE	0x10
#define PSD_Y		0x20
#define PSD_N		0x40

#define PSD0		0x0001
#define PSD1		0x0002
#define PSD2		0x0004
#define PSD3		0x0008
#define PSD4		0x0010
#define PSD5		0x0020
#define PSD6		0x0040
#define PSD7		0x0080
#define PSD8		0x0100
#define PSD9		0x0200
#define PSD_ALL		0x03FF

#define EN0			1
#define EN1			2
#define	EN2			4

#define x			0
#define X			0
#define y			1
#define Y			1
#define w			2
#define W			2
#define L			0
#define l			0
#define R			1
#define r			1
#define B			2
#define b			2
#define F			3
#define f			3

/******************************************************/
void StartMotion(void);
void StopMotion(uint16_t mode);

unsigned long READ_EN(unsigned char num);
unsigned long READ_distance(int f_agl);

void Holonomic(int f_agl, int f_speed);
void non_Holonomic(long Fx, long Fy, long Fw);
void MOTOR_CTR(long MT0, long MT1, long MT2);

void Holonomic_init(unsigned char PSD_mode, unsigned char IDT_PT_mode, unsigned char EN_mode);
void Holonomic_move(unsigned char mode, int f_agl, unsigned int f_speed, unsigned char f_acc, unsigned int num, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int move_distance);

#endif		//__MOVE_H
