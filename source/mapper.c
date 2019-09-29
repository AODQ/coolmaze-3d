#include <stdlib.h> 
#include <stdio.h> 
#include <forms.h> 

int main(int argc, char **argv){ //First form
  FL_FORM *simple;
  FL_OBJECT *w, *h, *sub;

  //Second form
  int width, height;
  FL_FORM *mapper, *bform;
  FL_OBJECT *name, *browser, **buttons, *start[2], *end[2], *save;

  fl_initialize(&argc, argv, "Demo", 0, 0);

  simple = fl_bgn_form(FL_UP_BOX, 230, 160);
    w = fl_add_input(FL_INT_INPUT, 40, 20, 80, 40, "width");
    h = fl_add_input(FL_INT_INPUT, 40, 60, 80, 40, "height");
    sub = fl_add_button(FL_NORMAL_BUTTON, 40, 120, 150, 60, "submit");
  fl_end_form();

  fl_show_form(simple, FL_PLACE_SIZE, FL_FULLBORDER, "Mapper");
  while(1){
    FL_OBJECT *obj = fl_do_forms();
    if(obj == sub
    && (width = strtol(fl_get_input(w), NULL, 10)) != 0 && width < 256
    && (height = strtol(fl_get_input(h), NULL, 10)) != 0 && height < 256){
      printf("%d, %d\n", width, height);
      buttons = malloc(sizeof(FL_OBJECT *) * width * height);

      #define SZ 16
      bform = fl_bgn_form(FL_UP_BOX, width*SZ, height*SZ);
      for(int i = 0; i < width*height; i++){
        buttons[i] = fl_add_button(
          FL_PUSH_BUTTON,
          SZ*(i%width), SZ*(i/width),
          SZ, SZ,
          NULL
        );
      }
      fl_end_form();
      #undef SZ

      break;
    }
  }
  fl_hide_form(simple);

  mapper = fl_bgn_form(FL_UP_BOX, 640, 480);
    browser = fl_add_formbrowser(FL_NORMAL_FORMBROWSER, 0, 0, 400, 400, NULL);
    fl_addto_formbrowser(browser, bform);
    start[0] = fl_add_input(FL_INT_INPUT, 460, 10, 60, 30, "Start");
    start[1] = fl_add_input(FL_INT_INPUT, 520, 10, 60, 30, NULL);
    end[0] = fl_add_input(FL_INT_INPUT, 460, 50, 60, 30, "End");
    end[1] = fl_add_input(FL_INT_INPUT, 520, 50, 60, 30, NULL);
    name = fl_add_input(FL_NORMAL_INPUT, 460, 100, 120, 30,  "name");
    save = fl_add_button(FL_NORMAL_BUTTON, 460, 150, 120, 60, "submit");
  fl_end_form();

  fl_show_form(mapper, FL_PLACE_SIZE, FL_FULLBORDER, "Mapper");
  while(1){
    FL_OBJECT *obj = fl_do_forms();
    if(obj == save){
      FILE* fp = fopen(fl_get_input(name), "wb");
      fputs("BAD1", fp);
      fputc((unsigned char)width, fp);
      fputc((unsigned char)height, fp);
      fputc((unsigned char)strtol(fl_get_input(start[0]), NULL, 10), fp);
      fputc((unsigned char)strtol(fl_get_input(start[1]), NULL, 10), fp);
      fputc((unsigned char)strtol(fl_get_input(end[0]), NULL, 10), fp);
      fputc((unsigned char)strtol(fl_get_input(end[1]), NULL, 10), fp);
      for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j+=8){
          unsigned char c = 0;
          for(int k = 0; k < (j+8 > width ? width - j : 8); k++){// TODO  
            c <<= 1;
            c |= fl_get_button(buttons[(i*width) + j + k]) ? 0 : 0x01 ;
          }
          c <<= (j > width ? j - width : 0);
          fputc(c, fp);
        }
      }
      break;
    }
  }
  fl_hide_form(mapper);
  return 0;
}

