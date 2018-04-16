/*
   hex_byte.cpp - Class for hex byte object
 */
#include "hex_byte.h"

byte get_high(byte b)
{
        return b >> 4;
}

byte get_low(byte b)
{
        return b & 0x0F;
}

byte set_high(byte b, byte c)
{
        c = c << 4;
        b = b & 15; // erase high nibble
        b = b | c;
        return b;
}

byte set_low(byte b, byte c)
{
        b = b & 240; // erase low nibble
        c = c & 15; // erase high nibble
        b = b | c;
        return b;
}

byte generate_byte()
{
        return byte(random(0, 255));
}

String get_hex_string(byte b)
{
        String s = "";
        s += nibble_to_ascii(get_high(b));
        s += nibble_to_ascii(get_low(b));
        return s;
}

char nibble_to_ascii(byte b)
{
        if(b < 10) {
                return b + 48;
        }
        else{
                return b - 10 + 65;
        }
}

String get_binary_string(byte b)
{
        String s = "";
        for (int i = 7; i >= 0; i--) {
                (bitRead(b, i)) ? (s += "1") : (s += "0");
        }
        return s;
}

String get_decimal_string(byte b)
{
        String s = "";
        s += b;
        return s;
}

byte hex_ascii_to_byte(char c)
{
        if(c >= 48 && c <= 57) {
                return c - 48;
        }
        if(c >= 65 && c <= 70) {
                return c - 65 + 10;
        }
        return 0;
}
