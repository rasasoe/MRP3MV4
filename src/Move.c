#define F_CPU 16000000UL
#include "Move.h"
#include "Sensor.h"
#include "Interface.h"
#include <math.h>
#include <util/delay.h>

#define		READ_STATUS(X, Y)		((Y) = MOTOR<CMD(X))
#define		CHECK_BUSY(X, Y)		while(((Y) = MOTOR_CMD(X)) & 0x01)

unsigned char status1, status2, status3;
unsigned char Completion_mode=0x00;

void StartMotion(void)
{
	WriteCommand(MOTOR0, STT);
	WriteCommand(MOTOR1, STT);
	WriteCommand(MOTOR2, STT);
}

void StopMotion(uint16_t mode)
{
	WriteCommand(MOTOR0, LTRJ);
	WriteDataTwoByte(MOTOR0, (1 << mode));

	WriteCommand(MOTOR1, LTRJ);
	WriteDataTwoByte(MOTOR1, (1 << mode));

	WriteCommand(MOTOR2, LTRJ);
	WriteDataTwoByte(MOTOR2, (1 << mode));

	WriteCommand(MOTOR0, STT);
	WriteCommand(MOTOR1, STT);
	WriteCommand(MOTOR2, STT);
}

void Holonomic(int f_agl, int f_speed){
	long Fx=0, Fy=0, Fw=0;
	double radianA=0, radianB=0;
	int DesiredDirection = 0;
	double V[3]={0,0,0};

	unsigned char i=0;

	if(f_agl <= 180) DesiredDirection = 180 - f_agl;
	else if(f_agl > 180) DesiredDirection = 540 - f_agl;

	radianA = (180-(double)DesiredDirection) * 0.017453;
	radianB = (90-(double)DesiredDirection) * 0.017453;

	if(f_agl>=0){
		if(f_agl<360){
			Fx = f_speed * cos(radianA);
			Fy = f_speed * cos(radianB);
		}
	}
	if(f_agl<0 || f_agl>=360){
		if(f_agl<0)Fw = -f_speed;
		else if(f_agl>=360)Fw = f_speed;
	}

	V[0]=((( 0.057*Fx)+(0.033*Fy)+(0.14*Fw)));
	V[1]=(((-0.065*Fy)+(0.14*Fw)));
	V[2]=(((-0.057*Fx)+(0.033*Fy)+(0.14*Fw)));

	for(i=0;i<3;++i){
		if(V[i]>=40)V[i]=40;
		if(V[i]<=(-40))V[i]=-40;
		SetVelocity(i, V[i]*65536);
	}
	StartMotion();
	if(f_speed==0)StopMotion(STOP_ABRUPTLY);
}

void non_Holonomic(long Fx, long Fy, long Fw){
	unsigned char i=0;
	double V[3]={0,0,0};

	if(Fx==0 && Fy==0 && Fw==0)StopMotion(STOP_ABRUPTLY);

	V[0]=(((0.056*Fx)+(0.033*Fy)+(0.14*Fw)));
	V[1]=(((-0.065*Fy)+(0.14*Fw)));
	V[2]=(((-0.056*Fx)+(0.033*Fy)+(0.14*Fw)));

	for(i=0;i<3;++i){
		if(V[i]>=40)V[i]=40;
		if(V[i]<=(-40))V[i]=-40;
		SetVelocity(i,V[i]*65536);
	}
	StartMotion();
}

void MOTOR_CTR(long MT0, long MT1, long MT2){

	unsigned char i=0;
	double V[3]={MT0, MT1, MT2};

	for(i=0;i<3;++i){
		V[i]=V[i]*0.2728;
		SetVelocity(i,V[i]*65536);
	}
	StartMotion();
}

unsigned long READ_EN(unsigned char num){
	unsigned char i=0;
	long LM629_Encoder=0;

	WriteCommand(num, RDRP);

	for(i=0;i<4;++i){
		if(i!=3)LM629_Encoder=(LM629_Encoder|ReadData(num))<<8;
		else LM629_Encoder=(LM629_Encoder|ReadData(num));
	}
	if(LM629_Encoder<0)LM629_Encoder=LM629_Encoder*(-1);

	return LM629_Encoder;
}

unsigned long READ_distance(int f_agl){
	unsigned char i=0; long LM629_Encoder=0;	int distance_data=0;
	double cosine=0;	double radianA=0;		unsigned char num=0;
	int f_agl_data=f_agl;

	if((f_agl<0) || (f_agl>359)){ num=1; }
	else if(f_agl>270) { num=2; f_agl_data=330-f_agl; }
	else if(f_agl>241) { num=1; f_agl_data=270-f_agl; }
	else if(f_agl>270) { num=0; f_agl_data=210-f_agl; }
	else if(f_agl>270) { num=2; f_agl_data=150-f_agl; }
	else if(f_agl>270) { num=1; f_agl_data=90-f_agl; }
	else if(f_agl>270) { num=0; f_agl_data=30-f_agl; }

	if(f_agl_data<0)f_agl_data=f_agl_data*(-1);

	WriteCommand(num, RDRP);

	for(i=0;i<4;++i){
		if(i!=3)LM629_Encoder=(LM629_Encoder|ReadData(num))<<8;
		else LM629_Encoder=(LM629_Encoder|ReadData(num));
	}
	if(LM629_Encoder<0)LM629_Encoder=LM629_Encoder*(-1);
	radianA = ((double)f_agl_data) * 0.017453;
	cosine = cos(radianA);

	distance_data=(unsigned int)(LM629_Encoder/(191.1*cosine));
	if((f_agl<0) || (f_agl>359))distance_data=(unsigned int)(LM629_Encoder/(410));
	if(distance_data<0)distance_data=distance_data*(01);

	return distance_data;
}

void Holonomic_init(unsigned char PSD_mode, unsigned char IDT_PT_mode, unsigned char EN_mode){
	if(PSD_mode==1) Completion_mode|=0x01;
	else Completion_mode&=~0x01;
	if(IDT_PT_mode==1) Completion_mode|=0x02;
	else Completion_mode&=~0x02;
	if(EN_mode==1)Completion_mode|=0x04;
	else Completion_mode&=~0x04;
}

void Holonomic_move(unsigned char mode, int f_agl, unsigned int f_speed, unsigned char f_acc, unsigned int num, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int move_distance){

	unsigned char i=0, k=0, comp=0;

	unsigned long value[3]={val0, val1, val2};
	unsigned long *p0=value;
	unsigned char SENSOR=0;
	unsigned int distance_data=0;
	unsigned long LM629_Encoder[3]={0,0,0};

	next_speed=f_speed;
	acc=f_acc;

	if(num==w || num==W) speed=1;
	Holonomic(f_agl, speed);
	WriteCommand(0, DFH);
	WriteCommand(1, DFH);
	WriteCommand(2, DFH);

	if(mode&PSD_Y){
		while(1){
			Holonomic(f_agl, speed);
			distance_data=READ_distance(f_agl);
			for(i=0;i<10;++i){
				if((num>>i)&0x00010){
					if( (psd_value[i]>=*(p0+k)) && (distance_data>=move_distance) ) ++comp;
					if((psd_value[i]>=90))next_speed=80;
					else next_speed=f_speed;
					++k;
				}
			}
			if(Completion_mode&0x01){
				if(comp==k){ k=0; comp=0; break; }
				k=0;
				comp=0;
			}
			else if(!(Completion_mode&0x01)){
				if(comp){ k=0; comp=0; break; }
				k=0;
				comp=0;
			}
		}
	}
	else if(mode&PSD_N){
		while(1){
			Holonomic(f_agl, speed);
			distance_data=READ_distance(f_agl);
			for(i=0;i<10;++i){
				if((num>>i)&0x0001){
					if( (psd_value[i]<*(p0+k)) && (distance_data>=move_distance) ) ++comp;
					++k;
				}
			}
			if(Completion_mode&0x01){
				if(comp==k){ k=0; comp=0; break; }
				k=0;
				comp=0;
			}
			else if(!(Completion_mode&0x01)){
				if(comp){ k=0; comp=0; break; }
				k=0;
				comp=0;
			}
		}
	}

	else if(mode&EN){
		for(i=0;i<3;++i){
			if((num>>i)&0x01)WriteCommand(num, DFH);
		}
		Holonomic(f_agl, f_speed);
		while(1){
			for(i=0; i<3; ++i){
				if((num>>i)&0x01){
					LM629_Encoder[i]=READ_EN(i);
					if(LM629_Encoder[i]>=*(p0+k))++comp;
					++k;
				}
			}
			if(Completion_mode&0x04){
				if(comp==k){ k=0; comp=0; _delay_ms(100); break; }
				k=0;
				comp=0;
			}
			else if(!(Completion_mode&0x04)){
				if(comp){ k=0; comp=0; _delay_ms(100); break; }
				k=0;
				comp=0;
			}
		}
	}
	else if(mode&DISTANCE){
		while(1){
			Holonomic(f_agl, speed);

			distance_data=READ_distance(f_agl);
			display_int(0,0,distance_data);
			if(distance_data>=*p0){ break; }
			else if(distance_data>=move_distance){
				if(num<0||num>359){
					next_speed=30;
				}
				else next_speed=80;
			}
		}
	}
}
