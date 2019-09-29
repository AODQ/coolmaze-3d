#ifndef NETPBM_H
#define NETPBM_H

typedef struct {
  unsigned int w, h;
  unsigned char * rgba;
} image_t;

image_t * ppm_read(const char * path, unsigned char * alpha);
image_t * pbm_read(const char * path, unsigned char * fore, unsigned char * back);

void img_flip(image_t * img);
void img_delete(image_t * img);

#endif

