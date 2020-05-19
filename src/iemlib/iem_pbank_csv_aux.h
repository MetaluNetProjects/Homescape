static void iem_pbank_csv_debug(char *buf, int n)
{
  while(n >= 16)
  {
    post("%x %x %x %x | %x %x %x %x | %x %x %x %x | %x %x %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
    n -= 16;
    buf += 16;
  }
  switch(n)
  {
  case 15:
    post("%x %x %x %x | %x %x %x %x | %x %x %x %x | %x %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14]);
    break;
  case 14:
    post("%x %x %x %x | %x %x %x %x | %x %x %x %x | %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13]);
    break;
  case 13:
    post("%x %x %x %x | %x %x %x %x | %x %x %x %x | %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12]);
    break;
  case 12:
    post("%x %x %x %x | %x %x %x %x | %x %x %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
    break;
  case 11:
    post("%x %x %x %x | %x %x %x %x | %x %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10]);
    break;
  case 10:
    post("%x %x %x %x | %x %x %x %x | %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);
    break;
  case 9:
    post("%x %x %x %x | %x %x %x %x | %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
    break;
  case 8:
    post("%x %x %x %x | %x %x %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    break;
  case 7:
    post("%x %x %x %x | %x %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
    break;
  case 6:
    post("%x %x %x %x | %x %x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
    break;
  case 5:
    post("%x %x %x %x | %x", buf[0], buf[1], buf[2], buf[3], buf[4]);
    break;
  case 4:
    post("%x %x %x %x", buf[0], buf[1], buf[2], buf[3]);
    break;
  case 3:
    post("%x %x %x", buf[0], buf[1], buf[2]);
    break;
  case 2:
    post("%x %x", buf[0], buf[1]);
    break;
  case 1:
    post("%x", buf[0]);
    break;
   }
}

/* 
   restrictions: 
   char >= 32 (Space )
   char <= 126 (Tilde~)
   char != 92 (Backslash\)
   char != 123 (opening curly brackets{)
   char != 125 (closing curly brackets})
*/

/* the text to float procedure is from miller's "void binbuf_text(t_binbuf *x, const char *text, size_t size)" */
/* convert text to atom list and truncate the text-atoms if they are larger than the atom-matrix */

static int iem_pbank_csv_convert_text_to_atom_list(t_iem_pbank_csv *x, const char *text, const int text_size)
{
  char buf[MAXPDSTRING+1], *bufp, *ebuf=buf+MAXPDSTRING;
  const char *textp=text, *etext = text+text_size;
  int natom = 0, para_counter=0, line_counter=0, truncated1_filled2=0, resid=text_size;
  const int nrl=x->x_nr_line, nrp=x->x_nr_para;
  t_atom *atp = x->x_atbegmem;
  t_float f;
  
  
  while(1)
  {
    /* int deb=0;
    post("deb=%d",deb++); */
    if(textp == etext)
      break;
    
    // if regular or text-width is greater than nrp: textp++, line_conter++, para_counter=0
    // if text-width is less than nrp:  textp++, loop until para_counter=nrp-1: atom=0.0f,  line_conter++, para_counter=0
    if(*textp == IEM_PBANK_UNIFIED_EOL)// text is "End Of Line"
    {
      while(para_counter < (nrp - 1))// fill para columns with zeros
      {
        truncated1_filled2 |= 2;
        atp++;
        para_counter++;
      }
      para_counter = 0;
      textp++;
      resid--;
      line_counter++;
      if(line_counter >= nrl)// truncate lines
      {
        if(resid > 0)
          truncated1_filled2 |= 1;
        break;
      }
    }
    // if regular or text-width is greater than nrp: textp++, line_conter++, para_counter=0
    // if text-width is less than nrp:  textp++, loop until para_counter=nrp-1: atom=0.0f,  line_conter++, para_counter=0
    else if(*textp == IEM_PBANK_UNIFIED_SEP)// text is "Separator"
    {
      textp++;
      resid--;
      para_counter++;
      if(para_counter >= nrp)
        truncated1_filled2 |= 1;
    }
    else
    {
      char c;
      int flst = 0;
      
      bufp = buf;
      do
      {
        c = *bufp = *textp++;
        resid--;
        if (flst >= 0)
        {
          int digit = (c >= '0' && c <= '9'),
          dot = (c == '.'), minus = (c == '-'),
          plusminus = (minus || (c == '+')),
          expon = (c == 'e' || c == 'E');
          if (flst == 0)  /* beginning */
          {
            if (minus) flst = 1;
            else if (digit) flst = 2;
            else if (dot) flst = 3;
            else flst = -1;
          }
          else if (flst == 1) /* got minus */
          {
            if (digit) flst = 2;
            else if (dot) flst = 3;
            else flst = -1;
          }
          else if (flst == 2) /* got digits */
          {
            if (dot) flst = 4;
            else if (expon) flst = 6;
            else if (!digit) flst = -1;
          }
          else if (flst == 3) /* got '.' without digits */
          {
            if (digit) flst = 5;
            else flst = -1;
          }
          else if (flst == 4) /* got '.' after digits */
          {
            if (digit) flst = 5;
            else if (expon) flst = 6;
            else flst = -1;
          }
          else if (flst == 5) /* got digits after . */
          {
            if (expon) flst = 6;
            else if (!digit) flst = -1;
          }
          else if (flst == 6) /* got 'e' */
          {
            if (plusminus) flst = 7;
            else if (digit) flst = 8;
            else flst = -1;
          }
          else if (flst == 7) /* got plus or minus */
          {
            if (digit) flst = 8;
            else flst = -1;
          }
          else if (flst == 8) /* got digits */
          {
            if (!digit) flst = -1;
          }
        }
        bufp++;
      }
      while ((textp != etext) && (bufp != ebuf) && (*textp != IEM_PBANK_UNIFIED_SEP) && (*textp != IEM_PBANK_UNIFIED_EOL));
      
      *bufp = 0;// terminate string
      
      if((line_counter < nrl) && (para_counter < nrp)) // if text items are within borders, then copy them into atom-matrix
      {
        if(flst == 2 || flst == 4 || flst == 5 || flst == 8) // if text-atom is float
        {
          f = atof(buf);
          if((f < 1.0e-20)&&(f > -1.0e-20))
            f = 0.0;
          SETFLOAT(atp, f);
        }
        else // else text-atom is a symbol
          SETSYMBOL(atp, gensym(buf));
        atp++;
      } // else ignore text items
    }
    
    if(textp == etext)
      break;
  }
  return(truncated1_filled2);
}


static int discuss_sep_eol(const char *formatsymbolname, char *sep, char *eol, char *formattextinfo)
{
  int eol_length=0;
  
    if(strlen(formatsymbolname) >= IEM_PBANK_FORMAT_SIZE)
    {
      switch(formatsymbolname[IEM_PBANK_ITEM_SEPARATOR])
      {
        case 'B':
        case 'b':
          sep[0] = ' ';
          strcpy(formattextinfo, "item-separator = BLANK; ");
          break;
        case 'S':
        case 's':
          sep[0] = ';';
          strcpy(formattextinfo, "item-separator = SEMICOLON; ");
          break;
        case 'T':
        case 't':
          sep[0] = '\t';
          strcpy(formattextinfo, "item-separator = TABULATOR; ");
          break;
        default:
          sep[0] = ' ';// default: blank-separator, return-eol, return depends on operating system
          strcpy(formattextinfo, "default item-separator = BLANK; default end_of_line_terminator = RETURN.");
      }
      
      switch(formatsymbolname[IEM_PBANK_END_OF_LINE])
      {
        case 'B':
        case 'b':
          sprintf(eol, " \n");
          eol_length = 2;
          strcat(formattextinfo, "end_of_line_terminator = BLANK-RETURN.");
          break;
        case 'S':
        case 's':
          sprintf(eol, ";\n");
          eol_length = 2;
          strcat(formattextinfo, "end_of_line_terminator = SEMICOLON-RETURN.");
          break;
        case 'T':
        case 't':
          sprintf(eol, "\t\n");
          eol_length = 2;
          strcat(formattextinfo, "end_of_line_terminator = TABULATOR-RETURN.");
          break;
        case 'R':
        case 'r':
          sprintf(eol, "\n");
          eol_length = 1;
          strcat(formattextinfo, "end_of_line_terminator = RETURN.");
          break;
        default:
          sprintf(eol, "\n");// default: blank-separator, return-eol, return depends on operating system
          eol_length = 1;
          strcpy(formattextinfo, "default item-separator = BLANK; default end_of_line_terminator = RETURN.");
      }
    }
    else
    {
      sep[0] = ' ';
      sprintf(eol, "\n");// default: blank-separator, return-eol, return depends on operating system
      eol_length = 1;
      strcpy(formattextinfo, "default item-separator = BLANK; default end_of_line_terminator = RETURN.");
    }
    return(eol_length);
}
    
static int iem_pbank_csv_text_filter(char **text_buffer, const int text_malloc, const char *sep, const char *eol, const int eolen)
{
  char *text_buffer2, *src, *dst;
  int curlen=0, txtlen=text_malloc, l=0;
  
  text_buffer2 = (char *)getbytes((2 * text_malloc + 256) * sizeof(char));
       
// 1.)  filter out only readable ASCII numbers (0x09, 0x0a, 0x0d, 0x20...0x7e, except 0x5c, 0x7b, 0x7d = 
//      = TAB, LF, CR, ' ' ... '~', except '\', '{', '}' = 
//      = Tabulator, LineFeed, CarriageReturn, Space ... Tilde, except Backslash, opening curly brackets, closing curly brackets), else drop them
    src = *text_buffer;
    dst = text_buffer2;
    curlen = 0;
    
    //post("ungefiltert:\n");
    for(l=0; l<txtlen; l++)
    {
      //post("%0x ", *src);
      if(((*src >= 0x20) && (*src <= 0x7e) && (*src != 0x5c) && (*src != 0x7d) && (*src != 0x7b)) || (*src == 0x09) || (*src == 0x0a) || (*src == 0x0d))
      {
        *dst++ = *src++;  // copy the same char
        curlen++;
      }
      else
        src++;// drop anything else
    }
    txtlen = curlen; // dst is txbuf_2, OK
       
// 2.) unify windows return (2 char to 1 char)
    src = text_buffer2;
    dst = *text_buffer;
    curlen = 0;
    //post("gefiltert_1:\n");
    for(l=0; l<txtlen; l++)
    {
      //post("%0x ", *src);
      if(l < (txtlen - 1))// because we seek 2 char
      {
        if((src[0] == 0x0d)&&(src[1] == 0x0a))  // windows return
        {
          *dst++ = IEM_PBANK_UNIFIED_RET;
          src += 2;
          l++;
          curlen++;
        }
        else
        {
          *dst++ = *src++;// ordinary copy
          curlen++;
        }
      }
      else
      {
        *dst++ = *src++;// ordinary copy
        curlen++;
      }
    }
    txtlen = curlen; // dst is txbuf_1, OK
       
// 3.) unify any other return, 4.) unify separator, 5.) replace comma ',' by dot '.' (MS Excel behavior to floating-point numbers)
    src = *text_buffer;
    dst = text_buffer2;
    //post("gefiltert_2:\n");
    for(l=0; l<txtlen; l++)
    {
      //post("%0x ", *src);
      if((*src == 0x0d) || (*src == 0x0a))  //  return
        *dst = IEM_PBANK_UNIFIED_RET;
      else if(*src == sep[0])  // replace 'sep' by IEM_PBANK_UNIFIED_SEP
        *dst = IEM_PBANK_UNIFIED_SEP;
      else if(*src == ',')  // replace 'sep' by IEM_PBANK_UNIFIED_SEP
        *dst = '.';
      else
        *dst = *src;
      src++;
      dst++;
    }// dst is txbuf_2, OK, txtlen is still the same
      
// 6.) unify EndOfLine
    src = text_buffer2;
    dst = *text_buffer;
    
    //post("gefiltert_5:\n");
    curlen = 0;
    if(eolen == 2) // EndOfLine are 2 char
    {
      for(l=0; l<txtlen; l++)
      {
        //post("%0x ", *src);
        if(l < (txtlen - 1))// because we seek 2 char
        {
          if((src[0] == eol[0])&&(src[1] == IEM_PBANK_UNIFIED_RET))
          {
            *dst++ = IEM_PBANK_UNIFIED_EOL;
            src += 2;
            l++;
          }
          else
            *dst++ = *src++;
          curlen++;
        }
        else
        {
          *dst++ = *src++;
          curlen++;
        }
      }
    }
    else // EndOfLine is only 1 char
    {
      for(l=0; l<txtlen; l++)
      {
        //post("%0x ", *src);
        if(*src == IEM_PBANK_UNIFIED_RET)
          *dst++ = IEM_PBANK_UNIFIED_EOL;
        else
          *dst++ = *src;
        src++;
        curlen++;
      }
    }
    txtlen = curlen; // dst is txbuf_1, OK
    
// 7.) remove Space ' ', Comma ',' and Semicolon ';'
    src = *text_buffer;
    dst = text_buffer2;
    //post("gefiltert_6:\n");
    curlen = 0;
    for(l=0; l<txtlen; l++)
    {
      //post("%0x ", *src);
      if((*src != 0x20) && (*src != 0x2c) && (*src != 0x3b)) // Space ' ', Comma ',' and Semicolon ';'
      {
        *dst++ = *src++;
        curlen++;
      }
      else
        src++;
    }
    txtlen = curlen;// dst is txbuf_2, OK
       
// 8a.) fill between 2 separators a zero
// 8b.) fill between separator and eol a zero
// 8c.) fill between eol and separator a zero
    src = text_buffer2;
    dst = *text_buffer;
    //post("gefiltert_7:\n");
    curlen = 0;
    for(l=0; l<txtlen; l++)
    {
      //post("%0x ", *src);
      if(l < (txtlen - 1))
      {
        if((src[0] == IEM_PBANK_UNIFIED_SEP) && (src[1] == IEM_PBANK_UNIFIED_SEP))  // fill between 2 sep a zero
        {
          dst[0] = IEM_PBANK_UNIFIED_SEP;
          dst[1] = '0';
          src++;
          l++;
          dst += 2;
          curlen += 2;
        }
        else if((src[0] == IEM_PBANK_UNIFIED_SEP) && (src[1] == IEM_PBANK_UNIFIED_EOL))  // fill between sep and eol a zero
        {
          dst[0] = IEM_PBANK_UNIFIED_SEP;
          dst[1] = '0';
          src++;
          l++;
          dst += 2;
          curlen += 2;
        }
        else if((src[0] == IEM_PBANK_UNIFIED_EOL)&&(src[1] == IEM_PBANK_UNIFIED_SEP))  // fill between eol and sep a zero
        {
          dst[0] = IEM_PBANK_UNIFIED_EOL;
          dst[1] = '0';
          src++;
          l++;
          dst += 2;
          curlen += 2;
        }
        else // copy the same char
        {
          *dst++ = *src++;
          curlen++;
        }
      }
      else
      {
        *dst++ = *src++;
        curlen++;
      }
    }
    txtlen = curlen; // dst is txbuf_1, OK
 /* debug:
    post("gefiltert_8:\n");
    src = *text_buffer;
    for(l=0; l<txtlen; l++)
    {
      post("%0x ", *src);
      src++;
    } */
    
    
    freebytes(text_buffer2, (2 * text_malloc + 256) * sizeof(char));
    
    return(txtlen);
}