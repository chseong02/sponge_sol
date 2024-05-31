Assignment 7 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

My assignment partner's name: Shin Donghun

My assignment partner's POVIS ID: sdh728

My assignment partner's ID (numeric): 20200837

This assignment took me about 1.5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

## Solo portion:
내 Implementation만으로 client, server 프로그램 모두 실행한 결과, Conversation은 
정상적으로 작동하였다.
프로그램 실행시 Connection을 만들어지고, Connection 이후 양방향으로 메세지를 보낼 수
있고, 양쪽에서 ctrl+D를 누를시 정상적으로 connection이 closing 되었다.
보내진 메세지, 수신된 메세지는 양쪽이 모두 동일하였다.

랜덤으로 생성된 1M 파일을 위해 정상적으로 연결하고, 파일을 보내고, 받고 정상 종료되었다.
이후 보내고 받은 파일의 sha256sum으로 확인한 결과 동일하였다.

다음 과정들을 성공시키기 위해 코드에 대해서 수정한 사항은 없다.

## Group portion:
Team name: Andreessen
Partner: 신동훈, 20200837
My Role: Client

본인은 Client, 신동훈 학우는 Server 프로그램을 실행한 후 진행한 Conversation은 
정상적으로 작동하였다.
프로그램 실행시 Connection을 만들어지고, Connection 이후 양방향으로 메세지를 보낼 수
있고, 양쪽에서 ctrl+D를 누를시 정상적으로 connection이 closing 되었다.
보내진 메세지, 수신된 메세지는 양쪽이 모두 동일하였다.

본인은 Client, 신동훈 학우는 Server 프로그램을 실행한 후 진행한 1M 파일 받기는 
정상적으로 작동하였다. 
랜덤으로 생성된 1M 파일을 위해 정상적으로 Connection을 생성하고, Server에서는 파일을 
보내고, 내가 실행한 Client는 파일을 받고 정상 종료되었다.
Server가 보낸 파일의 sha256sum hash값과 Client가 받은 파일의 sha256sum이 동일한 것
으로 보아 두 content는 identical하다.

다음 과정들을 성공시키기 위해 둘 모두 코드에 대해서 수정한 사항은 없다.

추가로 서로 Role을 변경하여 내가 Server, 신동훈 학우가 Client를 실행하였을 때에도
모두 정상작동하였다.

## Creative portion (optional):
별도로 추가한 점은 없다.

## Other remarks:
나의 TCP 구현 및 작동 방식이 다소 특이하다고 생각하여 다른 학우의 코드와 잘 
소통이 될지 걱정되었으나 문제가 없어 놀라웠다.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
