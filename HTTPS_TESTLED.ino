//Libraries
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>


//Custom Modules


//----------------------------Webpage root certification to HTTPS---------------------------------------------------------
const char IRG_Root_X1 [] PROGMEM = R"CERT(       
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)CERT";




X509List cert(IRG_Root_X1); // Creating lis of certificates with the server root certificate



//--------------------------------API key to prevent unauthorized access---------------------------------------------------------
String API="this_is_your_secure_key";

String site ="https://nodemcustatus.jandzsogyorgy.hu";
String load = "/testLED/db_read.php";
String update = "/testLED/db_update.php";


//----------------------------WIFI credentials---------------------------------------------------------
const char* ssid = "Wifi_SSID_name";
const char* password = "this_is_your_secure_wifi_pass";



int ticks=0;        //"Count" the arduino loops, as using delay is a REALLY bad solution



int WIFILED = 2;    //Default Wifi LED on ESP8266
int BUTTON = D1;    //Button connected to D1
int LED = D7;       //External LED connected to D7
int id = 1;         //External LED's ID in Database



int lastOnlineState=0; //DEFAULT VALUE, LED will be off until first data sync







void setup() 
{
  //--------------------------------  SERIAL CONNECTION -------------------------------------------
  Serial.begin(115200);   //Console baudrate
  Serial.println();
  Serial.println();
  Serial.println();

  //--------------------------------PIN MODES -------------------------------------------
  pinMode(LED_BUILTIN, OUTPUT);   //Pin's type
  pinMode(WIFILED, OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(BUTTON, INPUT);



  //---------------------------------------WIFI -------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) 
    {
      Serial.print('.');
      blinkLed(WIFILED,1000);  //Blink while connecting to wifi
    }

  //--------------------------------  NTP TIME SYNC TO IMPLEMENT HTTPS -------------------------------------------
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.println("\nWaiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) 
    {
      delay(500);
      Serial.print(".");
      now = time(nullptr);
    }

  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));



//--------------------------------  STARTUP -------------------------------------------
  loadState();    //Load LED's online state for the first time
}





void loop() 
{
  ticks++;

  if (ticks>900000)   //Read from database every 10 seconds (+-1 sec)
    {
      loadState();
    }



  if (digitalRead(BUTTON)==HIGH) //Read button's state
    {
      updateState();
    }

  
  controllLedState();



}







//----------------------------------------------------------------------------------FUNCTONS---------------------------------------------------------------------------------------



int loadState()
  {
    if ((WiFi.status() == WL_CONNECTED))  //Wait for WiFi connection
      {
        digitalWrite(WIFILED, LOW);       //Turn ON LED during I/O actions

        WiFiClientSecure client;
        client.setTrustAnchors(&cert);
        HTTPClient https;

        if (https.begin(client, site + load)) 
          { 
            Serial.print("\n[HTTPS] GET...\n");
            https.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
            int httpCodeGet = https.POST("API=" + API +"&ID=" + String(id)); //Send the request
            String payloadGet = https.getString();        //Get the response payload from server
            Serial.print("Response Code : ");             //If Response Code = 200 means Successful connection.
            Serial.println(httpCodeGet);                  //Print HTTP return code
            Serial.print("Returned data from Server : ");
            Serial.println(payloadGet);                   //Print request response payload
            https.end();
            lastOnlineState = payloadGet.toInt();
            digitalWrite(WIFILED, HIGH);                  //Turn OFF LED after I/O actions
            ticks=0;                                      //Reset counter
            return lastOnlineState;
          } 
        else 
          {
            Serial.printf("[HTTPS] Unable to connect\n");
          }
      }
    return -1;
  }





void updateState()
  {
    loadState();
    if ((WiFi.status() == WL_CONNECTED))  //Wait for WiFi connection
      {
          digitalWrite(WIFILED, LOW);   //Turn ON LED during I/O actions

          WiFiClientSecure client;
          client.setTrustAnchors(&cert);
          HTTPClient https;
          

          int newState;
          if (lastOnlineState==0)
            {
              newState = 1;
            }
          else 
              newState = 0;


          if (https.begin(client, site + update)) // HTTPS
            { 
              Serial.print("\n[HTTPS] POST...\n");
              https.addHeader("Content-Type", "application/x-www-form-urlencoded"); //  Specify content-type header
              int httpCodeSend = https.POST("API=" + API +"&Status=" + newState); //  Send the request
              String payloadSend = https.getString(); //  Get the response payload
              Serial.print("Response Code : "); //  If Response Code = 200 means Successful connection, if -1 means connection failed
              Serial.println(httpCodeSend); //  Print HTTP return code
              https.end();
              lastOnlineState = newState;
            } 
          else 
            {
              Serial.printf("[HTTPS] Unable to connect\n");
            }

          controllLedState();
          digitalWrite(WIFILED, HIGH);  //Turn OFF LED after I/O actions
      }
  }



void blinkLed(int LED,int delaytime)   //Delay in ms
{
  digitalWrite(LED, LOW);
  delay(delaytime);
  digitalWrite(LED, HIGH);
  delay(delaytime);
  digitalWrite(LED, LOW);
}


void controllLedState()
{
  if (lastOnlineState == 1)
    {
      digitalWrite(LED, HIGH); //Turn off Led
    }
  else 
    {
      digitalWrite(LED, LOW); //Turn off Led
    }  
}




