#include <RotaryEncoder.h>
#define encoderPinA 35  // Rorary Pin A
#define encoderPinB 32  // Rorary Pin B
#define encoderPinC 34  //Rotary - Taster

RotaryEncoder encoder (encoderPinB, encoderPinA, RotaryEncoder::LatchMode::FOUR0);

hw_timer_t * timer = NULL;// Hardware Timer registrieren

volatile int myEncoderPos = 0;
volatile int myEncoderPos_Old = 0;
volatile bool marker = false;
bool Timer_marker = false;


void Encoder_initialisieren()
{
  pinMode (encoderPinA, INPUT);
  pinMode (encoderPinB, INPUT);
  pinMode (encoderPinC, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinC), checkButton, CHANGE);
}

//----------------------------------------- Timer für langen Click von 2 Sekunden
void timer_initialisieren()
{
  timer = timerBegin(0, 80, true);              // Timer_0, Prescaler 80, neu starten bei Überlauf
  timerAttachInterrupt(timer, &onTimer, true);  // Timer Interrupt registrieren
  timerAlarmWrite(timer, 2000000, false);       // Timer auf 2 Sekunden stellen
}

void Timer_off()
{
  timerAlarmDisable(timer);
  timerWrite(timer, 0);
}

void Timer_on()
{
  timerWrite(timer, 0);
  timerAlarmEnable(timer);  
}

void IRAM_ATTR onTimer()
{                                         
  Timer_off();
  focus = 'A';
  Timer_marker = true;
  last_Button_count = Button_count ;  // Das Signal vom Timer nicht zusätzlich später noch als Click werten
  Last_click = millis();
}

//---------------------------------------------- RotaryEncoder  -----------------------------

void ICACHE_RAM_ATTR checkPosition()
{
  static long TickOld;
  long Tick;
    
  encoder.tick();
  Tick = encoder.getPosition();
  if (Tick > TickOld) myEncoderPos++;
  if (Tick < TickOld) myEncoderPos--;
  
  TickOld = Tick;  
}

void reset_Encoder(int pos)
{
  myEncoderPos = pos;
  encoder.setPosition(pos);
}

volatile int get_Encoder_pos(int min, int max, bool ueberlauf)
{
  if (myEncoderPos > max) if (ueberlauf) myEncoderPos = min;  else myEncoderPos = max;
  if (myEncoderPos < min) if (ueberlauf) myEncoderPos = max;  else myEncoderPos = min;
  return myEncoderPos;
}

void IRAM_ATTR  checkButton()  //------------------    Button auslesen  --------------------
{
 ButtonState = !digitalRead(encoderPinC);
 if (Timer_marker == false)
  {
     Last_click = millis();
    if (ButtonState == HIGH) Timer_on();
    else{
      Timer_off();
      Button_count++;
   }
  }
  Timer_marker = false;
 }


  
  
