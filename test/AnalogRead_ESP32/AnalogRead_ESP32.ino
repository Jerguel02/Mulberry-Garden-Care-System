#define PIN1 34
#define PIN2 35
#define PIN3 32

void setup() {
  Serial.begin(115200);
  pinMode(PIN1, OUTPUT);
  pinMode(PIN2, OUTPUT);
  pinMode(PIN3, OUTPUT);
  // put your setup code here, to run once:

}

void loop() {
  int ACS1_val = analogRead(PIN1);
  int ACS2_val = analogRead(PIN2);
  int ACS3_val = analogRead(PIN3);
  Serial.println(ACS1_val);
  Serial.println(ACS2_val);
  Serial.println(ACS3_val);
  Serial.println();
  delay(2000);
  // put your main code here, to run repeatedly:

}
