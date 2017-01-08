#include <RFM69.h>
#include <SPI.h>
#include <avr/interrupt.h>
#include <LowPower.h>

// Youtube video demo
// https://www.youtube.com/watch?v=kO-kUYALz_I

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      2   // My node ID
#define TONODEID      1   // Destination node ID

// RFM69 frequency settings
#define FREQUENCY   RF69_433MHZ

// AES encryption 
#define ENCRYPT       true 
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Packet sent/received indicator LED (optional):
#define LED_PIN           6 // LED positive pin
#define GND           8 // LED ground pin
//This is your 
int doorSensorPin = 3; 
RFM69 radio;

typedef struct {
  int           trigger;   //0 = closed  1 = open
} Payload;
Payload theData;

void setup()
{
  Serial.begin(9600);
  Serial.println("STARTUP");
  // Set up the indicator LED (optional):
  pinMode(13, OUTPUT);

  // Initialize the RFM69:
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  //radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);

  pinMode(doorSensorPin, INPUT_PULLUP);  
  
}

void loop()
{

  int proximity = digitalRead(doorSensorPin); // Read the state of the switch
  if (proximity == LOW) // If the pin reads low, the switch is closed.
  {
    Serial.println("Switch closed");
    theData.trigger = 0;
    sendData();
  }
  else
  {
    Serial.println("Switch OPEN");
    digitalWrite(LED_PIN, HIGH); // Turn the LED off
    theData.trigger = 1;
    sendData();
    delay(200);
  }
  digitalWrite(LED_PIN, LOW); // Turn the LED off
  delay(100);
   sleepNow();
      
}

void sleepNow()
{
    Serial.println("Sleepy Now");
    Serial.flush();
   
    //pin interrupt is now attached
    attachInterrupt(digitalPinToInterrupt(doorSensorPin), pinInterrupt, CHANGE);

    //allow a moment for interrupt to attach
    delay(100);
    //put the radio down for sleep
    radio.sleep();
    delay(100);
    //now go to sleep
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);    
    
    
}

void pinInterrupt(void)
{
   //remove when coming back alive
    detachInterrupt(doorSensorPin);
   
}

void sendData(){
 
      
      Serial.print("Sending to node ");
      Serial.println(TONODEID, DEC);
      //Sending data to the gateway device
      if (radio.sendWithRetry(TONODEID, (const void*)(&theData), sizeof(theData)))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");
      
      
  
}


