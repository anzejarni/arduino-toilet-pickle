#include <VirtualWire.h>

const int transmit_pin = 4;
const int lightsensor_pin = A0;

void setup()
{
  // Initialize light resistor
  pinMode(lightsensor_pin, INPUT);
    
  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(128);       // Bits per sec
  Serial.begin(9600);
}

boolean isDark()
{
  int light_value = analogRead(lightsensor_pin);
  if (light_value < 280) {
    return 1;
  } else {
    return 0;
  }
}

void sendFree()
{
  uint8_t code = 0;
  vw_send(&code, 1);
  vw_wait_tx();
}


void sendOccupied()
{
  uint8_t code = 1;
  vw_send(&code, 1);
  vw_wait_tx();
}

void loop()
{
  if (isDark()) {
   sendFree(); 
  } else {
   sendOccupied();
  }

  delay(1000);
}
