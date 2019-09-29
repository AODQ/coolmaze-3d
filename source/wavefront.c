#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <GL/gl.h>
#include "wavefront.h"
#include "glfns.h"


model_t * obj_read(const char * path)
{
  FILE * fp = fopen(path, "r");
  if(!fp) return NULL;
  size_t v_cap = 32, f_cap = 32;
  model_t * obj = malloc(sizeof(model_t));
  obj->n_verts = 0;
  obj->n_tris = 0;
  obj->verts = malloc(sizeof(vertex_t) * v_cap);
  obj->tris = malloc(sizeof(triangle_t) * f_cap);
  char buf[80];
  int len = 80;
  while(fgets(buf, len, fp)){
    char * str = strtok(buf, " ");
    if(strcmp(str, "v") == 0){
      vertex_t * vp = &(obj->verts[obj->n_verts]);
      str = strtok(NULL, "\n");
      sscanf(str, "%f %f %f", &(vp->x), &(vp->y), &(vp->z));
      if(obj->n_verts > v_cap /2){
        v_cap *= 2;
        obj->verts = realloc(obj->verts, sizeof(vertex_t) * v_cap);
      }
      obj->n_verts++;
    }
    else if(strcmp(str, "f") == 0){
      triangle_t * vp = &(obj->tris[obj->n_tris]);
      str = strtok(NULL, "\n");
      sscanf(str, "%d %d %d", vp->verts, vp->verts+1, vp->verts+2);
      for(int i = 0; i<3; i++) {vp->verts[i]--;}
      if(obj->n_tris > f_cap /2){
        f_cap *= 2;
        obj->tris = realloc(obj->tris, sizeof(triangle_t) * f_cap);
      }
      obj->n_tris++;
    }
  }
  printf("%d %d\n", obj->n_verts, obj->n_tris);
  fclose(fp);
  return obj;
}

void obj_delete(model_t * obj)
{
  free(obj->verts);
  free(obj->tris);
  free(obj);
}

