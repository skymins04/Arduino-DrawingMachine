2017-12-17-SUN

# SoftIce 동아리 연말 과제 - Arduino-DrawingMachine
아두이노를 이용한 기학학적 무늬 출력 장치
제작자: 10201 강민수
제작기간: 2017-12-04-MON ~ 2017-12-17_SUN

작품 구성 모듈: -stepping motor (28BYJ-48)x 3
               -stepping motor driver (ULN2003APG) x 3
               -16by2 LCD add I2C module x 1
               -rotary Encoder x 1
               -mini breadBoard x 1
               -Arudino Uno R3 x 1
               -9V 1A ACtoDC adapter x 1
               -USB 2.0 B type cable x 1
               -Dupont cable for wiring
              
아두이노 핀연결:
               왼쪽 모터의 드라이버: IN1 - 2, IN2 - 3, IN3 - 4, IN4 - 5
               오른쪽 모터의 드라이버: IN1 - 6, IN2 - 7, IN3 - 8, IN4 - 9
               턴 테이블 모터의 드라이버: IN1 - 10, IN2 - 11, IN3 - 12, IN4 - 13
               LCD-I2C: SDA - A4, SCL - A5 
               로터리 엔코더: CLK - 0, DT - 1, SW - A0
*Serial.end()를 이용해 디지털 0, 1번을 사용한다. (단, 당연히 Serial통신은 사용할 수 없다)
*모터 전원 공급:
               -아두이노 DC-IN 포트에 점퍼와이어를 납땜해 9v 스텝모터 드라이버에 전원공급
