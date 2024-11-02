#define currentpin  25

float R1 = 6800.0;
float R2 = 12000.0;

void setup() {
Serial.begin(115200);
pinMode(currentpin, INPUT);
}

void loop() {
int adc = analogRead(currentpin);
float adc_voltage = adc * (3.3 / 4096.0);
float current_voltage = (adc_voltage * (R1+R2)/R2);
float current = (current_voltage - 2.5) / 0.100;
Serial.print("Current Value: ");
Serial.println(current);
delay(1000);
}