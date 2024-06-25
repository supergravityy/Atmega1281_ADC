#Atmega1281을 이용한 ADC 프로젝트

## 개요
이 프로젝트는 Atmega1281을 사용하여 가변저항으로 조절된 DC 전압을 측정하고, UART 직렬통신과 인터럽트를 통해 제어하며, FND 7세그먼트 디스플레이에 소수점 둘째 자리까지 표현하는 시스템을 구현합니다.

## 주요 기능
- UART 직렬통신을 사용하여 AD 변환 제어
- 인터럽트를 통한 AD 변환 시작 및 완료 처리
- FND 7세그먼트를 이용한 소수점 2자리 출력
- LED 패턴 반복으로 인터럽트 동작 확인

## 설치 및 실행 방법
1. 저장소를 클론합니다.
    ```sh
    git clone https://github.com/yourusername/ADCProject.git
    ```
2. 프로젝트 디렉토리로 이동합니다.
    ```sh
    cd ADCProject
    ```
3. Atmega1281 보드에 프로그램을 업로드합니다.
    ```sh
    make upload
    ```

## 요구 사항
- AVR-GCC 컴파일러
- AVRDUDE
- Atmega1281 보드
