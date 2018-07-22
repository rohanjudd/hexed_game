#include <U8g2lib.h>

namespace display
{
//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
void initialise();
void set_contrast(byte b);
void splash_text(String s);
void menu_screen_init();
void menu_screen();
void game_screen_init();
void game_screen(String target, String guess, int score);
void screensaver_init();
void battery_screen(float voltage, boolean charge_state);
void sound_demo_screen_init();
void sound_demo_screen(byte index);
void save_settings_screen();
void show_dynamic_value(const char* name, byte val, byte line);
void highlight_choice(byte cursor_pos);
void hexagon_test();
void draw_hexagon(byte x, byte y, byte radius);
void drawString(unsigned char x, unsigned char y, const char* s);
void update_screen(String target, String guess, int score);
void firstPage();
boolean nextPage();
}
