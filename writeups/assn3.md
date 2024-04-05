Assignment 3 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 12 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

## Program Structure and Design of the TCPSender:
Assn3 명세서에서 언급된 TCP Sender가 가질 수 있는 여러 상태를 기준으로 코드 로직들을 
분리하였다.
acknowledged 되지 않고 Flight 중인 세그먼트를 저장하기 위해 FIFO 순으로 순회가능한 리스트를 
이용하였다.
- 가장 최신에 보낸 세그먼트 부터 탐색하여 탐색시간을 조금이라도 줄이기 위함이다. 
또한 일정 범위의 acknowledged된 세그먼트를 삭제할 때는 iter를 이용해 O(1) 내에 수행된다.
Ack 받기는 트랙킹 중인 세그먼트를 latest -> oldest 순으로 탐색하며 ack의 ackno와 동일한
ackno를 가지는 세그먼트를 찾음으로써 acknowledged된 범위와 그렇지 않은 범위를 구별하였다.

윈도우 채우기는 크게 CLOSED, SYN_SENT, 그 외로 구별하여 처리해주었다.
스트림으로부터 버퍼에 있는 스트링 크기, 남은 윈도우 크기, 최대 페이로드 용량 중 최솟값
을 읽어 들인 뒤 이를 페이로드에 넣어주고 버퍼의 EOF 여부, 페이로드를 고려한 남은 윈도우
크기를 고려하여 FIN Flag를 처리해주는 방식으로 설계하였다.

재전송 및 타이머는 올바른 Ack를 수신하였을 때 초기화해주고 타이머가 울리면 
재전송하고 윈도우 사이즈가 0이 아니면 RTO도 두 배해주었다. 틱 마다의 초를 더해가
타이머 시간을 계산하였다. 

`_tracked_count`의 경우 `_tracking_segments`만으로 도출가능한 값이므로 
중복된 상태값이나 시간 복잡도 개선을 위해 추가하였다.

## Implementation Challenges:
상태에 따라 원하는 동작이 매우 상이하여 이를 일반적으로 통합하여 구현하는데 어려움을
겪었다. 그래서 문서에 주어진 상태 및 그 명세를 이용하여 `_TCPSenderState`를 구현하였다.
또한 기준으로 상태를 구분하여 조금씩 다른 로직이 다소 중복되더라도 상태에 따라 코드줄마다 
구분하여 처리하는 것이 아닌 switch-case로 크게 분기를 나눠 처리해주었다.

버퍼 내 문자열은 모두 처리해주었으나 FIN Flag만을 전송하지 못한 케이스를 
고려해주는 것에서 어려움을 겪었으나 문제의 원인이 되던 버퍼 사이즈 검증에서
SynAckedEof에 대해 예외 처리해줌으로써 해결하였다.


## Remaining Bugs:
테스트를 모두 통과하였으므로 남아 있는 버그는 없을 것으로 예상된다.
명세서의 FAQ에서 없다고 명시된 partially acknowledges가 전송되는 테스트 케이스가 
존재하여 이러한 Ack들을 무시하는 형식으로 구현하여 이를 고려하도록 코드를 개선하면 좋을
것으로 생각된다. 또한 세그먼트으 헤더와 페이로드를 설정하고 전송하는 부분들이 사소하게
로직이 달라 일부 코드들이 반복되어 이를 리팩토링하여 개선할 수 있을 것이다.

- Optional: I had unexpected difficulty with: 윈도우 사이즈가 0일 때 윈도우 사이즈를
 1로 생각하여 전송을 허락하는 부분을 잘못 해석하여 Ack로부터 받은 윈도우 사이즈가 0인
 경우뿐만 아니라 Ack로부터 받은 윈도우 사이즈가 1이상이다가 segment 전송으로 인해
 내부 윈도우 사이즈 상태가 0이 된 경우에도 동일하게 적용해주어 어려움을 겪었다. 
 이 외에도 상태값들을 최소화하기 위해서는 상태값에 따라 세세히 처리해주어야하는 것이 많아 
 어려움을 겪었다.

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
