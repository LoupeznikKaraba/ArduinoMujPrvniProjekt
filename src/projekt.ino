#define MY_DEBUG
#define MY_GATEWAY_SERIAL
//#define MY_RADIO_NRF24

#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

#define MY_REPEATER_FEATURE
#define MY_NODE_ID 1
#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>
#include <Button.h>
#include <Arduino.h>

#define RELAY_PIN  7
#define BUTTON_PIN  2
#define BUTTON2_PIN 5
#define CHILD_ID 1
#define RELAY_ON 1
#define RELAY_OFF 0
                           //Connect a tactile button switch (or something similar)
                           //from Arduino pin 2 to ground.
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
                           //switch is closed, this is negative logic, i.e. a high state
                           //means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.
#define LED_PIN 13         //The standard Arduino "Pin 13" LED

Button button1(BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button button2(BUTTON2_PIN, PULLUP, INVERT, DEBOUNCE_MS);
bool x1 = false;
//Bounce debouncer = Bounce();
bool state = false;
bool initialValueSent = false;

MyMessage msg(CHILD_ID, V_STATUS);

void setup()
{
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  //debouncer.attach(BUTTON_PIN);
  //debouncer.interval(10);

  // Make sure relays are off when starting up
  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);
}

void presentation()  {
  sendSketchInfo("Relay+button", "1.0");
  present(CHILD_ID, S_BINARY);
}

void loop()
{
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    send(msg.set(state?RELAY_ON:RELAY_OFF));
    Serial.println("Requesting initial value from controller");
    request(CHILD_ID, V_STATUS);
    wait(2000, C_SET, V_STATUS);
  }
    button1.read();                    //Read the button
    if (button1.wasReleased()) {
    Serial.println("LOG: Press");
    if (x1){
      if(digitalRead(RELAY_PIN)){
        digitalWrite(RELAY_PIN, LOW);
        x1 = false;
        send(msg.set(RELAY_OFF), true); //novy radek
        }
      }else{
        x1 = true;
        }
    }
    button2.read();                    //Read the button
    if (button2.wasReleased()) {
    if(x1 == true){
      state = !state;
      // Send new state and request ack back
      send(msg.set(state?RELAY_ON:RELAY_OFF), true);
      digitalWrite(RELAY_PIN, state);
      }else{
        x1 = false;
        }
    }
  
  /**if (debouncer.update()) {
    if (debouncer.read()==LOW) {
      state = !state;
      // Send new state and request ack back
      send(msg.set(state?RELAY_ON:RELAY_OFF), true);
      digitalWrite(RELAY_PIN, state);
    }
  }**/
}

void receive(const MyMessage &message) {
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
  }

  if (message.type == V_STATUS) {
    if (!initialValueSent) {
      Serial.println("Receiving initial value from controller");
      initialValueSent = true;
    }
    // Change relay state
    state = (bool)message.getInt();
    digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
    send(msg.set(state?RELAY_ON:RELAY_OFF));
  }
}