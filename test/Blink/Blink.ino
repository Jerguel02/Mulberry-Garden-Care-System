#define currentpin  14

float R1 = 1000.0;
float R2 = 2000.0;

#define LED_BUILTIN 17
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(currentpin, INPUT);
}
bool state = false;
// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, state ? LOW : HIGH);  // turn the LED on (HIGH is the voltage level)
  int adc = analogRead(currentpin);
  float adc_voltage = adc * (3.3 / 4096.0);
  Serial.print("Voltage: ");
  Serial.println(adc_voltage);
  float current_voltage = (adc_voltage * (R1+R2)/R2);
  float current = (current_voltage) / 0.066;
  Serial.print("Current Value: ");
  Serial.println(current);
  delay(500);                      // wait for a second
  state = !state;
}
