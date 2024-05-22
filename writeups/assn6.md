Assignment 6 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 3 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

## Program Structure and Design of the Router:
### 라우팅 테이블
`map` 데이터 구조를 사용하여 라우팅 테이블을 표현하였다. 키 값은 prefix와 prefix의 길이
의 Pair로 하고 해당 라우트의 next_hop, interface_num을 value로 저장하게 하였다.
`map` 구조를 사용한 이유는 O(log n) 시간에 라우팅 테이블을 탐색 후 포워딩을 완료하기 
위함이다. 키 값으로 prefix와 그 길이를 모두 가지게 한 이유는 prefix의 뒷자리에 0 비트
들이 포함되게 될 시에 공통 앞부분을 가지는 다른 라우트와 동일한 값으로 처리되어 map 
entry가 덮어씌워지는 상황을 막기 위함이다.

### 프로그램 디자인
`add_route`는 맵에 entry를 추가하는 단순한 구조를 가지고 있다.
`route_on_datagram`은 포워딩을 수행하는 함수이다. 데이터 그램의 목적지의 주소를 전체
에서 뒷자리 하나씩을 줄여가며 해당 값, 길이를 키로 같는 라우트가 있는지 탐색한다.
만약 해당 라우트가 존재하고 ttl 조건도 만족하면 해당 라우트의 인터페이스로 데이터 그램을
보낸다. 끝까지 데이터 그램을 전송하지 못했다면 만족하는 라우트가 없으므로 드랍한다.

## Implementation Challenges:
### 라우트 매칭
최대한 빠른 시간내에 라우팅 테이블을 통해 전송할 라우트를 결정하기 위해 고민하였다.
리스트로 라우팅 테이블을 저장하고 일일이 매칭 여부를 결정하고 공통 길이를 비교하여 
라우트를 저장하기는 것은 O(n)의 시간이 걸리며 구현도 다소 복잡해지게 된다.
이를 간단히 구현하고 더 나은 시간 복잡도로 구현하기 위해 앞서 말하였듯이 맵을 사용하여
라우팅 테이블을 저장하였다. 또한 각 라우트에 대한 최대 배칭 길이를 계산하는 것이 아닌 
단순하게 목적지 주소의 길이를 줄여가며 매칭되는 맵 엔트리가 있는지 확인하게 하여 구현이
단순화되었다. 이 두가지 방법으로 O(log n) 시간 내 포워딩할 라우트를 결정하게 하였다.

목적지 주소를 줄여나갈 때 쉬프트는 정상적으로 작동하지 않아 직접 쉬프트와 유사한 작동을
수행하는 for문을 사용하여 해결하였다. 2로 k번 나눈 몫을 구하고 다시 2로 k번 곱해줌으로써
32-k 길이의 prefix을 구할 수 있었다. 이는 2의 거듭제곱을 곱하거나 나누는 것이 쉬프트
연산과 동일함을 이용한 것이다.

## Remaining Bugs:
모든 테스트를 통과하였으므로 버그는 없을 것으로 생각된다.
라우트 테이블 row들 중 동일한 prefix length를 가지는 동시에 동일한 route prefix를 
가지는 경우, 한 값으로 덮어씌워지게 된다. 이는 실 상황에 적용시 여러 동일 prefix를 가지는
row 중 하나의 값만 사용하게 되지만 동일 prefix length, route prefix를 다룰 수 있더라도
race가 발생하여 어떤 라우트가 사용될지 확정할 수 없으므로 동일 prefix length, route 
prefix를 가지는 경우는 없다고 생각하는 것도 합당하다고 생각하였다. 또한 이런 가정 하에
라우팅 테이블을 작성시 Map을 사용해 O(log n) 시간 내 검색이 가능하기에 더 장점이
있을 것으로 생각하였다.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: 동일 prefix length, 동일 route prefix시 처리
