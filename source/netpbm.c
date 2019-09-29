#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "netpbm.h"

void _consume_comment(FILE* fp)
{
  int lead = fgetc(fp);
  if(lead == '#'){
  }
  else {
    ungetc(lead, fp);
  }
}

image_t * ppm_read(const char * path, unsigned char * alpha)
{
  FILE * fp = fopen(path, "r");
  if(!fp) return NULL;
  char header[2];
  int status = fread(header, 1, 2, fp);
  if(status<2 || header[0] != 'P' || header[1] != '6') return NULL;
  image_t * img = malloc(sizeof(image_t));
  fscanf(fp, "%u\n", &(img->w));
  fscanf(fp, "%u\n", &(img->h));
  fscanf(fp, "%i\n", &status);
  img->rgba = malloc(4*img->w * img->h);
  for(unsigned int i = 0; i < img->w*img->h; i++){
    fread(img->rgba + i*4, 3, 1, fp);
    img->rgba[i*4 + 3] = 255;
  }
  fclose(fp);
  return img;
}

image_t * pbm_read(const char * path, unsigned char * fore, unsigned char * back)
{
  FILE * fp = fopen(path, "rb");
  if(!fp) return NULL;
  char header[2];
  int status = fread(header, 1, 2, fp);
  if(status < 2 || header[0] != 'P' || header[1] != '4') return NULL;
  image_t * img = malloc(sizeof(image_t));
  fscanf(fp, "%u %u ", &(img->w), &(img->h));
  //fscanf(fp, "%u\n", &(img->w));
  //fscanf(fp, "%u\n", &(img->h));
  img->rgba = malloc(4*img->w * img->h);
  for(unsigned int i = 0; i < img->h; i++){
    for(unsigned int j = 0; j < img->w; j+=8){
      int chunk = fgetc(fp);
      //TODO account for non byte aligned
      for(unsigned int off = 0; off < 8; off++){
        memcpy(
          img->rgba + (i*img->w + j + off)*4,
          (chunk & 0x80) ? fore : back,
          4
        );
        chunk <<= 1;
      }
    }
  }
  //PRAY THERES NO WHITESPASEC
  fclose(fp);
  return img;
}

void img_flip(image_t * img)
{
  for(int i = 0; i < img->h / 2; i++){
    for(int j = 0; j < img->w; j++){
    /*
      unsigned int tmp[4];
      memcpy(tmp, img->rgba + (i*img->w + j)*4, 4*sizeof(float));
      memcpy(img->rgba + (i*img->w + j)*4, img->rgba + (img->h - i - 1), 4*sizeof(float));
      memcpy(img
      */
      for(int k = 0; k < 4; k++){
        unsigned int tmp = img->rgba[4*(i*img->w + j)+k];
        img->rgba[4*(i*img->w + j)+k] = img->rgba[4*((img->h - i - 1)*img->w + j)+k];
        img->rgba[4*((img->h - i - 1)*img->w + j) + k] = tmp;
      }
    }
  }
}
  
void img_delete(image_t * ppm)
{
  free(ppm->rgba);
  free(ppm);
}

