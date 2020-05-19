/* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.

iemlib written by Thomas Musil, Copyright (c) IEM KUG Graz Austria 2000 - 2009 */


#include "m_pd.h"
#include "iemlib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


/* ---------------------------- iem_pbank_csv ------------------------------- */
/* -- is a list storage and management object, can store an array of lists -- */
/* ------------------------------- as an csv file --------------------------- */

/* read and write method needs 2 symbols,
1. symbol is a filename,
2. symbol is a 2 character descriptor

1.char: 'b'...for blank as ITEM_SEPARATOR (" ")
1.char: 's'...for semicolon as ITEM_SEPARATOR (";")
1.char: 't'...for tabulator as ITEM_SEPARATOR ("	" = 0x09)

2.char: 'b'...for blank,return as END_OF_LINE (" \n")
2.char: 's'...for semicolon,return as END_OF_LINE (";\n")
2.char: 't'...for tabulator,return as END_OF_LINE ("     \n")
2.char: 'r'...for return-only as END_OF_LINE ("\n")

change: recall + offset + number
*/

#define IEMLIB2_DEBUG 0

#define IEM_PBANK_ITEM_SEPARATOR 0
#define IEM_PBANK_END_OF_LINE 1
#define IEM_PBANK_FORMAT_SIZE 2
#define IEM_PBANK_MALLOC_SIZE 4
#define IEM_PBANK_UNIFIED_RET 0x01
#define IEM_PBANK_UNIFIED_SEP 0x02
#define IEM_PBANK_UNIFIED_EOL 0x03

static t_class *iem_pbank_csv_class;

typedef struct _iem_pbank_csv
{
  t_object x_obj;
  int        x_nr_para;
  int        x_nr_line;
  int        x_line;// current line index for recall and store
  t_atom    *x_atbegmem;// atom-matrix with x_nr_line rows and x_nr_para columns
  t_atom    *x_atbegbuf;// atom-line-buffer for list-inputs (x_nr_para atoms)
  t_atom    *x_atbegout;// atom-line-buffer for list-outputs (x_nr_para+1 atoms, optional one leading zero)
  t_canvas  *x_canvas;
  t_outlet  *x_list_out;
  t_outlet  *x_offset_list_out;
} t_iem_pbank_csv;

#include "iem_pbank_csv_aux.h"

static void iem_pbank_csv_write(t_iem_pbank_csv *x, t_symbol *filename, t_symbol *format)
{
  char completefilename[MAXPDSTRING], eol[4], sep[4], formattext[100];
  int size, p, l, nrl=x->x_nr_line, nrp=x->x_nr_para;
  int state, max=nrl*nrp, org_size;
  FILE *fh;
  t_atom *ap=x->x_atbegmem;
  const char *fs;
  
  fs = filename->s_name;
  if(fs[0] == '/')// linux, OSX
    strcpy(completefilename, fs);
  else if( (((fs[0] >= 'A')&&(fs[0] <= 'Z')) || ((fs[0] >= 'a')&&(fs[0] <= 'z'))) && (fs[1] == ':') && (fs[2] == '/') )// windows, backslash becomes slash in pd
    strcpy(completefilename, fs);
  else
  {
    strcpy(completefilename, canvas_getdir(x->x_canvas)->s_name);
    strcat(completefilename, "/");
    strcat(completefilename, fs);
  }
  
  fh = fopen(completefilename,"wb");
  fs = format->s_name;
  if(!fh)
    post("iem_pbank_csv_write: cannot create %s !!\n", completefilename);
  else
  {
    l = discuss_sep_eol(fs, sep, eol, formattext);// dummy usage of l
    
    ap = x->x_atbegmem;
    for(l=0; l<nrl; l++)
    {
      for(p=1; p<nrp; p++)// (nrp - 1)-times: atom + SEP
      {
        if(IS_A_FLOAT(ap, 0))
          fprintf(fh, "%g%c", ap->a_w.w_float, *sep);
        else if(IS_A_SYMBOL(ap, 0))
          fprintf(fh, "%s%c", ap->a_w.w_symbol->s_name, *sep);
        ap++;
      }
      if(IS_A_FLOAT(ap, 0))// last time: atom + EOL
        fprintf(fh, "%g%s", ap->a_w.w_float, eol);
      else if(IS_A_SYMBOL(ap, 0))
        fprintf(fh, "%s%s", ap->a_w.w_symbol->s_name, eol);
      ap++;
    }
    fclose(fh);
    post("iem_pbank_csv: wrote %d parameters x %d lines to file:\n%s\nwith following format:\n%s\n", nrp, nrl, completefilename, formattext);
  }
}

static void iem_pbank_csv_read(t_iem_pbank_csv *x, t_symbol *filename, t_symbol *format)
{
  char completefilename[MAXPDSTRING], eol[4], sep[4], formattext[100], *txbuf, trncflld[100];
  int size, p, l, i, j, nrl=x->x_nr_line, nrp=x->x_nr_para, atlen=0, eolen=0, tf=0;
  int txlen, curtxlen, txalloc, max;
  FILE *fh;
  t_atom *ap, *at;
  const char *fs;
  
  fs = filename->s_name;
  if(fs[0] == '/')/*make complete path + filename*/
    strcpy(completefilename, filename->s_name);
	else if( (((fs[0] >= 'A')&&(fs[0] <= 'Z')) || ((fs[0] >= 'a')&&(fs[0] <= 'z'))) && (fs[1] == ':') && (fs[2] == '/') )
    strcpy(completefilename, filename->s_name);
  else
  {
    strcpy(completefilename, canvas_getdir(x->x_canvas)->s_name);
    strcat(completefilename, "/");
    strcat(completefilename, fs);
  }
   
  fs = format->s_name;
  fh = fopen(completefilename,"rb");
  if(!fh)
    post("iem_pbank_csv_read: cannot open %s !!\n", completefilename);
  else
  {
    eolen = discuss_sep_eol(fs, sep, eol, formattext);
    fseek(fh, 0, SEEK_END);
    txalloc = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    txbuf = (char *)getbytes((2 * txalloc + 256) * sizeof(char));
    
    if(fread(txbuf, sizeof(char), txalloc, fh) < sizeof(char)*txalloc)
      post("pbank.csv:435: warning read error (not specified)");
    fclose(fh);
    
    /* correct the returns, unify SEP and EOL, filter out unwanted char */
    txlen = iem_pbank_csv_text_filter(&txbuf, txalloc, sep, eol, eolen);
    
    /* clear atom-matrix to zero*/
    at = x->x_atbegmem;
    for(l=0; l<nrl; l++)
    {
      for(p=0; p<nrp; p++)
      {
        SETFLOAT(at, 0.0f);
        at++;
      }
    }
    
    tf = iem_pbank_csv_convert_text_to_atom_list(x, txbuf, txlen);
    
    /*   debug:
    at = x->x_atbegmem;
    post("debug2 atom-matrix %d x %d:\n", nrl, nrp);
    for(l=0; l<nrl; l++)
    {
      for(p=0; p<nrp; p++)
      {
        t_symbol *sy;
        
        if(at->a_type == A_FLOAT)
          post("%g", atom_getfloat(at));
        else if(at->a_type == A_SYMBOL)
        {
          sy = atom_getsymbol(at);
          post("%s", sy->s_name);
        }
        at++;
      }
    } */
    
    freebytes(txbuf, (2 * txalloc + 256) * sizeof(char));
    
    switch(tf)
    {
      case 0:
        strcpy(trncflld, "");
        break;
      case 1:
        strcpy(trncflld, " truncated");
        break;
      case 2:
        strcpy(trncflld, " zero-filled");
        break;
      case 3:
        strcpy(trncflld, " truncated and zero-filled");
        break;
      }
    
    post("iem_pbank_csv: read %d parameters x %d lines from%s file:\n%s\nwith following format:\n%s\n", nrp, nrl, trncflld, completefilename, formattext);
  }
}

static void iem_pbank_csv_recall(t_iem_pbank_csv *x, t_symbol *s, int ac, t_atom *av)
{
  int i, onset=0, nrp=x->x_nr_para;
  t_atom *atbuf=x->x_atbegbuf, *atmem=x->x_atbegmem, *atout=x->x_atbegout;
  
  if(ac >= 2) nrp = atom_getintarg(1, ac, av);
  if(ac >= 1) onset = atom_getintarg(0, ac, av);
  if(onset < 0) onset = 0;
  if(onset >= x->x_nr_para) onset = x->x_nr_para - 1;
  if(nrp < 0) nrp = 0;
  if((onset+nrp) > x->x_nr_para) nrp = x->x_nr_para - onset;
  
  atmem += (x->x_nr_para * x->x_line) + onset;
  atbuf += onset;
  SETFLOAT(atout, (t_float)onset);
  atout++;
  for(i=0; i<nrp; i++)
  {
    *atbuf++ = *atmem;
    *atout++ = *atmem++;
  }
  outlet_list(x->x_offset_list_out, &s_list, nrp+1, x->x_atbegout);
  outlet_list(x->x_list_out, &s_list, nrp, x->x_atbegout+1);
}

static void iem_pbank_csv_bang(t_iem_pbank_csv *x)
{
  int i, nrp=x->x_nr_para;
  t_atom *atbuf=x->x_atbegbuf;
  t_atom *atout=x->x_atbegout;
  
  SETFLOAT(atout, 0.0f);
  atout++;
  for(i=0; i<nrp; i++)
    *atout++ = *atbuf++;
  outlet_list(x->x_offset_list_out, &s_list, nrp+1, x->x_atbegout);
  outlet_list(x->x_list_out, &s_list, nrp, x->x_atbegout+1);
}

static void iem_pbank_csv_store(t_iem_pbank_csv *x, t_symbol *s, int ac, t_atom *av)
{
  int i, onset=0, nrp=x->x_nr_para;
  t_atom *atbuf=x->x_atbegbuf, *atmem=x->x_atbegmem;
  
  if(ac >= 2) nrp = atom_getintarg(1, ac, av);
  if(ac >= 1) onset = atom_getintarg(0, ac, av);
  if(onset < 0) onset = 0;
  if(onset >= x->x_nr_para) onset = x->x_nr_para - 1;
  if(nrp < 0) nrp = 0;
  if((onset+nrp) > x->x_nr_para) nrp = x->x_nr_para - onset;
  
  atmem += (x->x_nr_para * x->x_line) + onset;
  atbuf += onset;
  for(i=0; i<nrp; i++)
    *atmem++ = *atbuf++;
}

static void iem_pbank_csv_list(t_iem_pbank_csv *x, t_symbol *s, int ac, t_atom *av)
{
  if(ac >= 2)
  {
    int para_index = atom_getintarg(0, ac, av);
    
    if(para_index >= 0)
    {
      if((para_index+ac-1) <= x->x_nr_para)
      {
        int i;
        
        for(i=1; i<ac; i++)
        {
          x->x_atbegbuf[para_index] = av[i];
          para_index++;
        }
      }
    }
  }
}

static void iem_pbank_csv_ft1(t_iem_pbank_csv *x, t_floatarg fline_nr)
{
  int line = (int)fline_nr;
  
  if(line < 0)
    line = 0;
  else if(line >= x->x_nr_line)
    line = x->x_nr_line - 1;
  x->x_line = line;
}

static void iem_pbank_csv_free(t_iem_pbank_csv *x)
{
  freebytes(x->x_atbegmem, ((x->x_nr_para + 1) * (x->x_nr_line +1) + 4) * sizeof(t_atom));
  freebytes(x->x_atbegbuf, (x->x_nr_para + 4) * sizeof(t_atom));
  freebytes(x->x_atbegout, (x->x_nr_para + 4) * sizeof(t_atom));
}

static void *iem_pbank_csv_new(t_symbol *s, int ac, t_atom *av)
{
  t_iem_pbank_csv *x = (t_iem_pbank_csv *)pd_new(iem_pbank_csv_class);
  int nrpp=0, nrp=10, nrl=10, p, l, i;
  t_atom *ap;
  
  if((ac >= 1) && IS_A_FLOAT(av,0))
    nrp = atom_getintarg(0, ac, av);
  if((ac >= 2) && IS_A_FLOAT(av,1))
    nrl = atom_getintarg(1, ac, av);
  if(nrp <= 0)
    nrp = 10;
  if(nrl <= 0)
    nrl = 10;
  x->x_line = 0;
  x->x_nr_para = nrp;
  x->x_nr_line = nrl;
  x->x_atbegmem = (t_atom *)getbytes(((x->x_nr_para + 1) * (x->x_nr_line +1) + 4) * sizeof(t_atom));
  x->x_atbegbuf = (t_atom *)getbytes((x->x_nr_para + 4) * sizeof(t_atom));
  x->x_atbegout = (t_atom *)getbytes((x->x_nr_para + 4) * sizeof(t_atom));
  ap = x->x_atbegmem;
  for(l=0; l<nrl; l++)
  {
    for(p=0; p<nrp; p++)
    {
      SETFLOAT(ap, 0.0f);
      ap++;
    }
  }
  ap = x->x_atbegbuf;
  for(p=0; p<nrp; p++)
  {
    SETFLOAT(ap, 0.0f);
    ap++;
  }
  ap = x->x_atbegout;
  for(p=0; p<=nrp; p++)
  {
    SETFLOAT(ap, 0.0f);
    ap++;
  }
  x->x_list_out = outlet_new(&x->x_obj, &s_list);     /*left out*/
  x->x_offset_list_out = outlet_new(&x->x_obj, &s_list);  /*right out*/
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ft1"));
  x->x_canvas = canvas_getcurrent();
  return (x);
}

/* ---------------- global setup function -------------------- */

void iem_pbank_csv_setup(void )
{
  iem_pbank_csv_class = class_new(gensym("iem_pbank_csv"), (t_newmethod)iem_pbank_csv_new,
                                  (t_method)iem_pbank_csv_free, sizeof(t_iem_pbank_csv), 0, A_GIMME, 0);
  class_addmethod(iem_pbank_csv_class, (t_method)iem_pbank_csv_recall, gensym("recall"), A_GIMME, 0);
  class_addmethod(iem_pbank_csv_class, (t_method)iem_pbank_csv_store, gensym("store"), A_GIMME, 0);
  class_addmethod(iem_pbank_csv_class, (t_method)iem_pbank_csv_read, gensym("read"), A_SYMBOL, A_DEFSYM, 0);
  class_addmethod(iem_pbank_csv_class, (t_method)iem_pbank_csv_write, gensym("write"), A_SYMBOL, A_DEFSYM, 0);
  class_addlist(iem_pbank_csv_class, iem_pbank_csv_list);
  class_addbang(iem_pbank_csv_class, iem_pbank_csv_bang);
  class_addmethod(iem_pbank_csv_class, (t_method)iem_pbank_csv_ft1, gensym("ft1"), A_FLOAT, 0);
}
