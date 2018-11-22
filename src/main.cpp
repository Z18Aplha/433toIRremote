#include <Arduino.h>
#include <RCSwitch.h>
#include <IRremote.h>

RCSwitch myReceiver = RCSwitch();
IRsend irTransmitter; //irLed must be connected to PWM PIN 3

//receiving variables
int prev_signal = 0;
unsigned long timestamp = 0;



void setup() {
  Serial.begin(9600);
  Serial.println("device started");

  myReceiver.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  Serial.println("device ready");
}



void loop() {

  if (myReceiver.available()) {

    int signal = myReceiver.getReceivedValue();
    char signal_hex[] = "";


    Serial.println("");
    Serial.print("message received: ");
    Serial.println(signal);

    if ((!(signal == prev_signal) || millis() - timestamp > 200) && (signal > 0) && !(signal == 23040)){  //23040 indicates issues (dont know why), need to be filtered! transformation in transmitter!
      irTransmitter.sendNEC(signal, 32);
      itoa(signal, signal_hex, 16);

      Serial.print("NEC send: ");
      Serial.println(signal_hex);
    }
    else{
      Serial.println("received multiple times, wrong format or '40'");
    }
  }
  myReceiver.resetAvailable();
}
