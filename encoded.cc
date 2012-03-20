// $Revision: 0.9 $ $Date: 2010/02/22 19:40:26 $
#include <stdio.h>
#include "morfeusz.h"
#include "encoded.hh"

extern unsigned char latin2_morf[256];
extern unsigned char morf_latin2[256];
extern unsigned char cp1250_morf[256];
extern unsigned char morf_cp1250[256];
extern unsigned char cp852_morf[256];
extern unsigned char morf_cp852[256];

char EncodedBuffer::buf[];
unsigned char *EncodedPtr::convtab=latin2_morf;
unsigned char *EncodedPtr::backconvtab=morf_latin2;
char *(*EncodedPtr::next_char)(char *) = &EncodedPtr::next_char_utf8;
char *(*EncodedPtr::prev_char)(char *,int) = &EncodedPtr::prev_char_utf8;
unsigned char (*EncodedPtr::derefinternal_impl)(unsigned char *)
  = &EncodedPtr::derefinternal_utf8;
void (*EncodedPtr::encode_and_append)(char *&, unsigned char)
  = &EncodedPtr::encode_and_append_utf8;

EncodedPtr EncodedBuffer::alloc(EncodedPtr f, EncodedPtr t) {
  char *fp=f.p, *tp=t.p;
  currstart=currptr;
  // sprawdzać zakres!!!
  while (fp<tp) *currptr++=*fp++;
  *currptr++ = '\0';
  return EncodedPtr(currstart);
}

EncodedPtr EncodedBuffer::allochaslo(EncodedPtr f, EncodedPtr t,
				     unsigned short wielkie,
				     int ileuciac,
				     const char *zak) {
  int len=0;  
  unsigned short cwielkie = wielkie; // kopia na zapas
  currstart=currptr;
  // sprawdzać zakres!!!
  while (f.p<t.p) {
    unsigned char c = f.derefinternal(); 
    if (c>=MORFENC_CASEABLE && c<MORFENC_NOTACHARCTER)
      (*EncodedPtr::encode_and_append)(currptr,
				       (wielkie&0x0001) ? c&0xFE : c|0x1);
    else { // trzeba skopiować niezmienione chary odpowiadające
	   // następnemu znakowi:
      char *nxt = (*EncodedPtr::next_char)(f.p);
      char *fp = f.p;
      while (fp<nxt) *currptr++=*fp++;
    }
    f++;
    wielkie >>= 1;
    len++;
  }
  // zmieniamy zakończenie formy hasłowej:
  backup(ileuciac);
  cwielkie >>= len - ileuciac;
  do {
    unsigned char c = (unsigned char)*zak;
    (*EncodedPtr::encode_and_append)(currptr, (cwielkie&0x001)? c&0xFE : c);
    cwielkie >>= 1;
  } while ( *zak++ );
  //  *currptr++ = '\0';
  return EncodedPtr(currstart);
}

EncodedPtr EncodedBuffer::genforme(EncodedPtr f, EncodedPtr t,
				   int ileuciac,
				   char pref,
				   const char *zak) {
  int len=0;  
  currstart=currptr;
  // najprzód dodajemy ewentualny prefiks:
  if (pref == 'J') {
    // Zakładamy, że litery n,a,j będą pod kodami ASCII. Bezpieczne
    // dla dotychczas stosowanych kodów, ale byłby problem dla UTF-16
    *currptr++='n';
    *currptr++='a';
    *currptr++='j';
  }
  if (pref == 'I') {
    *currptr++='n';
    *currptr++='i';
    *currptr++='e';
  }
  // sprawdzać zakres!!!
  while (f.p<t.p) {
    unsigned char c = f.derefinternal(); 
    if (c>=MORFENC_CASEABLE && c<MORFENC_NOTACHARCTER)
      (*EncodedPtr::encode_and_append)(currptr, c);
    else { // trzeba skopiować niezmienione chary odpowiadające
	   // następnemu znakowi:
      char *nxt = (*EncodedPtr::next_char)(f.p);
      char *fp = f.p;
      while (fp<nxt) *currptr++=*fp++;
    }
    f++;
    len++;
  }
  // zmieniamy zakończenie formy hasłowej:
  backup(ileuciac);
  do {
    unsigned char c = (unsigned char)*zak;
    (*EncodedPtr::encode_and_append)(currptr, c);
  } while ( *zak++ );
  //  *currptr++ = '\0';
  return EncodedPtr(currstart);
}


int EncodedPtr::set_encoding(int encoding) {
  switch (encoding) {
  case MORFEUSZ_UTF_8:
    next_char=&next_char_utf8;
    prev_char=&prev_char_utf8;
    derefinternal_impl=&derefinternal_utf8;
    encode_and_append=&encode_and_append_utf8;
    break;
  case MORFEUSZ_ISO8859_2:
    convtab=latin2_morf;
    backconvtab=morf_latin2;
    next_char=&next_char_8bit;
    prev_char=&prev_char_8bit;
    derefinternal_impl=&derefinternal_8bit;
    encode_and_append=&encode_and_append_8bit;
    break;
  case MORFEUSZ_CP1250:
    convtab=cp1250_morf;
    backconvtab=morf_cp1250;
    next_char=&next_char_8bit;
    prev_char=&prev_char_8bit;
    derefinternal_impl=&derefinternal_8bit;
    encode_and_append=&encode_and_append_8bit;
    break;
  case MORFEUSZ_CP852:
    convtab=cp852_morf;
    backconvtab=morf_cp852;
    next_char=&next_char_8bit;
    prev_char=&prev_char_8bit;
    derefinternal_impl=&derefinternal_8bit;
    encode_and_append=&encode_and_append_8bit;
    break;
  default:
    fprintf(stderr,"Wrong encoding %d\n",encoding);
    return 0;
  }
  return 1;
}

// UTF-8

/* funkcja do pobierania kolejnych znaków w UTF-8 pozyczona od SQLite'a */

/*
** This table maps from the first byte of a UTF-8 character to the number
** of trailing bytes expected. A value '255' indicates that the table key
** is not a legal first byte for a UTF-8 character.
*/
static const unsigned char xtra_utf8_bytes[256]  = {
/* 0xxxxxxx */
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,

/* 10wwwwww */
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

/* 110yyyyy */
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,

/* 1110zzzz */
2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,

/* 11110yyy */
3, 3, 3, 3, 3, 3, 3, 3,     255, 255, 255, 255, 255, 255, 255, 255,
};

/*
** This table maps from the number of trailing bytes in a UTF-8 character
** to an integer constant that is effectively calculated for each character
** read by a naive implementation of a UTF-8 character reader. The code
** in the READ_UTF8 macro explains things best.
*/
static const int xtra_utf8_bits[4] =  {
0,
12416,          /* (0xC0 << 6) + (0x80) */
925824,         /* (0xE0 << 12) + (0x80 << 6) + (0x80) */
63447168        /* (0xF0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
};

#define READ_UTF8(zIn, c) { \
  int xtra;                                            \
  c = *(zIn)++;                                        \
  xtra = xtra_utf8_bytes[c];                           \
  switch( xtra ){                                      \
    case 255: c = (int)0xFFFD; break;                  \
    case 3: c = (c<<6) + *(zIn)++;                     \
    case 2: c = (c<<6) + *(zIn)++;                     \
    case 1: c = (c<<6) + *(zIn)++;                     \
    c -= xtra_utf8_bits[xtra];                         \
  }                                                    \
}

/* koniec pożyczki */

char *EncodedPtr::next_char_utf8(char *p) {
  unsigned char xtra = xtra_utf8_bytes[(unsigned char)*p];
  if (xtra == 255) xtra=0;
  return p+1+xtra;
}

char *EncodedPtr::prev_char_utf8(char *p, int i) {
  unsigned char *q = (unsigned char*)p;
  for (int j=0; j<i; j++)
    while (*--q>>6==0x2);
  return (char*)q;
}

unsigned char EncodedPtr::derefinternal_utf8(unsigned char *q) {
  unsigned int unival = *q++;
  int xtra = xtra_utf8_bytes[unival];
  switch( xtra ){
    case 255: unival = (int)0xFFFD; break;
    case 3: unival = (unival<<6) + *q++;
    case 2: unival = (unival<<6) + *q++;
    case 1: unival = (unival<<6) + *q++;
    unival -= xtra_utf8_bits[xtra];
  }
  if (unival < MORFENC_MAXUNIVAL)
    return unicode_morf[unival];
  else
    return MORFENC_NOTACHARCTER;
}

void EncodedPtr::encode_and_append_utf8(char *&q, unsigned char c) {
  unsigned int unival = morf_unicode[c];
  if (unival < 0x80) {
    *q++ = unival;
  } else if (unival < 0x800) {
    *q++ = 0xC0 | (unival >> 6);
    *q++ = 0x80 | (unival & 0x3F);
  } else if (unival < 0x10000) {
    *q++ = 0xE0 | (unival >> 12);
    *q++ = 0x80 | ((unival & 0xFC0) >> 6);
    *q++ = 0x80 | (unival & 0x3F);
  } else if (unival < 0x200000) {
    *q++ = 0xF0 | (unival >> 18);
    *q++ = 0x80 | ((unival & 0x3F000) >> 12);
    *q++ = 0x80 | ((unival & 0xFC0) >> 6);
    *q++ = 0x80 | (unival & 0x3F);
  }
}
