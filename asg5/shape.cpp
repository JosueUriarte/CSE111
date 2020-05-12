// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>

using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

void* get_fontcode(const string& font_name){
   auto findFont = fontcode.find(font_name);
   return findFont->second;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{0, 0}, {width, 0},
   {width, height}, {0, height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

equilateral::equilateral(GLfloat width) :
   triangle({ {-width/2, 0}, {0, width/2*sqrtf(3)},
             {width/2, 0}}){
}

diamond::diamond (GLfloat width, GLfloat height): 
   polygon({ {-width/2, 0}, {0, height/2},
             {width/2, 0}, {0, -height/2}}){
}

triangle::triangle(const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}


//Drawing shapes ----------------------------------------------------

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   auto text = reinterpret_cast<const GLubyte*> (textdata.c_str());

   if(window::selected()){
      glColor3ubv((rgbcolor("white")).ubvec);
   }

   else{
      glColor3ubv(color.ubvec);
   }

   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, text);

   window::set_text(true);

   glEnd();
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);

   const float delta = 2 * M_PI / 32;

   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos/2 * cos (theta) + center.xpos;
      float ypos = dimension.ypos/2 * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }

   glEnd();

   if(window::selected()){

      glLineWidth(window::get_thickness());
      glBegin(GL_LINE_LOOP);
      glEnable(GL_LINE_SMOOTH);
      glColor3ubv(rgbcolor(window::get_color()).ubvec);

      for (float theta = 0; theta < 2 * M_PI; theta += delta){
         float xpos = dimension.xpos/2 * cos (theta) + center.xpos;
         float ypos = dimension.ypos/2 * sin (theta) + center.ypos;
         glVertex2f (xpos, ypos);
      }

      glutPostRedisplay();
      glEnd();
   }

}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   float xpos;
   float ypos;

   float xavg;
   float yavg;

   for(const auto& vertex: vertices){
      xavg += vertex.xpos;
      yavg += vertex.ypos;
   }

   xavg /= vertices.size();
   yavg /= vertices.size();

   glBegin (GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);

   for(auto itor = vertices.cbegin(); itor != vertices.cend(); ++itor){
      xpos = center.xpos + itor->xpos - xavg;
      ypos = center.ypos + itor->ypos - yavg;
      glVertex2f(xpos, ypos);
   }

   glEnd();

   if(window::selected()){

      glLineWidth(window::get_thickness());
      glBegin(GL_LINE_LOOP);
      glEnable(GL_LINE_SMOOTH);
      glColor3ubv(rgbcolor(window::get_color()).ubvec);

      for(auto itor = vertices.cbegin(); 
                           itor != vertices.cend(); ++itor){

         xpos = center.xpos + itor->xpos - xavg;
         ypos = center.ypos + itor->ypos - yavg;
         glVertex2f(xpos, ypos);
      }

      glEnd();

   }

}

//Dont Touch ------------------------------------------------------

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

