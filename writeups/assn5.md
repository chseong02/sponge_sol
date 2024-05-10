Assignment 5 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

## Program Structure and Design of the NetworkInterface:
### Time
Epoch time과 유사한 방식으로 _timer로 '시각'을 표현함으로써 tick을 비롯한 _timer를 
사용하는 로직을 간단하게 유지할 수 있었다. 또한 address mapping이 만료되는 시각, ARP가
time out되는 시각을 각 엔티티와 함께 저장하고 관리하여 공통된 시각인 _timer를 이용해
관리할 수 있게 하였다.

### Address Mapping Table
Map 구조를 이용하여 IP Address - (이더넷 주소, 매핑 만료 시각) 형식의 Map으로 Address
Mapping Table을 저장하였다. 메세지를 보낼 때 IP Address는 알고 있기에 Ip Address를 통해 테이블을 조회하여 매핑된 값들을 빨리 찾기 위함이다. recv_frame에서 받은 ARP 요청, 답장
의 sender의 ip, 이더넷 주소를 매핑하였다. 

### DataGram Delay Gueue
주소 매핑이 확정되지 않았을 때 큐잉된 인터넷 데이터그램들은 List 구조를 이용하여 
(데이터 그램, IP Address, ARP 요청 타임아웃 시각) 형식으로 저장하였다. ARP를 생성한
데이터 그램에 대해서는 타임아웃 시각을 현재 시각 + 5초로 설정하여 추후에 같은 Address에
대한 데이터 그램에 의해 ARP가 생성되지 못하도록 하였다. 또한 ARP를 미생성한 데이터그램에
대해서는 타임아웃 시각을 현재 시각으로 하여 타임아웃 관련 작동에 영향을 주지 못하게 
하였다.

### 전반적 구조
`send_datagram`은 입력받은 next_hop와 매칭되는 Address Mapping이 있다면 이를
이용해 IPv4 데이터 그램을 전송한다. Address Mapping이 없다면 보내고 싶었던 Datagram을
큐잉하고 해당 주소 Mapping에 대한 ARP 5초 내 전송 여부에 따라 ARP를 전송한다.

`recv_frame`은 IPv4를 받으면 파싱한 결과를 반환한다.
ARP라면 Sender의 주소 Mapping를 Table에 저장한다. ARP 중 Request를 받으면 sender 주소로 ARP Reply로 반환한다. 

`tick`은 시간 경과에 따른 _timer를 업데이트하고 이에 따른 address mapping 만료를 처리한다.

## Implementation Challenges:
TCP에서 사용했었던 타임아웃 구혆하기 위한 타이머 형식은 여기서 작동하기 어려웠다.
여러 대상과 통신하는 것을 고려해야하며 2가지(ARP, Address Mapping)에 대한 타이머를 고려
해야하기 때문이다. 만일 틱마다 모든 ARP, Address Mapping에 대한 타이머를 업데이트
해준다면 매우 많은 시간, 리소스를 사용할 것이다. 이러한 문제를 해결하기 위해 JWT 등에 
사용되는 에포치 타임에서 착안한 시각, 만료 시각 기재 방식을 사용하여 해결하였다.


## Remaining Bugs:
테스트를 모두 통과하였으므로 남아 있는 버그는 없을 것으로 기대한다.
tick마다 만료된 주소가 있는지 확인하는 것은 불필요한 루프를 생성하므로 send_datagram을 
하는 시점에 만료되었는지 확인하는 것으로 변경하면 더 효율적으로 바꿀 수 있을 것이다.
또한 짧은 시간 내 재발송을 막기 위한 딜레이 큐 순회를 맵 구조를 이용해 수행한다면 
더 효율적으로 작동할 것이다.


- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: 5초, 30초 등의 시간 간격을 두고 일정 작업들의 만료 여부를 결정하는데 5초, 30초 이상인지, 5초, 30초 초과인지 확실하지 않았다. 테스트 케이스는 이보다 널널한 시간 간격으로 주어져서 문제는 없었다.
