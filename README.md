# variety_ipc

여러가지 IPC를 테스트 한후 편의상 Unix Domain Socket을 사용하기로 함

## 작업 순서
1. UnixDomainSocket 테스트  (완료)
2. 테스트용 코드 작성 (완료)
3. 라이브러리 작업 (완료)
4. 프로토콜 정의
5. 유틸리티성 기능 작성 
6. ssl 작업 
7. boost::asio 사용 ??

## NamedPipe & Epoll 문제점
1. 코드상의 실수를 한것인지 아닌지 정확치는 아니하나 namedpipe 에서 connection이 종료되었을때 epoll 이벤트가 날라오지 않는다.
kepp-alive 메세지를 주기적으로 송수신할수 있겠지만, 불필요한 코드가 생성되는것 같아서 사용하지 아니함.

