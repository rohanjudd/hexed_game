/*
   hex_byte.h - Class for hex byte object
 */
#include "Arduino.h"

byte get_high(byte b);
byte get_low(byte b);
byte set_high(byte b, byte c);
byte set_low(byte b, byte c);
byte nibbles_to_byte(byte h, byte l);
byte generate_byte();
String get_hex_string(byte b);
String get_binary_string(byte b);
String get_decimal_string(byte b);
String get_string(byte b, byte f);
char nibble_to_ascii(byte b);
byte hex_ascii_to_byte(char c);
