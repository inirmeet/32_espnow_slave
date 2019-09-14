//Libs for espnow e wifi
#include <esp_now.h>
#include <WiFi.h>
 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Gpios we'll write the values received from the Master
//It's important that the Master source code has this same array
//with the same gpios in the same order
uint8_t gpios[] = {33,35,32,34};

//In the setup function we'll calculate the gpio count and put in this variable,
//so we don't need to change this variable everytime we change
//the gpios array total size, everything will be calculated automatically
//on setup function
int gpioCount;

boolean dataRec;
void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED initialize failed");
  }  
  delay(100);
  display.clearDisplay();
  display.display();
  //Calculation of gpio array size:
  //sizeof(gpios) returns how many bytes "gpios" array points to.
  //Elements in this array are of type uint8_t.
  //sizeof(uint8_t) return how many bytes uint8_t type has.
  //Therefore if we want to know how many gpios there are,
  //we divide the total byte count of the array by how many bytes
  //each element has.
  gpioCount = sizeof(gpios)/sizeof(uint8_t);

  //Puts ESP in STATION MODE
  WiFi.mode(WIFI_STA);

  //Shows on the Serial Monitor the STATION MODE Mac Address of this ESP
  Serial.print("Mac Address in Station: "); 
  Serial.println(WiFi.macAddress());

  //Calls the function that will initialize the ESP-NOW protocol
  InitESPNow();
  dataRec = false;
  //Registers the callback function that will be executed when 
  //this Slave receives data from the Master.
  //The function in this case is called OnDataRecv
  esp_now_register_recv_cb(OnDataRecv);

  //For each gpio on gpios array
//   for(int i=0; i<gpioCount; i++){
//    //We put in read mode
//    pinMode(gpios[i], OUTPUT);
//  }
}


  void InitESPNow() {
  //If the initialization was successful
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
    }
  //If there was an error
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

//Callback function that tells us when data from Master is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  display.clearDisplay();
  display.print("");
  display.display();
  dataRec = true;
  char macStr[18];
  //Copies the sender Mac Address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Prints it on Serial Monitor
  Serial.print("Received from: "); 
  Serial.println(macStr);
  Serial.println("");

  
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(macStr);
  //For each gpio
  for(int i=0; i<gpioCount; i++){

    //Sets its output to match the received value
    // digitalWrite(gpios[i], data[i]);
    Serial.println(data[i]);
    display.print(data[i]);
    display.print(" ");
  }
  display.display();
}

//We don't do anything on the loop.
//Everytime something comes from Master
//the OnDataRecv function is executed automatically
//because we added it as callback using esp_now_register_recv_cb
void loop() 
{
  if(dataRec == true)
  {
    dataRec = false;
  }
  else
  {
    display.clearDisplay();
    display.display();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("Data Not received!!"));
    display.display();
  }
  delay(1000);
}
