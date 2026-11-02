# slider.LED

아두이노와 p5.js를 이용하여 슬라이더로 LED 밝기를 조정하는 프로젝트를 진행했습니다. 이 프로젝트는 강의 시간에 진행한 p5.js통해 아두이노를 제어하는 것의 연장선으로 기존의 p5.js코드에 슬라이더를 추가하여 LED밝기를 조절하는 프로젝트입니다.

아래의 회로도와 같은 회로를 사용했습니다. 강의 시간에 사용한 회로에서 추가적으로 PWM 3번 핀에 LED를 연결하였습니다. 이는 p5.js에서 슬라이더를 활용해 PWM 3번 핀에 연결한 LED의 밝기를 조절하기 위함입니다.<img width="634" height="458" alt="image" src="https://github.com/user-attachments/assets/d8ae890d-cbdc-4035-9f07-607205e914c0" />


첫번째로, 기존의 p5.js 코드에서 15~25번 라인을 추가했습니다. 슬라이더를 생성하고 슬라이더를 세로로 만들어야 하기 때문에 기준점을 잡고 -90도를 회전시켜서 세로로 만들었습니다. 슬라이더의 값을 serial 통신을 통해 PWM:value 형식으로 아두이노로 데이터를 전송합니다.

https://github.com/user-attachments/assets/07e612a6-f353-4845-a816-658ad43ff628

https://github.com/user-attachments/assets/c5fe73d8-9e52-4cd5-a48b-f3178b52c98f
