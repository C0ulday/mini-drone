void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(analogReadMilliVolts(1));
  int x1 = analogRead(1);
  int y1 = analogRead(0);
  int x2 = analogRead(3);
  int y2 = analogRead(2);
  Serial.printf("x1 =%4d , y1 =%4d, x2 =%4d , y2=%4d\n",x1,y1,x2,y2);
  delay(100);

}
