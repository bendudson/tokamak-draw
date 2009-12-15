/*****************************************************************
 *
 *****************************************************************/

#ifndef __MODEL_H__
#define __MODEL_H__

#include "tokamak_draw.h"

enum DrawType {DRAW_LINE, DRAW_SOLID, DRAW_PLANES};

typedef struct {
  enum DrawType type; 
  
  float major_radius;
  float minor_radius;
  float elongation; 
  float triangularity;
  
  int number;
  TColor color;
  
  int m, n;         /* Only used for field-lines */
  float phi0, phi1; /* Only for solid surfaces */
  
}TModelItem;

typedef struct {
  int nitems;
  TModelItem *item; /* Array of items (surfaces etc.) to plot */
}TModel;

int model_load(TModel *model, char *filename);
int model_save(TModel *model, char *filename);

void model_free(TModel *model);

#endif /* __MODEL_H__ */
