const int ena1 = 4;
const int dir1 = 3;
const int pul1 = 2;

const int ena2 = 7;
const int dir2 = 6;
const int pul2 = 5;

const int ena3 = 8;
const int dir3 = 9;
const int pul3 = 10;

const int intervalo = 350;

boolean pulso = LOW;

void setup() {
  pinMode(ena1,OUTPUT);  
  pinMode(dir1,OUTPUT);
  pinMode(pul1,OUTPUT);
  digitalWrite(ena1,LOW);
  digitalWrite(dir1,HIGH);
  digitalWrite(pul1,HIGH);

  pinMode(ena2,OUTPUT);  
  pinMode(dir2,OUTPUT);
  pinMode(pul2,OUTPUT);
  digitalWrite(ena2,LOW);
  digitalWrite(dir2,LOW);
  digitalWrite(pul2,HIGH);

  pinMode(ena3,OUTPUT);  
  pinMode(dir3,OUTPUT);
  pinMode(pul3,OUTPUT);
  digitalWrite(ena3,LOW);
  digitalWrite(dir3,LOW);
  digitalWrite(pul3,HIGH);
  
}

void loop() {
  pulso = !pulso;
  digitalWrite(pul1,pulso);
  digitalWrite(pul2,pulso);
  digitalWrite(pul3,pulso);
  delayMicroseconds(intervalo);
}
