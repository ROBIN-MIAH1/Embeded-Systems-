#define RED 12
#define YELLOW 8
#define GREEN 5

int state = 0;

void setup()
  {
    pinMode(RED, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(GREEN, OUTPUT);
  }
    
void loop() {
digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);

// Turn all lights off digitalWrite (RED, LOW) digitalWrite (YELLOW, LOW): digitalWrite (GREEN, LOW);

switch (state) {

case 0: // Red

digitalWrite (RED, HIGH);

delay (5000);
  state = 1;
break;
  case 1: // Red


  digitalWrite (YELLOW, HIGH);

delay (5000);
  state = 2;
break;


case 2: // Green

digitalWrite (GREEN, HIGH);

delay(5000);
  state = 4;
  break;
  case 3: // Yellow

digitalWrite (YELLOW, HIGH);

delay(5000);
  state=4;
  break;
    case 4: // Yellow

digitalWrite (YELLOW, HIGH);

delay(5000);
  state=0;
  break;
}
}