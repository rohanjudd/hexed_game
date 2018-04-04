// Hexed Game
// Rohan Judd Feb 2018

#include "hex_byte.h"
#include "game.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_MCP23017.h>
//#include <Encoder.h>

void setup();
void loop();
void check_for_mode_change();
void check_guess();
void display_mode();
void update_screen();
void beep();
byte get_button_byte();
byte update_guess_hex();
byte pot_to_hex();
byte encoder_to_hex();
void draw_byte(byte b);
void debug_byte(String s, byte b);
void update_guess_binary();

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);

//Encoder myEnc(27,33);

byte bit_button[8] = {4, 5, 6, 7, 8, 9, 10, 12};
byte button_byte = 0;
byte last_button_byte = 0;
byte new_presses = 0;
byte guess = 0;
byte pot = 17;
byte led1 = 2;
byte led2 = 3;
byte button_a = 14;
boolean button_a_last = false;
byte button_b = 32;
byte piezo = 11;
boolean state = false;
int freq = 0;


Game hex_game(0);

void setup()
{
  /*
  for (int i = 0; i < 9; i++) {
    pinMode(bit_button[i], INPUT_PULLUP);
  }
  pinMode(pot, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(piezo, OUTPUT);
  pinMode(16, INPUT);
  randomSeed(analogRead(16));
*/
  pinMode(button_a, INPUT_PULLUP);
  pinMode(button_b, INPUT_PULLUP);

  u8g2.begin();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1500);

  Serial.begin(115200);
  Serial.println("Hexed");
  hex_game.new_target();
  display_mode();
}

void loop()
{
  //check_for_mode_change();
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
  delay(50);
}

void check_for_mode_change()
{
  if (!digitalRead(button_a)) {
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
  /*u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();
  display.setCursor(0, 0);
  display.println(hex_game.get_mode_string());
  display.display();
  delay(500);
  */
}

void update_screen()
{
  /*display.clearDisplay();
  display.setCursor(0, 0);
  display.println(hex_game.get_target_string());
  display.setCursor(0, 16);
  if (hex_game.get_input_mode()) {
    display.println(get_binary_string(guess));
  }
  else {
    display.println(get_hex_string(guess));
  }
  display.display();
  */
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
  byte output = 0;
  for (int i = 0; i <= 8; i++) {
    (!digitalRead(bit_button[i])) ? (bitSet(output, i)) : (0);
  }
  return output;
}

byte update_guess_hex()
{
  if (digitalRead(button_a) == 0)
  {
    guess = set_high(guess, encoder_to_hex());
  }
  if (digitalRead(button_b) == 0)
  {
    guess = set_low(guess, encoder_to_hex());
  }
}

byte pot_to_hex()
{
  return analogRead(pot) / 64;
}

byte encoder_to_hex()
{
  //return (myEnc.read() /4) % 16;
  return 0;
}

void draw_byte(byte b)
{
  String s = get_binary_string(b);
  /*
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println(s);
  display.display();
  */
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
  for (int i = 0; i <= 8; i++) {
    if ((bitRead(button_byte, i)) && (!bitRead(last_button_byte, i))) {
      bitSet(new_presses, i);
    }
  }
  last_button_byte = button_byte;
  for (int i = 0; i <= 8; i++) {
    if (bitRead(new_presses, i)) {
      bitWrite(guess, i, !bitRead(guess, i));
    }
  }
}
