#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

/* RGB */
#define B_PIN 3 
#define R_PIN 5
#define G_PIN 6

String s = "";
String rString = "";
String gString = "";
String bString = "";
bool state;

/* LCD display */
#define I2C_ADDR          0x27        
#define BACKLIGHT_PIN      3
#define En_pin             2
#define Rw_pin             1
#define Rs_pin             0
#define D4_pin             4
#define D5_pin             5
#define D6_pin             6
#define D7_pin             7

LiquidCrystal_I2C      lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void setup()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  state = true;
  DrawDisplay();
}

void loop()
{
  /* await serial information */
  while(Serial.available() > 0)
  {
    char c = Serial.read();
    s += c;

    /* this path contains is executed when the EOS is found */ 
    if(c == '\n')
    {
      /* this path contains the standby logic */
      if(c == 'O')
      {
        analogWrite(R_PIN, 0);
        analogWrite(G_PIN, 0);
        analogWrite(G_PIN, 0);
        lcd.setBacklight(LOW);
        state = false;
      }

      /* this path contains the power up logic */ 
      if(c == 'I')
      {
        analogWrite(R_PIN, rString.toInt());
        analogWrite(G_PIN, gString.toInt());
        analogWrite(B_PIN, bString.toInt());
        lcd.setBacklight(HIGH);
        state = true;
      }

      /* length of 10 indicates, that a full set of rgb information was received */
      if(s.length() == 10)
      {
        rString = (String)s[0] + (String)s[1] + (String)s[2];
        gString = (String)s[3] + (String)s[4] + (String)s[5];
        bString = (String)s[6] + (String)s[7] + (String)s[8];

        analogWrite(R_PIN, rString.toInt());
        analogWrite(G_PIN, gString.toInt());
        analogWrite(B_PIN, bString.toInt());
      }

      /* reset string and redraw the display */ 
      s = "";

      DrawDisplay();
    }
  }
}

void DrawDisplay()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("R: " + rString +  " G: " + gString);
  lcd.setCursor(0,1);
  lcd.print("B: " + bString);
}

String GetState()
{
  if(state == true)
  {
    return "On";
  }

  return "Off";
}