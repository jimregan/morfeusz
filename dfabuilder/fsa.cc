// $Revision: 0.5 $ $Date: 2004/05/23 11:10:29 $
#include "fsa.hh"

// Klasa dfa:

fsa_value dfa::lookup(fsa_alpha *key) {
 fsa_state_id s = start;
 unsigned child; fsa_transition_id tr;
 
 while (*key) {
   child = states[s].children;
   tr = states[s].trans;
   /* stara wersja (z b³êdem):
   if (!child) return 0;  // mo¿na wyrzuciæ je¶li bêdzie stra¿nik w 0 rekordzie
   while (child-- && transitions[tr].lab != *key) tr++;
   if (transitions[tr].lab != *key) return 0;
   */
   if (!child) return 0;
   while (transitions[tr].lab != *key && --child) tr++;
   if (!child) return 0;
   s = transitions[tr].state;
   key++;
 }
 return states[s].val;
}

fsa_value dfa::lookup_prefix_internal(fsa_state_id& s, EncodedPtr &key) {
  unsigned child; fsa_transition_id tr;
  
  if (key.isempty()) return 0;
  do {
    child = states[s].children;
    tr = states[s].trans;
    if (!child) return 0;
    while (transitions[tr].lab != key.derefinternal_lc() && --child) tr++;
    if (!child) return 0;
    s = transitions[tr].state;
    key++;
  } while (!key.isempty() && !states[s].val);
  return states[s].val;
}


// Klasa pref_iter:

fsa_value pref_iter::next() {
  return autom->lookup_prefix_internal(st, rst);
}
