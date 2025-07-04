

#define LED1 27
#define LED2 14
#define LED3 13
#define Zeit 300

bool LED1_set;
bool LED2_set;
bool LED3_set;
byte beat = 1;

void setup_Ampel() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
//  LED_Test();   // Alle LED einmal duchschalten
  digitalWrite(LED1, HIGH);  
    buzzer.tone(NOTE_C4, 250);
  digitalWrite(LED1, LOW);    
  digitalWrite(LED2, HIGH);   
    buzzer.tone(NOTE_E4, 250);
  digitalWrite(LED2, LOW);    
  digitalWrite(LED3, HIGH); 
    buzzer.tone(NOTE_G4, 1000);
  digitalWrite(LED3, LOW);    
  

}

void blink_LED(byte n, byte z)
{
  for (int i = 0 ; i < n ; i++)
  {
    if (n == 1)
    {
    digitalWrite(LED1, HIGH); 
    delay(400);
    digitalWrite(LED1, LOW); 
    delay(400);
    }

    if (n == 2)
    {
    digitalWrite(LED2, HIGH); 
    delay(400);
    digitalWrite(LED2, LOW); 
    delay(400);
    }

    if (n == 3)
    {
    digitalWrite(LED3, HIGH); 
    delay(400);
    digitalWrite(LED3, LOW); 
    delay(400);
    }
  }
}

void set_Ampel_1(byte color)
{
  switch(color){
        case 0:
      digitalWrite(LED1, LOW); 
      digitalWrite(LED2, LOW); 
      digitalWrite(LED3, LOW); 
    break;
    case 1:
      digitalWrite(LED1, HIGH); 
      digitalWrite(LED2, LOW); 
      digitalWrite(LED3, LOW); 
    break;
    case 2:
      digitalWrite(LED1, LOW); 
      digitalWrite(LED2, HIGH); 
      digitalWrite(LED3, LOW); 
    break;
    case 3:
      digitalWrite(LED1, LOW); 
      digitalWrite(LED2, LOW); 
      digitalWrite(LED3, HIGH); 
    break;
    case 4:
      digitalWrite(LED1, HIGH); 
      digitalWrite(LED2, HIGH); 
      digitalWrite(LED3, HIGH); 
    break;
  }
}

void set_Ampel_2(byte color, byte Helligkeit)
{
  switch(color){
        case 0:
      analogWrite(LED1, 0); 
      analogWrite(LED2, 0); 
      analogWrite(LED3, 0); 
    break;
    case 1:
      analogWrite(LED1, Helligkeit); 
      analogWrite(LED2, 0); 
      analogWrite(LED3, 0); 
    break;
    case 2:
      analogWrite(LED1, 0); 
      analogWrite(LED2, Helligkeit); 
      analogWrite(LED3, 0); 
    break;
    case 3:
      analogWrite(LED1, 0); 
      analogWrite(LED2, 0); 
      analogWrite(LED3, Helligkeit); 
    break;
    case 4:
      analogWrite(LED1, Helligkeit); 
      analogWrite(LED2, Helligkeit); 
      analogWrite(LED3, Helligkeit); 
    break;
  }
}

void Heardbeat()
{
  beat ++;
  if (beat > 3 )beat = 1;
  switch (beat){
    case 1:
      analogWrite(LED1, 30);
      analogWrite(LED2, 0);
      analogWrite(LED3, 0);
    break;
    case 2:
      analogWrite(LED1, 0);
      analogWrite(LED2, 30);
      analogWrite(LED3, 0);
    break;
    case 3:
      analogWrite(LED1, 0);
      analogWrite(LED2, 0);
      analogWrite(LED3, 30);
    break;  
  }
}

void LED_Test() {
  digitalWrite(LED1, HIGH);  
  delay(Zeit);                    
  digitalWrite(LED1, LOW);    
  digitalWrite(LED2, HIGH);   
  delay(Zeit);                     
  digitalWrite(LED2, LOW);    
  digitalWrite(LED3, HIGH); 
  delay(Zeit);                
  digitalWrite(LED3, LOW);    
  digitalWrite(LED2, HIGH);   
  delay(Zeit);               
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, HIGH); 
  delay(Zeit);
  digitalWrite(LED1, LOW);   
}

 void Set_LED1()
 {
  LED1_set = !LED1_set; 
  digitalWrite(LED1 , LED1_set);
 }

 void Set_LED2()
 {
  LED2_set = !LED2_set; 
  digitalWrite(LED2 , LED2_set);
 }

 void Set_LED3()
 {
  LED3_set = !LED3_set; 
  digitalWrite(LED3 , LED3_set);
 }
