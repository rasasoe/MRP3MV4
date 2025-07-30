#include <avr/io.h>
#include <util/delay.h>

#include "interface.h"
#include "Motor.h"
#include "Move.h"
#include "Sensor.h"
#include "Demo.h"
//#include "Camera.h"       // 카메라가 별도 파일이면 포함

int main(void)
{
    // === 시스템 초기화 ===
    Interface_init();       // 스위치 및 입출력 설정
    LM629_HW_Reset();       // LM629 모터 드라이버 초기화
    MCU_init();             // MCU 관련 초기화 (인터럽트, 포트 등)
    Sensor_init();          // 광/근접 센서 초기화
    Motor_init();           // 모터 드라이버 설정
  //  Camera_init();          // 카메라 모듈 초기화
    Holonomic_init(0, 0, 0);// MRP3M 플랫폼 주행 초기화 (x, y, θ=0)

    // === 데모 실행 ===
    Demo_main();            // SW1/SW2/SW3로 DEMO or TASK 실행 루프

    return 0;               // 무한루프 안이라 도달 못하지만 표준을 위해 작성
}
