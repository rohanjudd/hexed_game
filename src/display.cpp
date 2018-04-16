#include "display.h"

Display::Display()
{
}

Display::initialise()
{
        u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
        u8g2.begin();
        u8g2.setContrast(0);
        u8g2.setFont(u8g2_font_inb16_mr); // choose a suitable font
}

void Display::splash_text(String s)
{
        u8g2.clearBuffer(); // clear the internal memory
        u8g2.drawStr(0,18,"Hexed"); // write something to the internal memory
        u8g2.sendBuffer(); // transfer internal memory to the display
        delay(800);
}

void Display::hexagon_test()
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

void Display::draw_hexagon(byte x, byte y, byte radius)
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

void Display::update_screen(String t, String g)
{
        u8g2.clearBuffer();   // clear the internal memory
        u8g2.setCursor(0, 18);
        u8g2.print(t);
        u8g2.setCursor(0, 40);
        u8g2.print(g);
        u8g2.sendBuffer();
}
