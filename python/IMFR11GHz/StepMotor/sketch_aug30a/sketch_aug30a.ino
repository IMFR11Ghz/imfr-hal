
int pp=1;
int pp1=1;
int jj=10;         //variale de tiempo manejar entre 10-71   10 lo mas rapido y 71 es lo mas lento
int motorPin1 = 8; // PIN-es del Motor
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;
int boton = 13;
int boton1 = 12;
int buttonState = 0;
int buttonState1 = 0;
void setup() {

 pinMode(motorPin1, OUTPUT); // Configuración de los PIN-es como salida digital
 pinMode(motorPin2, OUTPUT);
 pinMode(motorPin3, OUTPUT);
 pinMode(motorPin4, OUTPUT);
 pinMode(boton, INPUT);
 pinMode(boton1, INPUT);


}

void loop() {
  buttonState = digitalRead(boton);
  buttonState1 = digitalRead(boton1);
if(buttonState==HIGH){

  if(pp==1){
   digitalWrite(motorPin1, HIGH); // Primer paso
 digitalWrite(motorPin2,LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
 delay(70);
 digitalWrite(motorPin1, HIGH); // Segundo paso
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
  delay(60);
 digitalWrite(motorPin1, LOW); // Tercer paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH); 
  delay(50);
 digitalWrite(motorPin1, LOW); // Cuarto paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
 delay(40);
 pp=8;}
 digitalWrite(motorPin1, HIGH); // Primer paso
 digitalWrite(motorPin2,LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
delay(jj);
 digitalWrite(motorPin1, HIGH); // Segundo paso
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(jj);
 digitalWrite(motorPin1, LOW); // Tercer paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH); 
delay(jj);
 digitalWrite(motorPin1, LOW); // Cuarto paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
delay(jj);
}
if(buttonState==LOW){
  pp=1;
}
if(buttonState1==LOW){
  pp1=1;
}
if(buttonState1==HIGH){

  if(pp1==1){
   digitalWrite(motorPin1, HIGH); // Primer paso
 digitalWrite(motorPin2,LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(70);
 digitalWrite(motorPin1, HIGH); // Segundo paso
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
  delay(60);
 digitalWrite(motorPin1, LOW); // Tercer paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW); 
  delay(50);
 digitalWrite(motorPin1, LOW); // Cuarto paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(40);
 pp1=9;}
   digitalWrite(motorPin1, HIGH); // Primer paso
 digitalWrite(motorPin2,LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(jj);
 digitalWrite(motorPin1, HIGH); // Segundo paso
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
  delay(jj);
 digitalWrite(motorPin1, LOW); // Tercer paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW); 
  delay(jj);
 digitalWrite(motorPin1, LOW); // Cuarto paso
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(jj);
}


}


