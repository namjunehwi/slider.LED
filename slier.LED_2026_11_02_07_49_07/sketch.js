// 복잡한 통신은 serial-helper.js가 처리합니다.
let serial, connectButton;
let ledOn = false;
let potValue = 0;
let statusText = 'Arduino를 연결해 주세요.';
let slider;

function setup() {
  createCanvas(600, 400);
  textFont('sans-serif');
  serial = new SerialHelper(9600, receiveLine, message => statusText = message);
  connectButton = createButton('Arduino 연결 / 해제');
  connectButton.mousePressed(() => serial.toggle());
  
  slider = createSlider(0,255,0);
  slider.position(500,200);
  slider.size(250,20);
  
  // 회전 중심을 왼쪽 위(550,200)로 잡고 -90도 회전
  slider.style('transform-origin', '500 200');
  slider.style('transform', 'rotate(-90deg)');

  slider.input(() => {
    serial.send('PWM:' + slider.value());
  });
}

function draw() {
  background(242, 246, 250);
  fill(25, 40, 60); noStroke(); textAlign(CENTER);
  textSize(24); text('Arduino Uno R3 · Web Serial', width / 2, 36);
  textSize(14); text(statusText, width / 2, 66);
  fill(ledOn ? '#22c55e' : '#64748b');
  circle(160, 175, 110);
  fill(255); textSize(22); text(ledOn ? 'ON' : 'OFF', 160, 183);
  fill(25, 40, 60); textSize(15); text('원을 클릭하면 LED 토글', 160, 260);
  text('가변저항 A0', 420, 120);
  fill('#dbe3ed'); rect(310, 155, 220, 40, 10);
  fill('#3b82f6'); rect(310, 155, map(potValue, 0, 1023, 0, 220), 40, 10);
  fill(25, 40, 60); textSize(24); text(potValue + ' / 1023', 420, 235);
  textSize(14); text('9600 baud · ON / OFF + 줄바꿈 또는 PWM:값', 300, 325);
  text('수신: 0~1023 또는 POT:값 · LED:ON / LED:OFF', 300, 352);
}

async function mousePressed() {
  if (dist(mouseX, mouseY, 160, 175) > 55) return;
  const next = !ledOn;
  if (await serial.send(next ? 'ON' : 'OFF')) ledOn = next;
}

function receiveLine(line) {
  // Arduino가 보내는 한 줄을 화면에 반영합니다.
  const message = line.trim().toUpperCase();
  if (message === 'LED:ON' || message === 'ON') ledOn = true;
  else if (message === 'LED:OFF' || message === 'OFF') ledOn = false;
  else {
    const match = message.match(/^(?:POT\s*[:=]\s*)?(\d+)$/);
    if (match) potValue = constrain(Number(match[1]), 0, 1023);
  }
}
