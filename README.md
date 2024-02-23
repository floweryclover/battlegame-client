# BattleGame Client
BattleGame Server용 클라이언트
* Unreal Engine 5.3

## 특징
* 언리얼 내장 RPC나 외부 네트워크 라이브러리 사용하지 않음
    * Winsock 이용
* 네트워크 송수신 루프는 별도 스레드에서 진행
    * 게임 스레드에서 CTSRPC 호출 및 STCRPC 처리
    * 게임 스레드에서 Send Queue에 넣으면 네트워크 스레드에서 꺼내서 처리
* Message Reliability 지정해서 송수신 가능
    * 캐릭터 이동과 같은 메시지는 UNRELIABLE로 지정해서 UDP 송수신

## 한계점
* 간혹 버그 있음
* 캐릭터 이동 동기화를 대충해서 완전 띄엄띄엄 움직임
* 공격 애니메이션, 사운드 등등 다 없음
* 애정이 없는 건 아닌데 단순 때움으로 넘긴 부분이 좀 많습니다..

## 이미지
![메인화면](https://github.com/floweryclover/battlegame-client/assets/35604150/7d5f3638-23cb-44be-ae76-10c6b7bb1ba9)
![게임대기화면](https://github.com/floweryclover/battlegame-client/assets/35604150/eeebabb3-1a24-48ba-b1f3-43501ca67382)
![게임화면1](https://github.com/floweryclover/battlegame-client/assets/35604150/6d52b759-08d2-4289-bc47-b6a88583ce84)
![게임화면2](https://github.com/floweryclover/battlegame-client/assets/35604150/62cbc721-13a8-44b3-94fb-dc9ae2982ce3)
![게임종료화면](https://github.com/floweryclover/battlegame-client/assets/35604150/a883dfd7-32a7-46fb-8241-ff6f0c8cdfab)


### 사용된 폰트
* 넥슨 메이플스토리 폰트, 넥슨 고딕 (https://levelup.nexon.com/)