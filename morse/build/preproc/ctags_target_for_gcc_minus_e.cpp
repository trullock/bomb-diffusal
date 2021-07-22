# 1 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
# 2 "d:\\git\\bomb-diffusal\\morse\\morse.ino" 2
# 3 "d:\\git\\bomb-diffusal\\morse\\morse.ino" 2
# 4 "d:\\git\\bomb-diffusal\\morse\\morse.ino" 2

byte strikes = 0;
byte status = 0;



Encoder knob(2, 3);
int knobPosition;

const String morseAlphabet[] = {
 ".-", // a
 "-...", // b
 "-.-.", // c
 "-..", // d
 ".",
 "..-.",
 "--.",
 "....",
 "..",
 ".---",
 "-.-",
 ".-..",
 "--",
 "-.",
 "---",
 ".--.",
 "--.-",
 ".-.",
 "...",
 "-",
 "..-",
 "...-",
 ".--",
 "-..-",
 "-.--",
 "--..",
 "-----", // 0
 ".----", // 1
 "..---", // 2
 "...--",
 "....-",
 ".....",
 "-....",
 "--...",
 "---..",
 "----."
};

int dotLength = 400;
int dashLength = 800;
int blipDelay = 200;
int characterDelay = 1000;
int loopDelay = 3000;


const String words[2] = {
 "alpha",
 "bravo"
};

String currentWord;
char currentChar;
int currentCharIndex = -1;
String currentMorseChar;
int currentMorseIndex = -1;

bool ledOn = false;
unsigned long nextMillis = 0;



unsigned long lastButtonMillis = 0;
bool lastButtonPressed = false;
bool buttonPressed = false;



bool armed = false;
byte difficulty = 1;
# 91 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void setup()
{
 // join i2c bus with address #8
 Wire.begin(8);
 // enable broadcasts to be received
 
# 96 "d:\\git\\bomb-diffusal\\morse\\morse.ino" 3
(*(volatile uint8_t *)(0xBA)) 
# 96 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
     = (8 << 1) | 1;

   Wire.onRequest(requestEvent);
 Wire.onReceive(receiveEvent);

 Serial.begin(9600);

 Serial.print("I2C Address: ");
 Serial.println(8);

 pinMode(7, 0x1);
 pinMode(5, 0x0);

 randomSeed(analogRead(0));
 int wordIndex = random(0, 2);
 currentWord = words[wordIndex];

 Serial.print("Current word: ");
 Serial.println(currentWord);
}

void loop()
{
 if(!armed)
  return;

 updateLed();
 handleButton();
}

void handleKnob() {
 int position = knob.read();
}


void handleButton() {
 bool pressed = digitalRead(5) == 0x1;

 if(pressed != lastButtonPressed)
 {
  lastButtonMillis = millis();
  lastButtonPressed = pressed;
  return;
 }

 if(millis() > (lastButtonMillis + 50))
 {
  if(pressed == buttonPressed)
   return;

  buttonPressed = pressed;

  if(!pressed)
  {
   return;
  }

  // TODO: implement properly

  strikes++;
  Serial.println("Striking");
 }


}

void updateLed()
{
 if(millis() < nextMillis)
  return;

 // If the LED is currently ON
 if(ledOn) {
  // turn LED off
  digitalWrite(7, 0x0);
  ledOn = false;

  // if we're at the end of a character
  if(currentMorseIndex == currentMorseChar.length() - 1)
  {
   // if it was the last char in the word
   if(currentCharIndex == currentWord.length() - 1)
    nextMillis = millis() + loopDelay;
   else
    nextMillis = millis() + characterDelay;
  }
  else
  {
   nextMillis = millis() + blipDelay;
  }

  currentMorseIndex++;

  return;
 }

 // else LED if currently OFF

 // if we're just starting or we've finished this character
 if(currentMorseChar == "" || currentMorseIndex == currentMorseChar.length())
 {
  // if it was the last char of the word
  if(currentCharIndex == currentWord.length() - 1)
   currentCharIndex = 0;
  else
   currentCharIndex++;

  currentMorseIndex = 0;

  // get the next char in alpha and morse
  currentChar = currentWord[currentCharIndex];
  currentMorseChar = morseAlphabet[currentChar - 97];
 }

 // wait for dot/dash length of time
 bool blipIsDot = currentMorseChar[currentMorseIndex] == '.';
 nextMillis = millis() + (blipIsDot ? dotLength : dashLength);

 ledOn = true;
 digitalWrite(7, 0x1);
}

void requestEvent() {
 Wire.write(status);
 Wire.write(strikes);
}

void arm()
{
 armed = true;
}

void receiveEvent (int howMany)
{

 Serial.print("Received data: ");
 Serial.println(howMany);

 if(howMany == 0)
  return;

 byte command = Wire.read();

 Serial.print("Received command: ");
 Serial.println(command);

 if(command == 2)
 {
  difficulty = Wire.read();

  Serial.print("Setting difficulty to: ");
  Serial.println(difficulty);

  if(armed)
   arm();
 }
 else if(command == 1)
 {
  Serial.println("Arming");
  arm();
 }
}
