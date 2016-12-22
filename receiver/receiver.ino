#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <VirtualWire.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9 
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

const uint8_t receive_pin = 4;
const uint8_t display_power_pin = 7;
uint8_t last_drawn_state = -1;
unsigned long last_update = 0;

void setup()
{
  pinMode(display_power_pin, OUTPUT);
  digitalWrite(display_power_pin, HIGH);
  
  Serial.begin(9600);

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(tft.getRotation()+1);

  // Initialise the IO and ISR
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_inverted(true);
  vw_setup(128);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
  Serial.print("Serial");

  drawNoData();
}

void drawOccupied()
{
  // large block of text
  tft.fillScreen(ST7735_RED);  

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 50);
  tft.println("ZASEDENO");
}

void drawFree()
{
  // large block of text
  tft.fillScreen(ST7735_GREEN);  

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(3);
  tft.setCursor(25, 50);
  tft.println("PROSTO");
}

void drawNoData()
{
  // large block of text
  tft.fillScreen(ST7735_BLACK);  

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2.5);
  tft.setCursor(20, 55);
  tft.println("NI PODATKA");
}

void drawWrongData()
{
  // large block of text
  tft.fillScreen(ST7735_BLACK);  

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 55);
  tft.println("NAPACEN");
  tft.setCursor(40, 70);
  tft.println("PODATEK");
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    if (buf[0] != last_drawn_state){
      switch (buf[0]) {
        default:
          drawWrongData();
        case 0:
          drawFree();
          break;
        case 1:
          drawOccupied();
          break;
      }
      
      last_drawn_state = buf[0];
    }

    last_update = millis();

    /*
    for (i = 0; i < buflen; i++)
    {                                                  
      Serial.print(buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    */
  }

  // If no transmission has been accepted in the last 40 seconds, dispay NO DATA
  if (last_drawn_state != -1 && millis() > last_update + 40000) {
      drawNoData();
      last_drawn_state = -1;
  }

}
