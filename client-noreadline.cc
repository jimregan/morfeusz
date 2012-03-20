#include <stdio.h>
#include "morfeusz.h"

#define BUFLEN 10000

int main() {
  InterpMorf *mo;
  char buf[BUFLEN];

  //  morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_ISO8859_2);
#ifdef __WIN32
  morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP852);
#else
  morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
#endif
  fprintf(stderr, "%s\n\n", morfeusz_about());


  while (fgets(buf, BUFLEN, stdin)) {
    int i = 0, prevp=-1;
    mo = morfeusz_analyse(buf);
    if (mo[0].p == -1) continue;
    printf("[");
    while (mo[i].p != -1) {
      if (prevp != -1)
	if (prevp != mo[i].p) printf("]\n[");
	else printf("; ");
      printf("%s,%s,%s",
	     //	     mo[i].p, mo[i].k,
	     mo[i].forma,
	     mo[i].haslo==NULL?"?":mo[i].haslo,
	     mo[i].interp==NULL?"?":mo[i].interp);
      prevp = mo[i].p;
      i++;
    }
    printf("]\n");
  }
  return 0;
}
