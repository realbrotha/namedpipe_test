# Boost Asio Socket

boost asio socket 

## Why ASIO ??
일반적으로 사용하던 c api ( socket create, bind, listen, connect, accpet, Epoll 등등)들은 태생적으로 절차지향적이고
범용적인 처리를 위해서 몹시 느슨한 타입체크를 진행한다. (물론 코딩하는데는 큰 문제는 없지만 ....)
좀더 객체지향적인 코드를 위해 Boost Asio를 사용하게 되었으며, 최대한 짧은 코드와 재사용 가능한 구현을 목표로 한다.

## 기타
- 라이브러리, 헤더온리 등등을 컴팩트하게 사용하지 아니함, 해당 부분은 나중에 최적화 할수 있음
- 우선 풀 컴파일을 해서, 나타난 결과물들을 통체로 넣어두고 사용


