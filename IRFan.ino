#include <IRremote.h>


#define FAN_ENABLE_PIN    5
#define FAN_DIR_A_PIN     3
#define FAN_DIR_B_PIN     4
#define IR_PIN           11

#define POWER         0xFFA25D
#define FUNC_STOP     0xFFE21D
#define VOL_UP        0xFF629D
#define FAST_BACK     0xFF22DD
#define PAUSE         0xFF02FD
#define FAST_FORWARD  0xFFC23D
#define DOWN          0xFFE01F
#define VOL_DOWN      0xFFA857
#define UP            0xFF906F
#define EQ            0xFF9867
#define ST_REPT       0xFFB04F
#define ZERO          0xFF6897
#define ONE           0xFF30CF
#define TWO           0xFF18E7
#define THREE         0xFF7A85
#define FOUR          0xFF10EF
#define FIVE          0xFF38C7
#define SIX           0xFF5AA5
#define SEVEN         0xFF42BD
#define EIGHT         0xFF4AB5
#define NINE          0xFF52AD
#define REPEAT_       0xFFFFFFFF

#define FAN_SPEED_STEP 25


IRrecv irrecv(IR_PIN);
decode_results results;

unsigned int fanSpeed = 100;
bool isPowered = true;
unsigned long lastBtn = POWER;


void translateIR(unsigned long btn)
{
  Serial.print("Button Pressed: ");
  switch(btn)
  {
    case POWER:
      Serial.println("POWER");
      isPowered = !isPowered;
      analogWrite(FAN_ENABLE_PIN, (isPowered ? fanSpeed : LOW));
      break;
    case VOL_UP:
      Serial.println("VOL+");
      if (fanSpeed <= 255 - FAN_SPEED_STEP)
        fanSpeed += FAN_SPEED_STEP;
      break;
    case VOL_DOWN:
      Serial.println("VOL-");
      if (fanSpeed >= FAN_SPEED_STEP)
        fanSpeed -= FAN_SPEED_STEP;
      break;
    case FAST_BACK:
      Serial.println("FAST BACK");
      fanSpeed = 0;
      break;
    case FAST_FORWARD:
      Serial.println("FAST FORWARD");
      fanSpeed = 255;
      break;
    case UP:
      Serial.println("UP");
      if (fanSpeed <= 255 - FAN_SPEED_STEP)
        fanSpeed += FAN_SPEED_STEP;
      break;
    case DOWN:
      Serial.println("DOWN");
      if (fanSpeed >= FAN_SPEED_STEP)
        fanSpeed -= FAN_SPEED_STEP;
      break;
    case FUNC_STOP:
      Serial.println("FUNC/STOP");
      break;
    case PAUSE:
      Serial.println("PAUSE");
      break;
    case EQ:
      Serial.println("EQ");
      break;
    case ST_REPT:
      Serial.println("ST/REPT");
      break;
    case ZERO:
      Serial.println("0");
      fanSpeed = FAN_SPEED_STEP;
      break;
    case ONE:
      Serial.println("1");
      fanSpeed = FAN_SPEED_STEP*2;
      break;
    case TWO:
      Serial.println("2");
      fanSpeed = FAN_SPEED_STEP*3;
      break;
    case THREE:
      Serial.println("3");
      fanSpeed = FAN_SPEED_STEP*4;
      break;
    case FOUR:
      Serial.println("4");
      fanSpeed = FAN_SPEED_STEP*5;
      break;
    case FIVE:
      Serial.println("5");
      fanSpeed = FAN_SPEED_STEP*6;
      break;
    case SIX:
      Serial.println("6");
      fanSpeed = FAN_SPEED_STEP*7;
      break;
    case SEVEN:
      Serial.println("7");
      fanSpeed = FAN_SPEED_STEP*8;
      break;
    case EIGHT:
      Serial.println("8");
      fanSpeed = FAN_SPEED_STEP*9;
      break;
    case NINE:
      Serial.println("9");
      fanSpeed = 255;
      break;
    case REPEAT_:
      Serial.print("REPEAT => ");
      translateIR(lastBtn);
      break;  
    default: 
      Serial.println("other button");
  }
  Serial.print("Fan Speed: ");
  Serial.println(fanSpeed);
  if (btn != REPEAT_)
    lastBtn = btn;
  if (isPowered)
    analogWrite(FAN_ENABLE_PIN, fanSpeed);
}


void setup()
{
  pinMode(FAN_ENABLE_PIN, OUTPUT);
  pinMode(FAN_DIR_A_PIN, OUTPUT);
  pinMode(FAN_DIR_B_PIN, OUTPUT);
  Serial.begin(9600);
  irrecv.enableIRIn();

  digitalWrite(FAN_DIR_A_PIN, HIGH);
  digitalWrite(FAN_DIR_B_PIN, LOW);
  analogWrite(FAN_ENABLE_PIN, 100);
}


void loop()
{
  if (irrecv.decode(&results))
  {
    translateIR(results.value);
    if (fanSpeed <= 75)
      analogWrite(FAN_ENABLE_PIN, 0);
    delay(500);
    irrecv.resume();
  }
}
