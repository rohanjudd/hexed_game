// Hexed Game
// Rohan Judd Apr 2018
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Encoder.h>
#include "hex_byte.h"
#include "game.h"
#include "display.h"

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


Adafruit_MCP23017 mcp;
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
Display display(u8g2);

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

        mcp.begin(); // use default address 0
        mcp.pinMode(0, INPUT);
        for(int i=0; i<=9; i++)
                mcp.pullUp(i, HIGH);

        display.initialise();
        //display.splash_text("");
        delay(800);

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
        //display.splash_text(games[mode_index].get_mode_string());
        delay(500);
}

void update_screen()
{
        byte input_mode = games[mode_index].get_input_mode();
        String g = "";
        switch(input_mode)
        {
        case 0:
                g = get_hex_string(guess);
                break;
        case 1:
                g = get_binary_string(guess);
                break;
        case 2:
                g = get_decimal_string(guess);
                break;
        }
        //display.update_screen(games[mode_index].get_target_string(), g);
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
        for(int i=0; i<=7; i++) //flip direction and invert
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
        for (int i = 0; i <= 8; i++) {
                if ((bitRead(button_byte, i)) && (!bitRead(last_button_byte, i)))
                        bitSet(new_presses, i);
        }
        last_button_byte = button_byte;
        for (int i = 0; i <= 8; i++) {
                if (bitRead(new_presses, i))
                        bitWrite(guess, i, !bitRead(guess, i));
        }
}
