#include <LedControl.h>
#include <MFRC522.h>
#include <pitches.h>
#include <Keypad.h>
#include <SPI.h>


#define VERBOSE         true

#define BUZZER_PIN        49
#define RST_PIN           13
#define SS_PIN            53

#define NOTE_DURATION    100
#define MELODY_LENGTH      3

#define PADLOCK_LENGTH     8
#define CODE_LENGTH        3
#define ROWS               4
#define COLS               4


const int MELODY[MELODY_LENGTH] = { NOTE_C5, NOTE_C5, NOTE_C5 };

const byte UID[4] = { 0x99, 0xC8, 0x16, 0xA3 };

const byte rowPins[ROWS] = { 9, 8, 7, 6 };
const byte colPins[COLS] = { 5, 4, 3, 2 };
const char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

const byte closedPadlock[PADLOCK_LENGTH] = {
  B00000000,
  B00011110,
  B00110010,
  B01010010,
  B01010010,
  B00110010,
  B00011110,
  B00000000
};
const byte midtimePadlock[PADLOCK_LENGTH] = {
  B00000000,
  B00001111,
  B00111001,
  B01001001,
  B01001001,
  B00101001,
  B00001111,
  B00000000
};
const byte openedPadlock[PADLOCK_LENGTH] = {
  B00000000,
  B00001111,
  B01111001,
  B10001001,
  B10001001,
  B01001001,
  B00001111,
  B00000000
};


byte INDEX = 0;
char code[CODE_LENGTH] = { hexaKeys[0][0], hexaKeys[1][1], hexaKeys[2][2] };
bool isCorrect = true;
bool isOpened = false;
bool drawMatrix = true;
bool soundIsRunning = false;
byte* lastMatrixDrawed = closedPadlock;

LedControl ledControl = LedControl(12,10,11,1);
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;


void playSound()
{
  if (soundIsRunning)
    return;

  Serial.println("PLAY SOUND -> BEGIN");
  soundIsRunning = true;
  unsigned int thisNote;
  for (thisNote = 0; thisNote < MELODY_LENGTH; thisNote++)
  {
    tone(BUZZER_PIN, MELODY[thisNote], NOTE_DURATION);
    delay(NOTE_DURATION);
  }
  soundIsRunning = false;
  Serial.println("PLAY SOUND -> FINISH");
}

void drawPadlock(byte* padlock)
{
  lastMatrixDrawed = padlock;
  if (!drawMatrix)
    return;

  ledControl.clearDisplay(0);
  unsigned int i;
  for (i = 0; i < PADLOCK_LENGTH; i++)
    ledControl.setRow(0, i, padlock[i]);
}

void blinkPadlock(byte* param, unsigned int times = 3, unsigned int delayTime = 250)
{
  if (!drawMatrix)
    return;

  unsigned int i;
  for (i = 0; i < times; i++)
  {
    ledControl.clearDisplay(0);
    delay(delayTime);
    drawPadlock(param);
    delay(delayTime);
  }
}

void reset()
{
  Serial.println("RESET");  
  if (isOpened)
  {
    drawPadlock(midtimePadlock);
    delay(500);
  }
  INDEX = 0;
  isCorrect = true;
  isOpened = false;
  drawPadlock(closedPadlock);
}

void enter()
{
  if (isOpened)
    return;

  Serial.print("Enter => ");
  if (INDEX < CODE_LENGTH)
  {
    Serial.println("Not enougth characters");
    blinkPadlock(closedPadlock, 5, 100);
  }
  else if (INDEX == CODE_LENGTH && isCorrect)
    goodPsw();
  else
    wrongPsw();
}

void wrongPsw()
{
  Serial.println("Wrong Password");
  playSound();
  blinkPadlock(closedPadlock);
  reset();
}

void goodPsw()
{
  Serial.println("Good Password");
  isOpened = true;
  drawPadlock(midtimePadlock);
  delay(500);
  drawPadlock(openedPadlock);
}

void setup()
{
  Serial.begin(9600);
  ledControl.shutdown(0, false);
  ledControl.setIntensity(0, 4);
  ledControl.clearDisplay(0);
  drawPadlock(closedPadlock);

  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;
}
  
void loop()
{
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() && !isOpened)
  {
    delay(50);
    if (VERBOSE)
      Serial.print(F("Card UID:"));
    bool isGood = true;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      if (VERBOSE)
      {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      isGood = isGood && (mfrc522.uid.uidByte[i] == UID[i]);
    }
    if (VERBOSE)
      Serial.println();
    delay(50);
    if (isGood)
      goodPsw();
    else
      wrongPsw();
    return;
  }

  char k = keypad.getKey();
  if (keypad.getState() != PRESSED || !k)
    return;

  switch (k)
  {
    case 'D':
      reset();
      return;
    case 'A':
      enter();
      return;
    case 'B':
      drawMatrix = !drawMatrix;
      if (drawMatrix)
        drawPadlock(lastMatrixDrawed);
      else
        ledControl.clearDisplay(0);
      return;
    case 'C':
    case '*':
    case '#':
      return;
  }
  
  if (INDEX >= CODE_LENGTH || k != code[INDEX])
    isCorrect = false;
  INDEX++;

  if (VERBOSE)
  {
    Serial.print("Key: ");
    Serial.print(k);
    Serial.print(" | INDEX: ");
    Serial.print(INDEX);
    Serial.print(" | isCorrect: ");
    Serial.println(isCorrect);
  }
}
