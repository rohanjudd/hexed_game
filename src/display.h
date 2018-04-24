#include <U8g2lib.h>

namespace display
{
//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
void initialise();
void splash_text(String s);
void hexagon_test();
void draw_hexagon(byte x, byte y, byte radius);
void update_screen(String t, String g);
}
