//*******************************libraries********************************
//RFID-----------------------------
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
//NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//************************************************************************
#define SS_PIN  D2  //D2
#define RST_PIN D1  //D1
#define green D3 //Green signal
#define beep D8 //Buzzer
//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
//************************************************************************
/* Set these to your desired credentials. */
const char *ssid = "HomeAP"; // Enter Your SSID here
const char *password = "routerkiller"; // Enter Your Password here
const char* device_token  = "1fc7f0bcf657e828"; // Using Device Token to pair RFID
//************************************************************************
String URL = "http://192.168.29.94:5000/entry"; //computer IP or the server domain
String getData, Link;
//String OldCardID = "";
//unsigned long previousMillis = 0;
//************************************************************************
void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  pinMode(green,OUTPUT);
  digitalWrite(green,LOW);
  pinMode(beep,OUTPUT);
  digitalWrite(beep,LOW);
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  //---------------------------------------------
  connectToWiFi();
}
//************************************************************************
void loop() {
  //check if there's a connection to Wi-Fi or not
  if(!WiFi.isConnected()){
    connectToWiFi();   //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
//  if (millis() - previousMillis >= 15000) {
//    previousMillis = millis();
//    OldCardID="";
//  }
  delay(50);
  //---------------------------------------------
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//got to start of loop if there is no card present
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  
  //---------------------------------------------
//  Serial.println(CardID);
  SendCardID(CardID);
  delay(1000);
}
//************send the Card UID to the website*************
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID : " + Card_uid); //Print Card ID and sending process is on.
  
  if(WiFi.isConnected()){
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "/" + Card_uid + "/" + device_token; // Add the Card ID to the GET array in order to send it
    //GET methode
    Link = URL + getData;
    http.begin(Link); //initiate HTTP request   //Specify content-type header

    int i;
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

    Serial.println("sending in : "+Link);   //Print HTTP return code
    Serial.println("HTTP return code : "+httpCode);   //Print HTTP return code
//    Serial.println("Card ID : "+Card_uid);     //Print Card ID
    Serial.println("Response Message : "+payload);    //Print request response payload

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
    //  Serial.println(user_name);

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
    //  Serial.println(user_name);
        
      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      else if (payload == "Attendance Added") {
        digitalWrite(green,HIGH);   
        digitalWrite(beep,HIGH);   
        delay(1000);
        digitalWrite(green,LOW);
        digitalWrite(beep,LOW);
         
      }
      else if (payload == "Wait for some seconds") {
        for(i=1;i<=3;i++){
        digitalWrite(beep,HIGH);   
        delay(100);
        digitalWrite(beep,LOW);
        delay(100);
      }
      }
      else if (payload == "Invalid Device ID") {
        
       for(i=1;i<=5;i++){
        digitalWrite(beep,HIGH);   
        delay(100);
        digitalWrite(beep,LOW);
        delay(100);
      }
      }
      
      delay(100);
//      http.end();  //Close connection
    }
  }
}
//********************connect to the WiFi******************
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
    digitalWrite(2,LOW);
    delay(1000);
    digitalWrite(2,HIGH);
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    delay(1000);
}
//=======================================================================
