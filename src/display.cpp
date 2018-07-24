#include "display.h"
namespace display
{
#include "config.h"

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
void initialise()
{
        u8g2.begin();
        u8g2.setContrast(0);
        u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
}

void set_contrast(byte b)
{
  u8g2.setContrast(b);
}

void power_save(bool b)
{
    u8g2.setPowerSave(b);
}

void menu_screen_init()
{
  u8g2.setFont(DISP_font);
  u8g2.setFontMode(1);  /* activate transparent font mode */
}

void menu_screen()
{

}

void game_screen_init()
{
  u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
}

void game_screen(String target, String guess, int score)
{
	u8g2.firstPage();
	do {
		u8g2.setDrawColor(1);
		u8g2.drawBox(0, 42, 128, 20);
		u8g2.setDrawColor(2);
    u8g2.setFont(DISP_font);
    u8g2.setCursor(60, 12);
    u8g2.print("score:");
		u8g2.setCursor(100, 12);
		u8g2.print(score);
    u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
		u8g2.setCursor(9, 40);
		u8g2.print(target);
		u8g2.setCursor(9, 60);
		u8g2.print(guess);
	} while( u8g2.nextPage() );
}

void screensaver_init()
{
  u8g2.firstPage();
  do {
  } while( u8g2.nextPage() );
}

void battery_screen(float v, boolean charge_state)
{
  char buf[20];
	sprintf (buf, "%.2fV", v);
  u8g2.firstPage();
  do {
    u8g2.drawStr( 0, (DISP_font_h * 1), buf);
    if(charge_state)
      u8g2.drawStr( 0, (DISP_font_h * 2), "Not Charging");
    else
      u8g2.drawStr( 0, (DISP_font_h * 2), "Charging");
  } while( u8g2.nextPage() );
}

void sound_demo_screen_init()
{
  u8g2.setFont(DISP_font);
}

void sound_demo_screen(byte index)
{
  char buf[20];
  sprintf (buf, "Sample %d", index);
  u8g2.firstPage();
  do {
    u8g2.drawStr( 0, (DISP_font_h * 1), buf);
  } while( u8g2.nextPage() );
}

void save_settings_screen()
{
  u8g2.setFont(DISP_font);
  u8g2.firstPage();
  do {
    u8g2.drawStr( 0, (DISP_font_h * 1), "Saving to EEPROM");
  } while( u8g2.nextPage() );
}

void show_dynamic_value(const char* name, byte val, byte line)
{
  char buf[20];
  sprintf (buf, name, val);
  u8g2.drawStr( DISP_box_x0+DISP_font_w + DISP_cur_space_behind,  (DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
}

void highlight_choice(byte cursor_pos)
{
  u8g2.setDrawColor(1);
  u8g2.drawBox(DISP_box_x0, DISP_box_y0 + 2 + DISP_font_h * cursor_pos, DISP_w , DISP_font_h + 1);
  u8g2.setDrawColor(2);
}

void splash_text(String s)
{
        u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
        u8g2.clearBuffer(); // clear the internal memory
        u8g2.setCursor(29, 36);
        u8g2.print(s);
        u8g2.sendBuffer(); // transfer internal memory to the display
        delay(500);
        for(int x=30; x>0; x--)
        {
        noise();
        u8g2.sendBuffer(); // transfer internal memory to the display
        delay(50);
        }
        u8g2.clearBuffer(); // clear the internal memory
        u8g2.sendBuffer(); // transfer internal memory to the display
}

void hexagon_test()
{
        u8g2.clearBuffer();
        for(int x=20; x<160; x+=20) {
                for(int y=20; y<44; y+=20) {
                        draw_hexagon(x,y,12);
                }
        }
        u8g2.sendBuffer();
        delay(2000);
}

void draw_hexagon(byte x, byte y, byte radius)
{
        boolean isFirst = true;
        byte lastX = 0;
        byte lastY = 0;
        float angle = 6.28 / 6.0;
        for (float a = 0; a < 6.28; a += angle) {
                byte sx = byte(round(x + cos(a) * radius));
                byte sy = byte(round(y + sin(a) * radius));
                if (!isFirst)
                        u8g2.drawLine(lastX, lastY, sx, sy);
                else
                        isFirst = false;
                lastX = sx;
                lastY = sy;
        }
}

void drawString(unsigned char x, unsigned char y, const char* s)
{
  u8g2.setDrawColor(2);
  u8g2.drawStr(x, y, s);
  u8g2.setDrawColor(1);
}

void firstPage()
{
  u8g2.firstPage();
}

boolean nextPage()
{
  return u8g2.nextPage();
}

void noise()
{
  u8g2.setDrawColor(0);
  for(int x=0; x < 128; x++)
  {
    for(int y=0; y < 64; y++)
    {
      if (!(int)random(10))
        u8g2.drawPixel(x,y);
    }
  }
  u8g2.setDrawColor(1);
}

}
