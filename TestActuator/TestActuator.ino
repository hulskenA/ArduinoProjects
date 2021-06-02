#define ACTUATOR_PIN  3
#define POTENTIOMETER_PIN 0


void setup()
{
  pinMode(ACTUATOR_PIN, OUTPUT);
  Serial.begin(9600);

  analogWrite(ACTUATOR_PIN, 0);
}

void loop()
{
  int adc  = analogRead(POTENTIOMETER_PIN);
  adc = map(adc, 0, 1023, 0, 255);
/*
  ----------map funtion------------the above funtion scales the output of adc, which is 10 bit and gives values btw 0 to 1023, in values btw 0 to 255 form analogWrite funtion which only receives  values btw this range
*/
  analogWrite(ACTUATOR_PIN, adc);
  Serial.println(adc);
}
