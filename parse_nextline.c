/*********************************************************************************
 * Low-level code to parse a script file
 * Generic code.
 *
 * MIT LICENSE:
 *
 * Copyright (c) 2006 B.Dudson, UKAEA Fusion and Oxford University
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Changelog: 
 *
 * October 2006: Initial release by Ben Dudson, UKAEA Fusion/Oxford University
 *
 ***********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Returns the next useful line from a file */
int parse_nextline(FILE *fp, char* buffer, int maxbuffer, int first)
{
  int i, n, started, p, space, quote;
  static int linenr = 0;
  
  if(first)
    linenr = 0;

  do{
    buffer[0] = 0;
    /* Get a line from the file */
    fgets(buffer, maxbuffer-1, fp);
    buffer[maxbuffer-1] = 0; /* ensure always have terminating zero */
    linenr++;
    
    n = strlen(buffer);
    /* strip out comments and leading whitespace.
       Reduce any whitespace to a single space.
       Capitalize everything not in quotations
    */
    started = 0;
    space = 0;
    p = 0;
    quote = 0;
    for(i=0;i<n;i++) {
      if(buffer[i] == '#') {
	/* Start of a comment - ignore the rest of the line */
	buffer[p] = 0;
	i = n;
      }else {
	if(started == 0) {
	  if(isalnum(buffer[i])) {
	    started = 1;
	    buffer[p] = toupper(buffer[i]);
	    p++;
	  }
	}else {
	  if(isspace(buffer[i])) {
	    /* white space */
	    if(space == 0) {
	      /* start of some white space */
	      buffer[p] = ' ';
	      p++;
	      space = 1;
	    }
	  }else {
	    /* a useful character */
	    space = 0; /* no longer part of white space */
	    buffer[p] = buffer[i];

	    if(buffer[i] == '"') {
	      if(quote == 0) {
		quote = 1;
	      }else if(quote == 1) {
		quote = 0;
	      }
	    }else if(buffer[i] == '\'') {
	      if(quote == 0) {
		quote = 2;
	      }else if(quote == 2) {
		quote = 0;
	      }
	    }else {
	      if(quote == 0)
		buffer[p] = toupper(buffer[p]);
	    }
	    p++;
	  }
	}
      }
    }
    buffer[p] = 0;
    n = p;
  }while((feof(fp) == 0) && (n == 0));
  if(n == 0) {
    return(-1);
  }
  return(linenr);
}
