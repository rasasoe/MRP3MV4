#define F_CPU 16000000UL
#include <util/delay.h>
#include "Motor.h"
#include "Interface.h"

// Status Byte를 읽는다
#define READ_STATUS(X, Y)	((Y) = MOTOR_CMD(X))
// Status Byte의 Busy Bit를 체크하여 리셋되면 루프를 탈출한다
#define		CHECK_BUSY(X, Y)			while(((Y) = MOTOR_CMD(X)) & 0x01)

// Status Byte를 저장할 변수
unsigned char status;


// Function : LM629에서 데이터를 읽는다.
// Parameter :
//		motor - 선택할 모터
// Return	:
//		data - 읽은 데이터
unsigned char ReadData(unsigned char motor)
{
	unsigned char data;
	
	// Status Byte의 Busy Bit 체크
	CHECK_BUSY(motor, status);
	// 데이터 입력
	data = MOTOR_DATA(motor);

	CHECK_BUSY(motor, status);
	// 입력된 데이터 리턴
	return data;
}

// Function : 커맨드를 LM629에 쓴다.
// Parameter :
//		motor - 선택할 모터
//		cmd - 실행할 커맨드
// Return	: 없음

void WriteCommand(unsigned char motor, unsigned char cmd)
{
	//Status Byte의 Busy Bit 체크
	CHECK_BUSY(motor, status);
	// 커맨드 출력
	MOTOR_CMD(motor) = cmd;

	CHECK_BUSY(motor, status);
}

// Function : 2 바이트 데이터를 LM629에 쓴다.
// Parameter:
//		motor - 선택할 모터
//		data - 쓸 데이터
// Return	: 없음
void WriteDataTwoByte(unsigned char motor, int data)
{
	// Status Byte의 Busy Bit 체크
	CHECK_BUSY(motor, status);
	// 2 바이트 데이터 출력
	MOTOR_DATA(motor) = (uint8_t)((data >> 8)&0xFF);
			CHECK_BUSY(motor, status);
	MOTOR_DATA(motor) = (uint8_t)(data&0xFF);
			CHECK_BUSY(motor, status);
}



// Function : 4 바이트 데이터를 LM629에 쓴다.
// Parameter : 
//		motor - 선택할 모터
//		data - 쓸 데이터
// Return	: 없음
void WriteDataFourByte(unsigned char motor, long data)
{
	WriteDataTwoByte(motor, (int16_t)(data >> 16));
	WriteDataTwoByte(motor, (int16_t)(data));
}


// Function : 모터 초기화
// Parameter : 없음
// Return	: 없음
void Motor_init(void)
{
	// Motor1 게인 설정
	SetGain(MOTOR0, 7, 3, 1);
	// Motor2 게인 설정
	SetGain(MOTOR1 ,7, 3, 1);
	// Motor3 게인 설정
	SetGain(MOTOR2, 7, 3, 1);

	SetAcceleration(MOTOR0, 0x3FFFFFFF);
	SetAcceleration(MOTOR1, 0x3FFFFFFF);
	SetAcceleration(MOTOR2, 0x3FFFFFFF);
}

// Function : LM629를 리셋한다.
// Parameter :
//		motor - 선택할 모터
// Return	: 없음
void Motor_Reset(uint8_t motor)
{
	// RESET 커맨드 출력
	WriteCommand(motor, RESET);
	// 리셋이 완료될 때까지 최소 1.5ms이상 대기
	delay_ms(2);
	// Motor1 인터럽트 마스크 리셋
	WriteCommand(motor, RSTI);
	WriteDataTwoByte(motor, 0x0000);
}


// Function : 필터값 KP, KI, KD 값을 설정한다.
// Parameter :
//		motor - 선택할 모터
//		kp	- P값
//		ki	- I값
//		kd	- D값
// Return	: 없음
void SetGain(uint8_t motor, int16_t kp, int16_t ki, int16_t kd)
{
	unsigned char LD_kp=0, LD_ki=0, LD_kd=0;

	if(kp)LD_kp=1;
	else LD_kp=0;
	if(ki)LD_ki=1;
	else LD_ki=0;
	if(kd)LD_kd=1;
	else LD_kd=0;
	// LFIL 커맨드 출력
	WriteCommand(motor, LFIL);
	WriteDataTwoByte(motor, (0x0000) | (LD_kp << 3) | (LD_ki << 2) | (LD_kd << 1) );
	// P값 출력
	if(kp)WriteDataTwoByte(motor, kp);
	// I값 출력
	if(ki)WriteDataTwoByte(motor, ki);
	// D값 출력
	if(kd)WriteDataTwoByte(motor, kd);
	// 필터 업데이트
	WriteCommand(motor, UDF);
}


// Function : 가속도값을 설정한다.
// Parameter :
//		motor - 선택할 모터
//		acc - 가속도값
// Return	: 없음
void SetAcceleration(uint8_t motor, uint32_t acc)
{
	// LTRJ 커맨드 출력
	WriteCommand(motor, LTRJ);
	// 컨트롤 워드 출력
	WriteDataTwoByte(motor, (1 << ACC_LOAD));
	// 가속도값 출력
	WriteDataFourByte(motor, acc);
}


// Function : 속도값을 설정한다.
// Parameter :
//		motor - 선택할 모터
//		velocity - 속도 설정 (0 ~ 1,073,741,823) 음수 입력시 역방향으로 회전
// Return	: 없음
void SetVelocity(uint8_t motor, int32_t velocity)
{
	unsigned char dir=1;

	if(velocity<0){
		velocity*=(-1);
		dir=0;
	}
	// LTRJ 커맨드 출력
	WriteCommand(motor,LTRJ);
	// 컨트롤 워드 출력
	WriteDataTwoByte(motor, (dir << FORWARD_DIR) | (1 << VELOCITY_MODE) | (1 << VELOCITY_LOAD));
	// 속도값 출력
	WriteDataFourByte(motor,velocity);
}

// Function : 포지션 값을 설정한다.
// Parameter :
//		motor - 선택할 모터
//		mode - 모드 설정 (0 -> Absolute Position, 1 -> Relative Position)
//		acc - 가속도 설정 (0 ~ 1,073,741,823)
//		velocity - 속도 설정 (0 ~ 1,073,741,823)
//		position - 모터위치설정 (-1,073,741,824 ~ 1,073,741,823)counts
//		Return	:없음
void SetPosition(uint8_t motor, uint8_t mode, int32_t acc,int32_t velocity,int32_t position)
{
	// LTRJ 커맨드 출력
	WriteCommand(motor, LTRJ);
	// 컨트롤 워드 출력
	// mode : 0 - Absolute Position, 1 - Relative Position
	// Position Mode로 설정

	WriteDataTwoByte(motor, (1 << ACC_LOAD) | (1 << VELOCITY_LOAD) | (1 << POSITION_LOAD) | (mode << POSITION_RELATIVE) );

	// 위치값 출력
	WriteDataFourByte(motor, acc);
	WriteDataFourByte(motor, velocity);
	WriteDataFourByte(motor, position);
}


// Function	: LM629 H/W Reset 최초 전원 인가시 반드시 실행해야 한다.
// Parameter : 없음
// Return	: 없음
void LM629_HW_Reset(void){
	unsigned char status[3]={0, }, i=0, step=0;

	DDRA=0x00;

	PORTC=0x00;
	DDRC=0xFF;

	PORTG=0x00;
	DDRG=0x03;

	while(1){
		if(step==0){
			while(1){
				PORTC |= 0x05;
				_delay_ms(3);
				PORTC &= ~0x05;
				_delay_ms(2);

				PORTG &= ~0x02;
				PORTC &= ~0x08;

				PORTC &= ~0x07;
				status[0]=PINA;

				PORTC |=0x01;
				status[1]=PINA;

				PORTC = (PORTC&0xF8)|0x02;
				status[2]=PINA;

				if( (status[0]==0xC4||status[0]==0x84) &&
					(status[1]==0xC4||status[1]==0x84) &&
					(status[2]==0xC4||status[2]==0x84) ){
						step=1;
						break;
				}
				else{
					PORTC=0x00;
					lcd_display_str(0,0, "Error. Restart");
				}
			}
		}
		if(step==1){
			// SRE : 외부 데이터 메모리 영역 활성화
			MCUCR = (1 << SRE) | (0 << SRW10);
			// SRL(0) : Upper Sector로만 사용 0x1100 ~ 0xFFFF(Upper)
			// SRW11:SSRW10(1:0) - Read/Write 신호에 2개의 Wait
			XMCRA = (0 << SRL2) | (0 << SRL1) | (0 << SRL0) | (1 << SRW11);
			//XMBK : Bus Keeper Enable, XMM(0x4) - A11 ~ A8까지만 외부 메모리 포트로 사용
			XMCRB = (1 << XMBK) | (1 << XMM2) | (0 << XMM1) | (0 << XMM0);
			while(1){
				for(i=0;i<3;++i){
					WriteCommand(i, RSTI);
					WriteDataTwoByte(i, 0x0000);
					READ_STATUS(i, status[i]);
				}
				if( (status[0]==0xC0||status[0]==0x80) &&
					(status[1]==0xC0||status[1]==0x80) &&
					(status[2]==0xC0||status[2]==0x80) ){
						step=2;
						break;
				}
				else{ //다시 처음부터 LM629초기화
					step=0;
					// SRE : 외부 데이터 메모리 영역 비활성화
					MCUCR &= ~(1 << SRE) & ~(1 << SRW10);
					PORTC=0x00;
					lcd_display_str(0,0,"Error. Restart");
					break;
				}
			}
		}
		if(step==2){
			lcd_display_str(0,0,"Initialize success.");
			for(i=0;i<3;++i){
				PORTB|=0x07;
				_delay_ms(150);
				PORTB&=~0x07;
				_delay_ms(150);
			}
			lcd_clear_screen();
			break;
		}
	}
}
