// $Revision: 0.4 $ $Date: 2009/02/06 17:24:43 $
// Reprezentacja opisu jednej pozycji w słowniku


// największa liczba interpretacji jednego segmentu w słowniku:
const int MAXOPIS = 10;

struct formy_opis {
  struct segment_t {
    char typ, ile_uciac;
    unsigned short wielkie;  // wzorzec bitowy wielkich liter w haśle
    const char *zakh, *znacznik;
  };
  //  const 
  char ile_int, typzb;
  segment_t segm[MAXOPIS];

  formy_opis() : ile_int(0) {}
  formy_opis(const char *op) : ile_int(*op++), typzb(*op++) {
    for (int i=0; i<ile_int; i++) {
      segm[i].typ = *op++;
      segm[i].ile_uciac = *op++;
      //      segm[i].wielkie = ((unsigned short)(*op++) << 8) + (unsigned char)(*op++);
      segm[i].wielkie = ((unsigned short)(*op++)) <<8;
      segm[i].wielkie += (unsigned short)(*op++);
      segm[i].zakh = op;
      while (*op++);
      segm[i].znacznik = op;
      while (*op++);
    }
  }

  bool istype(char t) {
    for (int liczint=0; liczint < ile_int; liczint++) {
      if (segm[liczint].typ==t) return true;
    }
    return false;
  }
};



