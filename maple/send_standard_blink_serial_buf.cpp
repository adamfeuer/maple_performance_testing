// Virtual Serial Send Speed Benchmark
//
// This program sends data as rapidly as possible

#include <wirish/wirish.h>
#include <libmaple/usart.h>

#define SERIAL Serial1 // Arduino Due, Maple
#define LED_PIN BOARD_LED_PIN
#define BUF_SIZE 1000

char buf[BUF_SIZE];

void setup() {
  /* Set up the LED to blink  */
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 0);

  SERIAL.begin(8500000);
  pinMode(2, OUTPUT);  // frequency is kbytes/sec
  
  for (int i=0; i++; i<BUF_SIZE) {
     buf[i] = 'x';
  }

}

byte pinstate=LOW;

void loop() {
  // send BUF_SIZE bytes
  SERIAL.write_buf((uint8 *)buf, BUF_SIZE);
 
  // toggle pin 2, so the frequency is 2*BUF_SIZE bytes/sec

  if (pinstate == LOW) {
    digitalWrite(2, HIGH);
    digitalWrite(LED_PIN, HIGH);
    pinstate = HIGH;
  } else {
    digitalWrite(2, LOW);
    digitalWrite(LED_PIN, LOW);
    pinstate = LOW;
  }

}


// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain() {
    init();
}

int main(void) {
    setup();

    while (true) {
        loop();
    }

    return 0;
}
