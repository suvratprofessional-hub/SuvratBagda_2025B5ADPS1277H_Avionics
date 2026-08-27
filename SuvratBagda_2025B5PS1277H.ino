#include <LiquidCrystal_I2C.h>
// Create an LCD controller for a 16-column, 2-row display at I2C address 0x27.
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Store the hardware pins and the current readings used by the game.
int button_old=1, button_new , button=7,redled=13,buzzer=6,photosensor=A0,light_reading,trig=9,echo=8;
// Store game state, timing values, and the duration of the sensor pulses.
int dt=50,anchor=0,count=0,storm=0,charybids=0,timetaken,dt1=10,sailing=1,wrecked=0;
// Store elapsed-time markers and the speed of sound used for distance conversion.
long timer1=0,timer2=0,speedofsound=34300;
// Convert the echo pulse duration from microseconds into centimeters.
long conversionfactor=2000000;
// Storing the calculated distance.
float distance;

void blinkRedLed()
{
  // Toggle the LED every 500 milliseconds while the storm is active.
  static unsigned long lastBlinkTime=0;
  static bool ledState=LOW;

  if(storm==1 && (wrecked==0&&anchor==0))
  {
    if(millis()-lastBlinkTime>=300)
    {
      lastBlinkTime=millis();
      ledState=!ledState;
      digitalWrite(redled,ledState);
    }
  }
  else if( wrecked==0&&anchor==1 || wrecked==1)
  {
    // Reset the LED as soon as the storm ends.
    ledState=LOW;
    digitalWrite(redled,LOW);
    lastBlinkTime=millis();
  }
}

void setup() {
  // put your setup code here, to run once:
  // Start serial communication and configure the input and output pins.
  Serial.begin(9600);
  pinMode(button,INPUT);
  digitalWrite(button,HIGH);
  pinMode(redled,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(photosensor, INPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  // Initialize the LCD and turn on its backlight.
  lcd.init();
  lcd.backlight(); 	
}

void loop() {
  // put your main code here, to run repeatedly:
  // Send an ultrasonic pulse and measure the returning echo.
  digitalWrite(trig,LOW);
  delayMicroseconds(dt1);
  digitalWrite(trig,HIGH);
  delayMicroseconds(dt);
  digitalWrite(trig, LOW);
  timetaken=pulseIn(echo,HIGH);
  distance=(speedofsound*timetaken)/conversionfactor;
  // Read the button to detect a new press and toggle the anchor state.
  button_new=digitalRead(button);
  if(button_new==0 && button_old==1 && sailing==1 && wrecked==0)
  {
    anchor=1;
    sailing=0;
  }
  else if(button_new==0 && button_old==1 && sailing==0 && wrecked==0)
  {
    anchor=0;
    sailing=1;
  }
  button_old=button_new;
  // Show the highest-priority current game state on the LCD.
  if(wrecked==1 && anchor==0 && sailing==0)
  {
    lcd.setCursor(0,0);
    lcd.print("WRECKED.        ");
  }

  else if(wrecked==0 && storm==1)
  {
    lcd.setCursor(0,0);
    lcd.print("STORM!!        ");
  }
  else if(wrecked==0 && charybids==1)
  {
    lcd.setCursor(0,0);
    lcd.print("CHARYBIDS!!         ");
  }
  else if(anchor==1 && sailing==0 && wrecked==0 )
  {
    lcd.setCursor(0,0);
    lcd.print("ANCHOR DOWN     ");

  }
  else if(sailing==1 && anchor==0 && wrecked==0)
  {
    lcd.setCursor(0,0);
    lcd.print("SAILING        ");
  }
  // Read the light level and start a storm when the surroundings are dark.
  light_reading=analogRead(photosensor);
  if(light_reading <=479 && anchor==0)
  {
    if(storm==0)
    {
      timer1=millis();
    }
    storm=1;

  }
  // Detect Charybdis when an obstacle is within 100 centimeters.
  else if(distance<=100 && anchor==0)
  {
    if(charybids==0)
    {
      timer2=millis();
    }
    charybids=1;
  }

  // Handle storm effects, recovery, and the wreck condition.
  if(storm==1 && wrecked==0)
  {
    blinkRedLed();
    if(anchor==0 && millis()-timer1<=5000 && light_reading >479)
    {
      storm=0;
      sailing=1;
      wrecked=0;
      digitalWrite(redled,LOW);
    }
    else if(anchor==0 && millis()-timer1 > 5000 )
    {
      sailing=0;
      wrecked=1;
      storm=0;
      digitalWrite(redled,LOW);
    }
    else if(anchor==1 && millis()-timer1 <=5000)
    {
      sailing=0;
      wrecked=0;
      storm=0;
      digitalWrite(redled,LOW);
    }

  }
  // Handle Charybdis effects, recovery, and the wreck condition.
  else if(charybids==1 && wrecked==0)
  {
    digitalWrite(buzzer,HIGH);

    if(anchor==1 && millis()-timer2<=5000)
    {
      charybids=0;
      sailing=0;
      wrecked=0;
      digitalWrite(buzzer,LOW);

    }
    else if(anchor==0 && millis()-timer2<=5000 && distance >100)
    {
      charybids=0;
      sailing=1;
      wrecked=0;
      digitalWrite(buzzer,LOW);
    }
    else if( anchor==0 && millis()-timer2 >5000)
    {
      sailing=0;
      wrecked=1;
      charybids=0;
      digitalWrite(buzzer, LOW);
    }
  }


}