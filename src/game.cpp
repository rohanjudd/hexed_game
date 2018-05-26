/*
   game.cpp - Class for game object
 */
#include "game.h"
#include "hex_byte.h"

static String FORMAT[3] = {"HEX", "BIN", "DEC"};
// 0 HEX
// 1 BIN
// 2 DEC

Game::Game(){
        _input_mode = 0;
        _output_mode = 1;
}

Game::Game(byte input_mode, byte output_mode){
        _input_mode = input_mode;
        _output_mode = output_mode;
}

void Game::set_modes(byte input_mode, byte output_mode){
        _input_mode = input_mode;
        _output_mode = output_mode;
        new_target();
}

void Game::new_target(){
        _target = random(0, 255);
}

byte Game::get_input_mode(){
        return _input_mode;
}

String Game::get_target_string(){
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

String Game::get_mode_string(){
        String mode_text = "";
        mode_text += FORMAT[_input_mode];
        mode_text += "->";
        mode_text += FORMAT[_output_mode];
        return mode_text;
}

boolean Game::check_guess(byte b){
        if (b == _target) {
                new_target();
                _score += 10;
                return true;
        }
        else {
                return false;
        }
}

int Game::get_score(){
        return _score;
}
