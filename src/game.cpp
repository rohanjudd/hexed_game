/*
  game.cpp - Class for game object
*/
#include "Arduino.h"
#include "game.h"
#include "hex_byte.h"

static String FORMAT[3] = {"HEX", "BIN", "DEC"};
// 0 HEX
// 1 BIN
// 2 DEC

Game::Game(byte arg)
{
  _input_mode = 0;
  _output_mode = 1;
}

void Game::new_target()
{
  _target = random(0, 255);
}

byte Game::get_target()
{
  return _target;
}

byte Game::get_input_mode()
{
  return _input_mode;
}

String Game::get_target_string()
{
  String target_text = "";
  switch (_output_mode) {
    case 0:
      target_text += "0x";
      target_text += get_hex_string(_target);
      break;
    case 1:
      //target_text += "0b";
      target_text += get_binary_string(_target);
      break;
    case 2:
      target_text += String(_target);
      break;
    default:
      break;
  }
  return target_text;
}

String Game::get_mode_string()
{
  String mode_text = "";
  mode_text += FORMAT[_input_mode];
  mode_text += " to ";
  mode_text += FORMAT[_output_mode];
  return mode_text;
}

void Game::change_input_mode()
{
  _input_mode = (_input_mode + 1 ) % 3;
  if (_input_mode == _output_mode) {
    _input_mode = (_input_mode + 1) % 3;
  }
  new_target();
}
void Game::change_output_mode()
{
  _output_mode = (_output_mode + 1 ) % 3;
  if (_output_mode == _input_mode) {
    _output_mode = (_output_mode + 1) % 3;
  }
  new_target();
}

void Game::change_mode()
{
  if (_input_mode == 1) {
    _input_mode = 0;
    _output_mode = 1;
  }
  else{
    _input_mode = 1;
    _output_mode = 0;
  }
  new_target();
}

boolean Game::check_guess(byte b)
{
  if (b == _target) {
    new_target();
    return true;
  }
  else {
    return false;
  }
}
