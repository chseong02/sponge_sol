Assignment 6 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 3 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
[]

Implementation Challenges:
[]

Remaining Bugs:
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
