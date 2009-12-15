
#include "model.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Parse next line routine */
int parse_nextline(FILE *fp, char* buffer, int maxbuffer, int first);

#define MAX_LINE_LEN 512
#define DELIMS " ,"

char *next_str(char *s, char *delim)
{
  int i, n;

  if((s == NULL) || (delim == NULL))
    return s;
  
  if(s[0] == '\0')
    return s;
  
  n = strlen(delim);
  do {
    s++;
    for(i=0;i<n;i++)
      if(s[0] == delim[i])
	n = -1;
  }while((s[0] != '\0') && (n >= 0));
  
  return s;
}

#define MAX_ARGS 10

/* Like strtok, this modifies its arguments (only a little nasty) */
char **split_args(char *s, char *delims, int *nargs)
{
  static char* args[MAX_ARGS];
  int i, j, n, found;

  n = strlen(delims);

  args[0] = s;
  i = 1;
  for(i=1;i<MAX_ARGS;i++) {
    args[i] = next_str(args[i-1], delims);
    
    if(args[i][0] == '\0') {
      *nargs = i;
      break;
    }
    args[i][0] = '\0'; /* Terminate the previous arg */

    /* Make sure this argument doesn't start with a delimiter */
    do {
      args[i]++;
      /* Check if this is also a delimiter */
      found = 0; 
      for(j=0;j<n;j++)
	if(args[i][0] == delims[j]) {
	  found = 1;
	  break;
	}
      if(!found)
	break; /* This character not a delimiter */
    }while(args[i][0] != '\0');
    
    if(args[i][0] == '\0') {
      *nargs = i;
      break;
    }
  }
  *nargs = i;
  return args;
}

/* Color lookup tables */

typedef struct {
  char *name;
  float r, g, b;
}TCTItem;

TCTItem color_table[] = { {"WHITE", 1.0, 1.0, 1.0}, 
			  {"BLACK", 0.0, 0.0, 0.0},
			  {"RED",   1.0, 0.0, 0.0},
			  {"GREEN", 0.0, 1.0, 0.0},
			  {"BLUE",  0.0, 0.0, 1.0},
			  {NULL}};

int find_color(char *name, TColor *color)
{
  int i = 0;
  do {
    if(strcmp(name, color_table[i].name) == 0) {
      /* Found! */
      color->r = color_table[i].r;
      color->g = color_table[i].g;
      color->b = color_table[i].b;
      return 0;
    }
    i++;
  }while(color_table[i].name != NULL);
  return 1; /* Not found */
}

enum NUM_OP {NUM_INVALID, NUM_SET, NUM_ADD, NUM_SUB, NUM_MUL, NUM_DIV};

/* Convert a string to a number, returning operation at the start */
enum NUM_OP str_to_num(char *s, float *val) {
  enum NUM_OP op;
  
  switch(s[0]) {
  case '+': op = NUM_ADD; break;
  case '-': op = NUM_SUB; break;
  case '*': op = NUM_MUL; break;
  case '/': op = NUM_DIV; break;
  default: {
    op = NUM_SET;
    s--;
  }
  }
  s++;
  
  if(sscanf(s, "%f", val) != 1) {
    return NUM_INVALID;
  }
  return op;
}

TModelItem *model_additem(TModel *model)
{
  if(model->nitems <= 0) {
    model->item = (TModelItem*) malloc(sizeof(TModelItem));
    model->nitems = 1;
  }else {
    model->nitems++;
    model->item = (TModelItem*) realloc(model->item, sizeof(TModelItem)*model->nitems);
  }
  
  if(model->item == NULL) {
    /* Memory allocation failed */
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
  }
  
  return model->item + (model->nitems-1);
}

#define CHECK_SETTING(buffer, name, n)  \
  if(strncmp(buffer, name, n) != 0) {    \
    fprintf(stderr, "Line %d: Unknown setting '%s'. Should be '%s'?\n", \
      linenr, buffer, name); \
    break; \
  }

#define GET_VALUE(an, dest, defval, errfmt)     \
  op = str_to_num(args[an], &val);           \
  switch(op) {                              \
  case NUM_SET: dest = val; break;          \
  case NUM_ADD: dest = defval + val; break; \
  case NUM_SUB: dest = defval - val; break; \
  case NUM_MUL: dest = defval * val; break; \
  case NUM_DIV: dest = defval / val; break; \
  default: {                                \
    fprintf(stderr, errfmt, linenr);        \
    break;                                  \
  }                                         \
  }

int model_load(TModel *model, char *filename)
{
  FILE *fp;
  TModelItem def;
  TModelItem *item;
  int n;
  
  char buffer[MAX_LINE_LEN];
  int linenr;
  char **args;
  int nargs;

  enum NUM_OP op;
  float val;

  if(filename == NULL) {
    fprintf(stderr,"Error: NULL filename passed to model_load\n");
    return 1;
  }
  if((fp = fopen(filename, "rt")) == NULL) {
    fprintf(stderr,"Error: Couldn't read file '%s'\n", filename);
    return 1;
  }
  
  item = &def; /* Initially setting defaults */
  memset(item, 0, sizeof(TModelItem)); /* Zero everything */

  /* Set some sensible defaults */
  def.number = 20;
  def.color.alpha = 1.0;
  def.phi0 = 0.0; def.phi1 = 2.*PI;
  
  model->nitems = 0;
  
  linenr = parse_nextline(fp, buffer, MAX_LINE_LEN-1, 1);
  if(linenr == -1) {
    return 1;
  }
  do {
    /* Special case of NAME */
    
    if(strncmp(buffer, "NAME", 4) == 0) {
      printf("Reading %s: %s\n", filename, buffer+4);
      continue;
    }
    
    /* Split the arguments */
    args = split_args(buffer, DELIMS, &nargs);

    /* buffer now contains the first word and args[1...] the arguments */
    n = strlen(buffer);
    
    switch(buffer[0]) {
      /* Test for a new model items */
    case 'S': {
      CHECK_SETTING(buffer, "SOLID", n);
      
      item = model_additem(model);
      memcpy(item, &def, sizeof(TModelItem)); /* Copy the defaults */
      item->type = DRAW_SOLID;
      
      break;
    }
    case 'L': {
      CHECK_SETTING(buffer, "LINES", n);
      
      item = model_additem(model);
      memcpy(item, &def, sizeof(TModelItem));
      item->type = DRAW_LINE;
      break;
    }
    case 'P': {
      if(buffer[1] == 'L') {
	CHECK_SETTING(buffer, "PLANES", n);
      
	item = model_additem(model);
	memcpy(item, &def, sizeof(TModelItem));
	item->type = DRAW_PLANES;
      }else {
	CHECK_SETTING(buffer, "PITCH", n);
	
	if(nargs != 3) {
	  fprintf(stderr, "Line %d: Syntax is 'PITCH <integer> <integer> ' e.g. 'PITCH 1 3'\n", linenr);
	  break;
	}
	
	if( (sscanf(args[1], "%d", &item->m) != 1) || (sscanf(args[2], "%d", &item->n) != 1) ) {
	  fprintf(stderr, "Line %d: Syntax is 'PITCH <integer> <integer> ' e.g. 'PITCH 1 3'\n", linenr);
	}
      }
      break;
    }
      /* Now check for settings */
    case 'A': { /* ALPHA */
      CHECK_SETTING(buffer, "ALPHA", n);
      
      if(nargs != 2) {
	fprintf(stderr, "Line %d: Syntax is 'ALPHA <number>' e.g. 'ALPHA 0.5'\n", linenr);
	break;
      }
      GET_VALUE(1, item->color.alpha, def.color.alpha, 
		"Line %d: Syntax is 'ALPHA <number>' e.g. 'ALPHA 1.0'\n");
      break;
    }
    case 'C': {
      CHECK_SETTING(buffer, "COLOR", n);
      if((nargs != 2) && (nargs != 4)) {
	fprintf(stderr, "Line %d: Syntax is 'COLOR <name>' or 'COLOR <number> <number> <number>'\n", linenr);
	break;
      }
      if(nargs == 2) {
	/* Look up color name */
	if(find_color(args[1], &item->color)) {
	  fprintf(stderr, "Line %d: Color name '%s' not known\n", linenr, args[1]);
	  break;
	}
      }else {
	/* Get RGB values */
	GET_VALUE(1, item->color.r, def.color.r, 
		  "Line %d: Syntax is 'COLOR <name>' or 'COLOR <number> <number> <number>'\n");
	
	GET_VALUE(2, item->color.g, def.color.g, 
		  "Line %d: Syntax is 'COLOR <name>' or 'COLOR <number> <number> <number>'\n");
	
	GET_VALUE(3, item->color.b, def.color.b, 
		  "Line %d: Syntax is 'COLOR <name>' or 'COLOR <number> <number> <number>'\n");
      }
      
      break;
    }
    case 'E': {
      CHECK_SETTING(buffer, "ELONGATION", n);
      if(nargs != 2) {
	fprintf(stderr, "Line %d: Syntax is 'ELONGATION <number>' e.g. 'ELONGATION 1.0'\n", linenr);
	break;
      }
      GET_VALUE(1, item->elongation, def.elongation, 
		"Line %d: Syntax is 'ELONGATION <number>' e.g. 'ELONGATION 1.0'\n");
      break;
    }
    case 'M': {
      if(buffer[1] == 'A') {
	CHECK_SETTING(buffer, "MAJOR", n);
	
	if(nargs != 2) {
	  fprintf(stderr, "Line %d: Syntax is 'MAJOR <number>' e.g. 'MAJOR 3.0'\n", linenr);
	  break;
	}
	GET_VALUE(1, item->major_radius, def.major_radius, 
		  "Line %d: Syntax is 'MAJOR <number>' e.g. 'MAJOR 3.0'\n");
      }else {
	CHECK_SETTING(buffer, "MINOR", n);
	if(nargs != 2) {
	  fprintf(stderr, "Line %d: Syntax is 'MINOR <number>' e.g. 'MINOR 1.0'\n", linenr);
	  break;
	}
	GET_VALUE(1, item->minor_radius, def.minor_radius, 
		  "Line %d: Syntax is 'MINOR <number>' e.g. 'MINOR 1.0'\n");
      }
      break;
    }
    case 'N': {
      CHECK_SETTING(buffer, "NUMBER", n);
      if(nargs != 2) {
	fprintf(stderr, "Line %d: Syntax is 'NUMBER <integer>' e.g. 'NUMBER 10'\n", linenr);
	break;
      }
      
      /* Get an integer */
      if(sscanf(args[1], "%d", &item->number) != 1) {
	fprintf(stderr, "Line %d: Syntax is 'NUMBER <integer>' e.g. 'NUMBER 10'\n", linenr);
	break;
      }
      break;
    }
    case 'R': {
      CHECK_SETTING(buffer, "RANGE", n);
      if(nargs != 3) {
	fprintf(stderr, "Line %d: Syntax is 'RANGE <angle0> <angle1>' e.g. 'RANGE 0 180'\n", linenr);
	break;
      }
      op = str_to_num(args[1], &val);
      switch(op) {
      case NUM_SET: item->phi0 = val*PI/180.; break;
      case NUM_ADD: item->phi0 = def.phi0 + val*PI/180.; break;
      case NUM_SUB: item->phi0 = def.phi0 - val*PI/180.; break;
      case NUM_MUL: item->phi0 = def.phi0 * val; break;
      case NUM_DIV: item->phi0 = def.phi0 / val; break;
      default: {
	fprintf(stderr, "Line %d: Syntax is 'RANGE <angle0> <angle1>' e.g. 'RANGE 0 180'\n", linenr);
	break;
      }                  
      }
      op = str_to_num(args[2], &val);
      switch(op) {
      case NUM_SET: item->phi1 = val*PI/180.; break;
      case NUM_ADD: item->phi1 = def.phi1 + val*PI/180.; break;
      case NUM_SUB: item->phi1 = def.phi1 - val*PI/180.; break;
      case NUM_MUL: item->phi1 = def.phi1 * val; break;
      case NUM_DIV: item->phi1 = def.phi1 / val; break;
      default: {
	fprintf(stderr, "Line %d: Syntax is 'RANGE <angle0> <angle1>' e.g. 'RANGE 0 180'\n", linenr);
	break;
      }                  
      }
      break;
    }
    case 'T': {
      CHECK_SETTING(buffer, "TRIANGULARITY", n);
      if(nargs != 2) {
	fprintf(stderr, "Line %d: Syntax is 'TRIANGULARITY <number>' e.g. 'TRIANGULARITY 0.2'\n", linenr);
	break;
      }
      GET_VALUE(1, item->triangularity, def.triangularity,
		"Line %d: Syntax is 'TRIANGULARITY <number>' e.g. 'TRIANGULARITY 0.2'\n");
      break;
    }
    default: {
      fprintf(stderr, "Line %d: Unknown command '%s'\n", linenr, buffer);
    }
    }
  }while((linenr = parse_nextline(fp, buffer, MAX_LINE_LEN-1, 0)) != -1);

  /* Close the file */
  fclose(fp);

  return 0;
}

int model_save(TModel *model, char *filename)
{
  return 1;
}

void model_free(TModel *model)
{
  if(model == NULL)
    return;
  
  if(model->nitems > 0) {
    free(model->item);
  }
  model->nitems = 0;
}

