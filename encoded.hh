// $Revision: 0.11 $ $Date: 2010/02/22 19:40:33 $

// Reprezentacja różnych kodowań wejścia.

#include "char-ranges.hh"

enum charclasses {
  ch_null,     /* end of string */
  ch_punct = '!',
  ch_white = ' ',
  ch_digit = '0',
  ch_apostrophe = '\''
};



class EncodedPtr {
  friend class EncodedBuffer;

  static unsigned char *convtab;      /* tablica konwersji encoded->internal */
  static unsigned char *backconvtab;  /* tablica konwersji internal->encoded */
  static unsigned char unicode_morf[MORFENC_MAXUNIVAL];
  static unsigned int  morf_unicode[256];

  // Funkcja znajdująca adres następnego znaku:
  static char *next_char_8bit(char *p) {return p+1;}
  static char *next_char_utf8(char *p);
  static char *(*next_char)(char *);

  // Funkcja znajdująca adres znaku o i wcześniejszego:
  static char *prev_char_8bit(char *p, int i) { return p-i; }
  static char *prev_char_utf8(char *p, int i);
  static char *(*prev_char)(char *, int);

  // Dereferencja wskaźnika z konwersją na kod Morfeusza:
  static unsigned char derefinternal_8bit(unsigned char *q) {
    return convtab[*q];}
  static unsigned char derefinternal_utf8(unsigned char *);
  static unsigned char (*derefinternal_impl)(unsigned char *);

  // Dodanie zewnętrznej reprezentacji c znaku i przesunięcie wskaźnika q:
  static void encode_and_append_8bit(char *&q, unsigned char c) {
    *q++ = backconvtab[c];
  }
  static void encode_and_append_utf8(char *&, unsigned char);
  static void (*encode_and_append)(char *&, unsigned char);

  char *p;
public:
  EncodedPtr () : p(0) {}
  EncodedPtr (char *s) : p(s) { }
  static int set_encoding(int);
  bool isempty() { return *p == '\0'; }   // !!!UTF16!!!
  bool isnonalpha() {
    return derefinternal() < MORFENC_DIGITS ; // NULL, WHITE, INTERP
  }
  void skiptononalpha() { while(!isnonalpha()) p = (*next_char)(p); } 
  unsigned char derefinternal() {
    return (*derefinternal_impl)((unsigned char*)p);
  } 
  unsigned char derefinternal_lc() {
    unsigned char c=derefinternal();
    return (c>=MORFENC_CASEABLE && c<MORFENC_NOTACHARCTER)? c|0x1 : c;
  } 
  void operator ++ (int) { p = (*next_char)(p); } 
  char *get_c_str() { return p; }
};

class EncodedBuffer {
  static const int MAX_BUFFER = 50000;
  static char buf[MAX_BUFFER];
  char *currstart, *currptr;

public:
  EncodedBuffer () { reset(); }
  void reset() { currptr=currstart=buf; }
  EncodedPtr alloc(EncodedPtr, EncodedPtr);
  EncodedPtr allochaslo(EncodedPtr, EncodedPtr, 
			unsigned short, int, const char*);
  EncodedPtr genforme(EncodedPtr, EncodedPtr, 
		      int, char, const char*);
  void backup(int i) {
    currptr = (*EncodedPtr::prev_char)(currptr,i); }
  void import(const char *s) {
    // sprawdzać zakres!!!
    do {
      (*EncodedPtr::encode_and_append)(currptr, (unsigned char)*s);
    } while ( *s++ );
  }
};
