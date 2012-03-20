// $Revision: 0.3 $ $Date: 2009/01/06 20:53:22 $

// Konsturowacz wyników w postaci tablicy elementów InterpMorf

class ResultBuffer {
  static const int MAX_RES = 3000;
  static InterpMorf interps[MAX_RES+1];
  int resfree; // wskazuje piewszą wolną komórkę tablicy interps
  
public:
  ResultBuffer () { reset(); }
  void reset() { resfree = 0; }
  InterpMorf *get_them() {
    if (resfree == MAX_RES) resfree=0;
    interps[resfree] = (InterpMorf){ -1, -1, NULL, NULL, NULL };
    return interps;
  }
  void alloc(int start, int stop, EncodedPtr forma, EncodedPtr haslo, char *tag) {
    if (resfree == MAX_RES) {resfree=1; interps[0].p=-1;}
    interps[resfree++] = (InterpMorf){start, stop,
				      forma.get_c_str(),
				      haslo.get_c_str(), tag };
  }
  void finalize_segment(int segint, int numint, int segstop, int numformy) {
    for (int i=segint; i<numint; i++)
      if (interps[i].k == segstop)
	interps[i].k = numformy;
  }
};
