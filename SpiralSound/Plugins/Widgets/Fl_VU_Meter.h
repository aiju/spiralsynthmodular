#ifndef _Fl_VU_Meter_H_
#define _Fl_VU_Meter_H_

#include <FL/Fl_Progress.H>

class Fl_VU_Meter : public Fl_Progress {
  public:
    FL_EXPORT Fl_VU_Meter (int x, int y, int w, int h, const char *l = 0);
    const bool vu_mode (void) { return m_VUMode; };
    void vu_mode (bool m) { m_VUMode=m; };
  protected:
    FL_EXPORT virtual void draw();
  private:
    bool m_VUMode;
};

#endif
