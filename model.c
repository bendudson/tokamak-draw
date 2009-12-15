
#include "model.h"

#include <stdio.h>
#include <string.h>

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

#define CHECK_SETTING(buffer, name, n)  \
  if(strncmp(buffer, name, n) != 0) {    \
    fprintf(stderr, "Line %d: Unknown setting '%s'. Should be '%s'?\n", \
      linenr, buffer, name); \
    break; \
  }

int model_load(TModel *model, char *filename)
{
  FILE *fp;
  TModelItem def;
  TModelItem *item;
  int n;
  
  char buffer[MAX_LINE_LEN];
  int linenr;
  char *str;

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
  
  model->nitems = 0;
  
  linenr = parse_nextline(fp, buffer, MAX_LINE_LEN-1, 1);
  if(linenr == -1) {
    return 1;
  }
  do {
    str = next_str(buffer, DELIMS);
    str[0] = '\0';
    str++;
    /* buffer now contains the first word and str the arguments */
    n = strlen(buffer);
    
    switch(buffer[0]) {
      /* Test for a new model items */
    case 'S': {
      CHECK_SETTING(buffer, "SOLID", n);
      break;
    }
    case 'L': {
      CHECK_SETTING(buffer, "LINES", n);
      break;
    }
    case 'P': {
      CHECK_SETTING(buffer, "PLANES", n);
      break;
    }
      /* Now check for settings */
    case 'A': { /* ALPHA */
      CHECK_SETTING(buffer, "ALPHA", n);
      break;
    }
    case 'C': {
      CHECK_SETTING(buffer, "COLOR", n);
      break;
    }
    case 'E': {
      CHECK_SETTING(buffer, "ELONGATION", n);
      break;
    }
    case 'M': {
      if(buffer[1] == 'A') {
	CHECK_SETTING(buffer, "MAJOR", n);
      }else {
	CHECK_SETTING(buffer, "MINOR", n);
      }
      break;
    }
    }
  }while((linenr = parse_nextline(fp, buffer, MAX_LINE_LEN-1, 0)) != -1);

  return 0;
}

int model_save(TModel *model, char *filename)
{
  return 1;
}


