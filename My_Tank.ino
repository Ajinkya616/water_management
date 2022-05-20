#include <ESP8266WiFi.h>

#define echoPin1 D5 
#define trigPin1 D6

#define echoPin2 D7 
#define trigPin2 D8

#define RelayPin D0

const char* ssid = "Redmi9";
const char* password = "12345678";

int getdata1();
int getdata2();
 
long duration1, distance1;
long duration2, distance2; 

WiFiServer server(80);
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

int getdata1()
{
  int p;
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  return p = duration1/58.2;  
}

int getdata2()
{
  int q;
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  return q = duration2 /58.2;
}
 
void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(RelayPin, OUTPUT);

  digitalWrite(RelayPin,LOW); 
}

String header;
 
void loop()
{
    
  int tank1 = getdata1();
  int tank2 = getdata2();

  float tank1Per = 100 - (tank1 / 22.0 ) * 100;
  float tank2Per = 100 - (tank2 / 22.0 ) * 100;
  
  Serial.println(tank1);
  Serial.println(tank2);

//  if(tank1Per < 100)
//  {
//   if(tank2Per > 10)
//    {
//        digitalWrite(RelayPin, HIGH);  
//    }  
//  }

  WiFiClient client = server.available();   

  if (client) {                             
      Serial.println("New Client.");          
      String currentLine = "";                
      currentTime = millis();
      previousTime = currentTime;
      while (client.connected() && currentTime - previousTime <= timeoutTime) { 
        currentTime = millis();         
        if (client.available()) {             
          char c = client.read();             
          Serial.write(c);                    
          header += c;
          if (c == '\n') {                    
          
            if (currentLine.length() == 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
 
              client.println("<meta http-equiv=\"refresh\" content=\"10\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #77878A;}</style></head>");
              
              client.println("<body><h1>My Tank Server</h1>");
              String tank1Label = "tank1: " + String(tank1Per) + " % filled";
              client.println(tank1Label);
              String tank2Label = "tank2: " + String(tank2Per) + " % filled";
              client.println("<br />");
              client.println(tank2Label);
              
              client.println("</body></html>");
              
              client.println();
              break;
            } else {
              currentLine = "";
            }
          } else if (c != '\r') { 
            currentLine += c;  
          }
        }
      }
      header = "";
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
    delay(700);
}
