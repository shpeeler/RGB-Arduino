#include <ESP8266WiFi.h>

/* AP/Service */
const char* ssid = "RaspiAccessPoint";
const char* password = "*";

WiFiServer server(80);

/* RGB */
#define B_PIN 13
#define R_PIN 14
#define G_PIN 12
#define LED_Disc 5

int r = 0;
int g = 0;
int b = 0;
bool state;

void setup()
{
  Serial.begin(115200);
  
  pinMode(LED_Disc, OUTPUT);

  Off();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    NotConnected();
    delay(500);
  }

  Connected();
  
  server.begin();
  state = true;
}

void loop()
{
  /* await serial information */

  // output the current state of the esp8266 connection
  if(WiFi.status() == WL_CONNECTED)
  {
    Connected();
  }
  else
  {
    NotConnected();
  }

  // check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // check if information is available
  while(!client.available()){
    delay(1);
  }
  
  // read information
  String request = client.readStringUntil('\r');
  client.flush();

  // RGB Values
  int indexRGB = request.indexOf("RGB=");
  if(indexRGB != -1)
  {
    // parse rgb values from string
    String fullRGBString = request.substring(indexRGB);
    int indexRGBValues = fullRGBString.indexOf("=");

    String values = fullRGBString.substring(indexRGBValues + 1); 
    // |R| |G| |B|
    // xxx xxx xxx
    
    String rString = (String)values[0] + (String)values[1] + (String)values[2];
    String gString = (String)values[3] + (String)values[4] + (String)values[5];
    String bString = (String)values[6] + (String)values[7] + (String)values[8];

    int r_loc = rString.toInt();
    int g_loc = gString.toInt();
    int b_loc = bString.toInt();

    if(r_loc == 0 && g_loc == 0 && b_loc == 0)
    {
      Off();
    }
    else
    {
      r = r_loc;
      b = b_loc;
      g = g_loc;

      analogWrite(R_PIN, r);
      analogWrite(G_PIN, g);
      analogWrite(B_PIN, b);
    }
  }

  // State
  int indexState = request.indexOf("State=");
  if(indexState != -1)
  {
    String fullStateString = request.substring(indexState);
    int indexStateValue = fullStateString.indexOf("=");
    
    String value = fullStateString.substring(indexStateValue + 1);
    client.println(value);
    
    if(value[0] == 'O') // turn off the strip
    {
      Off();
    }
    if(value[0] == 'I') // turn on the strip
    {
      On();
    }
  }
}

void On()
{
  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
}

void Off()
{
  analogWrite(R_PIN, 0);
  analogWrite(G_PIN, 0);
  analogWrite(B_PIN, 0);
}

// blue when connected
void Connected()
{
  digitalWrite(LED_Disc, LOW);
}

// purple when not connected to wifi
void NotConnected()
{
  digitalWrite(LED_Disc, HIGH);
}