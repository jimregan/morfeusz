// $Revision: 0.7 $ $Date: 2004/05/23 11:10:36 $

#include "encoded.hh"

typedef char* fsa_value;
typedef unsigned char fsa_alpha;
typedef unsigned fsa_state_id;
typedef unsigned fsa_transition_id;

struct fsa_state {
  fsa_value val;
  unsigned children :8;
  unsigned trans :24;
};


struct fsa_trans {
  fsa_alpha lab :8;
  unsigned state :24;
};

class dfa;

// Generator niepustych prefiksów danego napisu akceptowanych przez
// dany automat
class pref_iter {
  EncodedPtr txt, rst;
  fsa_state_id st;
  dfa * autom;
public:
  pref_iter() {};
  pref_iter(dfa* fsa, fsa_state_id strt, EncodedPtr tx) :
    txt(tx), rst(tx), st(strt), autom(fsa) {}
  fsa_value next();
  EncodedPtr text() {return txt;}
  EncodedPtr rest() {return rst;}
};

// £azik po stanach automatu (do badania napisów znak po znaku)

class dfa_walker {
  dfa * autom;
  fsa_state_id st;
public:
  dfa_walker() : autom(0), st(0) {}
  dfa_walker(dfa* fsa, fsa_state_id strt) : autom(fsa), st(strt) {}
  bool step(fsa_alpha);
  fsa_value get_value();
};

// Deterministyczny automat skoñczony
class dfa {
  const fsa_state *states;
  const fsa_trans *transitions;
  const fsa_state_id start;
  friend class pref_iter;
  friend class dfa_walker;
  fsa_value lookup_prefix_internal(fsa_state_id&, EncodedPtr&);
  fsa_state_id step(fsa_state_id, fsa_alpha);
public:
  dfa(const fsa_state *st, const fsa_trans *tr, fsa_state_id s) 
    : states(st), transitions(tr), start(s) {}
  fsa_value lookup(fsa_alpha*);
  pref_iter lookup_prefixes(EncodedPtr text) {
    return pref_iter(this,start,text); 
  }
  dfa_walker new_walker() {
    return dfa_walker(this,start);
  }
};

inline fsa_state_id dfa::step(fsa_state_id s, fsa_alpha c) {
  unsigned child;
  fsa_transition_id tr;
  child = states[s].children;
  tr = states[s].trans;
  if (!child) return 0;
  while (transitions[tr].lab != c && --child) tr++;
  if (!child) return 0;
  return transitions[tr].state;
}

inline fsa_value dfa_walker::get_value() {
  return autom->states[st].val; 
}

inline bool dfa_walker::step(fsa_alpha c) {
  st = autom->step(st,c);
  return st != 0;
}
