#define F_CPU 16000000UL
#include "Demo.h"
#include "Motor.h"
#include "Move.h"
#include "interface.h"
#include "Sensor.h"
#include <stdint.h>

unsigned char SW1_flg=1, SW2_flg=1, SW3_flg=1;
unsigned char PGM_cnt=1;

void Demo_main(void){

	delay_ms(100);
	lcd_display_str(0, 0,"MRP33M_V3 4MD");
	lcd_display_str(2, 0,"SW1 >>>> DEMO TEST");
	lcd_display_str(3, 0,"SW3 >>>> TASK TEST");

	while(1){
		if(SW1){
			while(SW1);
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN DEMO 001");
			lcd_display_str(3, 0,"SW3 -> ");
			delay_ms(200);
			while(1){
				if(SW1&&SW1_flg==1){ SW1_flg=0; delay_ms(50); --PGM_cnt; display_char(2,16,PGM_cnt);}
				else if(!SW1)SW1_flg=1;
				if(SW3&&SW3_flg==1){ SW3_flg=0; delay_ms(50); ++PGM_cnt; display_char(2,16,PGM_cnt);}
				else if(!SW3)SW3_flg=1;
				if(PGM_cnt==0){ PGM_cnt=7; display_char(2,16,PGM_cnt);}
				else if(PGM_cnt==8){ PGM_cnt=1; display_char(2,16,PGM_cnt);}

					if(SW2&&SW2_flg==1){
						delay_ms(30);
						while(SW2);
						if(PGM_cnt==1){ DEMO1(); }
						else if(PGM_cnt==2){ DEMO2(); }
						else if(PGM_cnt==3){ DEMO3(); }
						else if(PGM_cnt==4){ DEMO4(); }
						else if(PGM_cnt==5){ DEMO5(); }
						else if(PGM_cnt==6){ DEMO6(); }
						else if(PGM_cnt==7){ DEMO7(); }
					}
					else if(!SW2)SW2_flg=1;
				}
		}
		else if(SW3){
			while(SW3);
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN TASK 001");
			lcd_display_str(3, 0,"SW3 -> ");
			delay_ms(200);
			while(1){
				if(SW1&&SW1_flg==1){ SW1_flg=0; delay_ms(50); --PGM_cnt; display_char(2,16,PGM_cnt);}
				else if(!SW1)SW1_flg=1;
				if(SW3&&SW3_flg==1){ SW3_flg=0; delay_ms(50); ++PGM_cnt; display_char(2,16,PGM_cnt);}
				else if(!SW3)SW3_flg=1;
				if(PGM_cnt==0){ PGM_cnt=7; display_char(2,16,PGM_cnt);}
				else if(PGM_cnt==7){ PGM_cnt=1; display_char(2,16,PGM_cnt);}

				if(SW2&&SW2_flg==1){
					delay_ms(30);
					while(SW2);
					if(PGM_cnt==1){ DEMO1(); }
					else if(PGM_cnt==2){ TASK2(); }
					else if(PGM_cnt==3){ TASK3(); }
					else if(PGM_cnt==4){ TASK4(); }
					else if(PGM_cnt==5){ TASK5(); }
					else if(PGM_cnt==6){ TASK6(); }
				}
				else if(!SW2)SW2_flg=1;
			}
		}
	}
}

void DEMO1(void){
	unsigned int agl=0;
	lcd_clear_screen();
	lcd_display_str(1, 0,"Circular moving ");
	lcd_display_str(2, 0,"Agl : ");
	while(1){
		Holonomic(++agl,200);
		if(agl>=359)agl=0;
		display_int(2,6,agl);
		delay_ms(11);
		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN DEMO 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}

void DEMO2(void){
	int err=0;

	lcd_clear_screen();
	lcd_display_str(1, 0,"Distance keeping PSD");

	while(1){
		display_char(2,0,psd_value[0]);
		err=100-psd_value[0];
		if(err>5){
			Holonomic(0,err*3);
		}
		else if(err <- 5){
			err=err*(-1);
			Holonomic(180, err*3);
		}
		else Holonomic(0,0);

		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN DEMO 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}

void DEMO3(void){
	lcd_clear_screen();
	lcd_display_str(1, 0,"1M Moving ");
	Holonomic_move(DISTANCE, 0, 500, 4, 0, 1000, 0, 0, 600);
	delay_ms(100);
	Holonomic_move(DISTANCE, 180, 500, 4, 0, 1000, 0, 0, 600);

	StopMotion(STOP_ABRUPTLY);
	PGM_cnt=1;
	lcd_display_str(0, 0,"MRP33M_V2 4MD");
	lcd_display_str(1, 0,"SW1 <- ");
	lcd_display_str(2, 0,"SW2 RUN DEMO 003");
	lcd_display_str(3, 0,"SW3 -> ");
}

void DEMO4(void){
	unsigned char col_X=0, col_Y=0, col_9=0;
	int err_X=90, err_Y=120;

	lcd_clear_screen();
	lcd_display_str(1, 0,"Camera -> Red Ball ");

	while(1){
		col_9=Camera_Cmd(1, Coordinate_9);
		col_X=Camera_Cmd(1, Coordinate_X);
		col_Y=Camera_Cmd(1, Coordinate_Y);

		err_X = 90-col_X;
		err_Y = col_Y-120;

		if(!(col_9==0))non_Holonomic((err_X*4),(err_Y*2),(err_Y/2));
		else if(col_9==0)Holonomic(360, 45);

		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN DEMO 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}

/* -----------------------------------------------------------
   DEMO 5 : Vacuum-style 장애물 회피 (앞→후진→회전)
----------------------------------------------------------- */
void DEMO5(void)   /* ← 이제 Vacuum DEMO */
{
    enum { FORWARD, BACKWARD, ROTATE } state = FORWARD;
    const unsigned int THRESH   = 100;   // 거리 임계값
    const unsigned int MOVE_SPD = 400;   // 전/후진 속도
    const unsigned int ROT_SPD  = 300;   // ★ 더 빠르게 회전
    const uint8_t      ROT_TIME = 4;     // ★ sec 단위 (?120°)

    typeof(sec) rot_start = 0;

    lcd_clear_screen();
    lcd_display_str(1, 0, "Vacuum DEMO 5");
    lcd_display_str(2, 0, "PSD[0] : ");

    while (1)
    {
        uint8_t dist = psd_value[0];
        display_int(2, 9, dist);

        switch (state)
        {
        case FORWARD:
            if (dist >= THRESH) {          /* 벽 감지 → 정지 */
                Holonomic(0, 0);
                state = BACKWARD;
            } else {
                Holonomic(0, MOVE_SPD);    /* 계속 전진 */
            }
            break;

        case BACKWARD:
            if (dist <= THRESH) {          /* 안전거리 확보 */
                Holonomic(0, 0);
                state     = ROTATE;
                rot_start = sec;           /* 회전 시작 시각 */
                Holonomic(-1, ROT_SPD);    /* 제자리 우회전 */
            } else {
                Holonomic(180, MOVE_SPD);  /* 계속 후진 */
            }
            break;

        case ROTATE:
            if ((sec - rot_start) >= ROT_TIME) { /* ★ 4초 회전 */
                Holonomic(0, 0);
                state = FORWARD;           /* 다시 전진 */
            }
            break;
        }

        /* SW2 로 데모 종료 */
        if (SW2) {
            SW2_flg = 0;
            PGM_cnt = 1;
            Holonomic(0, 0);
            lcd_display_str(0,0,"MRP33M_V3 4MD");
            lcd_display_str(1,0,"SW1 <- ");
            lcd_display_str(2,0,"SW2 RUN DEMO 001");
            lcd_display_str(3,0,"SW3 -> ");
            break;
        }
    }
}

void DEMO6(void){
	lcd_clear_screen();
	lcd_display_str(1, 0,"Holonomic moving ");
	lcd_display_str(2, 0,"Diamond step ");
	while(1){
		Holonomic_move(DISTANCE, 45, 400, 4, 0, 500, 0, 0, 300);
		delay_ms(200);
		Holonomic_move(DISTANCE, 315, 400, 4, 0, 500, 0, 0, 300);
		delay_ms(200);
		Holonomic_move(DISTANCE, 225, 400, 4, 0, 500, 0, 0, 300);
		delay_ms(200);
		Holonomic_move(DISTANCE, 135, 400, 4, 0, 500, 0, 0, 300);

		PGM_cnt=1;
		Holonomic(0,0);
		lcd_display_str(0, 0,"MRP33M_V3 4MD");
		lcd_display_str(1, 0,"SW1 -> ");
		lcd_display_str(2, 0,"SW2 RUN DEMO 001");
		lcd_display_str(3, 0,"SW3 <- ");
		break;
	}
}

void DEMO7(void){
	unsigned char turn_cnt=0;
	unsigned char distance_data=0;

	lcd_clear_screen();
	lcd_display_str(1, 0,"non Holonomic moving");
	lcd_display_str(2, 0,"Dianmond step ");

	WriteCommand(0, DFH);
	WriteCommand(1, DFH);
	WriteCommand(2, DFH);

	while(1){
		Holonomic(0,200);
		distance_data=READ_distance(0);
		if(distance_data>=500){
				WriteCommand(0, DFH);
				WriteCommand(1, DFH);
				WriteCommand(2, DFH);

				while(1){
					distance_data=READ_distance(360);
					MOTOR_CTR(0,-30,-30);
					if(distance_data>=135){
						Holonomic(0,0);
						WriteCommand(0, DFH);
						WriteCommand(1, DFH);
						WriteCommand(2, DFH);
						break;
					}
				}
				++turn_cnt;
			}	

			if(turn_cnt>=4){
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 -> ");
			lcd_display_str(2, 0,"SW2 RUN DEMO 001");
			lcd_display_str(3, 0,"SW3 <- ");
			break;
		}
	}
}
	
void TASK1(void){
	char DIR='G';
	unsigned char SENSOR=0;

	lcd_clear_screen();
	lcd_display_str(1, 0,"LINE tracking ");
	
	while(1){
		SENSOR=READ_SENSOR();
		if(!(SENSOR&0x04) && (SENSOR&0x08) && !(SENSOR&0x10)){ non_Holonomic(200, 0, 0); }

		if(SENSOR&0x04 && (SENSOR&0x08) && !(SENSOR&0x10)){ non_Holonomic(200, 0, -15); DIR='L';}
		else if(SENSOR&0x04&&!(SENSOR&0x08)&&!(SENSOR&0x10)){non_Holonomic(200, -10, -15); DIR='L';}

		if(!(SENSOR&0x04) && !(SENSOR&0x08) && (SENSOR&0x10)){ non_Holonomic(200, 0, 15); DIR='R';}
		else if(!(SENSOR&0x04)&&!(SENSOR&0x08)&&(SENSOR&0x10)){non_Holonomic(200, 10, 15); DIR='R';}

		else if(!(SENSOR&0x04) && !(SENSOR&0x08) && !(SENSOR&0x10)){
			if(DIR=='L'){
				non_Holonomic(0, 0, -90);
			}
			else if(DIR=='R'){
				non_Holonomic(0, 0, 90);
			}
		}
		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW2 <- ");
			lcd_display_str(2, 0,"SW2 RUN TASK 001 ");
			break;
		}
	}
}

void TASK2(void){
	char DIR='G';
	unsigned int DIR_AGL=90;
	unsigned char SENSOR=0;

	lcd_clear_screen();
	lcd_display_str(1, 0,"LINE tracking ");
	lcd_display_str(2, 0,"Inductive Sensor ");

	while(1){
		SENSOR=READ_SENSOR();
		if(SENSOR&0x01){ non_Holonomic(200, 0, 10); DIR='R'; sec=0;}
		else if(!(SENSOR&0x01)){non_Holonomic(200, 0, -10); DIR='L';}

		if(!(SENSOR&0x01)&&sec>=5){
			if(DIR=='R')Holonomic(360, 50);
			else if(DIR=='L')Holonomic(-1, 50);
			WriteCommand(0, DFH);
			WriteCommand(1, DFH);
			WriteCommand(2, DFH);
			while(1){
				SENSOR=READ_SENSOR();
				if(SENSOR&0x01){
					StopMotion(STOP_ABRUPTLY);
					WriteCommand(0, DFH);
					WriteCommand(1, DFH);
					WriteCommand(2, DFH);
					break;
				}
				if(READ_distance(360)>=DIR_AGL){
					if(DIR=='R'){
						Holonomic(-1, 50);
						DIR='L';
						DIR_AGL=360;
						WriteCommand(0, DFH);
						WriteCommand(1, DFH);
						WriteCommand(2, DFH);
					}
					else if(DIR=='L'){
						Holonomic(360, 50);
						DIR = 'R';
						DIR_AGL=360;
						WriteCommand(0, DFH);
						WriteCommand(1, DFH);
						WriteCommand(2, DFH);
					}
				}
				sec=0;
			}
		}
		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0, "SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN TASK 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}

void TASK3(void){

	unsigned char col_X=0, col_Y=0, col_9;
	int err_X=90, err_Y=120;

	lcd_clear_screen();
	lcd_display_str(1, 0,"Camete -> Red Ball ");

	while(1){
		col_9=Camera_Cmd(1, Coordinate_9);
		col_X=Camera_Cmd(1, Coordinate_X);
		col_Y=Camera_Cmd(1, Coordinate_Y);

		err_X=(90-col_X);
		err_Y=(col_Y-120);

		if(!(col_9==0))non_Holonomic(err_X*4, 200, err_Y);
		else if(col_9==0)Holonomic(360, 45);

		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN TASK 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}
void TASK4(void){
	unsigned char i =0;
	unsigned char Max_PSD=0, Max_num=0;
	unsigned int PSD_AGL[9]={0, 320, 280, 240, 200, 160, 120, 80, 40};

	Holonomic(0, 250);
	lcd_clear_screen();
	lcd_display_str(1, 0,"Avoidance moving");
	lcd_display_str(3, 0,"Agl:");

	while(1){
		for(i=0;i<9;++i){
			if(psd_value[i]>130){
				if(psd_value[i]>Max_PSD){
					Max_PSD=psd_value[i];
					Max_num=i;
				}
			}
		}

		if((PSD_AGL[Max_num]+180)>359){
			Holonomic(PSD_AGL[Max_num]+180-360, 250);
			display_int(3, 5, PSD_AGL[Max_num]+180-360);
		}
		else if((PSD_AGL[Max_num]+180)<360){
		Holonomic(PSD_AGL[Max_num]+180, 250);
		display_int(3, 5, PSD_AGL[Max_num]+180);
		}
		Max_PSD=0;
		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0,0,"MRP33M_V3 4MD");
			lcd_display_str(2, 0,"SW2 RUN TASK 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}
void TASK5(void){
	unsigned char front=0, left=0, right=0, r_side=0;
	int err1=0, err2=0, err3=0;

	lcd_clear_screen();
	lcd_display_str(1, 0,"Moving Front");

	while(1){
		front=psd_value[0]; left=psd_value[1]; right=psd_value[8]; r_side=psd_value[7];

		err1=150-front;
		err2=(right-left)/2;

		if(r_side>100){
			err3=r_side/2;
		}
		else err3=0;

		non_Holonomic(err1, 150-err3, err2+err3);

		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN TASK 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}
void TASK6(void){
	unsigned char center=0, up=0, down=0, front=0;
	int err1=0, err2=0, err3=0;

	lcd_clear_screen();
	lcd_display_str(1, 0,"Moving side");
	while(1){
		front=psd_value[0]; up=psd_value[1]; center=psd_value[2]; down=psd_value[3];
		
		err1=center-150;
		err2=(up-down)/2;

		if(front>100){
			err3=front/2;
		}
		else err3=0;

		non_Holonomic(150-err3, err1, err2+err3);

		if(SW2){
			SW2_flg=0;
			PGM_cnt=1;
			Holonomic(0,0);
			lcd_display_str(0, 0,"MRP33M_V3 4MD");
			lcd_display_str(1, 0,"SW1 <- ");
			lcd_display_str(2, 0,"SW2 RUN TASK 001");
			lcd_display_str(3, 0,"SW3 -> ");
			break;
		}
	}
}
