#include <Arduino.h>
#include <RCSwitch.h>
#include <IRremote.h>

RCSwitch myReceiver = RCSwitch();
IRsend irTransmitter; //irLed must be connected to PWM PIN D3

//receiving variables
unsigned long prev_signal = 0;
unsigned long timestamp = 0;
unsigned long adress = 117440512;
unsigned long onkyo_internal_offset = 1258291200;
unsigned long LED_internal_offset = 16711680;



void setup() {
  Serial.begin(9600);
  Serial.println("device started");

  myReceiver.enableReceive(0);  // Receiver on interrupt 0 => that is pin D2

  Serial.println("device ready");
}



void loop() {

  if (myReceiver.available()) {

    unsigned long signal = myReceiver.getReceivedValue();
    unsigned long code = signal - adress;
    bool send = true;
    String code_hex = String(code, HEX);


    Serial.println("");
    Serial.print("message received: ");
    Serial.println(signal);
    Serial.print("code extracted: ");
    Serial.print(code);
    Serial.print(" (0x");
    Serial.print(code_hex);
    Serial.println(")");




    if (signal == prev_signal) {
      Serial.print("multiple received signal");
      if (millis() - timestamp < 500) {
        Serial.println(" - not send");
        send = false;
      }
      else {
        Serial.println(" - but enough time between them");
      }
    }

    if (signal == 23040) {
      Serial.println("received 23040, frequently send error - not send");
      send = false;
    }

    if (signal == 0) {
      Serial.println("no signal - not send");
      send = false;
    }

    if (code == 1261830144) {
      Serial.println("frequently error signal - not send");
      send = false;
    }

    if (((code - LED_internal_offset == 0) || (code - onkyo_internal_offset == 0))) {
      Serial.println("signal lost - not send");
      send = false;
    }

    if (!(code_hex.startsWith("4b") || code_hex.startsWith("ff"))) {
      Serial.println("signal is not a type of Onkyo or LED - not send");
      send = false;
    }

    if (send) {
      timestamp = millis()+100;
      prev_signal = signal;

      irTransmitter.sendNEC(code, 32);

      Serial.print("NEC send: ");
      Serial.println(code_hex);

    }
    else {
      send = true;
    }






/*

    if ((!(signal == prev_signal) || millis() - timestamp > 200) && !((code - LED_internal_offset == 0) || (code - onkyo_internal_offset == 0)) && (code > 0) && !(code == 1261830144) && ((code_hex.indexOf("4b") == 0) || (code_hex.indexOf("ff") == 0)) && !(signal == 23040)){  //23040 indicates issues (dont know why), need to be filtered! transformation in transmitter!
      timestamp = millis();
      prev_signal = signal;
      Serial.println("inloop");
      irTransmitter.sendNEC(signal, 32);
      Serial.println("send done");

      Serial.print("NEC send: ");
      Serial.println(code_hex);
    }
    else{
      timestamp = millis();
      prev_signal = signal;
      Serial.println("received multiple times, wrong format or '40'");
    }
    */

    myReceiver.resetAvailable();
  }
}
