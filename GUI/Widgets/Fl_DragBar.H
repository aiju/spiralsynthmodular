// generated by Fast Light User Interface Designer (fluid) version 1.0010

#ifndef Fl_DragBar_H
#define Fl_DragBar_H
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>

class Fl_DragBar : public Fl_Widget {
  int old_rx,old_ry;
  int _type;
public:
  enum DragType {WINDRAG=0,NICEWINDRAG,FLDRAG,NICEFLDRAG};
  Fl_DragBar(int x,int y,int w,int h,const char *l=0);
  ~Fl_DragBar();
private:
  void draw();
  int handle(int event);
public:
  void type(int t);
};
#endif
