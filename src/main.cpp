#include <LCDMenuLib2.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_MCP23017.h>
#include <Encoder.h>
#include <Preferences.h>
#include "config.h"
#include "hex_byte.h"
#include "game.h"
#include "SoundData.h"
#include <MAX5407.h>
#include <Game_Audio.h>

void menu_display();
void menu_clear();
void menu_control();
void screensaver(uint8_t param);
void back(uint8_t param);
void go_to_root(uint8_t param);
void brightness_control(uint8_t line);
void volume_control(uint8_t line);
void load_settings();
void save_settings(uint8_t param);
void font_demo(uint8_t param);
void start_game(byte game_type);
void encoder_hex_control();
void encoder_dec_control();
void check_guess();
void update_screen();
void game_screen(String target, String guess, int score);
void reset_encoders();
void monitor_battery(uint8_t param);
void play_sound(uint8_t param);
void sound_demo(uint8_t param);
float get_battery_voltage();
byte get_button_byte();
byte encoder_a_read();
byte encoder_b_read();
boolean COND_hide();

//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
Adafruit_MCP23017 mcp;
Preferences preferences;

Encoder encoder_a(15,33);
bool  a_button_last = true;
Encoder encoder_b(14,32);
bool  b_button_last = true;
byte battery_mon = A13;

byte last_button_byte = 0;
byte binary_guess = 0;
byte guess = 0;

const byte numModes = 6;
Game games[numModes];
byte mode_index = 0;

uint8_t timer_counter = 0;  // time counter (global variable)
unsigned long timer_1 = 0;    // timer variable (global variable)

static byte max_volume = 31;
static byte cs_pin = 12;
static byte ud_pin = 27;
static byte initial = 0;
static byte amp_shutdown = 13;
Digipot digipot(cs_pin, ud_pin, initial);

static byte charge_pin = A2;

byte brightness = 0;
byte volume = 10;

Game_Audio_Class GameAudio(A0,0);
Game_Audio_Wav_Class success_wav(success);
Game_Audio_Wav_Class voice_wav(denied);
Game_Audio_Wav_Class beep_wav(beep);
byte num_sounds = 3;
byte sound_index = 0;

LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, DISP_rows, DISP_cols, menu_display, menu_clear, menu_control);

LCDML_addAdvanced (0 , LCDML_0    , 1 , NULL,          "Play Game"       , NULL,                0,    _LCDML_TYPE_default);
LCDML_addAdvanced (1 , LCDML_0_1  , 1 , NULL,          "Hex to Bin"      , start_game,          0,    _LCDML_TYPE_default);
LCDML_addAdvanced (2 , LCDML_0_1  , 2 , NULL,          "Bin to Hex"      , start_game,          1,    _LCDML_TYPE_default);
LCDML_addAdvanced (3 , LCDML_0_1  , 3 , NULL,          "Bin to Dec"      , start_game,          2,    _LCDML_TYPE_default);
LCDML_addAdvanced (4 , LCDML_0_1  , 4 , NULL,          "Hex to Dec"      , start_game,          3,    _LCDML_TYPE_default);
LCDML_addAdvanced (5 , LCDML_0_1  , 5 , NULL,          "Dec to Hex"      , start_game,          4,    _LCDML_TYPE_default);
LCDML_addAdvanced (6 , LCDML_0_1  , 6 , NULL,          "Dec to Bin"      , start_game,          5,    _LCDML_TYPE_default);
LCDML_add         (7 , LCDML_0_1  , 7 , "Back"                           , back);
LCDML_add         (8 , LCDML_0    , 2 , "Settings"                       , NULL);
LCDML_addAdvanced (9 , LCDML_0_2  , 1 , NULL,          ""                , brightness_control,  0,    _LCDML_TYPE_dynParam);
LCDML_addAdvanced (10, LCDML_0_2  , 2 , NULL,          ""                , volume_control,      0,    _LCDML_TYPE_dynParam);
LCDML_addAdvanced (11, LCDML_0_2  , 3 , NULL,          "Monitor Battery" , monitor_battery,     0,    _LCDML_TYPE_default);
LCDML_addAdvanced (12, LCDML_0_2  , 4 , NULL,          "Save Changes"    , save_settings,       0,    _LCDML_TYPE_default);
LCDML_add         (13, LCDML_0_2  , 5 , "Back"                           , back);
LCDML_add         (14, LCDML_0    , 3 , "Testing"                        , NULL);
LCDML_add         (15, LCDML_0_3  , 1 , "Sound Demo"                     , sound_demo);
LCDML_add         (16, LCDML_0_3  , 2 , "Font Demo"                     , font_demo);
//LCDML_addAdvanced (16, LCDML_0_3  , 2 , NULL,          ""                , volume_control,      0,    _LCDML_TYPE_dynParam);
LCDML_add         (17, LCDML_0_3  , 3 , "Back"                           , back);
LCDML_addAdvanced (18 , LCDML_0   , 4 , COND_hide,     "screensaver"     , screensaver,         0,    _LCDML_TYPE_default);
#define DISP_cnt   18 // this value must be the same as the last menu element
LCDML_createMenu(DISP_cnt);

void setup()
{
	pinMode(battery_mon, INPUT);
	load_settings();

	u8g2.begin();
  u8g2.setContrast(brightness);

	Serial.begin(115200);
	Serial.println("Hexed");

	digipot.set_tap(volume);

	pinMode(amp_shutdown, OUTPUT);
	digitalWrite(amp_shutdown, LOW);

	pinMode(charge_pin, INPUT_PULLUP);

	mcp.begin(); // use default address 0
	mcp.pinMode(0, INPUT);
	for(int i=0; i<=9; i++)
		mcp.pullUp(i, HIGH);
	mcp.pinMode(10, OUTPUT);
	mcp.pinMode(11, OUTPUT);
	mcp.pinMode(12, OUTPUT);

	mcp.digitalWrite(10, HIGH);
	mcp.digitalWrite(11, LOW);

	LCDML_setup(DISP_cnt);
	//LCDML.MENU_enRollover();
	LCDML.SCREEN_enable(screensaver, 40000); // set to 40 seconds

	GameAudio.PlayWav(&success_wav, false, 1.3);

	games[0].set_modes(0,1);
	games[1].set_modes(1,0);
	games[2].set_modes(1,2);
	games[3].set_modes(0,2);
	games[4].set_modes(2,0);
	games[5].set_modes(2,1);
}

void loop()
{
	LCDML.loop();
	delay(20);
}

void start_game(byte game_type)
{
	if(LCDML.FUNC_setup())
	{
		u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
		mode_index = game_type;
	}
	if(LCDML.FUNC_loop()){
		if(LCDML.BT_checkAny())
		{
			switch(games[mode_index].get_input_mode())
			{
				case 0: {
                encoder_hex_control();
                break;
        }
        case 1: {
                guess = binary_guess;
                break;
        }
        case 2: {
								encoder_dec_control();
                break;
        }
			}
			if(LCDML.BT_checkEnter()){
				check_guess();
			}
		}
		update_screen();
		//check_guess();
	}
	if(LCDML.FUNC_close()){}
}

void encoder_hex_control()
{
	if(LCDML.BT_checkUp()){
		guess = encoder_adjust(guess, -1, 0);
		LCDML.BT_resetUp();
	}
	else if(LCDML.BT_checkDown()){
		guess = encoder_adjust(guess, 1, 0);
		LCDML.BT_resetDown();
	}
	if(LCDML.BT_checkLeft()){
		guess = encoder_adjust(guess, 0, 1);
		LCDML.BT_resetLeft();
	}
	else if(LCDML.BT_checkRight()){
		guess = encoder_adjust(guess, 0, -1);
		LCDML.BT_resetRight();
	}
}

void encoder_dec_control()
{
	if(LCDML.BT_checkUp()){
		guess -= 10;
		LCDML.BT_resetUp();
	}
	else if(LCDML.BT_checkDown()){
		guess += 10;
		LCDML.BT_resetDown();
	}
	if(LCDML.BT_checkLeft()){
		guess += 1;
		LCDML.BT_resetLeft();
	}
	else if(LCDML.BT_checkRight()){
		guess -= 1;
		LCDML.BT_resetRight();
	}
}

void update_screen()
{
        byte input_mode = games[mode_index].get_input_mode();
        game_screen(games[mode_index].get_target_string(),
                               get_string(guess, input_mode),
                               games[mode_index].get_score());
}

void game_screen(String target, String guess, int score)
{
	u8g2.firstPage();
	do {
		u8g2.setDrawColor(1);
		u8g2.drawBox(0, 22, 128, 20);
		u8g2.setDrawColor(2);
		u8g2.setCursor(0, 18);
		u8g2.print(target);
		u8g2.setCursor(0, 40);
		u8g2.print(guess);
		u8g2.setCursor(0, 60);
		u8g2.print(score);
	} while( u8g2.nextPage() );
}

void check_guess(){
        if (games[mode_index].check_guess(guess)) {
								GameAudio.PlayWav(&success_wav, false, 1.3);
                delay(500);
                guess = 0;
								binary_guess = 0;
                reset_encoders();
								update_screen();
        }
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

void screensaver(uint8_t param)
{
	if(LCDML.FUNC_setup())
	{
		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
		} while( u8g2.nextPage() );
		LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
	}

	if(LCDML.FUNC_loop())
	{
		if (LCDML.BT_checkAny()){
			//LCDML.FUNC_goBackToMenu();
		}
	}

	if(LCDML.FUNC_close()){
		//LCDML.MENU_goRoot();
	}
}

void monitor_battery(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		char buf[20];
		sprintf (buf, "%.2fV", get_battery_voltage());
		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (DISP_font_h * 1), buf);
			if(digitalRead(charge_pin))
				u8g2.drawStr( 0, (DISP_font_h * 2), "Not Charging");
			else
				u8g2.drawStr( 0, (DISP_font_h * 2), "Charging");
		} while( u8g2.nextPage() );

		LCDML.FUNC_setLoopInterval(500);  // starts a trigger event for the loop function every 100 milliseconds
	}

	if(LCDML.FUNC_loop())           // ****** LOOP *********
	{
		LCDML.SCREEN_resetTimer();
		char buf[20];
		sprintf (buf, "%.2fV", get_battery_voltage());

		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (DISP_font_h * 1), buf);
			if(digitalRead(charge_pin))
				u8g2.drawStr( 0, (DISP_font_h * 2), "Not Charging");
			else
				u8g2.drawStr( 0, (DISP_font_h * 2), "Charging");
		} while( u8g2.nextPage() );
	}

	if(LCDML.FUNC_close())      // ****** STABLE END *********
	{
		// you can here reset some global vars or do nothing
	}
}

void sound_demo(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		sound_index = 0;
		char buf[20];
		sprintf (buf, "Sample %d", sound_index);
		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (DISP_font_h * 1), buf);
		} while( u8g2.nextPage() );
	}

	if(LCDML.FUNC_loop())           // ****** LOOP *********
	{
		char buf[20];
		sprintf (buf, "Sample %d", sound_index);
		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (DISP_font_h * 1), buf);
		} while( u8g2.nextPage() );

		if(LCDML.BT_checkAny()){

			if(LCDML.BT_checkUp()){
				LCDML.BT_resetUp();
				sound_index++;
				if (sound_index > num_sounds - 1)
					sound_index = num_sounds - 1;
			}

			if(LCDML.BT_checkDown()){
				LCDML.BT_resetDown();
				if (sound_index > 0)
				{
					sound_index--;
				}
				else
				{
					sound_index = 0;
				}
			}

			if(LCDML.BT_checkEnter())
			{
				play_sound(last_button_byte);
			}
	}
	}

	if(LCDML.FUNC_close()){}
}

void play_sound(uint8_t param)
{
	switch(param)
	{
		case 1: {
						GameAudio.PlayWav(&voice_wav, true, 1.0);
						break;
		}
		case 2: {
						GameAudio.PlayWav(&voice_wav, true, 1.059);
						break;
		}
		case 4: {
						GameAudio.PlayWav(&voice_wav, true, 1.118);
						break;
		}
		case 8: {
						GameAudio.PlayWav(&voice_wav, true, 1.177);
						break;
		}
		case 16: {
						GameAudio.PlayWav(&success_wav, true, 1.0);
						break;
		}
		case 32: {
						GameAudio.PlayWav(&success_wav, true, 1.059);
						break;
		}
		case 64: {
						GameAudio.PlayWav(&success_wav, true, 1.118);
						break;
		}
		case 128: {
						GameAudio.PlayWav(&success_wav, true, 1.177);
						break;
		}
	}
	//while(GameAudio.IsPlaying()){		}
}

float get_battery_voltage()
{
	return analogRead(battery_mon) * 0.001772;
}

void back(uint8_t param)
{
	if(LCDML.FUNC_setup()){
		LCDML.FUNC_goBackToMenu(1);
	}
}

void go_to_root(uint8_t param)
{
	if(LCDML.FUNC_setup())
		LCDML.MENU_goRoot();
}

void brightness_control(uint8_t line)
{
	if (line == LCDML.MENU_getCursorPos()){
		if(LCDML.BT_checkAny()){
			if(LCDML.BT_checkLeft()){
				brightness++;
				LCDML.BT_resetLeft();
				u8g2.setContrast(brightness);
			}
			if(LCDML.BT_checkRight()){
				brightness--;
				LCDML.BT_resetRight();
				u8g2.setContrast(brightness);
			}
		}
	}

	char buf[20];
	sprintf (buf, "Brightness: %d", brightness);
	u8g2.drawStr( DISP_box_x0+DISP_font_w + DISP_cur_space_behind,  (DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
}

void volume_control(uint8_t line)
{
	if (line == LCDML.MENU_getCursorPos()){
		if(LCDML.BT_checkAny()){
			if(LCDML.BT_checkLeft()){
				LCDML.BT_resetLeft();
				volume++;
				if (volume > max_volume)
					volume = max_volume;
				else
				{
					digipot.set_tap(volume);
					volume = digipot.get_tap();
					Serial.println(digipot.get_tap());
					GameAudio.PlayWav(&beep_wav, false, 1.0);
				}
			}
			if(LCDML.BT_checkRight()){
				LCDML.BT_resetRight();
				if (volume > 0)
				{
					volume--;
					digipot.set_tap(volume);
					volume = digipot.get_tap();
					Serial.println(digipot.get_tap());
					GameAudio.PlayWav(&beep_wav, false, 1.0);
				}
			}
		}
	}

	char buf[20];
	sprintf (buf, "Volume: %d", volume);
	u8g2.drawStr( DISP_box_x0+DISP_font_w + DISP_cur_space_behind,  (DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
}

void load_settings()
{
	preferences.begin("hex_game", false);
	brightness = preferences.getUChar("brightness", 0);
	volume = preferences.getUChar("volume", 0);
	preferences.end();
}

void save_settings(uint8_t param)
{
	if(LCDML.FUNC_setup())
	{
		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (DISP_font_h * 1), "Saving to EEPROM");
		} while( u8g2.nextPage() );

		preferences.begin("hex_game", false);
		preferences.putUChar("brightness", brightness);
		preferences.putUChar("volume", volume);
		preferences.end();
		//LCDML.FUNC_setLoopInterval(1000);  // starts a trigger event for the loop function every 100 milliseconds
	}

	if(LCDML.FUNC_loop())
	{
		delay(500);
		LCDML.FUNC_goBackToMenu();
	}

	if(LCDML.FUNC_close()){
		LCDML.MENU_goRoot();
	}
}

void font_demo(uint8_t param)
{
	if(LCDML.FUNC_setup())
	{
		u8g2.setFont(DISP_font);
		u8g2.firstPage();
		do {
			u8g2.setFontMode(1);  /* activate transparent font mode */
			u8g2.setDrawColor(1); /* color 1 for the box */
			u8g2.drawBox(22, 2, 35, 50);
			//u8g2.setFont(u8g2.font_ncenB14_tf);
			u8g2.setDrawColor(0);
			u8g2.drawStr(5, 18, "abcd");
			u8g2.setDrawColor(1);
			u8g2.drawStr(5, 33, "abcd");
			u8g2.setDrawColor(2);
			u8g2.drawStr(5, 48, "abcd");
		} while( u8g2.nextPage() );
	}

	if(LCDML.FUNC_loop())
	{
		if(LCDML.BT_checkAny())
				LCDML.FUNC_goBackToMenu();
	}

	if(LCDML.FUNC_close()){
		LCDML.MENU_goRoot();
	}
}

boolean COND_hide()  // hide a menu element
{
	return false;  // hidden
}

void menu_clear()
{
}

void menu_display()
{
	u8g2.setFont(DISP_font);
	u8g2.setFontMode(1);  /* activate transparent font mode */
	char content_text[DISP_cols];  // save the content text of every menu element
	// menu element object
	LCDMenuLib2_menu *tmp;
	// some limit values
	uint8_t i = LCDML.MENU_getScroll();
	uint8_t maxi = DISP_rows + i;
	uint8_t n = 0;
	// init vars
	uint8_t n_max             = (LCDML.MENU_getChilds() >= DISP_rows) ? DISP_rows : (LCDML.MENU_getChilds());
	uint8_t scrollbar_min     = 0;
	uint8_t scrollbar_max     = LCDML.MENU_getChilds();
	uint8_t scrollbar_cur_pos = LCDML.MENU_getCursorPosAbs();
	uint8_t scroll_pos        = ((1.*n_max * DISP_rows) / (scrollbar_max - 1) * scrollbar_cur_pos);
	// generate content
	u8g2.firstPage();
	do {
		n = 0;
		i = LCDML.MENU_getScroll();
		// check if this element has children

		u8g2.setDrawColor(1);
		u8g2.drawBox(DISP_box_x0, DISP_box_y0 + 2 + DISP_font_h * (LCDML.MENU_getCursorPos()), DISP_w - 12  , DISP_font_h + 1);
		u8g2.setDrawColor(2);

		if ((tmp = LCDML.MENU_getObj()->getChild(LCDML.MENU_getScroll())))
		{
			do
			{
				if (tmp->checkCondition())
				{
					if(tmp->checkType_menu() == true)
					{
						LCDML_getContent(content_text, tmp->getID());
						u8g2.drawStr( DISP_box_x0+DISP_font_w, DISP_box_y0 + DISP_font_h * (n + 1), content_text);
					}
					else
					{
						if(tmp->checkType_dynParam()) {
							tmp->callback(n);
						}
					}
					i++;
					n++;
				}
				// try to go to the next sibling and check the number of displayed rows
			} while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
		}
		// set cursor
		//u8g2.drawStr( DISP_box_x0+DISP_cur_space_before, DISP_box_y0 + DISP_font_h * (LCDML.MENU_getCursorPos() + 1),  DISP_cursor_char);
		u8g2.setDrawColor(1);
		// display scrollbar when more content as rows available and with > 2
		if (scrollbar_max > n_max && DISP_scrollbar_w > 2)
		{
			// set frame for scrollbar
			//u8g2.drawFrame(DISP_box_x1 - DISP_scrollbar_w, DISP_box_y0, DISP_scrollbar_w, DISP_box_y1-DISP_box_y0);
			// calculate scrollbar length
			uint8_t scrollbar_block_length = scrollbar_max - n_max;
			scrollbar_block_length = (DISP_box_y1-DISP_box_y0) / (scrollbar_block_length + DISP_rows);
			//set scrollbar
			if (scrollbar_cur_pos == 0) {                                   // top position     (min)
				//u8g2.drawBox(DISP_box_x1 - (DISP_scrollbar_w-1), DISP_box_y0 + 1                                                     , (DISP_scrollbar_w-2)  , scrollbar_block_length);
			}
			else if (scrollbar_cur_pos == (scrollbar_max-1)) {            // bottom position  (max)
				//u8g2.drawBox(DISP_box_x1 - (DISP_scrollbar_w-1), DISP_box_y1 - scrollbar_block_length                                , (DISP_scrollbar_w-2)  , scrollbar_block_length);
			}
			else {                                                                // between top and bottom
				//u8g2.drawBox(DISP_box_x1 - (DISP_scrollbar_w-1), DISP_box_y0 + (scrollbar_block_length * scrollbar_cur_pos + 1),(DISP_scrollbar_w-2)  , scrollbar_block_length);
			}
		}
	} while ( u8g2.nextPage() );
}

void menu_control(void)
{
	if(LCDML.BT_setup()){}

	long encoder_a_pos = encoder_a.read();
  long encoder_b_pos = encoder_b.read();
	bool button_a  = !mcp.digitalRead(8);
  bool button_b  = !mcp.digitalRead(9);

	if(encoder_a_pos >= 3) {
		LCDML.BT_down();
		encoder_a.write(encoder_a_pos-4);
	}
	else if(encoder_a_pos <= -3){
		LCDML.BT_up();
		encoder_a.write(encoder_a_pos+4);
	}
  else if(encoder_b_pos >= 3){
		LCDML.BT_left();
		encoder_b.write(encoder_b_pos-4);
	}
	else if(encoder_b_pos <= -3){
		LCDML.BT_right();
		encoder_b.write(encoder_b_pos+4);
	}
	else{
		if(!button_a && a_button_last)
			a_button_last = LOW;
		else if(button_a && !a_button_last) {
			a_button_last = HIGH;
			LCDML.BT_quit();
		}

    if(!button_b && b_button_last)
      b_button_last = LOW;
    else if(button_b && !b_button_last) {
      b_button_last = HIGH;
      LCDML.BT_enter();
    }
	}
	byte button_byte = get_button_byte();
	if(button_byte != last_button_byte){
		for (int i = 0; i <= 8; i++) {
						if ((bitRead(button_byte, i)) && (!bitRead(last_button_byte, i)))
										bitWrite(binary_guess, i, !bitRead(binary_guess, i));
		}
		LCDML.BT_enter();
		last_button_byte = button_byte;
	}

}
