static uint8_t ip[] =           { 10,4,126,163 };

static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Sodaq_DS3231.h>
//#include <Ultrasonic.h>

String newPasswordString;
char newPassword[6];

Servo myservo;

String ledStatus[7];

int led1 = 3;
int led2 = 4;
int led3 = 5;
int led4 = 6;
int led5 = 7;
int led6 = 13;
int servo_pin = 13;

// Global Variables
uint8_t released_position =   90;          
uint8_t pressed_position =    0;           
uint8_t arm_speed =           0;          
uint16_t pressed_time =       3000;

// defines pins numbers
const int trigPin = 8;
const int echoPin = 9;

// defines variables
//Ultrasonic ultrasonic(trigPin, echoPin);

long duration;
long cm;
int distance;

bool statusPintu;
//int safetyDistance;

WebServer webserver("", 80);

long scanner(long cm);
char bufferRequest[64];

void setup() {
  //digitalWrite(solenoid, HIGH);
  myservo.write(released_position);
  delay(1000);
  myservo.detach();
  Serial.begin(9600);
  Serial.write(254);
  Serial.write(0x01);
  delay(200);
  
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
  pinMode(led2, OUTPUT);
  digitalWrite(led2, LOW);
  pinMode(led3, OUTPUT);
  digitalWrite(led3, LOW);
  pinMode(led4, OUTPUT);
  digitalWrite(led4, LOW);
  pinMode(led5, OUTPUT);
  digitalWrite(led5, LOW);
  pinMode(led6, OUTPUT);
  digitalWrite(led6, LOW);

  pinMode(servo_pin, OUTPUT);
  digitalWrite(servo_pin, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  Serial.println(F("inisialisasi..."));

  /* initialize the Ethernet adapter */
  Ethernet.begin(mac, ip);
  webserver.on("switch", &switchReq);
  webserver.on("status", &statusSwitch);

  /* start the webserver */
  webserver.begin();

  Serial.print(F("alamat perangkat ini : "));
  Serial.println(Ethernet.localIP());

  Wire.begin();
  rtc.begin();

}

void loop()
{
  if(statusPintu){
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);  

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(2);  // Only 2uS NOT 10!
    digitalWrite(trigPin, LOW); 
    duration = pulseIn(echoPin, HIGH);
    delay(100);

    cm = microsecondsToCentimeters(duration);
    Serial.print("cm: ");
    Serial.println(cm);

    if(cm > 10){
      myservo.write(0);
      digitalWrite(servo_pin, HIGH);
      delay(100);
      digitalWrite(servo_pin, LOW);
    }
  
    return (cm);
  }
  //sensor();  
  myservo.write(0);

  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);

   if(digitalRead(led1) == LOW){
    ledStatus[1]="Off";
  }
 else if(digitalRead(led1) == HIGH){
    ledStatus[1]="On";
  }
   if(digitalRead(led2) == LOW){
    ledStatus[2]="Off";
  }
 else if(digitalRead(led2) == HIGH){
    ledStatus[2]="On";
  }
   if(digitalRead(led3) == LOW){
    ledStatus[3]="Off";
  }
 else if(digitalRead(led3) == HIGH){
    ledStatus[3]="On";
  }
   if(digitalRead(led4) == LOW){
    ledStatus[4]="Off";
  }
 else if(digitalRead(led4) == HIGH){
    ledStatus[4]="On";
  } 
  if(digitalRead(led5) == LOW){
    ledStatus[5]="Off";
  }
 else if(digitalRead(led5) == HIGH){
    ledStatus[5]="On";
  }
 if(digitalRead(servo_pin) == LOW){
  ledStatus[6]="off"; 
  statusPintu = false; 
 }
 else if(digitalRead(servo_pin) == HIGH){
  ledStatus[6]="On";
  statusPintu = true;
  //sensor();
 }
  
}

void switchReq(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[32];
  char value[32];
  int id = -99; //!

  server.httpSuccess();

  while (strlen(url_tail))
  {
    rc = server.nextURLparam(&url_tail, name, 32, value, 32);
    if (rc != URLPARAM_EOS)
    {
      
      if(id !=-99)
      {
        if (strcmp(name, "action") == 0)
        {
          Serial.print(F("action="));
          Serial.println(value);

          if(strcmp(value, "0") == 0)
          {
            server.print(F("OFF"));
  
            Serial.println(F("OFF"));
            toggleOff(id);
            /*if(id==6){
              statusPintu = false;
            }*/
          }
          else if(strcmp(value, "1") == 0)
          {
            server.print(F("ON"));
            
            Serial.println(F("ON"));
            toggleOn(id);
            /*if (id==6){
              statusPintu = true;
            }*/
          }
        }
      }
      if (strcmp(name, "id") == 0)
      {
        id = atoi(value);
        Serial.print(F("id="));
        Serial.println(id);
      }

      
    }
  }
}

void toggleOn (int id){
  int action = 0;
  Serial.print("turning on led number ");
  Serial.println(id);
  if(id==1){
    digitalWrite(led1, HIGH);
    delay(1000);
  }
  else if(id==2){
    digitalWrite(led2, HIGH);
    delay(1000);
  }
  else if(id==3){
    digitalWrite(led3, HIGH);
    delay(1000);
  }
  else if(id==4){
    digitalWrite(led4, HIGH);
    delay(1000);
  }
    else if(id==5){
    digitalWrite(led5, HIGH);
    delay(1000);
  }
  else if(id==6){
    //statusPintu = true;
    myservo.write(90);
    servoOn();
    delay(1000);
    digitalWrite(servo_pin, HIGH);
    
  }
}
void toggleOff (int id){
  Serial.print("turning off led number ");
  Serial.println(id);
  if(id==1){
    digitalWrite(led1, LOW);
    delay(1000);
  }
  else if(id==2){
    digitalWrite(led2, LOW);
    delay(1000);
  }
  else if(id==3){
    digitalWrite(led3, LOW);
    delay(1000);
  }
  else if(id==4){
    digitalWrite(led4, LOW);
    delay(1000);
  }
  else if(id==5){
    digitalWrite(led5, LOW);
    delay(1000);
  }
  else if(id==6){
    //statusPintu = false;
    //myservo.write(0);
    servoOff();
    delay(1000);
    digitalWrite(servo_pin, LOW);
  }
}

void servoOn(){
  myservo.attach(servo_pin);
  
  // Slowly move the arm so it doesn't slip off the button
  for(uint8_t i = released_position; i > pressed_position; i--){    
    myservo.write(i);
    delay(arm_speed); 
    //digitalWrite(led, HIGH);                     
  }
  delay(pressed_time);                 
  myservo.write(pressed_position);
      
  delay(500);                             
  
  myservo.detach();                       
}
void servoOff(){
  myservo.attach(servo_pin);
  
  // Slowly move the arm so it doesn't slip off the button
  for(uint8_t i = pressed_position; i > released_position; i++){    
    myservo.write(i);
    delay(arm_speed); 
    //digitalWrite(led, HIGH);                     
  }
  delay(pressed_time);                 
  myservo.write(released_position);
      
  delay(500);                             
  
  myservo.detach();                       
}

void statusSwitch(){
 //webserver.println("Content-Type:application/json");
 webserver.println(prepareResponse());
}
String prepareResponse(){
  String res = "{\"data\":[";

  for(int i=1;i<=6;i++){
    res += "{\"id\": "; 
    res += String(i); 
    res += ", \"status\":\"";
    res += ledStatus[i];
    res += "\"}";
    if (i!=6){
      res += ",";
    }
  }
    
  res += "]}";
  return res;
}

void sensor(){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(2);  // Only 2uS NOT 10!
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH);
  delay(100);

  cm = microsecondsToCentimeters(duration);
  Serial.print("cm: ");
  Serial.println(cm);

  if(cm > 10){
    myservo.write(0);
    digitalWrite(servo_pin, HIGH);
    delay(100);
    digitalWrite(servo_pin, LOW);
  }
  
  return (cm);
}
long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

/*void sensor(){
    distance = ultrasonic.Ranging(CM);
  
 Serial.print("Distance in CM: ");
 /*if(distance <=10){
  myservo.write(0);
    servoOff();
    delay(1000);
    digitalWrite(servo_pin, LOW);
 }*/
 /*Serial.println(distance);
 delay(1000);
}*/
