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
void update_guess();
byte get_button_byte();
byte encoder_a_read();
byte encoder_b_read();

Adafruit_MCP23017 mcp;

Encoder encoder_a(15,33);
Encoder encoder_b(32,14);

byte last_button_byte = 0;
//byte new_presses = 0;
byte guess = 0;
boolean button_a_last = false;

const byte numModes = 4;
Game games[numModes];
byte mode_index = 0;

void setup(){
        Serial.begin(115200);
        Serial.println("Hexed");

        mcp.begin(); // use default address 0
        mcp.pinMode(0, INPUT);
        for(int i=0; i<=9; i++)
                mcp.pullUp(i, HIGH);

        display::initialise();
        display::splash_text("Hexed");
        delay(500);

        games[0].set_modes(0,1);
        games[1].set_modes(1,0);
        games[2].set_modes(1,2);
        games[3].set_modes(2,1);
        show_mode();
}

void loop(){
        check_for_mode_change();
        update_screen();
        update_guess();
        check_guess();
        delay(30);
}

void check_for_mode_change(){
        if (!mcp.digitalRead(8)) {
                if (!button_a_last)
                        next_mode();
                button_a_last = true;
        }
        else
                button_a_last = false;
}

void update_screen(){
        byte input_mode = games[mode_index].get_input_mode();
        display::update_screen(games[mode_index].get_target_string(),
                               get_string(guess, input_mode),
                               games[mode_index].get_score());
}

void check_guess(){
        if (games[mode_index].check_guess(guess)) {
                beep();
                delay(200);
                guess = 0;
                reset_encoders();
        }
}

void show_mode(){
        display::splash_text(games[mode_index].get_mode_string());
        delay(400);
}

void next_mode(){
        mode_index++;
        if(mode_index >= numModes)
                mode_index = 0;
        show_mode();
        guess = 0;
        reset_encoders();
}

void beep(){
        for (int i = 1912; i <= 3038; i += 30)
                delay(3);
}

byte get_button_byte(){
        byte port_a = mcp.readGPIO(0);
        byte output = 0;
        for(int i=0; i<=7; i++) //flip direction and invert
                bitWrite(output,i,!bitRead(port_a,7-i));
        return output;
}

byte encoder_a_read(){
        return round(float(encoder_a.read()) / 4.0);
}

byte encoder_b_read(){
        return round(float(encoder_b.read()) / 4.0);
}

void reset_encoders(){
        encoder_a.write(0);
        encoder_b.write(0);
}

void update_guess()
{
        switch(games[mode_index].get_input_mode())
        {
        case 0: {
                guess = nibbles_to_byte(encoder_a_read() % 16, encoder_b_read() % 16);
                break;
        }
        case 1: {
                byte button_byte = get_button_byte();
                for (int i = 0; i <= 8; i++) {
                        if ((bitRead(button_byte, i)) && (!bitRead(last_button_byte, i)))
                                bitWrite(guess, i, !bitRead(guess, i));
                }
                last_button_byte = button_byte;
                break;
        }
        case 2: {
                guess = (encoder_a_read() % 26) * 10;
                (guess >= 250) ? guess += encoder_b_read() % 6 : guess += encoder_b_read() % 10;
                break;
        }
        }
}
