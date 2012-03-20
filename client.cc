#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "morfeusz.h"

char * rl_gets(char *&line_read)
{
 /* If the buffer has already been allocated,
    return the memory to the free pool. */
 if (line_read)
   {
     free (line_read);
     line_read = (char *)NULL;
   }

 /* Get a line from the user. */
 line_read = readline (">");

 /* If the line has any text in it,
    save it on the history. */
 if (line_read && *line_read)
   add_history (line_read);

 return (line_read);
}



int main() {
  InterpMorf *mo;
  char *tekst = (char*)NULL;

  morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
  morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_KEEP_WHITESPACE);
  fprintf(stderr, "%s\n\n", morfeusz_about());

  while (rl_gets(tekst)) {
    int i = 0, prevp=-1;
    mo = morfeusz_analyse(tekst);
    if (mo[0].p == -1) continue;
    printf("[");
    while (mo[i].p != -1) {
      if (prevp != -1) {
	if (prevp != mo[i].p) printf("]\n[");
	else printf("; ");
      }
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
  printf("\n");
  return 0;
}
