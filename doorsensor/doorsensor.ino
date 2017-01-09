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

int sleepTimeDelay = 37; //  37 * 8 seconds = 296 seconds or almost 5 minute delay
int doorSensorPin = 3; //This is your trigger pin for magnetic sensor

//DO NOT CHANGE THE BELOW DEFAULT VALUE!
int sleepMode=0; // this is type of sleep mode 0 = forever  OR  1 = 8seconds 

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
  radio.setHighPower(); // Always use this for RFM69HCW

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
  
      //Choosing not to send when closed to save battery life
      //sendData();
    }
    else
    {
      Serial.println("Switch OPEN");
      digitalWrite(LED_PIN, HIGH); // Turn the LED off
      theData.trigger = 1;
      sendData();
      //sleep for X minutes
      for(int a=0; a < sleepTimeDelay; a++){ 
        Serial.print("Delay: ");
        Serial.println(a);
        sleepNow(1);
      }
      
    }
    digitalWrite(LED_PIN, LOW); // Turn the LED off
    delay(100);

    //sleep forever
    sleepNow(0);
      
}

void sleepNow(int sleepMode)
{
    Serial.print("Sleepy Now Mode:");
    Serial.println(sleepMode);
    Serial.flush();
    
    //put the radio down for sleep
    radio.sleep();
    
    //now go to sleep
    if(sleepMode == 0){
      attachInterrupt(digitalPinToInterrupt(doorSensorPin), pinInterrupt, CHANGE);
      //allow a moment for interrupt to attach
      delay(100);
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);    
    }else{
      detachInterrupt(digitalPinToInterrupt(doorSensorPin));
      //sleep only 8 seconds
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
    }
    
    
}

void pinInterrupt(void)
{
  
   
}

void sendData(){
 
      
      Serial.print("Sending to node ");
      Serial.println(TONODEID, DEC);
      //Sending data to the gateway device
      if (radio.sendWithRetry(TONODEID, (const void*)(&theData), sizeof(theData)))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");

      //give a small wait for wireless to finish
      delay(200);
  
}


