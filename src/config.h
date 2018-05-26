#define DISP_w                 128            // LCD width
#define DISP_h                 64             // LCD height
#define DISP_font              u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
#define DISP_font_w            6              // font width
#define DISP_font_h            13             // font height
#define DISP_cursor_char       ">"            // cursor char
#define DISP_cur_space_before  2              // cursor space between
#define DISP_cur_space_behind  4              // cursor space between
#define DISP_box_x0            0              // start point (x0, y0)
#define DISP_box_y0            0              // start point (x0, y0)
#define DISP_box_x1            128            // width x  (x0 + width)
#define DISP_box_y1            64             // hight y  (y0 + height)
#define DISP_scrollbar_w       6  // scrollbar width (if this value is < 3, the scrollbar is disabled)
#define DISP_cols_max          ((DISP_box_x1-DISP_box_x0)/DISP_font_w)
#define DISP_rows_max          ((DISP_box_y1-DISP_box_y0-((DISP_box_y1-DISP_box_y0)/DISP_font_h))/DISP_font_h)
#define DISP_rows              DISP_rows_max  // max rows
#define DISP_cols              20                   // max cols
