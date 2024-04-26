Assignment 4 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 16 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [3.59 Gbit/s, 3.44 Gbit/s]

Program Structure and Design of the TCPConnection:
[]

Implementation Challenges:
[]

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
