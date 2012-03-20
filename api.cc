// $Revision: 0.36 $ $Date: 2010/01/27 15:50:06 $
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "fsa.hh"
#include "morfeusz.h"
#include "formy_opis.hh"
#include "segmenty_opis.hh"
#include "result.hh"
#include "version.h"

static int morfeusz_option_whitespace = 0;

extern "C" DLLIMPORT
char *morfeusz_about() {
  return morfeusz_version;
}
 

EncodedBuffer textbuffer;
ResultBuffer results;
InterpMorf ResultBuffer::interps[];
extern dfa formy;
extern dfa segmenty;
const int maxdepth = 10;

extern "C" DLLIMPORT
InterpMorf *morfeusz_analyse(char *tekst) {
  EncodedPtr t(tekst);
  int numformy=0, numint=0; // ???

  textbuffer.reset(); results.reset();

  while (!t.isempty()) {
    int d=0;
    int segstart=numformy, segint=numint; // ???
    const int segstop=INT_MAX;
    pref_iter pi[maxdepth];
    dfa_walker segm[maxdepth];
    formy_opis res[maxdepth];
    pi[0] = formy.lookup_prefixes(t);
    while (d>=0) {
      while (char *proto_res=pi[d].next()) {
	res[d] = formy_opis(proto_res);
	if (d==0) segm[0] = segmenty.new_walker();
	else segm[d]=segm[d-1];
	if (!segm[d].step(res[d].typzb)) continue;
	//	  printf("'%s'-'%s'\t %s\n", pi[d].text(), pi[d].rest(), res[d]);
	if (!pi[d].rest().isnonalpha()) {
	  if (d==maxdepth-1) {
	    //	      printf("!!! d==%d\n", d);
	    continue;
	  }
	  pi[d+1] = formy.lookup_prefixes(pi[d].rest());
	  d++;
	  //	    printf("(%d\n", d);
	} else {
	  if (!segm[d].get_value()) continue;
	  // znaleziono interpretację całości
	  t=pi[d].rest(); // odcinamy zanalizowany fragment
	  int start=segstart, stop;
//	  if (!segm[d].get_value()) continue;
	  segmenty_opis os = segmenty_opis(segm[d].get_value());
	  // 'J': pomijamy naj, 'E': pomijamy nie
	  for (int i=((res[0].istype('J')||
		       (d>0 && res[0].istype('E') && res[1].istype('R')))?1:0); i<=d; i++) {
	    if (i==d) stop=segstop; else stop = ++numformy;
	    //	      printf("=<%d> %d:%d\t'%s'-'%s'\t\t <%s>\n",
	    //		     i, start, stop, pi[i].text(),
	    //		     pi[i].rest(), res[i]);
	    for (int liczint=0; liczint < res[i].ile_int; liczint++) {
	      char typ = res[i].segm[liczint].typ;
	      if (!os.issegm(typ,i)) continue;
	      // 'S'/'R': doczepiamy poprzedni segment (naj/nie) do formy:
	      EncodedPtr forma=textbuffer.alloc(pi[(typ=='S'||typ=='R')?0:i].text(), pi[i].rest() );
	      EncodedPtr haslo
		=textbuffer.allochaslo(pi[i].text(),
				       pi[i].rest(),
				       res[i].segm[liczint].wielkie,
				       res[i].segm[liczint].ile_uciac,
				       res[i].segm[liczint].zakh );
	      numint++;
	      results.alloc(start, stop, forma, haslo,
			      (char*)res[i].segm[liczint].znacznik);
	    }
	    start=stop;
	  }
	}
      }
      //	printf(" %d)\n", d);
      d--;
    }
    if (segint == numint) { // Nie znaleziono żadnej interpretacji
      unsigned char c;
      if ((c=t.derefinternal()) < MORFENC_INTERP ) {
	//      case ch_white:
	if (morfeusz_option_whitespace) {
	  while ((c=t.derefinternal()) >= MORFENC_WHITE && c < MORFENC_INTERP) t++;
	  EncodedPtr forma=textbuffer.alloc(pi[0].text(), t);
	  results.alloc(segstart, segstop, forma, EncodedPtr(" "), "sp" );
	} else {
	  t++;
	  continue; // pomijamy jeden znak
	}
      } else if ( c < MORFENC_DIGITS ) { 
	// case ch_punct:
	t++;
	EncodedPtr forma=textbuffer.alloc(pi[0].text(), t);
	results.alloc(segstart, segstop, forma, forma, "interp" );}
      else {
	t.skiptononalpha();
	EncodedPtr forma=textbuffer.alloc(pi[0].text(), t);
	results.alloc(segstart, segstop, forma, EncodedPtr(), NULL );
      }
      numint++;
    }
    ++numformy;
    results.finalize_segment(segint,numint,segstop,numformy);
  }
  
  return results.get_them();
}


extern "C" DLLIMPORT
int morfeusz_set_option(int option, int value) {
  switch (option) {
  case MORFOPT_ENCODING:
    return EncodedPtr::set_encoding(value);
  case MORFOPT_WHITESPACE:
    morfeusz_option_whitespace=value;
    return 1;
  default:
    fprintf(stderr,"Wrong option %d\n",option);
    return 0;
  }
  return 1;
}
