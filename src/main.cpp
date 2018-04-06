// Hexed Game
// Rohan Judd Apr 2018

#include "hex_byte.h"
#include "game.h"
#include "Encoder.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_MCP23017.h>

void setup();
void loop();
void check_for_mode_change();
void check_guess();
void display_mode();
void update_screen();
void beep();
byte get_button_byte();
byte update_guess_hex();
byte encoder_a_to_hex();
byte encoder_b_to_hex();
void debug_byte(String s, byte b);
void update_guess_binary();

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
Adafruit_MCP23017 mcp;

Encoder encoder_a(33,27);
Encoder encoder_b(32,14);

byte button_byte = 0;
byte last_button_byte = 0;
byte new_presses = 0;
byte guess = 0;
boolean button_a_last = false;
byte piezo = 11;
int freq = 0;

Game hex_game(0);

void setup()
{
  //pinMode(piezo, OUTPUT);
  mcp.begin();      // use default address 0
  mcp.pinMode(0, INPUT);
  for(int i=0;i<=9;i++)
      mcp.pullUp(i, HIGH);

  u8g2.begin();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_inb16_mr);	// choose a suitable font
  u8g2.drawStr(0,18,"Hexed");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(500);

  Serial.begin(115200);
  Serial.println("Hexed");
  hex_game.new_target();
  display_mode();
}

void loop()
{
  //check_encoder();
  check_for_mode_change();
  if (hex_game.get_input_mode()) {
    update_guess_binary();
  }
  else {
    update_guess_hex();
  }
  update_screen();
  //if (hex_game.get_input_mode() || button_byte == 0) {
    check_guess();
  //}
  delay(30);
}

void check_for_mode_change()
{
  if (!mcp.digitalRead(8)) {
    Serial.println("button_a pressed");
    if (!button_a_last) {
      hex_game.change_mode();
      display_mode();
      guess = 0;
    }
    button_a_last = true;
  }
  else {
    button_a_last = false;
  }
}

void check_guess()
{
  if (hex_game.check_guess(guess)) {
    beep();
    delay(200);
    guess = 0;
  }
}

void display_mode()
{
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setCursor(0, 18);
  u8g2.print(hex_game.get_mode_string());
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(500);
}

void update_screen()
{
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setCursor(0, 18);
  u8g2.print(hex_game.get_target_string());
  u8g2.setCursor(0, 40);
  if (hex_game.get_input_mode()) {
    u8g2.print(get_binary_string(guess));
  }
  else {
    u8g2.print(get_hex_string(guess));
  }
  u8g2.sendBuffer();
}

void beep()
{
  for (int i = 1912; i <= 3038; i += 30) {
  //  tone(piezo, i);
    delay(3);
  }
//  noTone(piezo);
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
  int val = encoder_a.read();
  Serial.print(val);
  Serial.print(" : ");
  float f = float(encoder_a.read()) / 4.0;
  Serial.print(f);
  Serial.print(" : ");
  int x = round(f);
  Serial.print(x);
  Serial.print(" : ");
  int m = x % 16;
  Serial.println(m);
  return m;
}

byte encoder_b_to_hex()
{
  int x = float(encoder_b.read()) / 4.0;
  return x % 16;
}

void debug_byte(String s, byte b)
{
  Serial.print(s);
  Serial.print(": ");
  Serial.println(get_binary_string(b));
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
