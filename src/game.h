/*
   game.h -
 */
#include <Arduino.h>

class Game
{
public:
Game();
Game(byte input_mode, byte output_mode);
void set_modes(byte input_mode, byte output_mode);
void new_target();
String get_target_string();
String get_mode_string();
byte get_input_mode();
boolean check_guess(byte b);
int get_score();

private:
byte _input_mode;
byte _output_mode;
byte _target;
int _score;
};
