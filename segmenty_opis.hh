// $Revision: 0.3 $ $Date: 2004/03/24 22:09:23 $
// Reprezentacja opisu jednej pozycji w s³owniku segmentów


// najwiêksza liczba interpretacji jednego segmentu w s³owniku:
const int MAXSEGMENT = 5;

struct segmenty_opis {
  char ile_int;
  const char *segm[MAXSEGMENT];

  //  segmenty_opis() : ile_int(0) {}

  // Opis segmentów sk³ada siê z jednego bajtu reprezentuj±cego liczbê
  // interpretacji, po którym nastêpuje ci±g napisów oddzielonych
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

