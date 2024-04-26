Assignment 4 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 16 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [3.59 Gbit/s, 3.44 Gbit/s]

## Program Structure and Design of the TCPConnection:
`segment_received`에서는 Sender, Receiver의 Receive 로직을 모두 포함하여 Receiver의
로직은 항상 실행되며 sender의 경우도 syn 이후에는 항상 이루어진다. 
그 외에 플래그에 따라 syn, reset을 수행하게 하였다. 방어적 프로그래밍을 위해 로직이 
끝난 이후에도 보낼 것이 없다면 empty ack을 보내게 된다.
`tick`에는 sender의 tick 로직과 Active Close와 초과 재전송에 의한 Reset을 처리해주었다.
`_is_satisfied_prereq`를 통해 Clean Shutdown을 위한 조건을 검증하고 반환하였다. 
`_move_to_segments_out`는 `_sender`의 큐에 있는 세그먼트를 `_segments_out`으로 옮기고
가능하다면 항상 ack, ackno, win을 포함하도록 하였다. 이로써 항상 ack이 존재할 것으로
생각할 수 있어 함수 내 조건을 나누기 간편해졌다.

### 독립성
`segment_received`, `tick` 등 각 함수들은 sender, receiver의 기능을 모두 포함하며, 
flag와 같은 segment의 특성에 따라 작용이 다르다. 코드 이해 및, 디버깅의 편리성을 위해 
이러한 상황별 실행은 if-else와 같은 구조보다 모두 독립적으로 실행할 수 있도록 if 사용을 지향하였다. 아래에서 설명한 것과 같이 SYN/ACK나 FIN/ACK는 SYN Segment, Ack Segment가
각각 연속하여 도착/전송하는 것과 동일한 것처럼 느껴지게 디자인하였다.


## Implementation Challenges:
### TCP Sender의 Payload 사이즈 결정
assn3의 fill_window 구현 내 payload 사이즈를 결정하는 로직이 잘못되어 있었는데 디버깅을
통해 해당 문제 및 원인을 찾고 해결하는 것이 가장 어려운 문제였다.
기존에는 트랙킹 중인 세그먼트에 대한 ack를 받을 때마다 해당 ack 세그먼트에 포함된
window_size로 window 크기를 변경해주고 window_size만큼 더 fill_window를 수행해주었다.
이렇게 구현시 순서대로 segment가 전송되고 순서대로 ack를 수신하면 문제가
발생하지 않지만 ack의 도착 순서가 섞이게 되면 초기의 window size를 넘어서서
in_flight_segment가 생기게 된다. 
해당 문제를 해결하기 위하여 unordered ack를 고려하기 위해 수신된 ack 중 가장 큰 ackno를 기록하고 기록하고 있는 ackno보다 같거나 클 때만 window를 업데이트해줌으로써 과거 ack를
무시할 수 있도록 하였다.
또한 ackno + window_size보다 작은 범위에서만 read를 수행할 수 있도록하여 window size를 넘어서는 in_flight_segment가 생기지 않게 하였다.

### SYN, FIN Segment의 ACK 여부 변형
테스트 케이스 내 다양한 SYN, FIN segment 교환 상황이 존재하였고 경우에 따라(세그먼트 
압축 여부로 보임) ACK가 포함되거나 그렇지 않기도 하였다. 많은 상황이 존재하기에 각 상황을
개별로 고려하기는 어려웠다. 그래서 대부분의 세그먼트가 기본적으로 ACK를 포함하게 하고
SYN/ACK 세그먼트를 SYN 세그먼트, ACK 세그먼트와 다른 특별한 의미의 세그먼트로 해석하지
않고 SYN 세그먼트 + ACK 세그먼트로 해석함으로써 해당 문제를 해결하였다. SYN, FIN, ACK
로직을 모두 분해하고 세그먼트가 3 Flag에 따른 조건문을 각각 모두 수행할 수 있게 하였다.

## Remaining Bugs:
`check_lab4`, `check_Webget` 테스트를 모두 통과하였으므로 남아 있는 버그는 없을 것으로
예상된다. 
현재 tcp_sender에서 생성한 segment에서 ackno를 설정하고 ack를 처리할 때 트랙킹하는
세그먼트의 ackno를 사용하고 있는데 해당 부분을 제거하면 좋을 것 같다. sender에서 ackno
를 설정하는 것은 기대되는 행동이 아니기 때문이다.

또한 alive 핑의 로직이 분산되고 중복되어 있는데 이를 정리할 수 있으면 더 좋을 것이다.


- Optional: I had unexpected difficulty with: 이전 Assn에서는 주로 cout을 통해 
디버깅하였으나 이번 Assn에서는 cout을 사용시 테스트에 실패하였으나 cerr을 통해 디버깅할
수 있었다. 하지만 테스트 내에 많은 태스크가 포함되기에 디버깅이 어려웠다.

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: 만들어진 TCP가 생각보다 높은 성능을 보여줘서 놀라웠다.

- Optional: I'm not sure about: 현재 구현상 대부분의 세그먼트가 ack Flag를 포함하게 
되는데 이것이 올바른 방향인지 확신이 없다.
