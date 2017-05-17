#include <FL/Fl.H>
#include "Fl_VU_Meter.h"
#include <FL/fl_draw.H>

Fl_VU_Meter::Fl_VU_Meter (int x, int y, int w, int h, const char *l) :
Fl_Progress (x, y, w, h, l) {
}

void Fl_VU_Meter::draw() {
     int progress, bx, by, bw, bh, tx, tw;

     bx = Fl::box_dx (box());
     by = Fl::box_dy (box());
     bw = Fl::box_dw (box());
     bh = Fl::box_dh (box());
     tx = x() + bx;
     tw = w() - bw;

     if (maximum() > minimum())
        progress = (int)(tw * (value() - minimum()) / (maximum() - minimum()) + 0.5f);
     else progress = 0;

     draw_box (box(), x(), y(), w(), h(), color());

     if (progress > 0) {
        int block_width = w() / 16;
        int block = 0;
        for (int block_pos = 0; block_pos < progress; block_pos += block_width) {
            Fl_Color colour;
            if (m_VUMode) {
               if (++block == 16) colour = FL_RED;
               else if (block > 10) colour = FL_YELLOW;
               else colour = FL_GREEN;
            }
            else colour = FL_GREEN;
            if (!active_r()) colour = fl_inactive (colour);
            fl_push_clip (x() + block_pos, y(), block_width - 2, h());
            draw_box (box(), x(), y(), w(), h(), colour);
            fl_pop_clip();
	}
     }
     // we don't care about the label
     // draw_label (tx, y() + by, tw, h() - bh);
}
