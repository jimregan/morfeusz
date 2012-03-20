// $Revision: 0.3 $ $Date: 2004/03/24 22:09:23 $
// Reprezentacja opisu jednej pozycji w s�owniku segment�w


// najwi�ksza liczba interpretacji jednego segmentu w s�owniku:
const int MAXSEGMENT = 5;

struct segmenty_opis {
  char ile_int;
  const char *segm[MAXSEGMENT];

  //  segmenty_opis() : ile_int(0) {}

  // Opis segment�w sk�ada si� z jednego bajtu reprezentuj�cego liczb�
  // interpretacji, po kt�rym nast�puje ci�g napis�w oddzielonych
  // NULLem.
  segmenty_opis(const char *op) : ile_int(*op++) {
    for (int i=0; i<ile_int; i++) {
      segm[i] = op;
      while (*op++);
    }
  }

  bool issegm(char t, int i) {
    for (int liczint=0; liczint < ile_int; liczint++) {
      if (segm[liczint][i]==t) return true;
    }
    return false;
  }

};

