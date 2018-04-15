// Hexed Game
// Rohan Judd Apr 2018
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_MCP23017.h>
#include <Encoder.h>

#include "hex_byte.h"
#include "game.h"

void setup();
void loop();
void check_for_mode_change();
void check_guess();
void show_mode();
void update_screen();
void next_mode();
void beep();
void reset_encoders();
byte get_button_byte();
byte update_guess_hex();
byte encoder_a_to_hex();
byte encoder_b_to_hex();
void update_guess_binary();
void hexagon(byte x, byte y, byte radius);

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
Adafruit_MCP23017 mcp;

Encoder encoder_a(15,33);
Encoder encoder_b(32,14);

byte button_byte = 0;
byte last_button_byte = 0;
byte new_presses = 0;
byte guess = 0;
boolean button_a_last = false;

const byte numModes = 3;
Game games[numModes];
byte mode_index = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Hexed");

  mcp.begin();      // use default address 0
  mcp.pinMode(0, INPUT);
  for(int i=0;i<=9;i++)
      mcp.pullUp(i, HIGH);

  u8g2.begin();
  u8g2.setContrast(0);
  u8g2.setFont(u8g2_font_inb16_mr);	// choose a suitable font

  u8g2.clearBuffer();					// clear the internal memory
  u8g2.drawStr(0,18,"Hexed");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(800);

  u8g2.clearBuffer();
  for(int x=20; x<160; x+=20){
    for(int y=20; y<44; y+=20){
      hexagon(x,y,12);
    }
  }
  u8g2.sendBuffer();
  delay(2000);


  games[0].set_modes(0,1);
  games[1].set_modes(1,0);
  games[2].set_modes(1,2);
  games[mode_index].new_target();
  show_mode();
}

void loop()
{
  check_for_mode_change();
  if (games[mode_index].get_input_mode()) {
    update_guess_binary();
  }
  else {
    update_guess_hex();
  }
  update_screen();
  check_guess();
  delay(30);
}

void check_for_mode_change()
{
  if (!mcp.digitalRead(8)) {
    if (!button_a_last) {
      next_mode();
      Serial.print("Changing Mode to: ");
      Serial.println(mode_index);
      show_mode();
      guess = 0;
    }
    button_a_last = true;
  }
  else
    button_a_last = false;
}

void check_guess()
{
  if (games[mode_index].check_guess(guess)) {
    beep();
    delay(200);
    guess = 0;
    reset_encoders();
  }
}

void show_mode()
{
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setCursor(0, 18);
  u8g2.print(games[mode_index].get_mode_string());
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(500);
}

void update_screen()
{
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setCursor(0, 18);
  u8g2.print(games[mode_index].get_target_string());
  u8g2.setCursor(0, 40);
  byte input_mode = games[mode_index].get_input_mode();
  switch(input_mode)
  {
    case 0:
      u8g2.print(get_hex_string(guess));
      break;
    case 1:
      u8g2.print(get_binary_string(guess));
      break;
    case 2:
      u8g2.print(get_decimal_string(guess));
      break;
  }
  u8g2.sendBuffer();
}

void next_mode()
{
  mode_index++;
  if(mode_index >= numModes)
    mode_index = 0;
}

void beep()
{
  for (int i = 1912; i <= 3038; i += 30) {
    delay(3);
  }
}

byte get_button_byte()
{
  byte port_a = mcp.readGPIO(0);
  byte output = 0;
  for(int i=0;i<=7;i++) //flip direction and invert
    bitWrite(output,i,!bitRead(port_a,7-i));
  return output;
}

byte update_guess_hex()
{
    guess = set_high(guess, encoder_a_to_hex());
    guess = set_low(guess, encoder_b_to_hex());
}

byte encoder_a_to_hex()
{
  return round(float(encoder_a.read()) / 4.0) % 16;
}

byte encoder_b_to_hex()
{
  return round(float(encoder_b.read()) / 4.0) % 16;
}

void reset_encoders()
{
  encoder_a.write(0);
  encoder_b.write(0);
}

void update_guess_binary()
{
  button_byte = get_button_byte();
  new_presses = 0;
  for (int i = 0; i <= 8; i++){
    if ((bitRead(button_byte, i)) && (!bitRead(last_button_byte, i)))
      bitSet(new_presses, i);
  }
  last_button_byte = button_byte;
  for (int i = 0; i <= 8; i++) {
    if (bitRead(new_presses, i))
      bitWrite(guess, i, !bitRead(guess, i));
  }
}

void hexagon(byte x, byte y, byte radius)
{
  boolean isFirst = true;
  byte lastX = 0;
  byte lastY = 0;
  float angle = 6.28 / 6.0;
  for (float a = 0; a < 6.28; a += angle) {
    byte sx = byte(round(x + cos(a) * radius));
    byte sy = byte(round(y + sin(a) * radius));
    Serial.print("sx: ");
    Serial.print(sx);
    Serial.print(" sy: ");
    Serial.println(sy);
    if (! isFirst)
      u8g2.drawLine(lastX, lastY, sx, sy);
    else
      isFirst = false;
    lastX = sx;
    lastY = sy;
  }
}
